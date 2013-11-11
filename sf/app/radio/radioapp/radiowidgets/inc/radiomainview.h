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

#ifndef RADIOMAINVIEW_H
#define RADIOMAINVIEW_H

// System includes
#include <QScopedPointer>

// User includes
#include "radioviewbase.h"

// Forward declarations
class RadioWindow;
class RadioStationModel;
class HbPushButton;
class RadioUiLoader;
class RadioFrequencyScanner;
class RadioStationCarousel;
class RadioFrequencyStrip;
class RadioStation;

// Class declaration
class RadioMainView : public RadioViewBase
    {
    Q_OBJECT

public:

    explicit RadioMainView();
    ~RadioMainView();

    void setScanningMode( bool scanning );

signals:

    void applicationReady();

private slots:

    void setFrequencyFromWidget( uint frequency, int reason, int direction );
    void setFrequencyFromEngine( uint frequency, int reason );
    void skip( int skipMode );
    void openStationsView();
    void startFirstTimeScanning();
    void toggleScanning();
    void toggleFavorite();
    void seekingStarted();
    void updateAntennaStatus( bool connected );
    void updateAudioRoute( bool loudspeaker );
    void setManualSeekMode( bool manualSeekActive );
    void handleFavoriteChange( const RadioStation& station );
    void saveActivity();

private:

// from base class RadioViewBase

    void preLazyLoadInit();
    void init();
    void setOrientation();
    void userAccepted();

// from base class QObject

    bool eventFilter( QObject* watched, QEvent* event );

// New functions

    void updateFavoriteButton();

private: // data

    QScopedPointer<RadioFrequencyScanner>   mFrequencyScanner;
    RadioStationCarousel*                   mCarousel;
    RadioFrequencyStrip*                    mFrequencyStrip;
    };

#endif // RADIOMAINVIEW_H
