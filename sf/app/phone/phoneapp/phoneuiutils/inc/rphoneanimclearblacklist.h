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
* Description:  Client side for blacklist clearing plugin.
*
*/


#ifndef RPHONEANIMCLEARBLACKLIST_H
#define RPHONEANIMCLEARBLACKLIST_H

// INCLUDES
#include <w32std.h>     // RAnim

// FORWARD DECLARATIONS
class CCoeControl;

// CLASS DECLARATION

/**
* Blacklist clearing plugin client side object.
*
* Used to clear DOS side blacklist. Window server Plug-in.
*
* @since 1.0
*/
class RPhoneAnimClearBlacklist : public RAnim
    {
    public: // Constructors and destructors
        
        /** 
        * Constructor. 
        * @param aAnimDll This is server side object.
        */
        RPhoneAnimClearBlacklist( RAnimDll& aAnimDll );
        
        /** 
        * Construction.
        * @param aDevice window.
        */
        void ConstructL( const RWindowBase& aDevice );
        
    public: // New functions
        
        /** 
        * Clear blacklist on next key press.
        * This function should be called in startup and after every call.
        */
        void ClearBlackListOnNextKey();
        
        /** 
        * Clear blacklist on PhoneCommand.
        * This function should be called in when blaclist is wanted to clear.
        */
        void ClearBlackListNow();
    };

#endif      // RPHONEANIMCLEARBLACKLIST_H
// End of File
