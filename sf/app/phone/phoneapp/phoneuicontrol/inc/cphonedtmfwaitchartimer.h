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
* Description: 
*     Class for implementing DTMF wait character timer.
*
*/


#ifndef CPHONEDTMFWAITCHARTIMER_H
#define CPHONEDTMFWAITCHARTIMER_H

// INCLUDES
#include "cphonetimer.h"

// FORWARD DECLARATIONS
class MPhoneStateMachine;

// CLASS DECLARATION

class CPhoneDtmfWaitCharTimer : public CBase,
                                protected MPhoneTimer
    {
    public:  

        /**
        * Destructor.
        */
        virtual ~CPhoneDtmfWaitCharTimer();
        
        /**
        * This function is called after timeout.
        */
        void HandleTimeOutL();

        /**
        * Two-phased constructor.
        * @param aStateMachine - pointer to our state machine.
        */
        static CPhoneDtmfWaitCharTimer* NewL( MPhoneStateMachine* aStateMachine );
        
        /**
        * This method should be called when DTMF wait note is
        * displayed and CBA is pressed for transmitting the rest
        * of the remaining DTMF string.
        * Calling this method triggers our internal timer so that
        * we get enough run time at the view's end to get our
        * method calls end properly. Otherwise we will fall into
        * recursive calls with multiple wait characters in a DTMF
        * string and that causes telephony application to crash.
        *
        * This is a dirty hack I know. If you come up with a better
        * solution you are more than welcome to change this.
        * To test this works make a single call and send 'www456ppp'
        * DTMF string. If it doesn't crash in the target HW it works :)
        */
        void ButtonPressedL();
        

    private:

        /**
        * By default EPOC constructor is private.
        */
        void ConstructL();

        /**
        * C++ default constructor.
        * @param aStateMachine - pointer to our state machine.
        */
        CPhoneDtmfWaitCharTimer( MPhoneStateMachine* aStateMachine );
                        
        
    private:
    
        /**
        * Our internal timer
        */
        CPhoneTimer* iTimer;
        
        /**
        * Pointer to the state machine for sending
        * EPEMessageContinueDTMFSending message.
        */
        MPhoneStateMachine* iStateMachine;
                
    };
    
#endif // CPHONEDTMFWAITCHARTIMER_H

// End of File
