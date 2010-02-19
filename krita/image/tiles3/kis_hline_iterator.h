/* 
 *  Copyright (c) 2010 Lukáš Tvrdý <lukast.dev@gmail.com>
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

#ifndef _KIS_HLINE_ITERATOR_H_
#define _KIS_HLINE_ITERATOR_H_

#include "kis_iterator_ng.h"
#include "kis_datamanager.h"
#include "kis_tiled_data_manager.h"
#include "kis_tile.h"
#include "kis_types.h"
#include "kis_shared.h"

#include "kis_iterator_ng.h"

class KisHLineIterator2 : public KisShared, public KisHLineIteratorNG{

public:
    struct KisTileInfo {
        KisTileSP tile;
        KisTileSP oldtile;
        quint8* data;
        quint8* oldData;
    };


public:    
    KisHLineIterator2(KisDataManager *dataManager, qint32 x, qint32 y, qint32 w, qint32 offsetX, qint32 offsetY);
    ~KisHLineIterator2();
    KisHLineIterator2& operator=(const KisHLineIterator2&);
    
    virtual bool nextPixel();
    virtual void nextRow();
    virtual const quint8* oldRawData() const;
    virtual quint8* rawData();

    
    
private:
    KisTiledDataManager *m_dataManager;
    qint32 m_pixelSize;        // bytes per pixel
    qint32 m_x;        // current x position
    qint32 m_y;        // current y position
    qint32 m_row;    // current row in tilemgr
    qint32 m_col;    // current col in tilemgr
    quint8 *m_data;
    quint8 *m_oldData;
    qint32 m_offset;
    KisTileSP m_tile;
    KisTileSP m_oldTile;
    bool m_writable;
    bool m_havePixels;
    
protected:
    qint32 m_right;
    qint32 m_left;
    qint32 m_leftCol;
    qint32 m_rightCol;

    qint32 m_xInTile;
    qint32 m_yInTile;
    qint32 m_leftInTile;
    qint32 m_rightInTile;

    QVector<KisTileInfo> m_tilesCache;
    quint32 m_tilesCacheSize;
    
    void fetchTileData(qint32 col, qint32 row);
    
protected:
    inline void lockTile(KisTileSP &tile) {
        if (m_writable)
            tile->lockForWrite();
        else
            tile->lockForRead();
    }
    inline void lockOldTile(KisTileSP &tile) {
        // Doesn't depend on current access type
        tile->lockForRead();
    }
    inline void unlockTile(KisTileSP &tile) {
        tile->unlock();
    }

    inline quint32 xToCol(quint32 x) const {
        return m_dataManager ? m_dataManager->xToCol(x) : 0;
    }
    inline quint32 yToRow(quint32 y) const {
        return m_dataManager ? m_dataManager->yToRow(y) : 0;
    }

    inline qint32 calcOffset(qint32 x, qint32 y) const {
        return m_pixelSize *(y * KisTileData::WIDTH + x);
    }

    inline qint32 calcXInTile(qint32 x, qint32 col) const {
        return x - col * KisTileData::WIDTH;
    }

    inline qint32 calcYInTile(qint32 y, qint32 row) const {
        return y - row * KisTileData::HEIGHT;
    }



private:
    inline qint32 calcLeftInTile(qint32 col) const {
        return (col > m_leftCol) ? 0
               : m_left - m_leftCol * KisTileData::WIDTH;
    }

    inline qint32 calcRightInTile(qint32 col) const {
        return (col < m_rightCol)
               ? KisTileData::WIDTH - 1
               : m_right - m_rightCol * KisTileData::WIDTH;
    }

    void switchToTile(qint32 col, qint32 xInTile);
    KisTileInfo fetchTileDataForCache(qint32 col, qint32 row);
    void preallocateTiles(qint32 row);
    


};
#endif