/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>

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

#include "latexexportdialog.h"
#include "latexexport.h"
// KDE
#include <kdebug.h>
#include <KoFilterChain.h>
#include <kpluginfactory.h>
// Qt
#include <QByteArray>

K_PLUGIN_FACTORY(LATEXExportFactory, registerPlugin<LATEXExport>();)
K_EXPORT_PLUGIN(LATEXExportFactory("calligrafilters"))


LATEXExport::LATEXExport(QObject* parent, const QVariantList&) :
        KoFilter(parent)
{
}

KoFilter::ConversionStatus LATEXExport::convert(const QByteArray& from, const QByteArray& to)
{
    QString config;

    if (to != "text/x-tex" || from != "application/x-kspread")
        return KoFilter::NotImplemented;

    KoStore* in = KoStore::createStore(m_chain->inputFile(), KoStore::Read);
    if (!in || !in->open("root")) {
        kError(30503) << "Unable to open input file!" << endl;
        delete in;
        return KoFilter::FileNotFound;
    }
    kDebug(30522) << "In the kspread latex export filter...";
    /* input file Reading */
    in->close();

    LatexExportDialog* dialog = new LatexExportDialog(in);
    dialog->setOutputFile(m_chain->outputFile());

    dialog->exec();
    delete dialog;
    delete in;

    return KoFilter::OK;
}

#include <latexexport.moc>
