/* -*- C++ -*-

  $Id$

  This file is part of KIllustrator.
  Copyright (C) 1998 Kai-Uwe Sattler (kus@iti.cs.uni-magdeburg.de)

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as
  published by  
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU Library General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/

#ifndef GLayer_h_
#define GLayer_h_

#include "GObject.h"

class GLayer : public QObject {
  Q_OBJECT
public:
  GLayer (const char* text = 0L);
  ~GLayer ();

  bool isVisible () const { return visibleFlag; }
  bool isPrintable () const { return printableFlag; }
  bool isEditable () const { return editableFlag; }
  bool isActive () const { return activeFlag; }

  const char* name () const;
  void setName (const char* text);

  void setVisible (bool flag);
  void setPrintable (bool flag);
  void setEditable (bool flag);
  void setActive (bool flag);

signals:
  void propertyChanged ();
  void contentChanged ();
  
private:
  QString ident;    // layer identifier
  bool visibleFlag, // layer is visible
    printableFlag,  // layer is printable
    editableFlag,   // layer is editable
    activeFlag;     // updates are going to this layer

  static int lastID;
};  

#endif
