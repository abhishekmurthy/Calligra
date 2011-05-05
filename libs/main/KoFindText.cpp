/* This file is part of the KDE project
 *
 * Copyright (c) 2010 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "KoFindText.h"

#include <QtGui/QTextDocument>
#include <QtGui/QTextCursor>
#include <QtGui/QTextBlock>
#include <QtGui/QTextLayout>
#include <QtGui/QPalette>
#include <QtGui/QStyle>
#include <QtGui/QApplication>
#include <QtGui/QAbstractTextDocumentLayout>

#include <KDE/KDebug>
#include <KDE/KLocalizedString>

#include <KoResourceManager.h>
#include <KoText.h>
#include <KoTextDocument.h>
#include <KoShape.h>
#include <KoShapeContainer.h>
#include <KoTextShapeData.h>

#include "KoFindOptionSet.h"
#include "KoFindOption.h"
#include "KoDocument.h"

class KoFindText::Private
{
public:
    Private(KoFindText* qq) : q(qq), selectionStart(-1), selectionEnd(-1) { }

    void updateSelections();
    void updateDocumentList();
    void documentDestroyed(QObject *document);
    void updateCurrentMatch(int position);

    KoFindText *q;

    QList<QTextDocument*> documents;

    QTextCursor selection;
    QHash<QTextDocument*, QVector<QAbstractTextDocumentLayout::Selection> > selections;

    int selectionStart;
    int selectionEnd;

    static QTextCharFormat *highlightFormat;
    static QTextCharFormat *currentMatchFormat;
    static QTextCharFormat *currentSelectionFormat;

    QPair<QTextDocument*, int> currentMatch;
};

QTextCharFormat *KoFindText::Private::currentSelectionFormat = 0;
QTextCharFormat *KoFindText::Private::highlightFormat = 0;
QTextCharFormat *KoFindText::Private::currentMatchFormat = 0;

KoFindText::KoFindText(const QList< QTextDocument* >& documents, QObject* parent)
    : KoFindBase(parent), d(new Private(this))
{
    d->documents = documents;
    d->updateDocumentList();

    if(!d->highlightFormat) {
        d->highlightFormat = new QTextCharFormat();
        d->highlightFormat->setBackground(Qt::yellow);
    }

    if(!d->currentMatchFormat) {
        d->currentMatchFormat = new QTextCharFormat();
        d->currentMatchFormat->setBackground(qApp->palette().highlight());
        d->currentMatchFormat->setForeground(qApp->palette().highlightedText());
    }

    if(!d->currentSelectionFormat) {
        d->currentSelectionFormat = new QTextCharFormat();
        d->currentSelectionFormat->setBackground(qApp->palette().alternateBase());
    }

    KoFindOptionSet *options = new KoFindOptionSet();
    options->addOption("caseSensitive", i18n("Case Sensitive"), i18n("Match cases when searching"), QVariant::fromValue<bool>(false));
    options->addOption("wholeWords", i18n("Whole Words Only"), i18n("Match only whole words"), QVariant::fromValue<bool>(false));
    setOptions(options);
}

KoFindText::~KoFindText()
{
    delete d;
}

void KoFindText::findImplementation(const QString &pattern, QList<KoFindMatch> & matchList)
{
    KoFindOptionSet *opts = options();
    QTextDocument::FindFlags flags = 0;

    if(opts->option("caseSensitive")->value().toBool()) {
        flags |= QTextDocument::FindCaseSensitively;
    }
    if(opts->option("wholeWords")->value().toBool()) {
        flags |= QTextDocument::FindWholeWords;
    }

    int start = 0;
    bool findInSelection = false;

    if(d->documents.size() == 0) {
        kWarning() << "No document available for searching!";
        return;
    }

    int position = 0;
    int currentMatch = 0;
    bool matchFound;

    foreach(QTextDocument* document, d->documents) {
        QTextCursor cursor = document->find(pattern, start, flags);
        QVector<QAbstractTextDocumentLayout::Selection> selections;
        while(!cursor.isNull()) {
            if(findInSelection && d->selectionEnd <= cursor.position()) {
                break;
            }

            QAbstractTextDocumentLayout::Selection selection;
            selection.cursor = cursor;
            selection.format = *(d->highlightFormat);
            selections.append(selection);

            KoFindMatch match;
            match.setContainer(QVariant::fromValue(document));
            match.setLocation(QVariant::fromValue(cursor));
            matchList.append(match);

            if(position <= qMin(cursor.anchor(), cursor.position())) {
                matchFound = true;
            }

            if(!matchFound) {
                currentMatch++;
            }

            cursor = document->find(pattern, cursor, flags);
        }
        d->selections.insert(document, selections);
    }

    if(d->selections.size() > 0) {
        if(position >= d->selections.size()) {
            position = 0;
        }

        setCurrentMatch(position);
        d->updateCurrentMatch(position);
    }

    d->updateSelections();
}

void KoFindText::replaceImplementation(const KoFindMatch &/*match*/, const QVariant &/*value*/)
{
    //Does nothing at the moment...
}

void KoFindText::clearMatches()
{
    d->selections.clear();
    foreach(QTextDocument* doc, d->documents) {
        d->selections.insert(doc, QVector<QAbstractTextDocumentLayout::Selection>());
    }
    d->updateSelections();

    d->selectionStart = -1;
    d->selectionEnd = -1;

    setCurrentMatch(0);
}

void KoFindText::findNext()
{
    if(d->selections.size() == 0) {
        return;
    }

    KoFindBase::findNext();
    d->updateCurrentMatch(currentMatchIndex());
    d->updateSelections();
}

void KoFindText::findPrevious()
{
    if(d->selections.size() == 0) {
        return;
    }
    
    KoFindBase::findPrevious();
    d->updateCurrentMatch(currentMatchIndex());
    d->updateSelections();
}

void KoFindText::addDocuments(const QList<QTextDocument*> &documents)
{
    d->documents.append(documents);
    d->updateDocumentList();
}

void KoFindText::findTextInShapes(const QList<KoShape*> &shapes, QList<QTextDocument*> &append)
{
    foreach(KoShape* shape, shapes) {
        KoShapeContainer *container = dynamic_cast<KoShapeContainer*>(shape);
        if(container) {
            findTextInShapes(container->shapes(), append);
        }

        KoTextShapeData *shapeData = dynamic_cast<KoTextShapeData*>(shape->userData());
        if (!shapeData)
            continue;

        if(shapeData->document()) {
            if(!append.contains(shapeData->document())) {
                append.append(shapeData->document());
            }
        }
    }
}

void KoFindText::Private::updateSelections()
{
    QHash< QTextDocument*, QVector<QAbstractTextDocumentLayout::Selection> >::iterator itr;
    for(itr = selections.begin(); itr != selections.end(); ++itr) {
        KoTextDocument doc(itr.key());
        doc.setSelections(itr.value());
    }
}

void KoFindText::Private::updateDocumentList()
{
    foreach(QTextDocument *document, documents) {
        connect(document, SIGNAL(destroyed(QObject*)), q, SLOT(documentDestroyed(QObject*)), Qt::UniqueConnection);
    }
}

void KoFindText::Private::documentDestroyed(QObject *document)
{
    QTextDocument* doc = qobject_cast<QTextDocument*>(document);
    if(doc) {
        selections.remove(doc);
        documents.removeOne(doc);
    }
}

void KoFindText::Private::updateCurrentMatch(int position)
{
    if(currentMatch.first != 0) {
        QVector<QAbstractTextDocumentLayout::Selection> sel = selections.value(currentMatch.first);
        sel[currentMatch.second].format = *highlightFormat;
        selections.insert(currentMatch.first, sel);
    }
    
    int counter = 0;
    QHash<QTextDocument*, QVector<QAbstractTextDocumentLayout::Selection> >::iterator itr;
    for(itr = selections.begin(); itr != selections.end(); ++itr) {
        if(counter + itr.value().size() > position) {
            break;
        }
        counter += itr.value().size();
    }
    currentMatch.first = itr.key();
    currentMatch.second = position - counter;
    QVector<QAbstractTextDocumentLayout::Selection> sel = selections.value(itr.key());
    sel[currentMatch.second].format = *currentMatchFormat;
    selections.insert(itr.key(), sel);
}

#include "KoFindText.moc"