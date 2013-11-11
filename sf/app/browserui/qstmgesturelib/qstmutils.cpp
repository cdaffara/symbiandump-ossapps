/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Gesture recognition utilities
*
*/

#include "qstmutils.h"
#include "qstmgesturedefs.h"

using namespace qstmUiEventEngine;

inline int QStm_Utils::longerEdge( const QSize& size )
{
	return MAX( size.height(), size.width() );
}


QRect QStm_Utils::toleranceRectMm( const QPoint& centerPoint, int size_mm )
{
	long toleranceLength = mm2Pixels(size_mm) / 2;
	return toleranceRectPx(centerPoint, toleranceLength);
}

QRect QStm_Utils::toleranceRectPx( const QPoint& centerPoint, int size_px )
{
	QRect toleranceRect( QPoint(0,0), QSize(size_px, size_px) );
	// grow by the tolerance length, while keeping the center point
	toleranceRect.moveCenter(centerPoint);
	return toleranceRect;
}


long QStm_Utils::mm2Pixels(long mm)
{
	//return Twips2Pixels(mm * KTwipsInMm);
	return mm * QApplication::desktop()->height() / QApplication::desktop()->heightMM();
}

long QStm_Utils::inches2Pixels(double inches)
	{
	//return Twips2Pixels(inches * KTwipsInInch);
	return inches * 25.4 * QApplication::desktop()->height() / QApplication::desktop()->heightMM();
	}

int QStm_Utils::distance(const QPoint& p1, const QPoint& p2)
{
/*
	double d = dx * dx + dy * dy;
	double dist ;
	Math::Sqrt(dist, d) ;
	return dist;
*/
	QPoint diff = p1 - p2;
	int x = diff.x();
	int y = diff.y();
	
	if(x<0) x=-x;
	if(y<0) y=-y;
	if(x < y) {
	  int t = x;
	  x = y;
	  y = t;        // ensures that x >= y
	}
	int dist = (y < ((13107 * x)>>15)) ?    // * (.4)
			  (x + ((y * 6310)>>15)) :      // * (.192582403)
			  (((x * 27926)>>15)            // * (.852245894)
				 + ((y * 18414)>>15));      // * (.561967668)
	return dist;
}


qreal QStm_Utils::distanceF(const QPoint& p1, const QPoint& p2)
{
	QPoint diff = p1 - p2;
	double dist;
	SQRT(dist, ((qreal)(diff.x() * diff.x() + diff.y() * diff.y()))) ;
	return dist;
}

