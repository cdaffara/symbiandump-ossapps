/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Definition of TPhoneCmdParamBoolean class.
*     
*
*/


#ifndef TPHONECMDPARAMBOOLEAN_H
#define TPHONECMDPARAMBOOLEAN_H

//  INCLUDES
#include "tphonecommandparam.h"

// CLASS DECLARATION

/**
*  A concrete parameter class for passing a TBool around. This
*  is used for various purposes where TBool is useful. 
*/
class TPhoneCmdParamBoolean : public TPhoneUICommandParam
    {
    public:  
        
        /**
        * C++ default constructor.
        */
        IMPORT_C TPhoneCmdParamBoolean();

    public: 
        
        /**
        * Sets the Boolean boolean value.
        * @param aBoolean is the Boolean value.
        */
        IMPORT_C void SetBoolean( TBool aBoolean );

        /**
        * Returns the Boolean.
        * @return Returns the Boolean
        */
        IMPORT_C TBool Boolean() const;

    private:    
        
        /**
        * Boolean value
        */
        TBool iBoolean;

    };

#endif      // TPhoneCmdParamBoolean_H 
            
// End of File
