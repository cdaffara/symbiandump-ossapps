/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This class takes care of accessory events that should be 
 *                handled in alarm.  I.e. stopping alarm using accessory end key.
 *
*/



#ifndef CALARMREMCONEVENTSHANDLER_H
#define CALARMREMCONEVENTSHANDLER_H

// INCLUDES
#include <RemConCallHandlingTargetObserver.h>

// FORWARD DECLARATIONS
class CRemConCallHandlingTarget;
class CRemConInterfaceSelector;
class CAlmAlarmControl;

/**
 *  Stop alarm using accessory.
 *
 *  CAlarmRemConEventsHandler listens for EndCall and AnswerEndCall
 *  commands from an accessory and stops the alarm if its active.
 *
 *  @lib AknAlarmService
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS( CAlarmRemConEventsHandler ) : public CActive, 
                                                 public MRemConCallHandlingTargetObserver
    {
public:

    enum TState
        {
        EStateIdle                 = 0, // not listening for commands, no active AOs
        EStateWaitingCommand       = 1, // wait for a command to activate the AO
        EStateHandleEndCallCommand = 2, // stop the alarm
        EStateStopAlarm            = 3, // stop the alarm and return to EStateWaitingCommand state
        EStateIgnoreCommand        = 4  // just send the response and keep waiting for more commands
        };

    static CAlarmRemConEventsHandler* NewL(CAlmAlarmControl& aAlarmUI);
    virtual ~CAlarmRemConEventsHandler();

// from base class CActive
    void DoCancel();
    TInt RunError(TInt aError);
    void RunL();

// from base class MRemConCallHandlingTargetObserver
    /**
    * From MRemConCallHandlingTargetObserver.
    * End an incoming/ongoing phone call.
    * Used to stop an active alarm.
    *
    * @since S60 v3.2
    **/
    void EndCall();

    /**
    * From MRemConCallHandlingTargetObserver.
    * End an incoming/ongoing phone call.
    * Used to stop an active alarm.
    *
    * @since S60 v3.2
    **/
    void AnswerEndCall();

    /**
    * From MRemConCallHandlingTargetObserver.
    * The following functions are not used by alarm.
    *
    * @since S60 v3.2
    **/
    void AnswerCall();
    void VoiceDial(const TBool aActivate);
    void LastNumberRedial();
    void DialCall(const TDesC8& aTelNumber);
    void MultipartyCalling(const TDesC8& aData);
    void GenerateDTMF(const TChar aChar);
    void SpeedDial(const TInt aIndex);

// new functions
    /**
    * Start observing accessory commands.
    * 
    * @since S60 v3.2
    **/
    void StartL();

    /**
    * Stop observing accessory commands.
    * 
    * @since S60 v3.2
    **/
    void Stop();

private:
    /**
    * Hidden default constructor.
    **/
    CAlarmRemConEventsHandler(CAlmAlarmControl& aAlarmUI);

    void ConstructL();

    void HandleRemConCommand(TInt aCommand);

    void CompleteSelf();

    /**
    * Reset state and delete all data.
    * 
    * @since S60 v3.2
    **/
    void Cleanup();

private: // data
    /**
    * CRemConInterfaceSelector for the iRemConCallHandlingTarget.
    * Own.
    **/
    CRemConInterfaceSelector* iRemConInterfaceSelector;

    /**
    * CRemConCallHandlingTarget deliveres call handling related 
    * operations from accessories to client that listens to them.
    * iRemConInterfaceSelector takes the resposibility.
    * Not own.
    **/
    CRemConCallHandlingTarget* iRemConCallHandlingTarget;

    /**
    * State of the OA.
    **/
    TState iState;

    /**
    * Id of the RemCon command we are currently handling.
    **/
    TInt iRemConOperationID;

    /**
    * Reference to the Alarm UI class.
    **/
    CAlmAlarmControl& iAlarmUI;

    };

#endif // CALARMREMCONEVENTSHANDLER_H


// End of File
