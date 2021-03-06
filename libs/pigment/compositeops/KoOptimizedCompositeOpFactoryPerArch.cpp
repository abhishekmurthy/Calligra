/*
 *  Copyright (c) 2012 Dmitry Kazakov <dimula73@gmail.com>
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

#include "KoOptimizedCompositeOpFactoryPerArch.h"

#include <QDebug>

#include "KoOptimizedCompositeOpAlphaDarken32.h"
#include "KoOptimizedCompositeOpOver32.h"

template<>
template<>
KoOptimizedCompositeOpFactoryPerArch<KoOptimizedCompositeOpAlphaDarken32>::ReturnType
KoOptimizedCompositeOpFactoryPerArch<KoOptimizedCompositeOpAlphaDarken32>::create<VC_IMPL>(ParamType param)
{
    return new KoOptimizedCompositeOpAlphaDarken32<VC_IMPL>(param);
}

template<>
template<>
KoOptimizedCompositeOpFactoryPerArch<KoOptimizedCompositeOpOver32>::ReturnType
KoOptimizedCompositeOpFactoryPerArch<KoOptimizedCompositeOpOver32>::create<VC_IMPL>(ParamType param)
{
    return new KoOptimizedCompositeOpOver32<VC_IMPL>(param);
}


#define __stringify(_s) #_s
#define stringify(_s) __stringify(_s)

#ifdef __SSE2__
#  define HAVE_SSE2 1
#else
#  define HAVE_SSE2 0
#endif

#ifdef __SSE3__
#  define HAVE_SSE3 1
#else
#  define HAVE_SSE3 0
#endif

#ifdef __SSSE3__
#  define HAVE_SSSE3 1
#else
#  define HAVE_SSSE3 0
#endif

#ifdef __SSE4_1__
#  define HAVE_SSE4_1 1
#else
#  define HAVE_SSE4_1 0
#endif

#ifdef __SSE4_2__
#  define HAVE_SSE4_2 1
#else
#  define HAVE_SSE4_2 0
#endif

#ifdef __SSE4a__
#  define HAVE_SSE4a 1
#else
#  define HAVE_SSE4a 0
#endif

#ifdef __AVX__
#  define HAVE_AVX 1
#else
#  define HAVE_AVX 0
#endif

inline void printFeatureSupported(const QString &feature,
                                  bool present)
{
    qDebug() << "\t" << feature << "\t---\t" << (present ? "yes" : "no");
}

template<>
KoReportCurrentArch::ReturnType
KoReportCurrentArch::create<VC_IMPL>(ParamType)
{
    qDebug() << "Compiled for arch:" << stringify(VC_IMPL);
    qDebug() << "Features supported:";
    printFeatureSupported("SSE2", HAVE_SSE2);
    printFeatureSupported("SSE3", HAVE_SSE3);
    printFeatureSupported("SSSE3", HAVE_SSSE3);
    printFeatureSupported("SSE4.1", HAVE_SSE4_1);
    printFeatureSupported("SSE4.2", HAVE_SSE4_2);
    printFeatureSupported("SSE4a", HAVE_SSE4a);
    printFeatureSupported("AVX ", HAVE_AVX);
}
