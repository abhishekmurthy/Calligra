/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>

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
   Boston, MA 02110-1301, USA.
*/

#ifndef CALLIGRA_SHEETS_ABOUTDATA
#define CALLIGRA_SHEETS_ABOUTDATA

#include <kaboutdata.h>
#include <klocale.h>
#include <calligraversion.h>

namespace Calligra
{
namespace Sheets
{

static const char* description = I18N_NOOP("Calligra Spreadsheet Application");

// Always the same as the Calligra version
static const char* version = CALLIGRA_VERSION_STRING;

KAboutData * newAboutData()
{
    KAboutData * aboutData = new KAboutData("sheets", 0, ki18nc("application name", "Calligra Sheets"),
                                            version, ki18n(description), KAboutData::License_LGPL,
                                            ki18n("(c) 1998-2011, The Calligra Sheets Team"), KLocalizedString(),
                                            "http://www.calligra.org/sheets/");
    aboutData->setProductName("calligrasheets"); // for bugs.kde.org
    aboutData->setProgramIconName(QLatin1String("calligrasheets"));
    aboutData->addAuthor(ki18n("Torben Weis"), ki18n("Original Author"), "weis@kde.org");
    aboutData->addAuthor(ki18n("Marijn Kruisselbrink"), ki18n("Maintainer"), "mkruisselbrink@kde.org");
    aboutData->addAuthor(ki18n("Sebastian Sauer"), ki18n("ODS and Excel, functions, scripting"), "mail@dipe.org");
    aboutData->addAuthor(ki18n("Laurent Montel"), KLocalizedString(), "montel@kde.org");
    aboutData->addAuthor(ki18n("John Dailey"), KLocalizedString(), "dailey@vt.edu");
    aboutData->addAuthor(ki18n("Philipp Müller"), KLocalizedString(), "philipp.mueller@gmx.de");
    aboutData->addAuthor(ki18n("Ariya Hidayat"), KLocalizedString(), "ariya@kde.org");
    aboutData->addAuthor(ki18n("Norbert Andres"), KLocalizedString(), "nandres@web.de");
    aboutData->addAuthor(ki18n("Shaheed Haque"),
                         ki18n("Import/export filter developer"),
                         "srhaque@iee.org");
    aboutData->addAuthor(ki18n("Werner Trobin"),
                         ki18n("Import/export filter developer"),
                         "trobin@kde.org");
    aboutData->addAuthor(ki18n("Nikolas Zimmermann"),
                         ki18n("Import/export filter developer"),
                         "wildfox@kde.org");
    aboutData->addAuthor(ki18n("David Faure"), KLocalizedString(), "faure@kde.org");
    aboutData->addAuthor(ki18n("Helge Deller"),
                         ki18n("Import/export filter developer"),
                         "deller@gmx.de");
    aboutData->addAuthor(ki18n("Percy Leonhart"),
                         ki18n("Import/export filter developer"),
                         "percy@eris23.org");
    aboutData->addAuthor(ki18n("Eva Brucherseifer"),
                         ki18n("Import/export filter developer"),
                         "eva@kde.org");
    aboutData->addAuthor(ki18n("Phillip Ezolt"),
                         ki18n("Import/export filter developer"),
                         "phillipezolt@hotmail.com");
    aboutData->addAuthor(ki18n("Enno Bartels"),
                         ki18n("Import/export filter developer"),
                         "ebartels@nwn.de");
    aboutData->addAuthor(ki18n("Graham Short"),
                         ki18n("Import/export filter developer"),
                         "grahshrt@netscape.net");
    aboutData->addAuthor(ki18n("Lukáš Tinkl"), KLocalizedString(), "lukas@kde.org");
    aboutData->addAuthor(ki18n("Tomas Mecir"), KLocalizedString(), "mecirt@gmail.com");
    aboutData->addAuthor(ki18n("Raphael Langerhorst"), KLocalizedString(), "raphael.langerhorst@kdemail.net");
    aboutData->addAuthor(ki18n("John Tapsell"), KLocalizedString(), "john.tapsell@kdemail.net");
    aboutData->addAuthor(ki18n("Robert Knight"), KLocalizedString() , "robertknight@gmail.com");
    aboutData->addAuthor(ki18n("Stefan Nikolaus"), KLocalizedString(), "stefan.nikolaus@kdemail.net");
    aboutData->addAuthor(ki18n("Sascha Pfau"), ki18n("Developer (functions)"), "mrpeacock@gmail.com");
    aboutData->addAuthor(ki18n("Brad Hards"),
                         ki18n("Implemented, reviewed and tested various functions"),
                         "bradh@frogmouth.net");
    return aboutData;
}

} // namespace Sheets
} // namespace Calligra

#endif
