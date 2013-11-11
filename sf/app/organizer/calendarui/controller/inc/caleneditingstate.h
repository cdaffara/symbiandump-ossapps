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


#ifndef CALENEDITINGSTATE_H
#define CALENEDITINGSTATE_H

// INCLUDES
#include <e32base.h>
#include "calenstate.h"

// CLASS DEFINITIONS
/**
* Calendar editing state
*/
class CCalenEditingState : public CCalenState
    {
    public:
        static CCalenEditingState* NewLC( CCalenController& aController,
                                                            RHashSet<TCalenNotification>&  aOutstandingNotifications );
        ~CCalenEditingState();
        
    public:
        // From CCalenState
        TBool HandleCommandL( const TCalenCommand& aCommand,
                            CCalenStateMachine& aStateMachine );
        void HandleNotificationL(const TCalenNotification& aNotification,
							CCalenStateMachine& aStateMachine );
        
    private:
        CCalenEditingState( CCalenController& aController,
                                            RHashSet<TCalenNotification>&  aOutstandingNotifications );
        void ConstructL();
    };


#endif // CALENEDITINGSTATE_H

// end of file
