/*
 *  Copyright (c) 2002 Patrick Julien <freak@codepimps.org>
 *  Copyright (c) 2005 Casper Boemann <cbr@boemann.dk>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "kis_layer.h"


#include <kicon.h>
#include <klocale.h>
#include <QIcon>
#include <QImage>
#include <QBitArray>

#include <KoProperties.h>
#include <KoCompositeOp.h>
#include <KoColorSpace.h>

#include "kis_debug.h"
#include "kis_group_layer.h"
#include "kis_image.h"

#include "kis_painter.h"
#include "kis_mask.h"
#include "kis_effect_mask.h"
#include "kis_transparency_mask.h"
#include "kis_selection_mask.h"
#include "kis_meta_data_store.h"
#include "kis_selection.h"

class KisLayer::Private
{

public:

    KisImageWSP image;
    QBitArray channelFlags;
    const KoCompositeOp * compositeOp;
    KisEffectMaskSP previewMask;
    KisMetaData::Store* metaDataStore;
};


KisLayer::KisLayer(KisImageWSP img, const QString &name, quint8 opacity)
        : KisNode()
        , m_d(new Private)
{
//     Q_ASSERT( img );
    setName(name);
    setOpacity(opacity);
    m_d->image = img;
    if (m_d->image)
        m_d->compositeOp = const_cast<KoCompositeOp*>(img->colorSpace()->compositeOp(COMPOSITE_OVER));
    else
        m_d->compositeOp = 0;
    m_d->metaDataStore = new KisMetaData::Store();
}

KisLayer::KisLayer(const KisLayer& rhs)
        : KisNode(rhs)
        , m_d(new Private())
{
    if (this != &rhs) {
        m_d->image = rhs.m_d->image;
        m_d->compositeOp = rhs.m_d->compositeOp;
        m_d->metaDataStore = new KisMetaData::Store(*rhs.m_d->metaDataStore);
    }
}

KisLayer::~KisLayer()
{
    delete m_d->metaDataStore;
    delete m_d;
}

const KoColorSpace * KisLayer::colorSpace() const
{
    if (m_d->image)
        return m_d->image->colorSpace();
    return 0;
}

KoDocumentSectionModel::PropertyList KisLayer::sectionModelProperties() const
{
    KoDocumentSectionModel::PropertyList l = KisBaseNode::sectionModelProperties();
    l << KoDocumentSectionModel::Property(i18n("Opacity"), i18n("%1%", percentOpacity()));
    if (compositeOp())
        l << KoDocumentSectionModel::Property(i18n("Composite Mode"), compositeOp()->description());
    return l;
}

void KisLayer::setSectionModelProperties(const KoDocumentSectionModel::PropertyList &properties)
{
    KisBaseNode::setSectionModelProperties(properties);
    /// TODO no nope not at all, the state contains a use-visible string not the actual property
//     setOpacity( properties.at( 2 ).state.toInt() );
//     setCompositeOp( const_cast<KoCompositeOp*>( image()->colorSpace()->compositeOp( properties.at( 3 ).state.toString() ) ) );
}

KisPaintDeviceSP KisLayer::original() const
{
    return projection();
}


void KisLayer::setChannelFlags(const QBitArray & channelFlags)
{
    Q_ASSERT(((quint32)channelFlags.count() == colorSpace()->channelCount() || channelFlags.isEmpty()));
    m_d->channelFlags = channelFlags;
}

QBitArray & KisLayer::channelFlags() const
{
    return m_d->channelFlags;
}

quint8 KisLayer::opacity() const
{
    return nodeProperties().intProperty("opacity", OPACITY_OPAQUE);
}

void KisLayer::setOpacity(quint8 val)
{
    if (opacity() != val) {
        nodeProperties().setProperty("opacity", val);
    }
}

quint8 KisLayer::percentOpacity() const
{
    return int(float(opacity() * 100) / 255 + 0.5);
}

void KisLayer::setPercentOpacity(quint8 val)
{
    setOpacity(int(float(val * 255) / 100 + 0.5));
}

bool KisLayer::temporary() const
{
    return nodeProperties().boolProperty("temporary", false);
}

void KisLayer::setTemporary(bool t)
{
    nodeProperties().setProperty("temporary", t);
}

const KoCompositeOp * KisLayer::compositeOp() const
{
    //dbgImage << m_d->compositeOp->description();
    return m_d->compositeOp;
}

void KisLayer::setCompositeOp(const KoCompositeOp* compositeOp)
{
    //dbgImage << "old: " <<  m_d->compositeOp->description() << ", new: " << compositeOp->description();
    if (m_d->compositeOp != compositeOp) {
        m_d->compositeOp = const_cast<KoCompositeOp*>(compositeOp);
    }
}

KisImageSP KisLayer::image() const
{
    return m_d->image;
}

void KisLayer::setImage(KisImageSP image)
{
    m_d->image = image;
    for (uint i = 0; i < childCount(); ++i) {
        // Only layers know about the image
        KisLayer * layer = dynamic_cast<KisLayer*>(at(i).data());
        if (layer)
            layer->setImage(image);
    }
}

KisSelectionMaskSP KisLayer::selectionMask() const
{
    QList<KisNodeSP> masks = childNodes(QStringList("KisSelectionMask"), KoProperties());
    Q_ASSERT(masks.size() <= 1); // Or do we allow more than one selection mask to a layer?
    if (masks.size() == 1) {
        KisSelectionMaskSP selection = dynamic_cast<KisSelectionMask*>(masks[0].data());
        return selection;
    }
    return 0;
}

KisSelectionSP KisLayer::selection() const
{
    KisSelectionMaskSP selMask = selectionMask();
    if (selMask && selMask->active())
        return selMask->selection();
    else if (m_d->image)
        return m_d->image->globalSelection();
    else
        return 0;
}

bool KisLayer::hasEffectMasks() const
{
    if (m_d->previewMask) return true;

    QList<KisNodeSP> masks = childNodes(QStringList("KisEffectMask"), KoProperties());
    if (!masks.isEmpty()) return true;

    return false;
}

void KisLayer::applyEffectMasks(const KisPaintDeviceSP projection, const QRect & rc)
{
    if (m_d->previewMask) {
        m_d->previewMask->apply(projection, rc);
    }

    KoProperties props;
    props.setProperty("visible", true);

    QList<KisNodeSP> masks = childNodes(QStringList("KisEffectMask"), props);

    // Then loop through the effect masks and apply them
    for (int i = 0; i < masks.size(); ++i) {

        const KisEffectMask * effectMask = dynamic_cast<const KisEffectMask*>(masks.at(i).data());

        if (effectMask) {
            dbgImage << " layer " << name() << " has effect mask " << effectMask->name();
            effectMask->apply(projection, rc);
        }
    }

}


QList<KisMaskSP> KisLayer::effectMasks() const
{
    QList<KisNodeSP> nodes = childNodes(QStringList("KisEffectMask"), KoProperties());
    QList<KisMaskSP> masks;
    foreach(KisNodeSP node,  nodes) {
        KisMaskSP mask = dynamic_cast<KisMask*>(node.data());
        if (mask)
            masks.append(mask);
    }
    return masks;

}

void KisLayer::setPreviewMask(KisEffectMaskSP mask)
{
    m_d->previewMask = mask;
    m_d->previewMask->setParent(this);
}

KisEffectMaskSP KisLayer::previewMask() const
{
    return m_d->previewMask;
}

void KisLayer::removePreviewMask()
{
    m_d->previewMask = 0;
}

KisLayerSP KisLayer::parentLayer() const
{
    return dynamic_cast<KisLayer*>(parent().data());
}

KisMetaData::Store* KisLayer::metaData()
{
    return m_d->metaDataStore;
}

struct KisIndirectPaintingSupport::Private {
    // To simulate the indirect painting
    KisPaintDeviceSP temporaryTarget;
    const KoCompositeOp* compositeOp;
    quint8 compositeOpacity;
};

void KisIndirectPaintingSupport::setTemporaryTarget(KisPaintDeviceSP t)
{
    d->temporaryTarget = t;
}

void KisIndirectPaintingSupport::setTemporaryCompositeOp(const KoCompositeOp* c)
{
    d->compositeOp = c;
}

void KisIndirectPaintingSupport::setTemporaryOpacity(quint8 o)
{
    d->compositeOpacity = o;
}

KisPaintDeviceSP KisIndirectPaintingSupport::temporaryTarget()
{
    return d->temporaryTarget;
}

const KoCompositeOp* KisIndirectPaintingSupport::temporaryCompositeOp() const
{
    return d->compositeOp;
}

quint8 KisIndirectPaintingSupport::temporaryOpacity() const
{
    return d->compositeOpacity;
}

bool KisIndirectPaintingSupport::hasTemporaryTarget() const
{
    return d->temporaryTarget;
}


KisIndirectPaintingSupport::KisIndirectPaintingSupport() : d(new Private)
{
    d->compositeOp = 0;
}
KisIndirectPaintingSupport::~KisIndirectPaintingSupport()
{
    delete d;
}

#include "kis_layer.moc"
