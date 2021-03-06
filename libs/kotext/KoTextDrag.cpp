/* This file is part of the KDE project
 * Copyright (C) 2007-2008 Thorsten Zachmann <zachmann@kde.org>
 * Copyright (C) 2008 Pierre Stirnweiss \pierre.stirnweiss_calligra@gadz.org>
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

#include "KoTextDrag.h"
#include <QApplication>
#include <QBuffer>
#include <QByteArray>
#include <QClipboard>
#include <QMimeData>
#include <QString>

#include <kdebug.h>

#include <KoStore.h>
#include <KoGenStyles.h>
#include <KoGenChanges.h>
#include <KoOdfWriteStore.h>
#include <KoXmlWriter.h>
#include <KoOdfDocument.h>
#include <KoEmbeddedDocumentSaver.h>
#include "KoShapeSavingContext.h"
#include "KoStyleManager.h"
#include <opendocument/KoTextSharedSavingData.h>

#include "KoTextOdfSaveHelper.h"

#ifdef SHOULD_BUILD_RDF
#include "KoTextRdfCore.h"
#endif

KoTextDrag::KoTextDrag()
        : m_mimeData(0)
{
}

KoTextDrag::~KoTextDrag()
{
    if (m_mimeData == 0) {
        delete m_mimeData;
    }
}

bool KoTextDrag::setOdf(const char * mimeType, KoTextOdfSaveHelper &helper)
{
    struct Finally {
        Finally(KoStore *s) : store(s) { }
        ~Finally() {
            delete store;
        }
        KoStore *store;
    };

    QBuffer buffer;
    KoStore* store = KoStore::createStore(&buffer, KoStore::Write, mimeType);
    Finally finally(store); // delete store when we exit this scope
    Q_ASSERT(store);
    Q_ASSERT(!store->bad());

    KoOdfWriteStore odfStore(store);
    KoEmbeddedDocumentSaver embeddedSaver;

    KoXmlWriter* manifestWriter = odfStore.manifestWriter(mimeType);
    KoXmlWriter* contentWriter = odfStore.contentWriter();

    if (!contentWriter) {
        return false;
    }

    KoGenStyles mainStyles;
    KoXmlWriter *bodyWriter = odfStore.bodyWriter();
    KoShapeSavingContext * context = helper.context(bodyWriter, mainStyles, embeddedSaver);
    KoGenChanges changes;

    KoSharedSavingData *sharedData = context->sharedData(KOTEXT_SHARED_SAVING_ID);
    KoTextSharedSavingData *textSharedData = 0;
    if (sharedData) {
        textSharedData = dynamic_cast<KoTextSharedSavingData *>(sharedData);
    }

    if (!textSharedData) {
        textSharedData = new KoTextSharedSavingData();
        textSharedData->setGenChanges(changes);
        if (!sharedData) {
            context->addSharedData(KOTEXT_SHARED_SAVING_ID, textSharedData);
        } else {
            kWarning(32500) << "A different type of sharedData was found under the" << KOTEXT_SHARED_SAVING_ID;
            Q_ASSERT(false);
        }
    }
#if SHOULD_BUILD_RDF    
    kDebug(30015) << "helper.model:" << helper.rdfModel();
    textSharedData->setRdfModel(helper.rdfModel());
#endif
    if (!helper.writeBody()) {
        return false;
    }
    // Save the named styles that was referred to by the copied text
    if (KoStyleManager *styleManager = helper.styleManager()) {
        styleManager->saveReferredStylesToOdf(*context);
    }

    mainStyles.saveOdfStyles(KoGenStyles::DocumentAutomaticStyles, contentWriter);
    changes.saveOdfChanges(contentWriter, false);

    odfStore.closeContentWriter();

    //add manifest line for content.xml
    manifestWriter->addManifestEntry("content.xml", "text/xml");

    kDebug(30015) << "testing to see if we should add rdf to odf file?";

#ifdef SHOULD_BUILD_RDF
    kDebug(30015) << "helper has model" << ( helper.rdfModel() != 0 );
    // RDF: Copy relevant RDF to output ODF
    if (QSharedPointer<Soprano::Model> m = helper.rdfModel()) {
        kDebug(30015) << "rdf model size:" << m->statementCount();
        KoTextRdfCore::createAndSaveManifest(m, textSharedData->getRdfIdMapping(),
                                             store, manifestWriter);
    }
#endif

    if (!mainStyles.saveOdfStylesDotXml(store, manifestWriter)) {
        return false;
    }

    if (!context->saveDataCenter(store, manifestWriter)) {
        kDebug(32500) << "save data centers failed";
        return false;
    }

    // Save embedded objects
    KoOdfDocument::SavingContext documentContext(odfStore, embeddedSaver);
    if (!embeddedSaver.saveEmbeddedDocuments(documentContext)) {
        kDebug(32500) << "save embedded documents failed";
        return false;
    }

    // Write out manifest file
    if (!odfStore.closeManifestWriter()) {
        return false;
    }

    delete store; // make sure the buffer if fully flushed.
    finally.store = 0;
    setData(mimeType, buffer.buffer());

    return true;
}

void KoTextDrag::setData(const QString & mimeType, const QByteArray & data)
{
    if (m_mimeData == 0) {
        m_mimeData = new QMimeData();
    }
    m_mimeData->setData(mimeType, data);
}

QMimeData * KoTextDrag::mimeData()
{
    QMimeData * mimeData = m_mimeData;
    m_mimeData = 0;
    return mimeData;
}
