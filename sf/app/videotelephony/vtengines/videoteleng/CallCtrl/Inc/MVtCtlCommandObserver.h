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
* Description:  Session command observer interface
*
*/



#ifndef MVTCTLCOMMANDOBSERVER_H
#define MVTCTLCOMMANDOBSERVER_H

//  INCLUDES
#include "MVtCtlCallControl.h"

// CLASS DECLARATION

/**
*  Session command observer interface
*
*  @lib CSCallCtrl
*  @since 2.6
*/
class MVtCtlCommandObserver
    {
    public:  // new functions

        /**
        * Handles command completion in session control.
        * @param aCmd command
        * @param aResult completion result
        */
        virtual void HandleVtSessionCommandL( 
            MVtCtlCallControl::TVtCtlCmd aCmd,
            TInt aResult ) = 0;
    };

#endif      // MVTCTLCOMMANDOBSERVER_H

// End of File
