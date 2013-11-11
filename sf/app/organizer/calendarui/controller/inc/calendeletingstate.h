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


#ifndef CALENDELETINGSTATE_H
#define CALENDELETINGSTATE_H

// INCLUDES
#include <e32base.h>
#include "calenstate.h"

// CLASS DEFINITIONS
/**
* Calendar deleting state
*/
class CCalenDeletingState : public CCalenState
    {
    public:
        static CCalenDeletingState* NewLC( CCalenController& aController,
                                                    RHashSet<TCalenNotification>& aOutstandingNotifications );
        ~CCalenDeletingState();
        
    public:
        // From CCCalenState
        TBool HandleCommandL( const TCalenCommand& aCommand,
                             CCalenStateMachine& aStateMachine );
        void HandleNotificationL(const TCalenNotification& aNotification,
							  CCalenStateMachine& aStateMachine ); 
        
    private:
        CCalenDeletingState( CCalenController& aController,
                                        RHashSet<TCalenNotification>& aOutstandingNotifications );
        void ConstructL();
    };


#endif // CALENDELETINGSTATE_H

// end of file
