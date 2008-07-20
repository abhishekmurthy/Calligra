/*
 *  Copyright (c) 2008 Boudewijn Rempt <boud@valdyas.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "kis_crash_filter_test.h"
#include <qtest_kde.h>
#include "filter/kis_filter_configuration.h"
#include "filter/kis_filter_registry.h"
#include "kis_selection.h"
#include "kis_processing_information.h"
#include "filter/kis_filter.h"
#include "kis_threaded_applicator.h"
#include "kis_selection.h"
#include "kis_pixel_selection.h"
#include <KoColorSpaceRegistry.h>


bool applyFilter( const KoColorSpace * cs,  KisFilterSP f ) {

//    QImage qimg( QString(FILES_DATA_DIR) + QDir::separator() + "lena.png");

    KisPaintDeviceSP dev = new KisPaintDevice(cs);
    dev->fill(0, 0, 100, 100, dev->defaultPixel());
//    dev->convertFromQImage(qimg, "", 0, 0);

    // Get the predefined configuration from a file
    KisFilterConfiguration * kfc = f->defaultConfiguration(dev);

    QFile file(QString(FILES_DATA_DIR) + QDir::separator() + f->id() + ".cfg");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "creating new file for " << f->id();
        file.open(QIODevice::WriteOnly | QIODevice::Text);
        QTextStream out(&file);
        out << kfc->toXML();
    }
    else {
        QString s;
        QTextStream in(&file);
        s = in.readAll();
        kfc->fromXML( s );
    }
    qDebug() << f->id() << ", " << cs->id();// << kfc->toXML() << "\n";

    KisConstProcessingInformation src( dev,  QPoint(0, 0), 0 );
    KisProcessingInformation dst( dev, QPoint(0, 0), 0 );

    f->process(src, dst, QSize( 100, 100 ), kfc);

#if 0
    QPoint errpoint;

    if ( !compareQImages( errpoint, result, dev->convertToQImage(0, 0, 0, qimg.width(), qimg.height() ) ) ) {
        //dev->convertToQImage(0, 0, 0, qimg.width(), qimg.height()).save(QString("lena_%1.png").arg(f->id()));
        return false;
    }
#endif
    return true;

}

bool testFilter(KisFilterSP f)
{

    QList<QString> csIds = KoColorSpaceRegistry::instance()->keys();
    bool ok = false;
    foreach( QString csId, csIds ) {
        // XXX: Let's not check the painterly colorspaces right now
        if ( csId.startsWith( "KS", Qt::CaseInsensitive ) ) {
            continue;
        }
        QList<const KoColorProfile*> profiles = KoColorSpaceRegistry::instance()->profilesFor ( csId );
        if ( profiles.size() == 0 ) {
            const KoColorSpace * cs = KoColorSpaceRegistry::instance()->colorSpace( csId, 0 );
            ok = applyFilter( cs, f );
        }
        else {
            foreach( const KoColorProfile * profile, profiles ) {
                const KoColorSpace * cs = KoColorSpaceRegistry::instance()->colorSpace( csId, profile );
                ok = applyFilter( cs, f );
            }

        }
    }

    return ok;
}

void KisCrashFilterTest::testCrashFilters()
{
    QStringList failures;
    QStringList successes;

    QList<QString> filterList = KisFilterRegistry::instance()->keys();
    for ( QList<QString>::Iterator it = filterList.begin(); it != filterList.end(); ++it ) {
        if (testFilter(KisFilterRegistry::instance()->value(*it)))
            successes << *it;
        else
            failures << *it;
    }
    qDebug() << "Success: " << successes;
    if (failures.size() > 0) {
        QFAIL(QString("Failed filters:\n\t %1").arg(failures.join("\n\t")).toAscii());
    }
}

QTEST_KDEMAIN(KisCrashFilterTest, GUI)
#include "kis_crash_filter_test.moc"
