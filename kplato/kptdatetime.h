/* This file is part of the KDE project
   Copyright (C) 2003 Dag Andersen <danders@get2net.dk>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <qdatetime.h>
#include "kptduration.h"

class KPTDuration;

/**
 * KPTDateTime is a @ref QDateTime which knows about @ref KPTDuration
 */
class KPTDateTime : public QDateTime {

public:
    KPTDateTime();
    KPTDateTime(QDateTime dt);

    KPTDateTime add(const KPTDuration &duration);
    KPTDateTime subtract(const KPTDuration &duration);
    KPTDuration duration(const KPTDateTime &dt);

    /**
     * Adds the duration @duration to the datetime
     */
    KPTDateTime operator+(const KPTDuration &duration) { return add(duration); }
    /**
     * Subtracts the duration @duration from the datetime
     */
    KPTDateTime operator-(const KPTDuration &duration) { return subtract(duration); }
    /**
     * Returns the absolute duration between the two datetimes
     */
    KPTDuration operator-(const KPTDateTime &dt) { return duration(dt); }
};
