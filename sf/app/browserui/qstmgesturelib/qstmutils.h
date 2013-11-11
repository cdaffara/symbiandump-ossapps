/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation, version 2.1 of the License.
* 
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this program.  If not, 
* see "http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html/".
*
* Description:
*
*/

#ifndef _GESTURE_UTILS_H_
#define _GESTURE_UTILS_H_

#include <QtCore>
#include <QtGui>
#ifdef Q_OS_SYMBIAN
#include <e32math.h>
#define SQRT(out,x) Math::Sqrt(out,x)
#else
#define SQRT(out,x) (out = sqrt(x))
#endif

#define MAX(a,b) ((a>=b) ? a : b)
#define MIN(a,b) ((a<=b) ? a : b)
#define ABS(x)   ((x>=0) ? x : -x)

namespace qstmUiEventEngine
{

    class QStm_Utils
    {
    public:
        static QRect  toleranceRectMm( const QPoint& aCenterPoint, int size_mm );
        static QRect  toleranceRectPx( const QPoint& aCenterPoint, int size_px );
        static long   mm2Pixels(long mm);
        static long   inches2Pixels(double inches);
        static qreal  distanceF(const QPoint& p1, const QPoint& p2);
        static int    distance(const QPoint& p1, const QPoint& p2);
        static int    longerEdge( const QSize& size );
    };
}

#endif // _GESTURE_UTILS_H_
