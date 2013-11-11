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

#ifndef RADIOSCANNERENGINE_H
#define RADIOSCANNERENGINE_H

// System includes
#include <QObject>
#include <QScopedPointer>
#include <QMap>

// User includes
#include "radiouiengineexport.h"

// Forward declarations
class RadioScannerEnginePrivate;
class RadioUiEnginePrivate;
class RadioStationHandlerIf;
class RadioStation;

// Constants

// Class declaration
class UI_ENGINE_DLL_EXPORT RadioScannerEngine : public QObject
    {
    Q_OBJECT
    Q_DECLARE_PRIVATE_D( d_ptr.data(), RadioScannerEngine )
    Q_DISABLE_COPY( RadioScannerEngine )

    friend class RadioUiEngine;

public:

    explicit RadioScannerEngine( RadioUiEnginePrivate& uiEngine );
    ~RadioScannerEngine();

// New functions

    /*!
     * Starts the scanning from minimum frequency
     */
    void startScanning();

    /*!
     * Continues the scanning upwards from current frequency
     */
    void continueScanning();

    /*!
     * Checks if the scanning is ongoing
     */
    bool isScanning() const;

    /*!
     * Cancels the scanning process
     */
    void cancel();

    /*!
     * Adds a new station that was found
     * @param frequency Frequency of the found station
     */
    void addScannedFrequency( const uint frequency );

signals:

    void stationFound( const RadioStation& station );

private:

    void emitStationFound( const RadioStation& station );

private: // data

    /**
     * Unmodifiable pointer to the private implementation
     */
    const QScopedPointer<RadioScannerEnginePrivate> d_ptr;

    };

#endif // RADIOSCANNERENGINE_H
