/* This file is part of the KDE project
   Copyright (C) 2002 Marco Zanon <info@marcozanon.com>
                  and Ariya Hidayat <ariya@kde.org>

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

#ifndef __STARWRITER5IMPORT_H
#define __STARWRITER5IMPORT_H

#include <koFilter.h>
#include <qstring.h>
#include <qcstring.h>

class StarWriter5Import: public KoFilter
{
    Q_OBJECT

public:
    // needed for ATTRIBUTES
    bool hasHeader;
    bool hasFooter;

    StarWriter5Import(KoFilter *parent, const char *name, const QStringList&);
    virtual ~StarWriter5Import();
    KoFilter::ConversionStatus convert(const QCString& from, const QCString& to);

private:
    // most important OLE streams
    QByteArray SwPageStyleSheets;
    QByteArray StarWriterDocument;

    // Preliminary check
    bool checkDocumentVersion();

    // Formatting routines
    bool addKWordHeader();
    bool addPageProperties();
    bool addStyles();
    bool addHeaders();
    bool addFooters();
    bool addBody();
    QString convertToKWordString(QByteArray s);

    // Node routines
    bool parseNodes(QByteArray n, QString caption);
    bool parseText(QByteArray n, QString caption);
    bool parseTable(QByteArray n);
    bool parseGraphics(QByteArray n);

    // finished KWord document
    QString maindoc;
};

#endif
