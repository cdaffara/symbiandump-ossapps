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
* Description:  Parameter class for TAny type pointer.
*
*/

#ifndef TPHONECMDPARAMPOINTER_H
#define TPHONECMDPARAMPOINTER_H

//  INCLUDES
#include "tphonecommandparam.h"

// CLASS DECLARATION

/**
*  A concrete parameter class for address pointer.
*  
*/
class TPhoneCmdParamPointer : public TPhoneUICommandParam
    {
    public:  
        
        /**
        * C++ default constructor.
        */
        IMPORT_C TPhoneCmdParamPointer();

    public: 
        
        /**
        * Sets the pointer address
        * @param aInteger is integer value
        */
        IMPORT_C void SetPointer( TAny* aPointer );

        /**
        * Returns the address pointer.
        * @return Returns address of the pointer
        */
        IMPORT_C TAny* Pointer() const;

    private:   

        /**
        * The address pointer
        */
        TAny* iPointer;
    };

#endif      // TPHONECMDPARAMPOINTER_H   
          
// End of File
