/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  An abstract class for get blocked key list.
*
*/


#ifndef MPhoneLockInfo_H
#define MPhoneLockInfo_H

// INCLUDES
#include <e32cmn.h>

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  An abstract class get phone lock info
*
*/
class MPhoneLockInfo
    {
    public:

        /**
        * Fetches autolock information - is it set on or not
        * @return is autolock set on (ETrue) or off (EFalse)
        */
        virtual TBool IsAutoLockOn() const = 0;
        
    };

#endif // MPhoneLockInfo_H
            
// End of File
