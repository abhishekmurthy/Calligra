/*
 * This file is part of Krita
 *
 * Copyright (c) 2007 Cyrille Berger <cberger@cberger.net>
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
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef _KIS_FILTERS_MODEL_H_
#define _KIS_FILTERS_MODEL_H_

#include <QAbstractItemModel>

#include <kis_types.h>

class KisFilter;

/**
 *
 */
class KisFiltersModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    KisFiltersModel(KisPaintDeviceSP thumb);
    ~KisFiltersModel();
    virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    virtual QModelIndex parent(const QModelIndex &child) const;
    QModelIndex indexForFilter(const QString& id);
    const KisFilter* indexToFilter(const QModelIndex& idx);
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    virtual Qt::ItemFlags flags(const QModelIndex & index) const;
private slots:
    void previewUpdated(int i);
private:
    struct Private;
    Private* const d;
};

#endif
