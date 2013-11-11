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

#ifndef RADIOFREQUENCYSTRIP_H
#define RADIOFREQUENCYSTRIP_H

// User includes
#include "radiofrequencystripbase.h"
#include "radio_global.h"
#include "radiowidgetsexport.h"

// Forward declarations
class RadioUiEngine;
class RadioStation;
class RadioUiLoader;

typedef QList<RadioStation> StationList;

// Class declaration
class WIDGETS_DLL_EXPORT RadioFrequencyStrip : public RadioFrequencyStripBase
{
    Q_OBJECT

public:

    RadioFrequencyStrip();

    void init( RadioUiEngine* engine, RadioUiLoader& uiLoader );

    void setFrequency( const uint newFrequency, int reason, Scroll::Direction direction = Scroll::Shortest );

signals:

    void frequencyChanged( uint frequency, int reason, int direction );
    void skipRequested( int skipMode );
    void seekRequested( int seekDirection );

private:

// from base class RadioFrequencyStripBase

    bool isInitialized() const;
    QList<FrequencyStrip::StationMarker> stationsInRange( uint minFrequency, uint maxFrequency ) const;
    void emitFrequencyChanged( uint frequency, int reason, int direction );
    void skipToPrevious();
    void skipToPreviousFavorite();
    void skipToNext();
    void skipToNextFavorite();
    void seekDown();
    void seekUp();
    bool isScanning() const;
    uint frequencyAtIndex( int index ) const;

private: // data

    RadioUiEngine*              mUiEngine;

};

#endif // RADIOFREQUENCYSTRIP_H
