/*
 *  channel.h - part of KImageShop
 *
 *  Copyright (c) 1999 Andrew Richards <A.Richards@phys.canterbury.ac.nz>
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
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef CHANNEL_H
#define CHANNEL_H

#include <qstring.h>
#include <qimage.h>
#include <qlist.h>
#include <qrect.h>
#include <qpoint.h>
#include <qcolor.h>


struct tileDescriptor {
	uchar *data; // pointer to memory block containing the tile graphic
               // 0 implies not allocated ie constant colour or blank
  QColor backGroundColour;
  
};

// imageExtents() - reports the extents of the image in canvas coords
//    when the channel has to do some internal resizing/reallocation
//    the value returned by this method is updated

class Channel
{

public:
	Channel(int w=0, int h=0);

	void loadViaQImage(QImage img, int channel);
	void resizeChannel(QPoint newPoint, bool init=false);
	void moveBy(int dx, int dy);
	void moveTo(int x, int y);
	void setPixel(int x, int y, uchar val);
	uchar getPixel(int x, int y);


	// Accessor methods
	int     xTiles()       { return xTilesNo;  };
	int     yTiles()       { return yTilesNo;  };
	QRect   tileExtents()  { return tilesRect; };
	QRect   imageExtents() { return imageRect; };
	uchar** tileBlock()    { return tileInfo;  };
	int     width()        { return imageRect.width();  };
	int     height()       { return imageRect.height(); };
	int lastTileOffsetX()  { return lastTileXOffset;    };
	int lastTileOffsetY()  { return lastTileYOffset;    };
	QPoint  offset()       { return offsetPoint; };

	// Debugging
	void dumpTileBlock();

 private:
	QRect   imageRect, tilesRect;
	QPoint  offsetPoint;
	int     xTilesNo, yTilesNo, lastTileXOffset, lastTileYOffset;
	uchar** tileInfo;
};



#endif
