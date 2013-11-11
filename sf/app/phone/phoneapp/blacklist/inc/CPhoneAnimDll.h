/*
* Copyright (c) 2002-2003 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  It defines factory class for blacklist clearing plug-in.
*
*/



#ifndef CPHONEANIMDLL_H
#define CPHONEANIMDLL_H

//  INCLUDES
#include    <e32std.h>             
#include    <w32adll.h>            

// CLASS DECLARATION

/** 
* Animation plug-in.
*
* @since 2.0 
*/
class CPhoneAnimDll 
    : public CAnimDll
    {
    public: // From base classes
        
        /** 
        * From CAnimDll, creates an instance of requested plugin.
        *
        * @return instance of animation service.
        */
        virtual CAnim* CreateInstanceL( TInt aType );

    };

/** 
* Creates an instance of CPhoneAnimDll. 
* Factory function. 
*
* @since 1.0
* @return instance of anim dll.
*/
IMPORT_C CAnimDll* CreateCAnimDllL();

#endif // CPHONEANIMDLL_H

// End of File
