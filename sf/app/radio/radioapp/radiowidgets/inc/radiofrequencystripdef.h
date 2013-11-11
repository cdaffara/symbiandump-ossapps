/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/

#ifndef RADIOFREQUENCYSTRIPDEF_H
#define RADIOFREQUENCYSTRIPDEF_H

#include <QLatin1String>
#include <QFont>

namespace Scroll
{
    enum Direction
    {
        Shortest,
        Left,
        Right
    };
}

namespace FrequencyStrip
{

    struct StationMarker
    {
        StationMarker() : mFrequency( 0 ), mIsFavorite( false ) {}
        StationMarker( uint frequency, bool isFavorite ) : mFrequency( frequency ), mIsFavorite( isFavorite ) {}
        uint mFrequency;
        bool mIsFavorite;
    };

    enum FrequencyChangeReason
    {
        Unspecified = 0,
        ManualSeekUpdate,
        ManualSeekTune
    };

    // Frequency lines
    const int TAB_HEIGHT_SMALL = 12;
    const int TAB_HEIGHT_BIG = 18;
    const int PEN_WIDTH_FAVORITE = 4;
    const qreal INDICATOR_WIDTH = 2.0;

    const qreal ROUNDER = 0.5;
    const int SELECTOR_WIDTH = 2;
    const int SELECTOR_Z_POS = 100;
    const int SELECTOR_MARGIN = 3;

    const int ONE_HERTZ = 1000000;
    const int HALF_HERTZ = ONE_HERTZ / 2;
    const int ONE_TAB_DISTANCE = 20;
    const uint ONE_TAB_IN_HZ = uint( 0.2 * ONE_HERTZ );
    const qreal PIXEL_IN_HZ = ONE_TAB_IN_HZ / ONE_TAB_DISTANCE;
    const qreal PIXMAP_OVERLAP = qreal( ONE_TAB_DISTANCE ) / 2;
    const int ITEM_WIDTH = ONE_TAB_DISTANCE * 5;
    const int PIXMAP_WIDTH = ITEM_WIDTH + (int)PIXMAP_OVERLAP;

    const uint ONE_HUNDRED_KHZ = 100000;

    static const QLatin1String DECIMAL_TEXT( ".0" );

    #ifdef BUILD_WIN32
        const int FREQUENCY_FONT_SIZE = 12;
        const int DECIMAL_FONT_SIZE = 8;
    #else
        const int FREQUENCY_FONT_SIZE = 6;
        const int DECIMAL_FONT_SIZE = 4;
    #endif // BUILD_WIN32

    const QFont FREQUENCY_FONT( "", FREQUENCY_FONT_SIZE, QFont::Bold );
    const QFont DECIMAL_FONT( "", DECIMAL_FONT_SIZE, QFont::Bold );
    const int FREQUENCY_TEXT_Y_POS = 25;
    const int STATION_MARKER_Y_POS = 10;

    static const QLatin1String SLIDE_TO_LEFT    ( "SlideToLeft" );
    static const QLatin1String SLIDE_FROM_LEFT  ( "SlideFromLeft" );
    static const QLatin1String SLIDE_TO_RIGHT   ( "SlideToRight" );
    static const QLatin1String SLIDE_FROM_RIGHT ( "SlideFromRight" );
    static const QLatin1String LEFT_BUTTON      ( "tv:left_button" );
    static const QLatin1String RIGHT_BUTTON     ( "tv:right_button" );

    static const QLatin1String TEXT_COLOR_ATTRIBUTE( "text" );
    const int MANUALSEEK_START_TIMEOUT = 400;
    const int MANUALSEEK_END_TIMEOUT = 2000;
    const int MANUALSEEK_SIGNAL_DELAY = 300;

}

#endif // RADIOFREQUENCYSTRIPDEF_H
