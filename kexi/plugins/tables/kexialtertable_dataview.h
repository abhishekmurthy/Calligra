/* This file is part of the KDE project
   Copyright (C) 2004 Jaroslaw Staniek <js@iidea.pl>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef KEXIALTERTABLEDATAVIEW_H
#define KEXIALTERTABLEDATAVIEW_H

#include <kexidatatable.h>
#include <kexipropertybuffer.h>
#include <kexitablepart.h>

class KexiAlterTable_DataView : public KexiDataTable
{
	Q_OBJECT

	public:
		KexiAlterTable_DataView(KexiMainWindow *win, QWidget *parent, const char *name = 0);

		virtual ~KexiAlterTable_DataView();

		KexiTablePart::TempData* tempData() const;

	protected:
//		//! called just once from ctor
//		void init();
//		void initActions();
//		//! called whenever data should be reloaded (on switching to this view mode)
//		void initData();

		virtual tristate beforeSwitchTo(int mode, bool &dontStore);
		virtual tristate afterSwitchFrom(int mode);

};

#endif
