/* This file is part of the KDE project
   Copyright (C) 2002 Laurent MONTEL <lmontel@mandrakesoft.com>

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

#ifndef KPRESENTER_OBJ2D_IFACE_H
#define KPRESENTER_OBJ2D_IFACE_H

#include <dcopobject.h>
#include <dcopref.h>

#include <qstring.h>
#include <qcolor.h>

#include "KPresenterObjectIface.h"

class KP2DObject;

class KPresenterObject2DIface : virtual public KPresenterObjectIface
{
    K_DCOP
public:
    KPresenterObject2DIface( KP2DObject *obj_ );

k_dcop:
    virtual QColor gradientColor1() const;
    virtual QColor gradientColor2() const;
    virtual void setGUnbalanced( bool b );
    virtual void setGXFactor( int f );
    virtual void setGYFactor( int f );
    virtual int xGradientFactor() const;
    virtual int yGradientFactor() const;
    virtual QString gradientFillType() const;
    virtual void setGradientType( const QString & );
    virtual void setFillType( const QString & );
    virtual void setGradientColor1( const QColor &col );
    virtual void setGradientColor2( const QColor &col );

private:
    KP2DObject *obj;

};

#endif
