/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Observer interface for line events.
*
*/



#ifndef MVTCTLLINEEVENTOBSERVER_H
#define MVTCTLLINEEVENTOBSERVER_H

//  INCLUDES
#include <e32def.h>

// CLASS DECLARATION

/**
*  Observer interface for line events
*
*  @lib CSCallCtrl
*  @since 2.6
*/
class MVtCtlLineEventObserver
    {
    public:  // new functions

        /**
        * ELineEventCallAdded    call added to line
        * ELineEventIncomingCall incoming call
        * ELineEventStatusChange line status changed
        */
        enum TLineEvent {
            ELineEventCallAdded,
            ELineEventIncomingCall,
            ELineEventStatusChange
            };

        /**
        * Event handler for line events.
        * @param aEvent line event
        * @param aCallName call name
        */
        virtual void HandleLineEventL( 
            const TLineEvent aEvent, 
            TDesC& aCallName ) = 0;

        /**
        * Event handler for line events.
        * @param aEvent line event
        * @param aCallName call name
        * @param aStatus call status
        */
        /*****************************************************
        *   Series 60 Customer / ETel
        *   Series 60  ETel API
        *****************************************************/
        virtual void HandleLineEventL( 
            const TLineEvent aEvent, 
            const RMobileCall::TMobileCallStatus aStatus ) = 0;
    };

#endif      // MVTCTLLINEEVENTOBSERVER_H   

// End of File
