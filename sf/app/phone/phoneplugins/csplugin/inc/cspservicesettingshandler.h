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
* Description:  Header of CSPServiceSettingsHandler
*
*/


#ifndef CSPSERVICESETTINGSHANDLER_H
#define CSPSERVICESETTINGSHANDLER_H

#include <e32base.h>

#include "rcsplinecontainer.h"

/**
* Makes asynchronic request to ETel interface according to given request type.
* Provides canceling via CActive::Cancel().
* 
*/
class CSPServiceSettingsHandler: public CBase   
    {
    public:
    
        /**
        * Two phased constructing of the DTMF provider instance.
        * @return the instance to service settings handler
        */
        static CSPServiceSettingsHandler* NewL( );
        
        
        /**
        * C++ default destructor
        */
        virtual ~CSPServiceSettingsHandler( );
        
        /**
        * Find service ID by name.
        * @since S60 3.2
        * @param aName the name of service
        * @param aServiceId service ID
        */
        void FindByNameL( const TDesC& aName, TUint32& aServiceId );        
                             
        /**
        * Find CS service ID.
        * @param aServiceId the service id as output
        * @return error code if read was not succesfull
        */
        TInt ReadCSServiceId( TUint32& aServiceId );

        
        
    private:
        /**
        * Constructs the requester.
        *
        */
        CSPServiceSettingsHandler(  );
        
        /**
        * Constructing the provider in the second phase.
        */
        void ConstructL();
    
    private: // data
    
        
        
    };
    
#endif // CSPSERVICESETTINGSHANDLER