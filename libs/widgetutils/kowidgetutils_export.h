/* This file is part of the KDE project
   Copyright (C) 2012 Jarosław Staniek <staniek@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef KOWIDGETUTILS_EXPORT_H
#define KOWIDGETUTILS_EXPORT_H

/* needed for KDE_EXPORT and KDE_IMPORT macros */
#include <kdemacros.h>

/* We use _WIN32/_WIN64 instead of Q_OS_WIN so that this header can be used from C files too */
#if defined _WIN32 || defined _WIN64

#ifndef KOWIDGETUTILS_EXPORT
# if defined(MAKE_KOWIDGETUTILS_LIB)
/* We are building this library */
#  define KOWIDGETUTILS_EXPORT KDE_EXPORT
# else
/* We are using this library */
#  define KOWIDGETUTILS_EXPORT KDE_IMPORT
# endif
#endif

#else /* UNIX */

#define KOWIDGETUTILS_EXPORT KDE_EXPORT

#endif

#endif
