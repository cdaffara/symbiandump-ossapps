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

#ifndef RADIOSTATIONMODEL_P_H_
#define RADIOSTATIONMODEL_P_H_

// System includes
#include <QList>
#include <QMap>
#include <QScopedPointer>
#include <QIcon>

// User includes
#include "radioenginewrapperobserver.h"
#include "radiostationhandlerif.h"

// Forward declarations
class RadioUiEnginePrivate;
class RadioPresetStorage;
class RadioEngineWrapper;
class QTimer;

// Class declaration
class RadioStationModelPrivate : public RadioStationHandlerIf
                               , public RadioEngineWrapperObserver
{
    Q_DECLARE_PUBLIC( RadioStationModel )
    Q_DISABLE_COPY( RadioStationModelPrivate )

public:

    explicit RadioStationModelPrivate( RadioStationModel* model, RadioUiEnginePrivate& uiEngine );

    virtual ~RadioStationModelPrivate();

private:

// from base class RadioStationHandlerIf

    uint currentFrequency() const;
    int currentPresetIndex() const;
    void setCurrentStation( uint frequency );
    bool containsFrequency( uint frequency );
    bool containsPresetIndex( int presetIndex );
    void startDynamicPsCheck();
    void addScannedFrequency( uint frequency );
    void setCurrentPsName( uint frequency, const QString& name );
    void setCurrentRadioText( uint frequency, const QString& radioText );
    void setCurrentRadioTextPlus( uint frequency, int rtClass, const QString& rtItem );
    void setCurrentPiCode( uint frequency, int piCode );
    void setCurrentGenre( uint frequency, int genre );

// from base class RadioEngineWrapperObserver

    void tunedToFrequency( uint frequency, int reason );

// New functions

    void doSaveStation( RadioStation& station, bool persistentSave = true );

    QList<RadioStation> favorites() const;
    QList<RadioStation> locals() const;

private: // data

    /**
     * Pointer to the public class
     * Not own.
     */
    RadioStationModel*          q_ptr;

    /**
     * Reference to the ui engine
     */
    RadioUiEnginePrivate&       mUiEngine;

    /**
     * Pointer to the preset storage
     */
    RadioPresetStorage*         mPresetStorage;

    /**
     * Pointer to the radio engine wrapper
     */
    RadioEngineWrapper*         mWrapper;

    /**
     * List of station items
     */
    Stations                    mStations;

    /**
     * Station used to store information of manually tuned frequencies
     */
    RadioStation                mManualStation;

    /**
     * Pointer to the current station
     * Not own.
     */
    RadioStation*               mCurrentStation;

    /**
     * Dynamic PS timer
     * Own.
     */
    QScopedPointer<QTimer>      mDynamicPsTimer;

    /**
     * Level of detail shown in Qt::DisplayRole and Qt::DecorationRole roles
     */
    RadioStationModel::Detail   mDetailLevel;

    /**
     * Pointer to favorite icon.
     * Not own.
     */
    QIcon                       mFavoriteIcon;

    /**
    * Pointer to nonFavorite icon.
    * Not own.
    */
    QIcon                       mNonFavoriteIcon;

    /**
     * Pointer to the now playing marker icon
     * Not own.
     */
    QIcon                       mNowPlayingIcon;

};

#endif // RADIOSTATIONMODEL_P_H_
