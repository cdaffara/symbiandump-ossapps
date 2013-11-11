/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Storage for data.
*
*/


#ifndef __MPHONESTORAGE_H
#define __MPHONESTORAGE_H

// INCLUDES

#include <e32base.h>
#include <e32keys.h>

// CLASS DECLARATION

class MPhoneStorage
    {
    public:

        /**
        * Destructor.
        */
        virtual ~MPhoneStorage() {};

        virtual TBool NeedToEnableKeylock() = 0;
        
        virtual void SetNeedToEnableKeylock( TBool aEnableKeylock ) = 0;
    };

#endif // __MPHONESTORAGE_H

// End of File
