/*
** A program to convert the XML rendered by KWord into LATEX.
**
** Copyright (C) 2000, 2001, 2002 Robert JACOLIN
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Library General Public
** License as published by the Free Software Foundation; either
** version 2 of the License, or (at your option) any later version.
**
** This library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** Library General Public License for more details.
**
** To receive a copy of the GNU Library General Public License, write to the
** Free Software Foundation, Inc., 59 Temple Place - Suite 330,
** Boston, MA  02111-1307, USA.
**
*/

#include <stdlib.h>		/* for atoi function    */

#include <kdebug.h>
#include <ktempfile.h>
#include <koStore.h>

#include <qdir.h>

#include "fileheader.h"
#include "document.h"
#include "textFrame.h"
#include "formula.h"
#include "pixmapFrame.h"

/*******************************************/
/* Constructor                             */
/*******************************************/
Document::Document()
{
}

/*******************************************/
/* Destructor                              */
/*******************************************/
Document::~Document()
{
	kdDebug() << "Document destructor" << endl;
}

/*******************************************/
/* Analyse                                 */
/*******************************************/
void Document::analyse(const QDomNode balise)
{
	//QDomNode balise = getChild(balise_initial, "FRAMESET");
	kdDebug() << getChildName(balise, 0) << endl;
	for(int index= 0; index < getNbChild(balise); index++)
	{
		Element *elt = 0;
		kdDebug() << "--------------------------------------------------" << endl;
		
		kdDebug() << getChildName(balise, index) << endl;
		switch(getTypeFrameset(getChild(balise, index)))
		{
			case ST_NONE: 
				kdDebug() << "NONE" << endl;
				break;
			case ST_TEXT: 
				kdDebug() << "TEXT" << endl;
				elt = new TextFrame;
				elt->analyse(getChild(balise, index));
				break;
			case ST_PICTURE:
				kdDebug() << "PICTURE" << endl;
				elt = new PixmapFrame();
				elt->analyse(getChild(balise, index));
				break;
			case ST_PART:
				kdDebug() << "PART" << endl;
				//elt = new Part;
				//elt->analyse(getChild(balise, index));
				break;
			case ST_FORMULA:
				/* Just save the frameset in a QString input
				 * call the formula latex export filter
				 * save in output
				 * generate : write the output
				 */
				kdDebug() << "FORMULA" << endl;
				elt = new Formula;
				elt->analyse(getChild(balise, index));
				break;
			case ST_HLINE:
				kdDebug() << "HLINE" << endl;
				break;
			default:
				kdDebug() << "error " << elt->getType() << " " << ST_TEXT << endl;
		}
			
		/* 3. Add the Element in one of the lists */
		if(elt != 0)
		{
			kdDebug() << "INFO : " << elt->getSection() << endl;
			switch(elt->getSection())
			{
				case SS_FOOTERS: kdDebug() << " FOOTER" <<endl;
					       _footers.append(elt);
					       break;
				case SS_HEADERS: kdDebug() << " HEADER" << endl;
						_headers.append(elt);
					break;
				case SS_BODY:
					if(!elt->isTable())
					{
						switch(elt->getType())
						{
							case ST_TEXT:
									_corps.append(elt);
									kdDebug() << " BODY" << endl;
								break;
							case ST_PART:
									kdDebug() << " PART" <<endl;
									//_parts.append(elt);
								break;
							case ST_FORMULA:
									kdDebug() << " FORMULA" <<endl;
									_formulas.append(elt);
								break;
							case ST_PICTURE:
									kdDebug() << " PIXMAP" <<endl;
									_pixmaps.append(elt);
								break;
							default:
									kdError() << "Element frame type no supported or type unexpected." << endl;
						}
					}
					break;
				case SS_TABLE:
					kdDebug() << " TABLE" <<endl;
					/* Don't add simplely the cell */
					/* heriter ListTable de ListElement et surcharger
					 * la methode add. Une cellule est un element.
					 */
					_tables.add(elt);
					FileHeader::instance()->useTable();
					break;
				case SS_FOOTNOTES: /* Just for the new kwd file version */
						_footnotes.append(elt);
				break;
				default: kdDebug() << "UNKNOWN" << endl;
					break;
			}
		}
		kdDebug() << "END OF ANALYSE OF A FRAMESET" << endl;
	}
}

/*******************************************/
/* AnalysePixmaps                          */
/*******************************************/
void Document::analysePixmaps(const QDomNode balise)
{
	//QDomNode balise = getChild(balise_initial, "FRAMESET");
	for(int index= 0; index < getNbChild(balise); index++)
	{
		Key *key = 0;
		kdDebug() << "NEW PIXMAP" << endl;
		
		key = new Key(Key::PIXMAP);
		key->analyse(getChild(balise, "KEY"));
		_keys.append(key);
	}
}

/*******************************************/
/* getTypeFrameset                         */
/*******************************************/
SType Document::getTypeFrameset(const QDomNode balise)
{
	SType type = ST_NONE;
	type = (SType) getAttr(balise, "frameType").toInt();
	return type;
}

/*******************************************/
/* Generate                                */
/*******************************************/
void Document::generate(QTextStream &out, bool hasPreambule)
{
	kdDebug() << "DOC. GENERATION." << endl;
	
	if(hasPreambule)
		generatePreambule(out);
	kdDebug() << "preambule : " << hasPreambule << endl;

	/* Body */
	kdDebug() << endl << "body : " << _corps.count() << endl;

	if(hasPreambule)
	{
		out << "\\begin{document}" << endl;
		Config::instance()->indent();
	}
	QString dir = "";
	if(Config::instance()->getPicturesDir() != "" && 
			Config::instance()->getPicturesDir() != NULL && 
			FileHeader::instance()->hasGraphics())
	{
		out << endl << "\\graphicspath{{" << Config::instance()->getPicturesDir() << "}}" << endl;
	}

	if(_corps.getFirst() != 0)
		_corps.getFirst()->generate(out);

	/* Just for test */
	/*if(_tables.getFirst() != 0)
		_tables.getFirst()->generate(out);
	if(_formulas.getFirst() != 0)
		_formulas.getFirst()->generate(out);*/
	if(hasPreambule)
		out << "\\end{document}" << endl;
	Config::instance()->desindent();
	if(Config::instance()->getIndentation() != 0)
			kdError() << "Error : indent != 0 at the end ! " << endl;
}

/*******************************************/
/* GeneratePreambule                       */
/*******************************************/
void Document::generatePreambule(QTextStream &out)
{
	Element* header;
	Element* footer;

	/* For each header */
	if(FileHeader::instance()->hasHeader())
	{
		kdDebug() << "header : " << _headers.count() << endl;

		/* default : no rule */
		out << "\\renewcommand{\\headrulewidth}{0pt}" << endl;
		for(header = _headers.first(); header != 0; header = _headers.next())
		{
			generateTypeHeader(out, header);
		}
	}
	
	/* For each footer */
	if(FileHeader::instance()->hasFooter())
	{
		kdDebug() << "footer : " << _footers.count() << endl;

		/* default : no rule */
		out << "\\renewcommand{\\footrulewidth}{0pt}" << endl;
		for(footer = _footers.first(); footer != 0; footer = _footers.next())
		{
			generateTypeFooter(out, footer);
		}
	}
	/* Specify what header/footer style to use */
	if(FileHeader::instance()->hasHeader() || FileHeader::instance()->hasFooter())
		out << "\\pagestyle{fancy}" << endl;
	else
	{
		out << "\\pagestyle{empty}" << endl;
	}
}

/*******************************************/
/* GenerateTypeHeader                      */
/*******************************************/
void Document::generateTypeHeader(QTextStream &out, Element *header)
{
	kdDebug() << "generate header" << endl;
	if((FileHeader::instance()->getHeadType() == FileHeader::TH_ALL ||
		FileHeader::instance()->getHeadType() == FileHeader::TH_FIRST) && header->getInfo() == SI_EVEN)
	{
		out << "\\fancyhead[L]{}" << endl;
		out << "\\fancyhead[C]{";
		header->generate(out);
		out << "}" << endl;
		out << "\\fancyhead[R]{}" << endl;
	}

	switch(header->getInfo())
	{
		case SI_NONE:
		case SI_FIRST:
			break;
		case SI_ODD:
			out << "\\fancyhead[RO]{}" << endl;
			out << "\\fancyhead[CO]{";
			header->generate(out);
			out << "}" << endl;
			out << "\\fancyhead[LO]{}" << endl;
			break;
		case SI_EVEN:
			out << "\\fancyhead[RE]{}" << endl;
			out << "\\fancyhead[CE]{";
			header->generate(out);
			out << "}" << endl;
			out << "\\fancyhead[LE]{}" << endl;
			break;
	}
	
	if(header->getInfo() == SI_FIRST)
	{
		out << "\\fancyhead{";
		header->generate(out);
		out << "}" << endl;
		out << "\\thispagestyle{fancy}" << endl;
	}
}

/*******************************************/
/* GenerateTypeFooter                      */
/*******************************************/
void Document::generateTypeFooter(QTextStream &out, Element *footer)
{
	if(FileHeader::instance()->getFootType() == FileHeader::TH_ALL &&
			footer->getInfo() == SI_EVEN)
	{
		out << "\\fancyfoot[L]{}" << endl;
		out << "\\fancyfoot[C]{";
		footer->generate(out);
		out << "}" << endl;
		out << "\\fancyfoot[R]{}" << endl;
	}
	else if(FileHeader::instance()->getFootType() == FileHeader::TH_EVODD)
	{
		switch(footer->getInfo())
		{
			case SI_NONE:
			case SI_FIRST:
				break;
			case SI_ODD:
				out << "\\fancyfoot[CO]{";
				footer->generate(out);
				out << "}";;
				break;
			case SI_EVEN:
				out << "\\fancyfoot[CE]{";
				footer->generate(out);
				out << "}";
				break;
		}
	}
	else if(FileHeader::instance()->getFootType() == FileHeader::TH_FIRST &&
			footer->getInfo() == SI_FIRST)
	{
		out << "\\fanycfoot{";
		footer->generate(out);
		out << "}" << endl;
		out << "\\thispagestyle{fancy}" << endl;
	}
}

Element* Document::searchAnchor(QString anchor)
{
	Element *elt = _tables.first();
	while(elt != 0)
	{
		kdDebug() << elt->getGrpMgr() << endl;
		if(elt->getGrpMgr() == anchor)
			return elt;
		elt = _tables.next();
	}
	kdDebug() << "No in table, search in formulae list." << endl;
	elt = _formulas.first();
	while(elt != 0)
	{
		if(elt->getName() == anchor)
			return elt;
		elt = _formulas.next();
	}
	kdDebug() << "No in table and formulae, search in pictures." << endl;
	elt = _pixmaps.first();
	while(elt != 0)
	{
		if(elt->getName() == anchor)
			return elt;
		elt = _pixmaps.next();
	}
	return NULL;

}

Element* Document::searchFootnote(QString footnote)
{
	Element* elt = _footnotes.first();
	while(elt != 0)
	{
		if(elt->getName() == footnote)
			return elt;
		elt = _footnotes.next();
	}
	return NULL;

}

Key* Document::searchKey(QString keyName)
{
	Key* key = _keys.first();
	while(key != 0)
	{
		kdDebug() << "key " << key->getFilename() << endl;
		if(key->getFilename() == keyName)
			return key;
		key = _keys.next();
	}
	return NULL;

}

QString Document::extractData(QString key)
{
	QString data = searchKey(key)->getName();
	kdDebug() << "Opening " << data << endl;
	if(!getStorage()->isOpen())
	{
		if(!getStorage()->open(data))
		{
			kdError() << "Unable to open " << data << endl;
			return QString("");
		}
	}

	/* Temp file with the default name in the default temp dir */
	KTempFile temp;
	//temp.setAutoDelete(true);
	QFile* tempFile = temp.file();

	const Q_LONG buflen = 4096;
	char buffer[ buflen ];
	Q_LONG readBytes = getStorage()->read( buffer, buflen );

	while ( readBytes > 0 )
	{
		tempFile->writeBlock( buffer, readBytes );
		readBytes = getStorage()->read( buffer, buflen );
	}
	temp.close();
	if(!getStorage()->close())
	{
		kdError() << "Unable to close " << data << endl;
		return QString("");
	}
	kdDebug() << "temp filename : " << temp.name() << endl;
	return temp.name();
}
