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
* Description:  Call status observer interface
*
*/



#ifndef MVTCTLCALLSTATUSOBSERVER_H
#define MVTCTLCALLSTATUSOBSERVER_H

//  INCLUDES
#include "MVtCtlCallControl.h"

// CLASS DECLARATION

/**
*  Call status observer interface
*
*  @lib CSCallCtrl
*  @since 2.6
*/
class MVtCtlCallStatusObserver
    {
    public:  // new functions

        /**
        * Handles call status change.
        * @param aId session id
        * @param aStatus new session status
        * @param aPreviousStatus previous session status
        */
        virtual void HandleCallStatusChangeL( 
            TVtCtlSessionId aId,
            MVtCtlCallControl::TVtCtlState aStatus,
            MVtCtlCallControl::TVtCtlState aPreviousStatus ) = 0;
    };

#endif      // MVTCTLCALLSTATUSOBSERVER_H

// End of File
