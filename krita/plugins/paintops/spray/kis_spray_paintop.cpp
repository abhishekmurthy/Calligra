/*
 *  Copyright (c) 2008,2009,2010 Lukáš Tvrdý <lukast.dev@gmail.com>
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

#include "kis_spray_paintop.h"
#include "kis_spray_paintop_settings.h"

#include <cmath>

#include <QRect>
#include <kis_global.h>
#include <kis_paint_device.h>
#include <kis_painter.h>
#include <kis_types.h>
#include <kis_paintop.h>

#include <kis_pressure_rotation_option.h>
#include <kis_pressure_opacity_option.h>
#include <kis_pressure_size_option.h>

#ifdef BENCHMARK
#include <QTime>
#endif


KisSprayPaintOp::KisSprayPaintOp(const KisSprayPaintOpSettings *settings, KisPainter * painter, KisImageWSP image)
        : KisPaintOp(painter)
        , m_settings(settings)
        , m_image(image)
{
    Q_ASSERT(settings);
    Q_ASSERT(painter);

    m_rotationOption.readOptionSetting(settings);
    m_opacityOption.readOptionSetting(settings);
    m_sizeOption.readOptionSetting(settings);
    m_rotationOption.sensor()->reset();
    m_opacityOption.sensor()->reset();
    m_sizeOption.sensor()->reset();

    m_settings->debug();
    
    
    m_sprayBrush.setDiameter(settings->diameter());
    m_sprayBrush.setJitterShapeSize(settings->randomSize());

    if (settings->proportional()) {
        m_sprayBrush.setObjectDimension(settings->width()  / 100.0 * settings->diameter() * settings->scale(),
                                        settings->height() / 100.0 * settings->diameter() * settings->aspect() * settings->scale());
    } else {
        m_sprayBrush.setObjectDimension(settings->width(), settings->height());
    }

    m_sprayBrush.setAmount(settings->amount());
    m_sprayBrush.setScale(settings->scale());

    m_sprayBrush.setCoverity(settings->coverage() / 100.0);
    m_sprayBrush.setUseDensity(settings->useDensity());
    m_sprayBrush.setParticleCount(settings->particleCount());


    // spacing
    if ((settings->diameter() * 0.5) > 1) {
        m_ySpacing = m_xSpacing = settings->diameter() * 0.5 * settings->spacing();
    } else {
        m_ySpacing = m_xSpacing = 1.0;
    }
    m_spacing = m_xSpacing;

    m_sprayBrush.setUseRandomOpacity(settings->useRandomOpacity());
    m_sprayBrush.setSettingsObject(m_settings);

    m_sprayBrush.init();
    
#ifdef BENCHMARK
    m_count = m_total = 0;
#endif

}

KisSprayPaintOp::~KisSprayPaintOp()
{
}

double KisSprayPaintOp::spacing(double & xSpacing, double & ySpacing, double pressure1, double pressure2) const
{
    Q_UNUSED(pressure1);
    Q_UNUSED(pressure2);
    xSpacing = m_xSpacing;
    ySpacing = m_ySpacing;

    return m_spacing;
}


void KisSprayPaintOp::paintAt(const KisPaintInformation& info)
{
#ifdef BENCHMARK
    QTime time;
    time.start();
#endif

    if (!painter()) return;

    if (!m_dab) {
        m_dab = new KisPaintDevice(painter()->device()->colorSpace());
    } else {
        m_dab->clear();
    }

    double rotation = m_rotationOption.apply(info);
    quint8 origOpacity = m_opacityOption.apply(painter(), info);
    double scale = KisPaintOp::scaleForPressure(m_sizeOption.apply(info));
    
    m_sprayBrush.paint( m_dab,
                        m_settings->node()->paintDevice(), 
                        info, 
                        rotation,
                        scale,
                        painter()->paintColor(), 
                        painter()->backgroundColor());

    QRect rc = m_dab->extent();
    painter()->bitBlt(rc.topLeft(), m_dab, rc);
    painter()->setOpacity(origOpacity);

#ifdef BENCHMARK
    int msec = time.elapsed();
    kDebug() << msec << " ms/dab " << "[average: " << m_total / (qreal)m_count << "]";
    m_total += msec;
    m_count++;
#endif
}

