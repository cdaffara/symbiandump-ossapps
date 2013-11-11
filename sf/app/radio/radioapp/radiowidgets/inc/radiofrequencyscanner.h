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

#ifndef RADIOPRESETSCANNER_H_
#define RADIOPRESETSCANNER_H_

// System includes
#include <QObject>
#include <QSharedPointer>

// Forward declarations
class HbProgressDialog;
class RadioStation;
class RadioUiEngine;
class RadioWindow;
class RadioFrequencyStrip;
class RadioStationCarousel;
class RadioScannerEngine;

// Class declaration
class RadioFrequencyScanner : public QObject
{
    Q_OBJECT

public:

    RadioFrequencyScanner( RadioUiEngine& uiEngine, QObject* parent );
    ~RadioFrequencyScanner();

    void startScanning();

    bool isAlive() const;

signals:

    void frequencyScannerFinished();

public slots:

    void cancelScanning();

private slots:

    void delayedStart();
    void updateScanProgress( const RadioStation& station );
    void continueScanning();
    void restoreUiControls();

private:

    void finishScanning();

private: // data

    /*!
     * Reference to the Ui engine
     */
    RadioUiEngine&                      mUiEngine;

    bool                                mInMainView;

    QSharedPointer<RadioScannerEngine>  mScannerEngine;

    /**
     * Scanning progress note
     * Own.
     */
    QWeakPointer<HbProgressDialog>      mScanningProgressNote;

    int                                 mStripScrollTime;
    int                                 mCarouselScrollTime;

    bool                                mIsAlive;

    bool                                mUserCanceled;

};

#endif // RADIOPRESETSCANNER_H_
