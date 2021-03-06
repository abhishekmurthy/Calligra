/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef KEXICOMPLETER_H
#define KEXICOMPLETER_H

#include <kexiutils_export.h>

#include <QObject>
#include <QPoint>
#include <QString>
#include <QAbstractItemModel>
#include <QRect>

#ifndef QT_NO_COMPLETER

class QAbstractItemView;
class QAbstractProxyModel;
class QWidget;
class QItemSelection;

class KexiCompleterPrivate;

class KEXIUTILS_EXPORT KexiCompleter : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString completionPrefix READ completionPrefix WRITE setCompletionPrefix)
    Q_PROPERTY(ModelSorting modelSorting READ modelSorting WRITE setModelSorting)
    Q_PROPERTY(CompletionMode completionMode READ completionMode WRITE setCompletionMode)
    Q_PROPERTY(int completionColumn READ completionColumn WRITE setCompletionColumn)
    Q_PROPERTY(int completionRole READ completionRole WRITE setCompletionRole)
    Q_PROPERTY(int maxVisibleItems READ maxVisibleItems WRITE setMaxVisibleItems)
    Q_PROPERTY(Qt::CaseSensitivity caseSensitivity READ caseSensitivity WRITE setCaseSensitivity)
    Q_PROPERTY(bool substringCompletion READ substringCompletion WRITE setSubstringCompletion)
    Q_PROPERTY(bool wrapAround READ wrapAround WRITE setWrapAround)

public:
    enum CompletionMode {
        PopupCompletion,
        UnfilteredPopupCompletion,
        InlineCompletion
    };

    enum ModelSorting {
        UnsortedModel = 0,
        CaseSensitivelySortedModel,
        CaseInsensitivelySortedModel
    };

    KexiCompleter(QObject *parent = 0);
    KexiCompleter(QAbstractItemModel *model, QObject *parent = 0);
#ifndef QT_NO_STRINGLISTMODEL
    KexiCompleter(const QStringList& completions, QObject *parent = 0);
#endif
    ~KexiCompleter();

    void setWidget(QWidget *widget);
    QWidget *widget() const;

    void setModel(QAbstractItemModel *c);
    QAbstractItemModel *model() const;

    void setCompletionMode(CompletionMode mode);
    CompletionMode completionMode() const;

    QAbstractItemView *popup() const;
    void setPopup(QAbstractItemView *popup);

    void setCaseSensitivity(Qt::CaseSensitivity caseSensitivity);
    Qt::CaseSensitivity caseSensitivity() const;

    void setSubstringCompletion(bool substringCompletion);
    bool substringCompletion() const;

    void setModelSorting(ModelSorting sorting);
    ModelSorting modelSorting() const;

    void setCompletionColumn(int column);
    int  completionColumn() const;

    void setCompletionRole(int role);
    int  completionRole() const;

    bool wrapAround() const;

    int maxVisibleItems() const;
    void setMaxVisibleItems(int maxItems);

    int completionCount() const;
    bool setCurrentRow(int row);
    int currentRow() const;

    QModelIndex currentIndex() const;
    QString currentCompletion() const;

    QAbstractItemModel *completionModel() const;

    QString completionPrefix() const;

public Q_SLOTS:
    void setCompletionPrefix(const QString &prefix);
    void complete(const QRect& rect = QRect());
    void setWrapAround(bool wrap);

public:
    virtual QString pathFromIndex(const QModelIndex &index) const;
    virtual QStringList splitPath(const QString &path) const;

protected:
    bool eventFilter(QObject *o, QEvent *e);
    bool event(QEvent *);

Q_SIGNALS:
    void activated(const QString &text);
    void activated(const QModelIndex &index);
    void highlighted(const QString &text);
    void highlighted(const QModelIndex &index);

private:
    Q_DISABLE_COPY(KexiCompleter)

    friend class KexiCompleterPrivate;
    KexiCompleterPrivate * const d;

private Q_SLOTS:
    void _q_complete(const QModelIndex&);
    void _q_completionSelected(const QItemSelection&);
    void _q_autoResizePopup();
    void _q_fileSystemModelDirectoryLoaded(const QString&);
};

#endif // QT_NO_COMPLETER

#endif // KEXICOMPLETER_H
