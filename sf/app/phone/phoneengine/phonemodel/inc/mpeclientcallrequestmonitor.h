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
* Description:  This file contains the interface for using call request monitors.
*
*/



#ifndef MPEClientCallRequestMonitor_H
#define MPEClientCallRequestMonitor_H

//INCLUDES
#include <cphcltdialdata.h>

// CLASS DECLARATION

/**
*  Monitor call requests from the phone server(/phone client). 
*  
* 
*  @lib PhoneEngineBase.lib
*  @since S60_5.0
*/
NONSHARABLE_CLASS( MPEClientCallRequestMonitor ) 
{
    public:    //New functions
        /**
        * Gets dial data
        * @return Reference to TPhCltTelephoneNumber, telephony number.
        */
        virtual CPhCltDialData* ClientDialData() = 0;
        
        /**
        * Sends respond to the phone client
        * @param aRespond  contains respond to the phone client.
        */
        virtual void SendRespond( const TInt aRespond ) = 0;
        
        /**
        * Tells whether this monitor is active.
        */
        virtual TBool IsActive() = 0;

    };

#endif // MPEClientCallRequestMonitor_H

// End of File
