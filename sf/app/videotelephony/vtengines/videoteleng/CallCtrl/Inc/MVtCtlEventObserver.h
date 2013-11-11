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
* Description:  Session event observer interface.
*
*/



#ifndef MVTCTLEVENTOBSERVER_H
#define MVTCTLEVENTOBSERVER_H

//  INCLUDES
#include "VtCtlEvents.h"

/**
*  Session event observer interface
*
*  @lib CSCallCtrl
*  @since 2.6
*/
class MVtCtlEventObserver
    {
    public:  // new functions

        /**
        * Handles event from session control.
        * @param aEvent session event
        * @param aParams parameters related to event.
        *        Actual type depends on event.
        */
        virtual void HandleVtSessionEventL( 
            TInt aEvent,
            TAny* aParams ) = 0;
    };

#endif      // MVTCTLEVENTOBSERVER_H   

// End of File
