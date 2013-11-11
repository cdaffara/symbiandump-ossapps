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
* Description:  Parameter class for TInt.
*
*/

#ifndef TPHONECMDPARAMINTEGER_H
#define TPHONECMDPARAMINTEGER_H

//  INCLUDES
#include "tphonecommandparam.h"

// CLASS DECLARATION

/**
*  A concrete parameter class for one integer value.
*  
*/
class TPhoneCmdParamInteger : public TPhoneUICommandParam
    {
    public:  
        
        /**
        * C++ default constructor.
        */
        IMPORT_C TPhoneCmdParamInteger();

    public: 
        
        /**
        * Sets the integer value
        * @param aInteger is integer value
        */
        IMPORT_C void SetInteger( TInt aInteger );

        /**
        * Returns the integer.
        * @return Returns the carried integer
        */
        IMPORT_C TInt Integer() const;

    private:   

        /**
        * The integer
        */
        TInt iInteger;
    };

#endif      // TPHONECMDPARAMINTEGER_H   
            
// End of File
