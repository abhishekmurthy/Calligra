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

#include "kra/kis_kra_load_visitor.h"
#include "kis_kra_tags.h"


#include <QRect>

#include <KoColorSpaceRegistry.h>
#include <colorprofiles/KoIccColorProfile.h>
#include <KoStore.h>
#include <KoColorSpace.h>

// kritaimage
#include <kis_types.h>
#include <kis_node_visitor.h>
#include <kis_image.h>
#include <kis_selection.h>
#include <kis_layer.h>
#include <kis_transparency_mask.h>
#include <kis_paint_layer.h>
#include <kis_group_layer.h>
#include <kis_adjustment_layer.h>
#include <filter/kis_filter_configuration.h>
#include <kis_datamanager.h>
#include <generator/kis_generator_layer.h>
#include <kis_pixel_selection.h>

using namespace KRA;

KisKraLoadVisitor::KisKraLoadVisitor(KisImageSP img, KoStore *store, QMap<KisNode *, QString> &layerFilenames, const QString & name) :
        KisNodeVisitor(),
        m_layerFilenames(layerFilenames)
{
    m_external = false;
    m_img = img;
    m_store = store;
    m_name = name;
}

void KisKraLoadVisitor::setExternalUri(const QString &uri)
{
    m_external = true;
    m_uri = uri;
}

bool KisKraLoadVisitor::visit(KisExternalLayer *)
{
    return true;
}

bool KisKraLoadVisitor::visit(KisPaintLayer *layer)
{

    if ( !loadPaintDevice( layer->paintDevice(), getLocation( layer ) ) ) {
         return false;
    }
    if ( !loadProfile( layer->paintDevice(), getLocation( layer, DOT_ICC ) ) ) {
        return false;
    }

    // Check whether there is a file with a .mask extension in the
    // layer directory, if so, it's an old-style transparency mask
    // that should be converted.
    QString location = getLocation( layer, ".mask" );

    if ( m_store->open( location ) ) {

        KisSelectionSP selection = KisSelectionSP(new KisSelection());
        KisPixelSelectionSP pixelSelection = selection->getOrCreatePixelSelection();
        if (!pixelSelection->read(m_store)) {
            pixelSelection->disconnect();
        }
        else {
            KisTransparencyMask* mask = new KisTransparencyMask();
            mask->setSelection( selection );
            m_img->addNode(mask, layer, layer->firstChild());
        }
        m_store->close();
    }

    layer->setDirty(m_img->bounds());
    return true;

}

bool KisKraLoadVisitor::visit(KisGroupLayer *layer)
{
    bool result = visitAll(layer);

    layer->setDirty(m_img->bounds());
    return result;
}

bool KisKraLoadVisitor::visit(KisAdjustmentLayer* layer)
{
    //connect(*layer->paintDevice(), SIGNAL(ioProgress(qint8)), m_img, SLOT(slotIOProgress(qint8)));

    // The selection -- if present. If not, we simply cannot open the dratted thing.
    QString location = m_external ? QString::null : m_uri;
    location += m_name + "/layers/" + m_layerFilenames[layer] + ".selection";
    if (m_store->hasFile(location)) {
        if (m_store->open(location)) {
            KisSelectionSP selection = KisSelectionSP(new KisSelection());
            if (!selection->read(m_store)) {
                selection->disconnect();
                m_store->close();
            } else {
                layer->setSelection(selection);
            }
            m_store->close();
        }
    }

    // filter configuration
    location = m_external ? QString::null : m_uri;
    location += m_name + "/layers/" + m_layerFilenames[layer] + ".filterconfig";

    if (m_store->hasFile(location) && layer->filter()) {
        QByteArray data;
        m_store->open(location);
        data = m_store->read(m_store->size());
        m_store->close();
        if (!data.isEmpty()) {
            KisFilterConfiguration * kfc = layer->filter();
            kfc->fromLegacyXML(QString(data));
        }
    }

    layer->setDirty(m_img->bounds());
    return true;

}

bool KisKraLoadVisitor::visit(KisGeneratorLayer* layer)
{
    QString location = m_external ? QString::null : m_uri;
    location += m_name + "/layers/" + m_layerFilenames[layer];

    // Layer data
    if (m_store->open(location)) {
        if (!layer->paintDevice()->read(m_store)) {
            layer->paintDevice()->disconnect();
            m_store->close();
            //IODone();
            return false;
        }

        m_store->close();
    } else {
        kError() << "No image data: that's an error!";
        return false;
    }

    // icc profile
    location = m_external ? QString::null : m_uri;
    location += m_name + "/layers/" + m_layerFilenames[layer] + ".icc";

    if (m_store->hasFile(location)) {
        QByteArray data;
        m_store->open(location);
        data = m_store->read(m_store->size());
        m_store->close();
        // Create a colorspace with the embedded profile
        const KoColorSpace * cs = KoColorSpaceRegistry::instance()->colorSpace(layer->paintDevice()->colorSpace()->id(), new KoIccColorProfile(data));
        // replace the old colorspace
        layer->paintDevice()->setDataManager(layer->paintDevice()->dataManager(), cs);

    }

    location = m_external ? QString::null : m_uri;
    location += m_name + "/layers/" + m_layerFilenames[layer] + ".selection";
    if (m_store->hasFile(location)) {
        m_store->open(location);
        KisSelectionSP selection = KisSelectionSP(new KisSelection());
        if (!selection->read(m_store)) {
            selection->disconnect();
            m_store->close();
        } else {
            layer->setSelection(selection);
        }
        m_store->close();

    }

    // filter configuration
    location = m_external ? QString::null : m_uri;
    location += m_name + "/layers/" + m_layerFilenames[layer] + ".generatorconfig";

    if (m_store->hasFile(location) && layer->generator()) {
        QByteArray data;
        m_store->open(location);
        data = m_store->read(m_store->size());
        m_store->close();
        if (!data.isEmpty()) {
            KisFilterConfiguration * kfc = layer->generator();
            kfc->fromLegacyXML(QString(data));
        }
    }

    layer->setDirty(m_img->bounds());
    return true;
}


bool KisKraLoadVisitor::visit(KisCloneLayer *layer)
{
    return true;
}

bool KisKraLoadVisitor::visit(KisFilterMask *mask)
{
    return true;
}

bool KisKraLoadVisitor::visit(KisTransparencyMask *mask)
{
    return true;
}

bool KisKraLoadVisitor::visit(KisTransformationMask *mask)
{
    return true;
}

bool KisKraLoadVisitor::visit(KisSelectionMask *mask)
{
    return true;
}

QString KisKraLoadVisitor::getLocation( KisNode* node, const QString& suffix )
{
    QString location = m_external ? QString::null : m_uri;
    location += m_name + "/" + LAYERS + "/" + m_layerFilenames[node] + suffix;


    return location;
}

bool KisKraLoadVisitor::loadPaintDevice( KisPaintDeviceSP device, const QString& location )
{
    //connect(*device, SIGNAL(ioProgress(qint8)), m_img, SLOT(slotIOProgress(qint8)));

    // Layer data
    if (m_store->open(location)) {
        if (!device->read(m_store)) {
            device->disconnect();
            m_store->close();
            //IODone();
            return false;
        }

        m_store->close();
    } else {
        kError() << "No image data: that's an error!";
        return false;
    }
}


bool KisKraLoadVisitor::loadProfile( KisPaintDeviceSP device, const QString& location )
{

    if (m_store->hasFile(location)) {
        QByteArray data;
        m_store->open(location);
        data = m_store->read(m_store->size());
        m_store->close();
        // Create a colorspace with the embedded profile
        const KoColorSpace * cs =
            KoColorSpaceRegistry::instance()->colorSpace(device->colorSpace()->id(),
                                                         new KoIccColorProfile(data));
        // replace the old colorspace
        device->setDataManager(device->dataManager(), cs);

    }


}


bool KisKraLoadVisitor::loadFilterConfiguration()
{
}

bool KisKraLoadVisitor::loadSelection()
{
}
