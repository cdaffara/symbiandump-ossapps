/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: 
*   See class description.
*
*/


#ifndef __TPHONECMDPARAMCALLSTATEDATA_H
#define __TPHONECMDPARAMCALLSTATEDATA_H

//  INCLUDES

#include <w32std.h>
#include "tphonecommandparam.h"

// DATA TYPES

// CLASS DECLARATION

/**
*  A parameter class for call header information.
*/
class TPhoneCmdParamCallStateData : public TPhoneUICommandParam
    {   
    public:  
        
        /**
        * C++ default constructor.
        */
        IMPORT_C TPhoneCmdParamCallStateData();

    public: 
        
        /**
        * Sets the call header call state.
        * @param aCallState is the call state
        */
        IMPORT_C void SetCallState( TInt aCallState );
        
        /**
        * Sets the call id.
        * @param aCallId is the call id
        */
        IMPORT_C void SetCallId( TInt aCallId );

        /**
        * Returns the call header call state.
        * @return Returns the call state
        */
        IMPORT_C TInt CallState() const;
        
        /**
        * Returns the call id.
        * @return Returns the call id
        */
        IMPORT_C TInt CallId() const;   

    private:    

        /**
        * Call header call state
        */
        TInt iCallState;

        /**
        * Call id
        */
        TInt iCallId;

    };

#endif // _TPHONECMDPARAMCALLSTATEDATA_H  
            
// End of File
