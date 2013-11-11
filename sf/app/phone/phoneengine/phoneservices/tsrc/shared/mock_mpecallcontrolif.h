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

#ifndef CPECALLCONTROLIFMOCK_H
#define CPECALLCONTROLIFMOCK_H

#include <e32base.h>
#include "mpecallcontrolif.h"

// CLASS DECLARATION

/**
*  Dials a phone call. 
* 
*  @lib phoneengine.lib
*  @since S60_10.1
*/
NONSHARABLE_CLASS( CPECallControlIFMock ) : public CBase, public MPECallControlIF 
{
    public:    //New functions

        /**
        * Dials a phone call.
        * @param aClientCall, Informs is the current call client originated or not.
        *                     By default not client originated.
        * @return Return possible error code.
        */
        TInt HandleDialServiceCall( const TBool aClientCall = EFalse );        

        /**
        * Starts tone playing. 
        */
        void HandlePlayDTMFL();

        /**
        * Stops tone playing. 
        * @return error code.
        */
        TInt HandleEndDTMF();
    };

#endif // CPECALLCONTROLIFMOCK_H

// End of File
