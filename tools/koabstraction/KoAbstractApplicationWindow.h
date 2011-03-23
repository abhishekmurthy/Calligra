/* This file is part of the KDE project
 * Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
 * Copyright (C) 2010 Boudewijn Rempt <boud@kogmbh.com>
 * Copyright (C) 2010-2011 Jarosław Staniek <staniek@kde.org>
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

#ifndef KOABSTRACTAPPLICATION_H
#define KOABSTRACTAPPLICATION_H

#include "koabstraction_export.h"

class KoAbstractApplicationController;

//! Class for gluing KoAbstractApplicationController with the custom application main window.
/*!
 * It should be inherited by the application main window class.
 */
class KOABSTRACTION_EXPORT KoAbstractApplicationWindow
{
public:
    /*!
     * Constructor, takes mandatory controller object implementation @a controller 
     * which is then owned by the abstract application and available through
     * @ref controller().
     */
    KoAbstractApplicationWindow(KoAbstractApplicationController *controller);

    virtual ~KoAbstractApplicationWindow();

    /*!
     * @return controller the application's controller implementing fundamental features.
     */
    KoAbstractApplicationController* controller() const;

private:
    class Private;
    Private * const d;
};

#endif