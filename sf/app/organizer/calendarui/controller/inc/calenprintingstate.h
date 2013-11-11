/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Calendar state machine
*
*/


#ifndef CALENPRINTINGSTATE_H
#define CALENPRINTINGSTATE_H

// INCLUDES
#include <e32base.h>
#include "calenstate.h"

// CLASS DEFINITIONS
/**
* Calendar printing state
*/
class CCalenPrintingState : public CCalenState
    {
    public:
        static CCalenPrintingState* NewLC( CCalenController& aController, 
                                                RHashSet<TCalenNotification>& aOutstandingNotifications );
        ~CCalenPrintingState();
        
    public:
        // From CCalenState
        TBool HandleCommandL( const TCalenCommand& aCommand,
                            CCalenStateMachine& aStateMachine );
        void HandleNotificationL(const TCalenNotification& aNotification,
			                CCalenStateMachine& aStateMachine ); 
        
    private:
        CCalenPrintingState( CCalenController& aController,
                                                RHashSet<TCalenNotification>& aOutstandingNotifications );
        void ConstructL();
    };


#endif // CALENDPRINTINGSTATE_H

// end of file
