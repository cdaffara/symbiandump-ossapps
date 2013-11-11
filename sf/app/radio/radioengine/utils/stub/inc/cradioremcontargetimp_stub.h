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

#ifndef CRADIOREMCONTARGETIMP_H
#define CRADIOREMCONTARGETIMP_H

// System includes
#include <RemConCallHandlingTargetObserver.h>
#include <remconcoreapitargetobserver.h>
#include <RadioStubManager.h>

// User includes
#include "cradioremcontarget.h"

// Forward declarations
class CRemConCallHandlingTarget;
class CRemConCoreApiTarget;
class CRemConInterfaceSelector;

// Class declaration

/**
 * Media key listener
 */
NONSHARABLE_CLASS( CRadioRemConTargetImp ) : public CRadioRemConTarget
                                           , public MRemConCoreApiTargetObserver
                                           , public MRemConCallHandlingTargetObserver
    {
public:

    static CRadioRemConTargetImp* NewL();

    virtual ~CRadioRemConTargetImp();

private:

// from base class CRadioRemConTarget

    void SetControlEventObserver( MRadioControlEventObserver* aControlEventObserver );

// from base class MRemConCoreApiTargetObserver

    void MrccatoCommand( TRemConCoreApiOperationId aOperationId, TRemConCoreApiButtonAction aButtonAct );
    void MrccatoPlay( TRemConCoreApiPlaybackSpeed aSpeed, TRemConCoreApiButtonAction aButtonAct );
    void MrccatoTuneFunction( TBool aTwoPart, TUint aMajorChannel, TUint aMinorChannel, TRemConCoreApiButtonAction aButtonAct );
    void MrccatoSelectDiskFunction( TUint aDisk, TRemConCoreApiButtonAction aButtonAct );
    void MrccatoSelectAvInputFunction( TUint8 aAvInputSignalNumber, TRemConCoreApiButtonAction aButtonAct );
    void MrccatoSelectAudioInputFunction( TUint8 aAudioInputSignalNumber, TRemConCoreApiButtonAction aButtonAct );

// from base class MRemConCallHandlingTargetObserver

    void AnswerCall();
    void AnswerEndCall();
    void DialCall( const TDesC8& aTelNumber );
    void EndCall();
    void GenerateDTMF( const TChar aChar );
    void LastNumberRedial();
    void MultipartyCalling( const TDesC8& aData );
    void SpeedDial( const TInt aIndex );
    void VoiceDial( const TBool aActivate );

private:

    CRadioRemConTargetImp();

    void ConstructL();

    /**
     * Callback for repeat timer.
     * @param aPtr Pointer to this class.
     */
    static TInt RepeatTimerCallback( TAny* aPtr );

private: // data

    /** Pointer to observer in UI side, can be NULL. Not owned. */
    MRadioControlEventObserver* iObserver;

    /** RemCon interface selector. */
    CRemConInterfaceSelector*   iInterfaceSelector;

    /** RemCon Core API target class. */
    CRemConCoreApiTarget*       iCoreTarget;

    /** RemCon Call API target class. */
    CRemConCallHandlingTarget*  iCallTarget;

    /** Timer for generating repeat events. */
    CPeriodic*                  iRepeatTimer;

    /** Operation id to repeat. */
    TRemConCoreApiOperationId   iRepeatId;

		// RadioStubManager pointer points inside RadioStubManagerChunk
		SRadioStubManager* iRadioStubManager;
		
		// RadioStubManagerChunk handle
		RChunk iRadioStubManagerChunk;

    };

#endif // CRADIOREMCONTARGETIMP_H
