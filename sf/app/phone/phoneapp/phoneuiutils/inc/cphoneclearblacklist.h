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
* Description:  Singleton class that provides interfaces for controlling
*                call blacklist clearing
*
*/


#ifndef CPHONECLEARBLACKLIST_H
#define CPHONECLEARBLACKLIST_H

// INCLUDES
#include <e32base.h>
#include <coemain.h>
// #include <w32std.h>

// FORWARD DECLARATIONS
class RWsSession;
class RWindowBase;
class RAnimDll;
class RPhoneAnimClearBlacklist;

// CLASS DECLARATION

/**
* Blacklist clearing.
*
* Used to clear the call blacklist.
*
*  @since 5.0
*/
class CPhoneClearBlacklist : public CCoeStatic
    {
    public: // Constructors and destructors.

        /**
        * First call initializes the singleton object. 
        * Singleton must be initialized only once and then Instance() method 
        * may be used to get handle to instance.
        * @param aWsSession window server session.
        * @param aDevice window.
        * @return the created instance.
        */
        IMPORT_C static CPhoneClearBlacklist* CreateL(
            RWsSession& aWsSession, 
            const RWindowBase& aDevice );
    
        /**
        * Returns an instance of this class.
        * @return A pointer to CPhoneClearBlacklist class 
        */
        IMPORT_C static CPhoneClearBlacklist* Instance();

        /**
        * C++ destructor.
        */
        IMPORT_C ~CPhoneClearBlacklist();
        
    public: // New functions
        
        /** 
        * Clear blacklist on next key press.
        * This function should be called in startup and after every call.
        */
        IMPORT_C void ClearBlackListOnNextKey();
        
        /** 
        * Clear blacklist.
        * This function should be called when call is established in redial.
        */
        IMPORT_C void ClearBlackListNow();
        
    private:

        /** 
        * Constructor. 
        */
        CPhoneClearBlacklist();

        /** 
        * 2nd phase constructor
        */
        void ConstructL( RWsSession& aWsSession, const RWindowBase& aDevice );
        
    private:

        // Owned anim dll instance.
        RAnimDll*                   iAnimDll;

        // Owned clear blacklist subsession.
        RPhoneAnimClearBlacklist*   iAnim;

    };

#endif
// End of File
