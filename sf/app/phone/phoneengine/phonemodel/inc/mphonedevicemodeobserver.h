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
* Description:  Observer for device call event
*
*/


#ifndef MPHONEDEVICEMODEOBSERVER_H
#define MPHONEDEVICEMODEOBSERVER_H

#include <e32def.h>

/**
*  Observer for device call events 
*
*  @since Series60 3.0
*/
NONSHARABLE_CLASS( MPhoneDeviceModeObserver )
    {
    public: // New functions
        
        /**
        * Answers the arriving call
        */
        virtual void Answer()=0;

        /**
        * Ends the CS and DS voice calls
        */
        virtual void EndVoiceCalls()=0;
    };

#endif      // MPHONEDEVICEMODEOBSERVER_H
            
// End of File
