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

#ifndef __TPHONECMDPARAMSTRING_H
#define __TPHONECMDPARAMSTRING_H

//  INCLUDES
#include "tphonecommandparam.h"

// CLASS DECLARATION

/**
*  A concrete parameter class for one TDesC value.
*  
*/
class TPhoneCmdParamString : public TPhoneUICommandParam
    {
    public:  
        
        /**
        * C++ default constructor.
        */
        IMPORT_C TPhoneCmdParamString();

    public: 
        
        /**
        * Sets the string value
        * @param aString is string value
        */
        IMPORT_C void SetString( TPtr* aString );

        /**
        * Returns the string.
        * @return Returns the carried string
        */
        IMPORT_C TPtr* String();

    private:   

        /**
        * The string
        */
        TPtr* iString;
    };

#endif      // __TPHONECMDPARAMSTRING_H   
            
// End of File
