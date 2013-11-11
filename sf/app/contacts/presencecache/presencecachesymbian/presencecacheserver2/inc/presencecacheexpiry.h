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
* Description:  
*
*/


#ifndef CPRESENCECACHEEXPIRY_H
#define CPRESENCECACHEEXPIRY_H

//  INCLUDES
#include <e32base.h>

class CPresenceCacheServer;
class CPresenceCacheBuddyStore;

// CLASS DECLARATION
/**
* 
*  @lib N/A
*  @since 9.2
*/
class CPresenceCacheExpiry : public CBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CPresenceCacheExpiry* NewL(
                CPresenceCacheServer& aServer );

        /**
        * Destructor.
        */
        virtual ~CPresenceCacheExpiry();
        
    public:  // New functions
        /**
         * Start expiry check if not ongoing
         * Non-blocking function
         */
        inline void Start();

    private:  // New functions
        static TInt ExpiryCheck(TAny *aPtr);
        void DoExpiryCheckL();
        void NotifySubscribersL( 
                CPresenceCacheBuddyStore& aBuddyStore );
        
    private:

        /**
        * C++ default constructor.
        */
        inline CPresenceCacheExpiry( CPresenceCacheServer& aServer );

    private:    // Data
        CAsyncCallBack iAsyncCallBack;
        CPresenceCacheServer& iServer;
    };

inline void CPresenceCacheExpiry::Start()
    {
    iAsyncCallBack.CallBack();
    }


#endif      // CPRESENCECACHEEXPIRY_H

// End of File
