/*
 *  Copyright (c) 2003 Boudewijn Rempt (boud@valdyas.org)
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
#ifndef KIS_STRATEGY_COLORSPACE_CMYK_H_
#define KIS_STRATEGY_COLORSPACE_CMYK_H_

#include <QColor>
#include <QMap>
#include <krita_export.h>
#include "kis_global.h"
#include "KoLcmsColorSpaceTrait.h"
#include "KoU8ColorSpaceTrait.h"

class KRITACOLOR_EXPORT KisCmykColorSpace : public KoU8ColorSpaceTrait, public KoLcmsColorSpaceTrait {

public:


    struct Pixel {
            quint16 cyan;
            quint16 magenta;
            quint16 yellow;
            quint16 black;
            quint16 alpha;
        };
public:
    KisCmykColorSpace(KoColorSpaceFactoryRegistry * parent, KoColorProfile *p);
    virtual ~KisCmykColorSpace();


    virtual bool willDegrade(ColorSpaceIndependence independence)
        {
            if (independence == TO_RGBA8)
                return true;
            else
                return false;
        };



public:

    virtual void mixColors(const quint8 **colors, const quint8 *weights, quint32 nColors, quint8 *dst) const;
    virtual void applyAdjustment(const quint8 *src, quint8 *dst, KoColorAdjustment *adj, qint32 nPixels);
    virtual void invertColor(quint8 * src, qint32 nPixels);
    virtual void convolveColors(quint8** colors, qint32 * kernelValues, KoChannelInfo::enumChannelFlags channelFlags, quint8 *dst, qint32 factor, qint32 offset, qint32 nColors) const;
    // XXX: darken & intensity8?

    virtual Q3ValueVector<KoChannelInfo *> channels() const;
    virtual quint32 nChannels() const;
    virtual quint32 nColorChannels() const;
    virtual quint32 pixelSize() const;
    virtual void getSingleChannelPixel(quint8 *dstPixel, const quint8 *srcPixel, quint32 channelIndex);

    virtual KoCompositeOpList userVisiblecompositeOps() const;

protected:

    virtual void bitBlt(quint8 *dst,
                        qint32 dstRowStride,
                        const quint8 *src,
                        qint32 srcRowStride,
                        const quint8 *srcAlphaMask,
                        qint32 maskRowStride,
                        quint8 opacity,
                        qint32 rows,
                        qint32 cols,
                        const KoCompositeOp& op);

    void compositeOver(quint8 *dst, qint32 dstRowStride, const quint8 *src, qint32 srcRowStride, const quint8 *mask, qint32 maskRowStride, qint32 rows, qint32 columns, quint8 opacity);
    void compositeMultiply(quint8 *dst, qint32 dstRowStride, const quint8 *src, qint32 srcRowStride, const quint8 *mask, qint32 maskRowStride, qint32 rows, qint32 columns, quint8 opacity);
    void compositeDivide(quint8 *dst, qint32 dstRowStride, const quint8 *src, qint32 srcRowStride, const quint8 *mask, qint32 maskRowStride, qint32 rows, qint32 columns, quint8 opacity);
    void compositeScreen(quint8 *dst, qint32 dstRowStride, const quint8 *src, qint32 srcRowStride, const quint8 *mask, qint32 maskRowStride, qint32 rows, qint32 columns, quint8 opacity);
    void compositeOverlay(quint8 *dst, qint32 dstRowStride, const quint8 *src, qint32 srcRowStride, const quint8 *mask, qint32 maskRowStride, qint32 rows, qint32 columns, quint8 opacity);
    void compositeDodge(quint8 *dst, qint32 dstRowStride, const quint8 *src, qint32 srcRowStride, const quint8 *mask, qint32 maskRowStride, qint32 rows, qint32 columns, quint8 opacity);
    void compositeBurn(quint8 *dst, qint32 dstRowStride, const quint8 *src, qint32 srcRowStride, const quint8 *mask, qint32 maskRowStride, qint32 rows, qint32 columns, quint8 opacity);
    void compositeDarken(quint8 *dst, qint32 dstRowStride, const quint8 *src, qint32 srcRowStride, const quint8 *mask, qint32 maskRowStride, qint32 rows, qint32 columns, quint8 opacity);
    void compositeLighten(quint8 *dst, qint32 dstRowStride, const quint8 *src, qint32 srcRowStride, const quint8 *mask, qint32 maskRowStride, qint32 rows, qint32 columns, quint8 opacity);
    void compositeErase(quint8 *dst, qint32 dstRowStride, const quint8 *src, qint32 srcRowStride, const quint8 *mask, qint32 maskRowStride, qint32 rows, qint32 columns, quint8 opacity);

private:

    quint8 * m_qcolordata;

    static const quint8 PIXEL_CYAN = 0;
    static const quint8 PIXEL_MAGENTA = 1;
    static const quint8 PIXEL_YELLOW = 2;
    static const quint8 PIXEL_BLACK = 3;
    static const quint8 PIXEL_CMYK_ALPHA = 4;
};

class KisCmykColorSpaceFactory : public KoColorSpaceFactory
{
public:
    /**
     * Krita definition for use in .kra files and internally: unchanging name +
     * i18n'able description.
     */
    virtual KoID id() const { return KoID("CMYK", i18n("CMYK (8-bit integer/channel)")); };

    /**
     * lcms colorspace type definition.
     */
    virtual quint32 colorSpaceType() { return TYPE_CMYK5_8; };

    virtual icColorSpaceSignature colorSpaceSignature() { return icSigCmykData; };

    virtual KoColorSpace *createColorSpace(KoColorSpaceFactoryRegistry * parent, KoColorProfile *p) { return new KisCmykColorSpace(parent, p); };

    virtual QString defaultProfile() { return "Adobe CMYK"; }; //  Do not i18n -- this is from a data file
};

#endif // KIS_STRATEGY_COLORSPACE_CMYK_H_
