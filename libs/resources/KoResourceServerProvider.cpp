/*  This file is part of the KDE project

    Copyright (c) 1999 Matthias Elter <elter@kde.org>
    Copyright (c) 2003 Patrick Julien <freak@codepimps.org>
    Copyright (c) 2005 Sven Langkamp <sven.langkamp@gmail.com>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "KoResourceServerProvider.h"

#include <QFileInfo>
#include <QStringList>
#include <QDir>

#include <kdebug.h>
#include <kglobal.h>
#include <kstandarddirs.h>
#include <kcomponentdata.h>

#include "KoSegmentGradient.h"
#include "KoStopGradient.h"

class GradientResourceServer : public KoResourceServer<KoAbstractGradient> {

public:

    GradientResourceServer(const QString& type) : KoResourceServer<KoAbstractGradient>(type)
    {
    }

private:
    virtual KoAbstractGradient* createResource( const QString & filename ) {

        QString fileExtension;
        int index = filename.lastIndexOf('.');

        if (index != -1)
            fileExtension = filename.mid(index).toLower();

        KoAbstractGradient* grad = 0;

        if(fileExtension == ".svg" || fileExtension == ".kgr")
            grad = new KoStopGradient(filename);
        else if(fileExtension == ".ggr" )
            grad = new KoSegmentGradient(filename);

        return grad;
    }
};

KoResourceLoaderThread::KoResourceLoaderThread(KoResourceServerBase * server, const QString & extensions)
    : QThread()
    , m_server(server)
{
    m_fileNames = getFileNames( extensions );
}

void KoResourceLoaderThread::run()
{
    m_server->loadResources(m_fileNames);
}

QStringList KoResourceLoaderThread::getFileNames( const QString & extensions)
{
    QStringList extensionList = extensions.split(":");
    QStringList fileNames;

    foreach (QString extension, extensionList) {
        fileNames += KGlobal::mainComponent().dirs()->findAllResources(m_server->type().toAscii(), extension);
    }
    return fileNames;
}


KoResourceServerProvider *KoResourceServerProvider::m_singleton = 0;

KoResourceServerProvider::KoResourceServerProvider()
: m_patternLoader(0), m_gradientLoader(0), m_paletteLoader(0)
{
    KGlobal::mainComponent().dirs()->addResourceType("ko_patterns", "data", "krita/patterns/");
    KGlobal::mainComponent().dirs()->addResourceDir("ko_patterns", "/usr/share/create/patterns/gimp");
    KGlobal::mainComponent().dirs()->addResourceDir("ko_patterns", QDir::homePath() + QString("/.create/patterns/gimp"));

    KGlobal::mainComponent().dirs()->addResourceType("ko_gradients", "data", "krita/gradients/");
    KGlobal::mainComponent().dirs()->addResourceDir("ko_gradients", "/usr/share/create/gradients/gimp");
    KGlobal::mainComponent().dirs()->addResourceDir("ko_gradients", QDir::homePath() + QString("/.create/gradients/gimp"));

    KGlobal::mainComponent().dirs()->addResourceType("ko_palettes", "data", "krita/palettes/");
    KGlobal::mainComponent().dirs()->addResourceDir("ko_palettes", "/usr/share/create/swatches");
    KGlobal::mainComponent().dirs()->addResourceDir("ko_palettes", QDir::homePath() + QString("/.create/swatches"));

    m_patternServer = new KoResourceServer<KoPattern>("ko_patterns");
    m_gradientServer = new GradientResourceServer("ko_gradients");
    m_paletteServer = new KoResourceServer<KoColorSet>("ko_palettes");
}

KoResourceServerProvider::~KoResourceServerProvider()
{
    delete m_patternLoader;
    delete m_gradientLoader;
    delete m_paletteLoader;
}

KoResourceServerProvider* KoResourceServerProvider::instance()
{
    if(KoResourceServerProvider::m_singleton == 0)
    {
        KoResourceServerProvider::m_singleton = new KoResourceServerProvider();
    }
    return KoResourceServerProvider::m_singleton;
}

KoResourceServer<KoPattern>* KoResourceServerProvider::patternServer()
{
    if( ! m_patternLoader )
    {
        kDebug() << "start loading patterns";
        m_patternLoader = new KoResourceLoaderThread(m_patternServer, "*.pat");
        m_patternLoader->start();
        m_patternLoader->wait();
    }
    return m_patternServer;
}

KoResourceServer<KoAbstractGradient>* KoResourceServerProvider::gradientServer()
{
    if( ! m_gradientLoader )
    {
        kDebug() << "start loading gradients";
        m_gradientLoader = new KoResourceLoaderThread(m_gradientServer, "*.kgr:*.svg:*.ggr");
        m_gradientLoader->start();
        m_gradientLoader->wait();
    }
    return m_gradientServer;
}

KoResourceServer<KoColorSet>* KoResourceServerProvider::paletteServer()
{
    if( ! m_paletteLoader )
    {
        kDebug() << "start loading palettes";
        m_paletteLoader = new KoResourceLoaderThread(m_paletteServer, "*.gpl:*.pal:*.act");
        m_paletteLoader->start();
        m_paletteLoader->wait();
    }
    return m_paletteServer;
}

