/* This file is part of the KDE project
   Copyright (C) 2003 Percy Leonhardt

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

#include "ooimpressexport.h"

#include <qdom.h>
#include <qfile.h>

#include <kdebug.h>
#include <kgenericfactory.h>
#include <koFilterChain.h>
#include <koGlobal.h>
#include <koUnit.h>

typedef KGenericFactory<OoImpressExport, KoFilter> OoImpressExportFactory;
K_EXPORT_COMPONENT_FACTORY( libooimpressexport, OoImpressExportFactory( "ooimpressexport" ) );


OoImpressExport::OoImpressExport( KoFilter *, const char *, const QStringList & )
    : KoFilter()
    , m_currentPage( 0 )
    , m_pageHeight( 0 )
{
}

KoFilter::ConversionStatus OoImpressExport::convert( const QCString & from,
                                                     const QCString & to )
{
    kdDebug() << "Entering Ooimpress Export filter: " << from << " - " << to << endl;

    if ( ( to != "application/vnd.sun.xml.impress") || (from != "application/x-kpresenter" ) )
    {
        kdWarning() << "Invalid mimetypes " << to << " " << from << endl;
        return KoFilter::NotImplemented;
    }

    // read in the KPresenter file
    KoFilter::ConversionStatus preStatus = openFile();

    if ( preStatus != KoFilter::OK )
        return preStatus;

    QDomImplementation impl;
    QDomDocument meta( impl.createDocumentType( "office:document-meta", "-//OpenOffice.org//DTD OfficeDokument 1.0//EN", "office.dtd" ) );

    createDocumentMeta( meta );

    // store document meta
    KoStore * store = KoStore::createStore( m_chain->outputFile(), KoStore::Write, "", KoStore::Zip );

    if ( !store )
    {
        kdWarning() << "Couldn't open the requested file." << endl;
        return KoFilter::FileNotFound;
    }

    if ( !store->open( "meta.xml" ) )
    {
        kdWarning() << "Couldn't open the file 'meta.xml'." << endl;
        delete store;
        return KoFilter::CreationError;
    }

    QCString metaString = meta.toCString();
    //kdDebug() << "meta :" << metaString << endl;
    store->write( metaString , metaString.length() );
    store->close();

    QDomDocument content( impl.createDocumentType( "office:document-content", "-//OpenOffice.org//DTD OfficeDocument 1.0//EN", "office.dtd" ) );

    createDocumentContent( content );

    // add the automatic styles
    m_styleFactory.addAutomaticStyles( content, m_styles );

    // store document content
    if ( !store->open( "content.xml" ) )
    {
        kdWarning() << "Couldn't open the file 'content.xml'." << endl;
        delete store;
        return KoFilter::CreationError;
    }

    QCString contentString = content.toCString();
    kdDebug() << "content :" << contentString << endl;
    store->write( contentString , contentString.length() );
    store->close();

    QDomDocument styles( impl.createDocumentType( "office:document-styles", "-//OpenOffice.org//DTD OfficeDocument 1.0//EN", "office.dtd" ) );

    createDocumentStyles( styles );

    // store document styles
    if ( !store->open( "styles.xml" ) )
    {
        kdWarning() << "Couldn't open the file 'styles.xml'." << endl;
        delete store;
        return KoFilter::CreationError;
    }

    QCString stylesString = styles.toCString();
    //kdDebug() << "styles :" << stylesString << endl;
    store->write( stylesString , stylesString.length() );
    store->close();

    QDomDocument manifest( impl.createDocumentType( "manifest:manifest", "-//OpenOffice.org//DTD Manifest 1.0//EN", "Manifest.dtd" ) );

    createDocumentManifest( manifest );

    // store document manifest
    store->enterDirectory( "META-INF" );
    if ( !store->open( "manifest.xml" ) )
    {
        kdWarning() << "Couldn't open the file 'META-INF/manifest.xml'." << endl;
        delete store;
        return KoFilter::CreationError;
    }

    QCString manifestString = manifest.toCString();
    //kdDebug() << "manifest :" << manifestString << endl;
    store->write( manifestString , manifestString.length() );
    store->close();

    delete store;

    return KoFilter::OK;
}

KoFilter::ConversionStatus OoImpressExport::openFile()
{
    KoStore * store = KoStore::createStore( m_chain->inputFile(), KoStore::Read );

    if ( !store )
    {
        kdWarning() << "Couldn't open the requested file." << endl;
        return KoFilter::FileNotFound;
    }

    if ( !store->open( "maindoc.xml" ) )
    {
        kdWarning() << "This file doesn't seem to be a valid KPresenter file" << endl;
        delete store;
        return KoFilter::WrongFormat;
    }

    m_maindoc.setContent( store->device() );
    store->close();

    if ( store->open( "documentinfo.xml" ) )
    {
        m_documentinfo.setContent( store->device() );
        store->close();
    }
    else
        kdWarning() << "Documentinfo do not exist!" << endl;

    delete store;

    emit sigProgress( 10 );

    return KoFilter::OK;
}

void OoImpressExport::createDocumentMeta( QDomDocument & docmeta )
{
    docmeta.appendChild( docmeta.createProcessingInstruction( "xml","version=\"1.0\" encoding=\"UTF-8\"" ) );

    QDomElement content = docmeta.createElement( "office:document-meta" );
    content.setAttribute( "xmlns:office", "http://openoffice.org/2000/office" );
    content.setAttribute( "xmlns:xlink", "http://www.w3.org/1999/xlink" );
    content.setAttribute( "xmlns:dc", "http://purl.org/dc/elements/1.1/" );
    content.setAttribute( "xmlns:meta", "http://openoffice.org/2000/meta" );
    content.setAttribute( "office:version", "1.0" );

    QDomNode meta = docmeta.createElement( "office:meta" );

    QDomElement generator = docmeta.createElement( "meta:generator" );
    generator.appendChild( docmeta.createTextNode( "KPresenter 1.2" ) );
    meta.appendChild( generator );

    QDomNode i = m_documentinfo.namedItem( "document-info" );
    if ( !i.isNull() )
    {
        QDomNode n = i.namedItem( "author" ).namedItem( "full-name" );
        if ( !n.isNull() )
        {
            QDomElement fullName = n.toElement();
            QDomElement creator = docmeta.createElement( "meta:initial-creator" );
            creator.appendChild( docmeta.createTextNode( fullName.text() ) );
            meta.appendChild( creator );

            creator = docmeta.createElement( "meta:creator" );
            creator.appendChild( docmeta.createTextNode( fullName.text() ) );
            meta.appendChild( creator );
        }

        n = i.namedItem( "about" ).namedItem( "title" );
        if ( !n.isNull() )
        {
            QDomElement title = n.toElement();
            QDomElement user = docmeta.createElement( "meta:user-defined" );
            user.setAttribute( "meta:name", "Info 1" );
            user.appendChild( docmeta.createTextNode( title.text() ) );
            meta.appendChild( user );
        }

        n = i.namedItem( "about" ).namedItem( "abstract" );
        if ( !n.isNull() )
        {
            QDomElement user = docmeta.createElement( "meta:user-defined" );
            user.setAttribute( "meta:name", "Info 2" );
            user.appendChild( n.firstChild() );
            meta.appendChild( user );
        }
    }

//     QDomElement statistic = docmeta.createElement( "meta:document-statistic" );
//     statistic.setAttribute( "meta:object-count", 0 );
//     meta.appendChild( data );

    content.appendChild( meta );
    docmeta.appendChild( content );
}

void OoImpressExport::createDocumentStyles( QDomDocument & docstyles )
{
    docstyles.appendChild( docstyles.createProcessingInstruction( "xml","version=\"1.0\" encoding=\"UTF-8\"" ) );

    QDomElement content = docstyles.createElement( "office:document-content" );
    content.setAttribute( "xmlns:office", "http://openoffice.org/2000/office" );
    content.setAttribute( "xmlns:style", "http://openoffice.org/2000/style" );
    content.setAttribute( "xmlns:text", "http://openoffice.org/2000/text" );
    content.setAttribute( "xmlns:table", "http://openoffice.org/2000/table" );
    content.setAttribute( "xmlns:draw", "http://openoffice.org/2000/drawing" );
    content.setAttribute( "xmlns:fo", "http://www.w3.org/1999/XSL/Format" );
    content.setAttribute( "xmlns:xlink", "http://www.w3.org/1999/xlink" );
    content.setAttribute( "xmlns:number", "http://openoffice.org/2000/datastyle" );
    content.setAttribute( "xmlns:svg", "http://www.w3.org/2000/svg" );
    content.setAttribute( "xmlns:chart", "http://openoffice.org/2000/chart" );
    content.setAttribute( "xmlns:dr3d", "http://openoffice.org/2000/dr3d" );
    content.setAttribute( "xmlns:math", "http://www.w3.org/1998/Math/MathML" );
    content.setAttribute( "xmlns:form", "http://openoffice.org/2000/form" );
    content.setAttribute( "xmlns:script", "http://openoffice.org/2000/script" );
    content.setAttribute( "office:version", "1.0" );

    // order important here!
    QDomElement styles = docstyles.createElement( "office:styles" );
    m_styleFactory.addOfficeStyles( docstyles, styles );
    content.appendChild( styles );

    QDomElement automatic = docstyles.createElement( "office:automatic-styles" );
    m_styleFactory.addOfficeAutomatic( docstyles, automatic );
    content.appendChild( automatic );

    QDomElement master = docstyles.createElement( "office:master-styles" );
    m_styleFactory.addOfficeMaster( docstyles, master );
    content.appendChild( master );

    docstyles.appendChild( content );
}

void OoImpressExport::createDocumentContent( QDomDocument & doccontent )
{
    doccontent.appendChild( doccontent.createProcessingInstruction( "xml","version=\"1.0\" encoding=\"UTF-8\"" ) );

    QDomElement content = doccontent.createElement( "office:document-content" );
    content.setAttribute( "xmlns:office", "http://openoffice.org/2000/office");
    content.setAttribute( "xmlns:style", "http://openoffice.org/2000/style" );
    content.setAttribute( "xmlns:text", "http://openoffice.org/2000/text" );
    content.setAttribute( "xmlns:table", "http://openoffice.org/2000/table" );
    content.setAttribute( "xmlns:draw", "http://openoffice.org/2000/drawing" );
    content.setAttribute( "xmlns:fo", "http://www.w3.org/1999/XSL/Format" );
    content.setAttribute( "xmlns:xlink", "http://www.w3.org/1999/xlink" );
    content.setAttribute( "xmlns:number", "http://openoffice.org/2000/datastyle" );
    content.setAttribute( "xmlns:svg", "http://www.w3.org/2000/svg" );
    content.setAttribute( "xmlns:chart", "http://openoffice.org/2000/chart" );
    content.setAttribute( "xmlns:dr3d", "http://openoffice.org/2000/dr3d" );
    content.setAttribute( "xmlns:math", "http://www.w3.org/1998/Math/MathML" );
    content.setAttribute( "xmlns:form", "http://openoffice.org/2000/form" );
    content.setAttribute( "xmlns:script", "http://openoffice.org/2000/script" );
    content.setAttribute( "office:class", "presentation" );
    content.setAttribute( "office:version", "1.0" );

    QDomElement script = doccontent.createElement( "office:script" );
    content.appendChild( script );

    m_styles = doccontent.createElement( "office:automatic-styles" );
    exportPaperSettings( doccontent );
    content.appendChild( m_styles );

    QDomElement body = doccontent.createElement( "office:body" );
    exportBody( doccontent, body );
    content.appendChild( body );

    doccontent.appendChild( content );
}

void OoImpressExport::createDocumentManifest( QDomDocument & docmanifest )
{
    docmanifest.appendChild( docmanifest.createProcessingInstruction( "xml","version=\"1.0\" encoding=\"UTF-8\"" ) );

    QDomElement manifest = docmanifest.createElement( "manifest:manifest" );
    manifest.setAttribute( "xmlns:manifest", "http://openoffice.org/2001/manifest" );

    QDomElement entry = docmanifest.createElement( "manifest:file-entry" );
    entry.setAttribute( "manifest:media-type", "application/vnd.sun.xml.impress" );
    entry.setAttribute( "manifest:full-path", "/" );
    manifest.appendChild( entry );

    entry = docmanifest.createElement( "manifest:file-entry" );
    entry.setAttribute( "manifest:media-type", "" );
    entry.setAttribute( "manifest:full-path", "Pictures/" );
    manifest.appendChild( entry );

    entry = docmanifest.createElement( "manifest:file-entry" );
    entry.setAttribute( "manifest:media-type", "text/xml" );
    entry.setAttribute( "manifest:full-path", "content.xml" );
    manifest.appendChild( entry );

    entry = docmanifest.createElement( "manifest:file-entry" );
    entry.setAttribute( "manifest:media-type", "text/xml" );
    entry.setAttribute( "manifest:full-path", "styles.xml" );
    manifest.appendChild( entry );

    entry = docmanifest.createElement( "manifest:file-entry" );
    entry.setAttribute( "manifest:media-type", "text/xml" );
    entry.setAttribute( "manifest:full-path", "meta.xml" );
    manifest.appendChild( entry );

//     entry = docmanifest.createElement( "manifest:file-entry" );
//     entry.setAttribute( "manifest:media-type", "text/xml" );
//     entry.setAttribute( "manifest:full-path", "settings.xml" );
//     manifest.appendChild( entry );

    docmanifest.appendChild( manifest );
}

void OoImpressExport::exportPaperSettings( QDomDocument & doccontent )
{
    // store the paper settings
    QDomElement style = doccontent.createElement( "style:style" );
    style.setAttribute( "style:name", "dp1" );
    style.setAttribute( "style:family", "drawing-page" );
    QDomElement properties = doccontent.createElement( "style:properties" );
    properties.setAttribute( "presentation:background-visible", "true" );
    properties.setAttribute( "presentation:background-objects-visible", "true" );
    style.appendChild( properties );
    m_styles.appendChild( style );

    QDomNode doc = m_maindoc.namedItem( "DOC" );
    QDomNode paper = doc.namedItem( "PAPER" );

    QDomElement p = paper.toElement();
    m_masterPageStyle = m_styleFactory.createPageMasterStyle( p );
    m_pageHeight = p.attribute( "ptHeight" ).toFloat();
}

void OoImpressExport::exportBody( QDomDocument & doccontent, QDomElement & body )
{
    QDomNode doc = m_maindoc.namedItem( "DOC" );
    QDomNode background = doc.namedItem( "BACKGROUND" );
    QDomNode header = doc.namedItem( "HEADER" );
    QDomNode footer = doc.namedItem( "FOOTER" );
    QDomNode titles = doc.namedItem( "PAGETITLES" );
    QDomNode notes = doc.namedItem( "PAGENOTES" );
    QDomNode objects = doc.namedItem( "OBJECTS" );
    QDomNode pictures = doc.namedItem( "PICTURES" );
    QDomNode sounds = doc.namedItem( "SOUNDS" );

    m_currentPage = 1;

    // parse all pages
    for ( QDomNode title = titles.firstChild(); !title.isNull();
          title = title.nextSibling() )
    {
        QDomElement t = title.toElement();
        QDomElement drawPage = doccontent.createElement( "draw:page" );
        drawPage.setAttribute( "draw:name", t.attribute( "title" ) );
        drawPage.setAttribute( "draw:style-name", "dp1" );
        drawPage.setAttribute( "draw:id", m_currentPage );
        drawPage.setAttribute( "draw:master-page-name", m_masterPageStyle );

        // I am not sure if objects are always stored sorted so I parse all
        // of them to find the ones belonging to a certain page.
        for ( QDomNode object = objects.firstChild(); !object.isNull();
              object = object.nextSibling() )
        {
            QDomElement o = object.toElement();

            QDomElement orig = o.namedItem( "ORIG" ).toElement();
            float y = orig.attribute( "y" ).toFloat();

            if ( y < m_pageHeight * ( m_currentPage - 1 ) ||
                 y >= m_pageHeight * m_currentPage )
                continue; // object not on current page

            switch( o.attribute( "type" ).toInt() )
            {
            case 0: // image
                break;
            case 1: // line
                appendLine( doccontent, o, drawPage );
                break;
            case 2: // rectangle
                appendRectangle( doccontent, o, drawPage );
                break;
            case 3: // circle, ellipse
                appendEllipse( doccontent, o, drawPage );
                break;
            case 4: // textbox
                appendTextbox( doccontent, o, drawPage );
                break;
            case 8: // pie, chord, arc
                break;
            case 12: // polyline
                break;
            case 16: // polygon
                break;
            }
        }


        body.appendChild( drawPage );

        m_currentPage++;
    }
}

void OoImpressExport::appendTextbox( QDomDocument & doc, QDomElement & source, QDomElement & target )
{
    QDomElement textbox = doc.createElement( "draw:text-box" );
    setGraphicStyle( source, textbox );
    set2DGeometry( source, textbox );
    appendText( doc, source, textbox );

    target.appendChild( textbox );
}

void OoImpressExport::appendText( QDomDocument & doc, QDomElement & source, QDomElement & target )
{
    QDomElement textobj = source.namedItem( "TEXTOBJ" ).toElement();
    QDomElement paragraph = textobj.namedItem( "P" ).toElement();
    QDomElement text = paragraph.namedItem( "TEXT" ).toElement();

    QDomElement t = doc.createElement( "text:p" );
    t.appendChild( doc.createTextNode( text.text() ) );

    target.appendChild( t );
}

void OoImpressExport::appendLine( QDomDocument & doc, QDomElement & source, QDomElement & target )
{
    QDomElement line = doc.createElement( "draw:line" );
    setLineGeometry( source, line );

    target.appendChild( line );
}

void OoImpressExport::appendRectangle( QDomDocument & doc, QDomElement & source, QDomElement & target )
{
    QDomElement rectangle = doc.createElement( "draw:rect" );
    set2DGeometry( source, rectangle );

    target.appendChild( rectangle );
}

void OoImpressExport::appendEllipse( QDomDocument & doc, QDomElement & source, QDomElement & target )
{
    QDomElement ellipse = doc.createElement( "draw:ellipse" );
    set2DGeometry( source, ellipse );

    target.appendChild( ellipse );
}

void OoImpressExport::set2DGeometry( QDomElement & source, QDomElement & target )
{
    QDomElement orig = source.namedItem( "ORIG" ).toElement();
    QDomElement size = source.namedItem( "SIZE" ).toElement();

    float y = orig.attribute( "y" ).toFloat();
    y -= m_pageHeight * ( m_currentPage - 1 );

    target.setAttribute( "svg:x", StyleFactory::toCM( orig.attribute( "x" ) ) );
    target.setAttribute( "svg:y", QString( "%1cm" ).arg( KoUnit::toCM( y ) ) );
    target.setAttribute( "svg:width", StyleFactory::toCM( size.attribute( "width" ) ) );
    target.setAttribute( "svg:height", StyleFactory::toCM( size.attribute( "height" ) ) );
}

void OoImpressExport::setLineGeometry( QDomElement & source, QDomElement & target )
{
    QDomElement orig = source.namedItem( "ORIG" ).toElement();
    QDomElement size = source.namedItem( "SIZE" ).toElement();
    QDomElement linetype = source.namedItem( "LINETYPE" ).toElement();

    float x1 = orig.attribute( "x" ).toFloat();
    float y1 = orig.attribute( "y" ).toFloat();
    float x2 = size.attribute( "width" ).toFloat();
    float y2 = size.attribute( "height" ).toFloat();
    float type = linetype.attribute( "value" ).toInt();
    y1 -= m_pageHeight * ( m_currentPage - 1 );
    x2 += x1;
    y2 += y1;

    target.setAttribute( "svg:x1", StyleFactory::toCM( orig.attribute( "x" ) ) );
    target.setAttribute( "svg:x2", QString( "%1cm" ).arg( KoUnit::toCM( x2 ) ) );
    if ( type == 3 ) // from left bottom to right top
    {
        target.setAttribute( "svg:y1", QString( "%1cm" ).arg( KoUnit::toCM( y2 ) ) );
        target.setAttribute( "svg:y2", QString( "%1cm" ).arg( KoUnit::toCM( y1 ) ) );
    }
    else // from left top to right bottom
    {
        target.setAttribute( "svg:y1", QString( "%1cm" ).arg( KoUnit::toCM( y1 ) ) );
        target.setAttribute( "svg:y2", QString( "%1cm" ).arg( KoUnit::toCM( y2 ) ) );
    }
}

void OoImpressExport::setGraphicStyle( QDomElement & source, QDomElement & target )
{
    QString gs = m_styleFactory.createGraphicStyle( source );
    target.setAttribute( "draw:style-name", gs );
}

#include "ooimpressexport.moc"
