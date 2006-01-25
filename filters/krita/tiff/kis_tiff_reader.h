/*
 *  Copyright (c) 2006 Cyrille Berger <cberger@cberger.net>
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

#ifndef _KIS_TIFF_READER_H_
#define _KIS_TIFF_READER_H_

#include <tiffio.h>

/*#include <stdio.h>

#include <qvaluevector.h>

#include <kio/job.h>

#include <kis_progress_subject.h> */

#include "kis_types.h"
#include "kis_global.h"
// #include "kis_annotation.h"


#include <kis_iterator.h>

#define Q_UINT32_MAX 4294967295

class TIFFStreamBase;

class KisTIFFPostProcessor {
    public:
        KisTIFFPostProcessor(uint8 nbcolorssamples) : m_nbcolorssamples(nbcolorssamples) { }
    public:
        virtual void postProcess8bit( Q_UINT8* ) { };
        virtual void postProcess16bit( Q_UINT16* ) { };
        virtual void postProcess32bit( Q_UINT32* ) { };
    protected:
        inline uint8 nbColorsSamples() { return m_nbcolorssamples; }
    private:
        uint8 m_nbcolorssamples;
};

class KisTIFFPostProcessorInvert : public KisTIFFPostProcessor {
    public:
        KisTIFFPostProcessorInvert(uint8 nbcolorssamples) : KisTIFFPostProcessor(nbcolorssamples) {}
    public:
        virtual void postProcess8bit( Q_UINT8* data )
        {
            for(int i = 0; i < nbColorsSamples(); i++)
            {
                data[i] = Q_UINT8_MAX - data[i];
            }
        }
        virtual void postProcess16bit( Q_UINT16* data )
        {
            Q_UINT16* d = (Q_UINT16*) data;
            for(int i = 0; i < nbColorsSamples(); i++)
            {
                d[i] = Q_UINT16_MAX - d[i];
            }
        }
        virtual void postProcess32bit( Q_UINT32* data )
        {
            Q_UINT32* d = (Q_UINT32*) data;
            for(int i = 0; i < nbColorsSamples(); i++)
            {
                d[i] = Q_UINT32_MAX - d[i];
            }
        }
};

class KisTIFFPostProcessorICCLABtoCIELAB : public KisTIFFPostProcessor {
    public:
        KisTIFFPostProcessorICCLABtoCIELAB(uint8 nbcolorssamples) : KisTIFFPostProcessor(nbcolorssamples) {}
    public:
        void postProcess8bit(Q_UINT8* data)
        {
            Q_INT8* ds = (Q_INT8*) data;
            for(int i = 1; i < nbColorsSamples(); i++)
            {
                ds[i] = data[i] - Q_UINT8_MAX/2;
            }
        }
        void postProcess16bit(Q_UINT16* data)
        {
            Q_UINT16* d = (Q_UINT16*) data;
            Q_INT16* ds = (Q_INT16*) data;
            for(int i = 1; i < nbColorsSamples(); i++)
            {
                ds[i] = d[i] - Q_UINT16_MAX /2;
            }
        }
        void postProcess32bit(Q_UINT32* data)
        {
            Q_UINT32* d = (Q_UINT32*) data;
            Q_INT32* ds = (Q_INT32*) data;
            for(int i = 1; i < nbColorsSamples(); i++)
            {
                ds[i] = d[i] - Q_UINT32_MAX /2;
            }
        }
};


class KisTIFFReaderBase {
    public:
        KisTIFFReaderBase(  Q_UINT8* poses, int8 alphapos, uint8 sourceDepth, uint8 nbcolorssamples, uint8 extrasamplescount, cmsHTRANSFORM transformProfile, KisTIFFPostProcessor* postprocessor) : m_alphapos(alphapos), m_sourceDepth(sourceDepth), m_nbcolorssamples(nbcolorssamples), m_nbextrasamples(extrasamplescount), m_poses(poses), m_transformProfile(transformProfile), m_postprocess(postprocessor)
        {
            
        }
    public:
        virtual void copyDataToChannels( KisHLineIterator it, TIFFStreamBase* tiffstream) =0;
    protected:
        inline Q_UINT8 alphaPos() { return m_alphapos; }
        inline Q_UINT8 sourceDepth() { return m_sourceDepth; }
        inline Q_UINT8 nbColorsSamples() { return m_nbcolorssamples; }
        inline Q_UINT8 nbExtraSamples() { return m_nbextrasamples; }
        inline Q_UINT8* poses() { return m_poses; }
        inline cmsHTRANSFORM transform() { return m_transformProfile; }
        inline KisTIFFPostProcessor* postProcessor() { return m_postprocess; }
    private:
        Q_UINT8 m_alphapos;
        Q_UINT8 m_sourceDepth;
        Q_UINT8 m_nbcolorssamples;
        Q_UINT8 m_nbextrasamples;
        Q_UINT8* m_poses;
        cmsHTRANSFORM m_transformProfile;
        KisTIFFPostProcessor* m_postprocess;
};

class KisTIFFReaderTarget8bit : public KisTIFFReaderBase {
    public:
        KisTIFFReaderTarget8bit( Q_UINT8* poses, int8 alphapos, uint8 sourceDepth, uint8 nbcolorssamples, uint8 extrasamplescount,  cmsHTRANSFORM transformProfile, KisTIFFPostProcessor* postprocessor) : KisTIFFReaderBase(poses, alphapos, sourceDepth,  nbcolorssamples, extrasamplescount, transformProfile, postprocessor )
        {
            
        }
    public:
        virtual void copyDataToChannels( KisHLineIterator it, TIFFStreamBase* tiffstream);
};


class KisTIFFReaderTarget16bit : public KisTIFFReaderBase {
    public:
        KisTIFFReaderTarget16bit( Q_UINT8* poses, int8 alphapos, uint8 sourceDepth, uint8 nbcolorssamples, uint8 extrasamplescount, cmsHTRANSFORM transformProfile, KisTIFFPostProcessor* postprocessor) : KisTIFFReaderBase(poses, alphapos, sourceDepth,  nbcolorssamples, extrasamplescount, transformProfile, postprocessor )
        {
            
        }
    public:
        virtual void copyDataToChannels( KisHLineIterator it, TIFFStreamBase* tiffstream) ;
};

class KisTIFFReaderFromPalette : public  KisTIFFReaderBase {
    public:
        KisTIFFReaderFromPalette( uint16 *red, uint16 *green, uint16 *blue, Q_UINT8* poses, int8 alphapos, uint8 sourceDepth, uint8 nbcolorssamples, uint8 extrasamplescount, cmsHTRANSFORM transformProfile, KisTIFFPostProcessor* postprocessor) : KisTIFFReaderBase(poses, alphapos, sourceDepth,  nbcolorssamples, extrasamplescount, transformProfile, postprocessor ), m_red(red), m_green(green), m_blue(blue)
        {
            
        }
    public:
        virtual void copyDataToChannels( KisHLineIterator it, TIFFStreamBase* tiffstream) ;
    private:
        uint16 *m_red,  *m_green, *m_blue;
};

// class KisTIFFReaderBaseTarget32bit {
//     public:
//         KisTIFFReaderBaseTarget32bit( int8 alphapos, uint8 sourceDepth, uint8 nbcolorssamples, uint8 extrasamplescount, Q_UINT8* poses, cmsHTRANSFORM transformProfile, KisTIFFPostProcessor postprocessor) : KisTiffReader(alphapos, sourceDepth,  nbcolorssamples, extrasamplescount, poses, transformProfile, postprocessor )
//         {
//             
//         }
//     public:
//         virtual void copyDataToChannels( KisHLineIterator it, TIFFStream* tiffstream) =0;
// };

#endif
