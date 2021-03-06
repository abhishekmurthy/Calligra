/*
 *  Copyright (c) 2009 Boudewijn Rempt <boud@valdyas.org>
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
#ifndef KIS_MYPAINT_PAINTOP_SETTINGS_H_
#define KIS_MYPAINT_PAINTOP_SETTINGS_H_

#include <QObject>
#include <QList>
#include <kis_paintop_settings.h>
#include <kis_types.h>

#include "mypaint_paintop_settings_widget.h"

class QWidget;
class QDomElement;
class QDomDocument;

class MyPaintBrushResource;

class MyPaintSettings : public QObject, public KisPaintOpSettings
{
    Q_OBJECT

public:
    MyPaintSettings();
    virtual ~MyPaintSettings() {}

    MyPaintBrushResource* brush() const;

    /// the mypaint brushlib-based paintop always paints in wash-mode, i.e., with an extra temporary layer
    bool paintIncremental() {
        return false;
    }
    
    virtual QPainterPath brushOutline(const QPointF& pos, OutlineMode mode, qreal scale = 1.0, qreal rotation = 0.0) const;

private:
    MyPaintSettingsWidget* m_options;
};
#endif
