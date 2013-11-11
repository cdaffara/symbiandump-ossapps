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
* Description:  Header file for MCSPCallErrorObserver class for 
*                observing errors related to call requests, e.g. dialling.
*
*/



#ifndef MCSPCALLERROROBSERVER_H
#define MCSPCALLERROROBSERVER_H

//INCLUDES
#include <ccpdefs.h>



// CLASS DECLARATION

/**
*  Interface for monitoring incoming call.
*
*  @since S60 3.2
*/
class MCSPCallErrorObserver
    {
    public:
        
        /**
        * Informs about failed dial.
        * @param aErrorCode
        */
        virtual void DialRequestFailed( TInt aErrorCode ) = 0;

        /**
        * Informs about failed emergency dial.
        * @param aErrorCode
        */
        virtual void EmergencyDialRequestFailed( TInt aErrorCode ) = 0;
        
        /**
        * Informs about erroneous request.
        * @since S60 3.2
        * @param aError request related to the error.
        */    
        virtual void NotifyErrorOccurred( TCCPError aErr ) = 0;



    };

#endif // MINCOMINGCALLOBSERVER_H

//End of file
