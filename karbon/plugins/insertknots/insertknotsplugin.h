/* This file is part of the KDE project
   Copyright (C) 2002-2003,2005 Rob Buis <buis@kde.org>
   Copyright (C) 2005 Thomas Zander <zander@kde.org>
   Copyright (C) 2006 Tim Beaulen <tbscope@gmail.com>
   Copyright (C) 2007 Jan Hambrecht <jaham@gmx.net>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef INSERTKNOTSPLUGIN_H
#define INSERTKNOTSPLUGIN_H

#include <kparts/plugin.h>
#include <kdialog.h>

class InsertKnotsDlg;

class InsertKnotsPlugin : public KParts::Plugin
{
    Q_OBJECT
public:
    InsertKnotsPlugin( QWidget *parent, const QStringList & );
    virtual ~InsertKnotsPlugin() {}

private slots:
    void slotInsertKnots();

private:
    InsertKnotsDlg * m_insertKnotsDlg;
};

class KIntSpinBox;

class InsertKnotsDlg : public KDialog
{
    Q_OBJECT

public:
    explicit InsertKnotsDlg( QWidget* parent = 0L, const char* name = 0L );

    uint knots() const;
    void setKnots( uint value );

private:
    KIntSpinBox * m_knots;
};

#endif

