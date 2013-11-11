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


#ifndef __TPHONECMDPARAMKEYEVENT_H
#define __TPHONECMDPARAMKEYEVENT_H

//  INCLUDES

#include <w32std.h>
#include "tphonecommandparam.h"

// DATA TYPES

// CLASS DECLARATION

/**
*  A parameter class for key event information.
*/
class TPhoneCmdParamKeyEvent : public TPhoneUICommandParam
    {   
    public:  
        
        /**
        * C++ default constructor.
        */
        IMPORT_C TPhoneCmdParamKeyEvent();

    public: 
        
        /**
        * Sets the key event.
        * @param aKeyEvent is the key event
        */
        IMPORT_C void SetKeyEvent( const TKeyEvent& aKeyEvent );

        
        /**
        * Sets the key event code.
        * @param aEventCode is the key event code
        */
        IMPORT_C void SetEventCode( TEventCode aEventCode );

        /**
        * Returns the key event.
        * @return Returns the key event
        */
        IMPORT_C const TKeyEvent KeyEvent() const;


        /**
        * Returns the key code.
        * @return Returns the key code
        */
        IMPORT_C TEventCode EventCode() const;

    private:    
        
        /**
        * Key event
        */
        TKeyEvent iKeyEvent;

        /**
        * Key event code
        */
        TEventCode iEventCode;

    };

#endif // __TPHONECMDPARAMKEYEVENT_H   
            
// End of File
