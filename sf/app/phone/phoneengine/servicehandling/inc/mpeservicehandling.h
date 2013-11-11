/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Declares interface for service enabling. 
*
*/



#ifndef MPESERVICEHANDLING_H
#define MPESERVICEHANDLING_H

//INCLUDES

// CLASS DECLARATION

/**
 *  Declares interface for service enabling.
 *  
 *  This interface offers the service enabling and service status error 
 *  checking services for Phone Engine. VoIP service needs to be enabled 
 *  before it's possible to make a VoIP call.
 *
 *  @lib peservicehandling.lib
 */
class MPEServiceHandling
    {
    public:

        /**
         * Destructor.
         */
        virtual ~MPEServiceHandling( ) { };
        
        /**
         * Enables service defined by the given identifier.
         * This function does the service enabling and service status error checking.
         * If service is enabled successfully, send message 
         * (EPEMessageServiceEnabled) to PhoneEngine. 
         * 
         * @param   aServiceId      Identifier of the service.
         */                                     
        virtual void EnableServiceL( TInt aServiceId ) = 0;
        
        /**
         * Cancel current service enabling, if service connecting state.
         */ 
        virtual void CancelServiceEnabling() const = 0;
        
        /**
         * Disable service.
         */ 
        virtual void DisableService() const = 0;
        
    };

#endif // MPESERVICEHANDLING_H

//End of file
