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
* Description:  Functional implentation of key event service.
*
*/


#ifndef CKEYEVENTHANDLER_H
#define CKEYEVENTHANDLER_H


// INCLUDES
#include <HtiServicePluginInterface.h>
#include <w32std.h>

// CONSTANTS

// CLASS DECLARATION

/**
*  Functional implentation of key event service.
*/
class CKeyEventHandler : public CActive
    {
public:

    static CKeyEventHandler* NewL();

    // Interface implementation
    void ProcessMessageL( const TDesC8& aMessage,
        THtiMessagePriority aPriority );

    virtual ~CKeyEventHandler();

    void SetDispatcher( MHtiDispatcher* aDispatcher );

    enum TError
        {
        EMissingCommand          = 0x01,
        EUnrecognizedCommand     = 0x02,
        ENotReadyCommand         = 0x03,
        EInvalidScancode         = 0x80,
        EEmptyUnicodeCharArray   = 0x90,
        EInvalidUnicodeCharArray = 0x91
        };

    enum TCommand
        {
        EPressKey         = 0x01,
        EKeyDown          = 0x02,
        EKeyUp            = 0x03,
        ETypeText         = 0x04,
        ELongKeyPress     = 0x05,
        ETypeTextPassword = 0x06,
        EPressKeySequence = 0x07,
        EPointer          = 0x10,
        EResultOk = 0xFF // only for response message
        };

protected:

    void RunL();
    TInt RunError(TInt aError);
    void DoCancel();

private:

    CKeyEventHandler();
    void ConstructL();

private: // helpers

    void HandleTypeTextFuncL( const TDesC8& aUnicodeChars );
    void HandleKeyboardFuncL( const TDesC8& aData );
    void HandleLongKeyPressFuncL( const TDesC8& aData );
    void HandlePressKeySequenceFuncL( const TDesC8& aData );

    void SendOkMsgL();
    void SendErrorMessageL( TInt aError, const TDesC8& aDescription );

    void TypeTextWithDelay( TInt aNextIndex );
    void SimulateKeyEventL( const TDesC8& aUnicodeChars, TInt aIndex );
    void PressKeySequenceKeyL();

    void KeyUp( TUint aCode );
    void KeyDown( TUint aCode );

private:

    RWsSession iWsSession;
    MHtiDispatcher* iDispatcher; // referenced

    // Typetext specialities:
    RTimer  iTimer;
    TInt    iFocusedWindowGroup;
    HBufC8* iUnicodeChars;
    TInt    iUnicodeCharIndex;

    TBool                           iReady;
    TUint8                          iCommand;
    TUint                           iHoldingScanCode;
    TTimeIntervalMicroSeconds32     iKeySequenceHoldTime;
    TTimeIntervalMicroSeconds32     iKeySequenceInterval;
    TBool                           iHoldingKey;
    };

#endif // CKEYEVENTHANDLER_H
