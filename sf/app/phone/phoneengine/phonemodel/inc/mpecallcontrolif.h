/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This file contains the interface for dialling a phone call.
*
*/

#ifndef MPECALLCONTROLIF_H
#define MPECALLCONTROLIF_H

#include <e32base.h>

// CLASS DECLARATION

/**
*  Dials a phone call. 
* 
*  @lib phoneengine.lib
*  @since S60_10.1
*/
NONSHARABLE_CLASS( MPECallControlIF ) 
{
    public:    //New functions

        /**
        * Dials a phone call.
        * @param aClientCall, Informs is the current call client originated or not.
        *                     By default not client originated.
        * @return Return possible error code.
        */
        virtual TInt HandleDialServiceCall( 
                const TBool aClientCall = EFalse ) = 0;        

        /**
        * Starts tone playing. 
        */
        virtual void HandlePlayDTMFL() = 0;

        /**
        * Stops tone playing. 
        * @return error code.
        */
        virtual TInt HandleEndDTMF() = 0;

    };

#endif // MPECALLCONTROLIF_H

// End of File
