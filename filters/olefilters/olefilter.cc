/* This file is part of the KDE project
   Copyright (C) 1999 Werner Trobin <wtrobin@carinthia.com>

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

#include <olefilter.h>
#include <olefilter.moc>

OLEFilter::OLEFilter(KoFilter *parent, QString name) :
                     KoFilter(parent, name) {
    olefile.data=0L;
    docfile=0L;
    store=0L;
    success=true;
}

OLEFilter::~OLEFilter() {

    delete [] olefile.data;
    olefile.data=0L;
    delete docfile;
    docfile=0L;
    delete store;
    store=0L;
}

const bool OLEFilter::filter(const QCString &fileIn, const QCString &fileOut,
                             const QCString& from, const QCString& to) {

    if(to!="application/x-kword" &&
       to!="application/x-kspread" &&
       to!="application/x-kpresenter")
        return false;
    if(from!="application/x-winword97" &&
       from!="application/x-excel97" &&
       from!="application/x-powerpoint97")
        return false;

    QFile in(fileIn);
    if(!in.open(IO_ReadOnly)) {
        kdebug(KDEBUG_ERROR, 31000, "OLEFilter::filter(): Unable to open input file!");
        in.close();
        return false;
    }

    olefile.length=in.size();
    olefile.data=new unsigned char[olefile.length];
    in.readBlock((char*)olefile.data, olefile.length);
    in.close();

    docfile=new KLaola(olefile);
    if(!docfile->isOk()) {
        kdebug(KDEBUG_ERROR, 31000, "OLEFilter::filter(): Unable to read input file correctly!");
        delete [] olefile.data;
        olefile.data=0L;
        return false;
    }

    store=new KoTarStore(fileOut, KoStore::Write);
    if(store->bad()) {
        kdebug(KDEBUG_ERROR, 31000, "OLEFilter::filter(): Unable to open output file!");
        delete [] olefile.data;
        olefile.data=0L;
        delete store;
        store=0L;
        return false;
    }

    storePath.resize(0);
    partMap.insert("root", "root");
    convert("root");
    return success;
}

void OLEFilter::slotSavePic(Picture *) {
    // we'll use Reggies Collection class here, soon (at least I hope so :)
}

void OLEFilter::slotPart(const char *nameIN, char **nameOUT) {

    if(nameIN==0)
        return;

    QMap<QString, QString>::Iterator it=partMap.find(nameIN);
    QString value;

    if(it!=partMap.end())
        value=partMap[nameIN];
    else {
        QString key=QString(nameIN);
        value="tar:";
        for(unsigned int i=0; i<storePath.size(); ++i) {
            value+='/';
            value+=QString::number(static_cast<unsigned int>(storePath[i]));
        }
        partMap.insert(key, value);
    }
    unsigned int len=value.length();
    *nameOUT=new char[len+1];
    strncpy(*nameOUT, (const char*)value, len);
    *(*nameOUT+len)='\0';
}

void OLEFilter::slotGetStream(const long &handle, myFile &stream) {
    stream=docfile->stream(handle);
}

// I can't guarantee that you get the right stream as the names
// are not unique! (searching only the current dir!)
void OLEFilter::slotGetStream(const QString &name, myFile &stream) {

    QArray<long> handle;

    handle=docfile->find(name, true);  // search only in current dir!

    if(handle.size()==1)
        stream=docfile->stream(handle[0]);
    else {
        stream.data=0L;
        stream.length=0;
    }
}

void OLEFilter::convert(const QString &dirname) {

    QList<OLENode> list=docfile->parseCurrentDir();
    OLENode *node;
    bool onlyDirs=true;
    bool resized=false;
    short index=storePath.size()-1;

    for(node=list.first(); node!=0; node=list.next()) {
        if(node->type==1) {         // it is a dir!
            if(!resized) {
                ++index;
                storePath.resize(index+1);
                storePath[index]=0;
                resized=true;
            }
            else
                ++storePath[index];
            docfile->enterDir(node->handle);
            convert(node->name);
            docfile->leaveDir();
        }
        else {
            onlyDirs=false;
        }
    }

    if(resized)
        storePath.resize(index);

    if(!onlyDirs) {
        FilterBase *myFilter=0L;
        node=list.first();

        do {
            if(node->name=="WordDocument" || node->name=="1Table" ||
               node->name=="0Table" || node->name=="ObjectPool") {

                myFile main, table0, table1, data;
                QArray<long> tmp;

                kdebug(KDEBUG_INFO, 31000, "OLEFilter::convert(): WinWord");

                main.data=0L;
                table0.data=0L;
                table1.data=0L;
                data.data=0L;

                tmp=docfile->find("WordDocument", true);
                if(tmp.size()==1)
                    main=docfile->stream(tmp[0]);

                tmp=docfile->find("0Table", true);
                if(tmp.size()==1)
                    table0=docfile->stream(tmp[0]);

                tmp=docfile->find("1Table", true);
                if(tmp.size()==1)
                    table1=docfile->stream(tmp[0]);

                tmp=docfile->find("Data", true);
                if(tmp.size()==1)
                    data=docfile->stream(tmp[0]);

                myFilter=new WordFilter(main, table0, table1, data);
                connectCommon(&myFilter);
            }
            else if(node->name=="Workbook") {
                // Excel
                kdebug(KDEBUG_INFO, 31000, "OLEFilter::convert(): Excel");

                myFile workbook;
                workbook.data=0L;

                workbook=docfile->stream(node->handle);
                myFilter=new ExcelFilter(workbook);
                connectCommon(&myFilter);
            }
            else if(node->name=="PowerPoint Document") {
                // PowerPoint
                kdebug(KDEBUG_INFO, 31000, "OLEFilter::convert(): Power Point");
                myFilter=new PowerPointFilter();
                // connect SIGNALs&SLOTs
                connectCommon(&myFilter);
            }
            else
                node=list.next();
        } while(myFilter==0L && node!=0);

        if(myFilter==0L) {
            // unknown
            kdebug(KDEBUG_INFO, 31000, "OLEFilter::convert(): superunknown -> black hole sun ;)");
            myFilter=new FilterBase();
            // connect SIGNALs&SLOTs
            connectCommon(&myFilter);
        }

        success=myFilter->filter();
        QString file=myFilter->part();
        char *tmp=0L;
        slotPart(dirname, &tmp);
        if(!store->open(tmp, "")) {
            success=false;
            kdebug(KDEBUG_INFO, 31000, "OLEFilter::convert(): Could not open KoTarStore!");
            return;
        }
        store->write((const char*)(file.utf8()), file.length());
        store->close();
        delete [] tmp;
        delete myFilter;
    }
}

void OLEFilter::connectCommon(FilterBase **myFilter) {

    QObject::connect(*myFilter, SIGNAL(signalSavePic(Picture *)), this,
                     SLOT(slotSavePic(Picture *)));
    QObject::connect(*myFilter, SIGNAL(signalPart(const char *, char **)),
                     this, SLOT(slotPart(const char *, char **)));
    QObject::connect(*myFilter, SIGNAL(signalGetStream(const long &, myFile &)), this,
                     SLOT(slotGetStream(const long &, myFile &)));
    QObject::connect(*myFilter, SIGNAL(signalGetStream(const QString &, myFile &)), this,
                     SLOT(slotGetStream(const QString &, myFile &)));
}
