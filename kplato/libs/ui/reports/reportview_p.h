/*
 * KPlato Report Plugin
 * Copyright (C) 2010 by Dag Andersen <danders@get2net.dk>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#ifndef KPLATOREPORTVIEW_P_H
#define KPLATOREPORTVIEW_P_H


#include "kplatoui_export.h"

#include "reportdesigner.h"

class KoDocument;

class KoReportData;
class ORPreRender;
class ORODocument;
class ReportViewPageSelect;
class RecordNavigator;
class ScriptAdaptor;
class ReportDesigner;

class QScrollArea;
class QDomElement;

namespace KPlato
{

//-----------------
class KPlato_ReportDesigner : public ReportDesigner
{
    Q_OBJECT
public:
    KPlato_ReportDesigner( QWidget *parent = 0);

};


} // namespace KPlato

#endif
