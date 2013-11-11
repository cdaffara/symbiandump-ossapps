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



#ifndef __TPHONECMDPARAMNUMBERENTRYOBSERVER_H
#define __TPHONECMDPARAMNUMBERENTRYOBSERVER_H

//  INCLUDES

#include <w32std.h>
#include "tphonecommandparam.h"

// DATA TYPES

// CLASS DECLARATION

/**
*  A parameter class for key event information.
*/
class TPhoneCmdParamNumberEntryObserver : public TPhoneUICommandParam
    {   
    public:  
        
        /**
        * C++ default constructor.
        */
        IMPORT_C TPhoneCmdParamNumberEntryObserver();

    public: 
        
        /**
        * Sets the callback function
        * @param aKeyEvent is the callback function
        */
        IMPORT_C void SetObserver( TCallBack aCallBack );

        /**
        * Returns the observer.
        * @return Returns the observer
        */
        IMPORT_C TCallBack Observer();

    private:    
        
        TCallBack iCallBack;
    };

#endif // __TPHONECMDPARAMNUMBERENTRYOBSERVER_H   
            
// End of File
