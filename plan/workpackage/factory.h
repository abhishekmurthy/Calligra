/* This file is part of the KDE project
   Copyright (C) 1998, 1999, 2000 Torben Weis <weis@kde.org>
   Copyright (C) 2009 Dag Andersen <danders@get2net.dk>

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

#ifndef KPLATOWORK_FACTORY_H
#define KPLATOWORK_FACTORY_H

#include "kplatowork_export.h"

#include <kpluginfactory.h>

class KAboutData;

namespace KPlatoWork
{

class KPLATOWORK_EXPORT Factory : public KPluginFactory
{
    Q_OBJECT
public:
    explicit Factory( QObject* parent = 0 );
    ~Factory();

    virtual QObject* create(const char* iface, QWidget* parentWidget, QObject *parent, const QVariantList& args, const QString& keyword);

    static const KComponentData &global();

    // _Creates_ a KAboutData but doesn't keep ownership
    static KAboutData* aboutData();

private:
    static KComponentData* s_global;
    static KAboutData* s_aboutData;
};

} // KPlatoWork namespace

#endif
