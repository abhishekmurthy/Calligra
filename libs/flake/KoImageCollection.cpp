/* This file is part of the KDE project
 * Copyright (C) 2007, 2009 Thomas Zander <zander@kde.org>
 * Copyright (C) 2008 Thorsten Zachmann <zachmann@kde.org>
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
#include "KoImageCollection.h"
#include "KoImageData.h"
#include "KoImageData_p.h"
#include "KoShapeSavingContext.h"

#include <KoStoreDevice.h>
#include <KoXmlWriter.h>

#include <QMap>
#include <kdebug.h>
#include <kmimetype.h>

class KoImageCollection::Private
{
public:
    ~Private()
    {
        foreach(KoImageDataPrivate *id, images)
            id->collection = 0;
    }

    QMap<QByteArray, KoImageDataPrivate*> images;
    // an extra map to find all dataObjects based on the key of a store.
    QMap<QByteArray, KoImageDataPrivate*> storeImages;
};

KoImageCollection::KoImageCollection()
    : d(new Private())
{
}

KoImageCollection::~KoImageCollection()
{
    delete d;
}

bool KoImageCollection::completeLoading(KoStore *store)
{
    Q_UNUSED( store );
    return true;
}

bool KoImageCollection::completeSaving(KoStore *store, KoXmlWriter * manifestWriter, KoShapeSavingContext * context)
{
/*
    QMap<QByteArray, QString> images(context->imagesToSave());
    QMap<QByteArray, QString>::iterator it(images.begin());

    QMap<QByteArray, KoImageDataPrivate *>::iterator dataIt(d->images.begin());

    while (it != images.end()) {
        if ( dataIt == d->images.end() ) {
            // this should not happen
            kWarning(30006) << "image not found";
            Q_ASSERT(0);
            break;
        }
        else if ( dataIt.key() == it.key() ) {
            if (store->open(it.value())) {
                KoStoreDevice device(store);
                bool ok = dataIt.value()->saveToFile(device);
                store->close();
                // TODO error handling
                if ( ok ) {
                    const QString mimetype(KMimeType::findByPath(it.value(), 0 , true)->name());
                    manifestWriter->addManifestEntry(it.value(), mimetype);
                }
                else {
                    kWarning(30006) << "saving image failed";
                }
            }
            else {
                kWarning(30006) << "saving image failed: open store failed";
            }
            ++dataIt;
            ++it;
        }
        else if ( dataIt.key() < it.key() ) {
            ++dataIt;
        }
        else {
            // this should not happen
            kWarning(30006) << "image not found";
            Q_ASSERT(0);
        }
    }

*/
    return true;
}

KoImageData KoImageCollection::getImage(const QImage &image)
{
    Q_ASSERT(!image.isNull());
    const qint64 key = image.cacheKey();
    QByteArray key2 = QString::number(key).toLatin1();
    if (d->images.contains(key2))
        return d->images.value(key2);
    KoImageData data;
    data.setImage(image);
    data.priv()->collection = this;
    Q_ASSERT(data.key() == key2);
    d->images.insert(key2, data.priv());
    return data;
}

KoImageData KoImageCollection::getImage(const KUrl &url)
{
    Q_ASSERT(!url.isEmpty() && url.isValid());

    QByteArray key = url.toEncoded();
    if (d->images.contains(key))
        return d->images.value(key);
    KoImageData data;
    data.setImage(url);
    data.priv()->collection = this;
    Q_ASSERT(data.key() == key);
    d->images.insert(key, data.priv());
    return data;
}

KoImageData KoImageCollection::getImage(const QString &href, KoStore *store)
{
    // the tricky thing with a 'store' is that we need to read the data now
    // as the store will no longer be readable after the loading completed.
    // The solution we use is to read the data, store it in a QTemporaryFile
    // and read and parse it on demand when the image data is actually needed.
    // This leads to having two keys, one for the store and one for the
    // actual image data. We need the latter so if someone else gets the same
    // image data he can find this data and share (warm fuzzy feeling here)
    QByteArray storeKey = (QString::number((qint64) store) + href).toLatin1();
    if (d->storeImages.contains(storeKey))
        return d->storeImages.value(storeKey);

    KoImageData data;
    data.setImage(href, store);
    data.priv()->collection = this;
    d->storeImages.insert(storeKey, data.priv());
    d->images.insert(data.key(), data.priv());
    return data;
}

int KoImageCollection::size() const
{
    return d->images.count();
}

int KoImageCollection::count() const
{
    return d->images.count();
}

void KoImageCollection::removeOnKey(const QByteArray &imageDataKey)
{
    d->images.remove(imageDataKey);
}
