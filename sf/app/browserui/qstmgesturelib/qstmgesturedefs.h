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

#ifndef QSTMGESTUREDEFS_H_
#define QSTMGESTUREDEFS_H_
#include <QtCore/qglobal.h>


#ifdef QSTMGESTURELIB
#   define QSTMGESTURELIB_EXPORT Q_DECL_EXPORT
#else
#   define QSTMGESTURELIB_EXPORT Q_DECL_IMPORT
#endif


namespace qstmUiEventEngine
{

// time to hold the stylus in the same position to activate holding
const int KHoldDuration = 500; // ms

// time to tap again for the second tap to be a double tap
const int KMaxDoubleTapDuration = 400; // ms

// time in which speed becomes zero if user stops stylus movement before
// lifting the pointer
const int KSpeedStopTime = 150; // ms

// tolerance in degrees to either side of an angle, for direction to be the
// same as reference angle
const qreal KAngleTolerance = 25;

// Nth pointer event before the latest point
const int KPreviousPointOffset = 3;

// Tolerance for ignoring small movements, percent of longer screen edge
const int KGestureTolerancePercent = 2; // of 100

const int KFingerSize_mm = 8;

const int KTwipsInInch = 1440;

const qreal KTwipsInMm = 56.7;

const int  KMaxTapDuration = 300; //ms

const int  KFlickSpeed = 500;

const int KDoubleTapMaxActivationInterval = 500;     // 500 ms

const int KDoubleTapMinActivationInterval = 40;   // 40 ms

const int KDoubleTapIdleInterval = 700;           // 700 ms, to prevent triple-tap effects

const int KLongTapDuration = 2000; //2 sec

const int KMinTimeBetweenDrags = 20; // 20 ms

}
#endif /* QSTMGESTUREDEFS_H_ */
