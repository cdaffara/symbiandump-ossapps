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
* Description:  Provides service enabling implementation.
*
*/


#ifndef C_PESERVICEHANDLING_H
#define C_PESERVICEHANDLING_H

#include <e32base.h>
#include "mpeservicehandling.h"

class  MPEPhoneModelInternal;

/**
 *  CPEServiceHandling implements the intefaces MPEServiceHandling and 
 *  thus provides the service enabling imlementation for Phone Engine.
 *  
 *  @lib peservicehandling.lib
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS( CPEServiceHandling ): public CBase,
                                         public MPEServiceHandling
    {

    public: // Constructors and destructor

        /**
         * Two-phased constructor.
         */
        IMPORT_C static CPEServiceHandling* NewL( MPEPhoneModelInternal& aModel );

        /**
         * Two-phased constructor.
         */
        IMPORT_C static CPEServiceHandling* NewLC( MPEPhoneModelInternal& aModel );

        /**
         * Destructors.
         */
        IMPORT_C virtual ~CPEServiceHandling();

    public: // from MPEServiceHandling
    
        /**
         * From MPEServiceHandling.
         * Enables service defined by the given identifier.
         * This function does the service enabling and service status error checking.
         * If service is enabled successfully, send message 
         * (EPEMessageServiceEnabled) to PhoneEngine. 
         * 
         * @since   S60 5.0
         * @param   aServiceId      Identifier of the service.
         */                                     
        void EnableServiceL( TInt aServiceId );
       
        /**
         * From MPEServiceHandling.
         * Cancel current service.
         */ 
        void CancelServiceEnabling() const;
        
        /**
         * From MPEServiceHandling.
         * Disable service.
         */ 
        void DisableService() const;
    
    private: // Constructors

        /**
         * C++ default constructor.
         */
        CPEServiceHandling( MPEPhoneModelInternal& aModel );
        
        /**
         * By default Symbian 2nd phase constructor is private.
         */
        void ConstructL();
               
    private: // data
        
        /**
         * PhoneModel.
         * Not own.
         */
        MPEPhoneModelInternal& iModel;
        
        /**
         * Current service identifier.
         */
        TInt iCurrentServiceId;
    };

#endif // C_PESERVICEHANDLING_H
