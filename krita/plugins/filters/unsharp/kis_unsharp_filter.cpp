/*
 * This file is part of Krita
 *
 * Copyright (c) 2006 Cyrille Berger <cberger@cberger.net>
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

#include "kis_unsharp_filter.h"
#include <QBitArray>

#include <kundo2command.h>

#include <kcombobox.h>
#include <knuminput.h>

#include <kis_mask_generator.h>
#include <kis_convolution_kernel.h>
#include <kis_convolution_painter.h>
#include <filter/kis_filter_configuration.h>
#include <kis_processing_information.h>
#include <KoProgressUpdater.h>
#include <KoUpdater.h>
#include <kis_paint_device.h>

#include "kis_wdg_unsharp.h"
#include "ui_wdgunsharp.h"
#include <kis_iterator_ng.h>

KisUnsharpFilter::KisUnsharpFilter() : KisFilter(id(), categoryEnhance(), i18n("&Unsharp Mask..."))
{
    setSupportsPainting(true);
    setSupportsIncrementalPainting(false);
    setSupportsAdjustmentLayers(false);
    setColorSpaceIndependence(FULLY_INDEPENDENT);
}

KisConfigWidget * KisUnsharpFilter::createConfigurationWidget(QWidget* parent, const KisPaintDeviceSP) const
{
    return new KisWdgUnsharp(parent);
}

KisFilterConfiguration* KisUnsharpFilter::factoryConfiguration(const KisPaintDeviceSP) const
{
    KisFilterConfiguration* config = new KisFilterConfiguration(id().id(), 1);
    config->setProperty("halfSize", 5);
    config->setProperty("amount", 0.5);
    config->setProperty("threshold", 10);
    return config;
}

void KisUnsharpFilter::process(KisPaintDeviceSP device,
                              const QRect& applyRect,
                              const KisFilterConfiguration* config,
                              KoUpdater* progressUpdater
                              ) const
{

    QPointer<KoUpdater> filterUpdater = 0;
    QPointer<KoUpdater> convolutionUpdater = 0;
    KoProgressUpdater* updater = 0;

    if (progressUpdater) {
        updater = new KoProgressUpdater(progressUpdater);
        updater->start();
        // Two sub-sub tasks that each go from 0 to 100.
        convolutionUpdater = updater->startSubtask();
        filterUpdater = updater->startSubtask();
    }

    if (!config) config = new KisFilterConfiguration(id().id(), 1);

    QVariant value;
    uint halfSize = (config->getProperty("halfSize", value)) ? value.toUInt() : 5;
    uint brushsize = 2 * halfSize + 1;
    double amount = (config->getProperty("amount", value)) ? value.toDouble() : 0.5;
    uint threshold = (config->getProperty("threshold", value)) ? value.toUInt() : 10;

    KisCircleMaskGenerator* kas = new KisCircleMaskGenerator(brushsize, 1, halfSize, halfSize, 2);

    KisConvolutionKernelSP kernel = KisConvolutionKernel::fromMaskGenerator(kas);

    KisPaintDeviceSP interm = new KisPaintDevice(*device);
    const KoColorSpace * cs = interm->colorSpace();
    KoConvolutionOp * convolutionOp = cs->convolutionOp();

    KisConvolutionPainter painter(interm);   // TODO no need for a full copy and then a transaction
    if (progressUpdater) {
        painter.setProgress(convolutionUpdater);
    }
    QBitArray channelFlags = config->channelFlags();
    if (channelFlags.isEmpty()) {
        channelFlags = cs->channelFlags();
    }
    painter.setChannelFlags(channelFlags);
    painter.beginTransaction("convolution step");
    painter.applyMatrix(kernel, interm, applyRect.topLeft(), applyRect.topLeft(), applyRect.size(), BORDER_REPEAT);
    painter.deleteTransaction();

    if (progressUpdater && progressUpdater->interrupted()) {
        return;
    }

    KisHLineIteratorSP dstIt = device->createHLineIteratorNG(applyRect.x(), applyRect.y(), applyRect.width());
    KisHLineConstIteratorSP intermIt = interm->createHLineConstIteratorNG(applyRect.x(), applyRect.y(), applyRect.width());

    int cdepth = cs -> pixelSize();
    quint8 *colors[2];
    colors[0] = new quint8[cdepth];
    colors[1] = new quint8[cdepth];

    int pixelsProcessed = 0;
    qreal weights[2];
    qreal factor = 128;

    // XXX: Added static cast to avoid warning
    weights[0] = static_cast<qreal>(factor * (1. + amount));
    weights[1] = static_cast<qreal>(-factor * amount);

    int steps = 100 / applyRect.width() * applyRect.height();

    for (int j = 0; j < applyRect.height(); j++) {
        do {
            quint8 diff = cs->difference(dstIt->oldRawData(), intermIt->oldRawData());
            if (diff > threshold) {
                memcpy(colors[0], dstIt->oldRawData(), cdepth);
                memcpy(colors[1], intermIt->oldRawData(), cdepth);
                convolutionOp->convolveColors(colors, weights, dstIt->rawData(), factor, 0, 2.0, channelFlags);
            } else {
                memcpy(dstIt->rawData(), dstIt->oldRawData(), cdepth);
            }
            ++pixelsProcessed;
            if (progressUpdater) filterUpdater->setProgress(steps * pixelsProcessed);
            intermIt->nextPixel();
        } while (dstIt->nextPixel());

        if (progressUpdater && progressUpdater->interrupted()) {
            return;
        }
        dstIt->nextRow();
        intermIt->nextRow();
    }
    delete colors[0];
    delete colors[1];
    delete updater;

    if (progressUpdater) progressUpdater->setProgress(100);
}

int KisUnsharpFilter::overlapMarginNeeded(const KisFilterConfiguration* _config) const
{
    QVariant value;
    return (_config->getProperty("halfSize", value)) ? value.toUInt() : 5;
}
