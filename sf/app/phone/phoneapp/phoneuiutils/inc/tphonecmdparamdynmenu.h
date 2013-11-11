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
* Description:  Parameter class for dynamic menu generation
*
*/

#ifndef __TPHONECMDPARAMDYNMENU_H
#define __TPHONECMDPARAMDYNMENU_H

//  INCLUDES
#include "tphonecommandparam.h"

// CLASS DECLARATION

/**
*  A concrete parameter class for one integer value.
*  
*/
class TPhoneCmdParamDynMenu : public TPhoneUICommandParam
    {
    public:  
        
        /**
        * C++ default constructor.
        */
        IMPORT_C TPhoneCmdParamDynMenu();

    public: 
        
        /**
        * Sets the resourceId value
        * @param aInteger is integer value
        */
        IMPORT_C void SetResourceId( TInt aInteger );

        /**
        * Sets the DynMenu pointer value
        * @param aPointer is integer value
        */
        IMPORT_C void SetDynMenu( TAny* aPointer );

        /**
        * Returns the integer.
        * @return Returns the carried integer
        */
        IMPORT_C  TInt ResourceId() const;

        /**
        * Returns the integer.
        * @return Returns the pointer
        */
        IMPORT_C TAny* DynMenu();

    private:   

        /**
        * The integer
        */
        TInt iResourceId;

        TAny* iDynMenu;
    };

#endif      // __TPHONECMDPARAMDYNMENU_H   
            
// End of File
