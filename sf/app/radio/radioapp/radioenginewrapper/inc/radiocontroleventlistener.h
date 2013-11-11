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

#ifndef RADIOCONTROLEVENTLISTENER_H_
#define RADIOCONTROLEVENTLISTENER_H_

// System includes
#include <QtGlobal>
#include <QScopedPointer>

// User includes
#include "mradiocontroleventobserver.h"

// Forward declarations
class RadioEngineWrapperPrivate;
class CRadioRemConTarget;

class RadioControlEventListener : public MRadioControlEventObserver
{
    Q_DISABLE_COPY( RadioControlEventListener )

public:

    explicit RadioControlEventListener( RadioEngineWrapperPrivate& engine );

    ~RadioControlEventListener();

    void init();

private:

// from base class MRadioControlEventObserver

    void StepToChannelL( RadioEngine::TRadioTuneDirection aDirection );
    void SetChannelL( TInt aChannelId );
    void SeekL( RadioEngine::TRadioTuneDirection aDirection );
    void StepToFrequencyL( RadioEngine::TRadioTuneDirection aDirection );
    void SetFrequencyL( TUint32 aFreq );
    void AdjustVolumeL( RadioEngine::TRadioVolumeSetDirection aDirection );
    void MuteL( TBool aMute );
    void PlayL( TBool aDownPressed );
    void PauseL( TBool aDownPressed );
    void PausePlayL( TBool aDownPressed );
    void StopL( TBool aDownPressed );
    void ForwardL( TBool aDownPressed );
    void FastForwardL( TBool aDownPressed );
    void BackwardL( TBool aDownPressed );
    void RewindL( TBool aDownPressed );
    void ChannelUpL( TBool aDownPressed );
    void ChannelDownL( TBool aDownPressed );
    void VolumeUpL( TBool aDownPressed );
    void VolumeDownL( TBool aDownPressed );
    void ChangeViewL( TUid aViewId, TBool aForceChange );
    void SetAudioRouteL( RadioEngine::TRadioAudioRoute aRoute );
    void AnswerEndCallL();

private: // data

    RadioEngineWrapperPrivate&          mEngine;

    QScopedPointer<CRadioRemConTarget>  mRemCon;

};

#endif // RADIOCONTROLEVENTLISTENER_H_
