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

#include <KIllustrator_doc.h>
#include <KIllustrator_view.h>
#include <KIllustrator_factory.h>

#include <GDocument.h>
#include <GPart.h>
#include <Coord.h>
#include <GLayer.h>

#include <kmessagebox.h>
#include <qstring.h>
#include <qfileinfo.h>
#include <qdom.h>
#include <klocale.h>
#include <kstddirs.h>
#include <koTemplateChooseDia.h>
#include <koQueryTrader.h>
#include <koStore.h>
#include <kdebug.h>

KIllustratorChild::KIllustratorChild (KIllustratorDocument* killu,
                                      KoDocument* doc,
                                      const QRect& rect )
  : KoDocumentChild ( killu, doc, rect )
{
}

KIllustratorChild::~KIllustratorChild ()
{
}

KoDocument *KIllustratorChild::hitTest( const QPoint &, const QWMatrix & )
{
    return 0L;
}
// ----------------------------------------------------------

KIllustratorDocument::KIllustratorDocument( QWidget *parentWidget, const char *widgetName, QObject* parent, const char* name, bool singleViewMode )
    : KoDocument( parentWidget, widgetName, parent, name, singleViewMode )
{
    setInstance( KIllustratorFactory::global() );
    m_gdocument = new GDocument(this);
    connect(m_gdocument, SIGNAL(wasModified(bool)), this, SLOT(modified(bool)));
    GObject::registerPrototype ("object", new GPart());
}

KIllustratorDocument::~KIllustratorDocument()
{
    delete m_gdocument;
}

bool KIllustratorDocument::loadXML (QIODevice *, const QDomDocument &doc)
{
  if ( m_gdocument->readFromXml (doc)) {
    // now look for part objects in order to create the child list
    QListIterator<GLayer> i(m_gdocument->getLayers());
    for ( ; i.current(); ++i) {
      GLayer* layer = *i;
      const QList<GObject>& contents = layer->objects ();
      for (QListIterator<GObject> oi(contents); oi.current(); ++oi) {
        if ((*oi)->isA ("GPart")) {
          GPart *part = (GPart *) *oi;
          insertChild (part->getChild ());
        }
      }
    }
    return true;
  }
  return false;
}

bool KIllustratorDocument::loadChildren (KoStore* store)
{
  QListIterator<KoDocumentChild> it ( children() );
  for (; it.current (); ++it) {
    if (! ((KoDocumentChild*)it.current())->loadDocument (store))
      return false;
  }
  return true;
}

bool KIllustratorDocument::saveChildren (KoStore* _store, const QString &_path)
{
  int i = 0;
  QListIterator<KoDocumentChild> it ( children() );
  for( ; it.current(); ++it )
  {
    QString path = QString( "%1/%2" ).arg( _path ).arg( i++ );
    if ( !((KoDocumentChild*)it.current())->document()->saveToStore( _store, path ) )
      return false;
  }
  return true;
}

// I admire that piece of art, so I didn't dare touching it... (Werner)
/*
bool KIllustratorDocument::completeSaving (KoStore* store)
{
  if (!store)
    return true;

  return true;
}
*/

void KIllustratorDocument::insertPart (const QRect& rect, KoDocumentEntry& e)
{
    KoDocument* doc = e.createDoc(this);

    if ( !doc || ! doc->initDoc() )
    {
        KMessageBox::error((QWidget *) 0L, i18n ("KIllustrator Error"),
                               i18n ("Could not insert document"));
        return;
    }

    KIllustratorChild *child = new KIllustratorChild (this, doc, rect );
    insertChild( child );

    GPart* part = new GPart (child);
    m_gdocument->insertObject (part);
    emit partInserted (child, part);
}

void KIllustratorDocument::insertChild( KoDocumentChild* child )
{
    KoDocument::insertChild( child );
    setModified (true);
}

void KIllustratorDocument::changeChildGeometry (KIllustratorChild* child, const QRect& r)
{
  child->setGeometry (r);
  setModified (true);
  emit childGeometryChanged (child);
}

bool KIllustratorDocument::initDoc()
{
  return insertNewTemplate (0, 0, true);
}

KoView* KIllustratorDocument::createViewInstance( QWidget* parent, const char* name )
{
    return new KIllustratorView( parent, name, this );
}

void KIllustratorDocument::paintContent( QPainter& painter, const QRect& rect, bool transparent )
{
    Rect r( (float)rect.x(), (float)rect.y(), (float)rect.width(), (float)rect.height() );

    if ( !transparent )
        painter.fillRect( rect, white );
    m_gdocument->drawContentsInRegion( painter, r );
}

GDocument* KIllustratorDocument::gdoc()
{
    return m_gdocument;
}

bool KIllustratorDocument::insertNewTemplate (int, int, bool) {
  QString templ;
  KoTemplateChooseDia::ReturnType ret;

  ret = KoTemplateChooseDia::choose (KIllustratorFactory::global(),
                                     templ,
                                     "application/x-killustrator", "*.kil",
                                     i18n("KIllustrator"),
                                     KoTemplateChooseDia::Everything,
                                     "killustrator_template");
  if (ret == KoTemplateChooseDia::Template) {
    QFileInfo fileInfo (templ);
    QString fileName (fileInfo.dirPath (true) + "/" +
                      fileInfo.baseName () + ".kil");
    // load it
    bool ok = loadNativeFormat (fileName);
    setModified (true);
    return ok;
  } else if (ret == KoTemplateChooseDia::File) {
    // load it
    KURL url;
    url.setPath (templ);
    bool ok = openURL (url);
    return ok;
  } else if ( ret == KoTemplateChooseDia::Empty ){
    return true;
  } else
    return false;
}

#include <KIllustrator_doc.moc>
