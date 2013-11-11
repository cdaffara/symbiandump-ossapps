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
 * Description: Implementation of applicationmanagement components
 *
 */


#ifndef __AMAPPHIDEUTIL_H__
#define __AMAPPHIDEUTIL_H__

#include <e32base.h>

namespace NApplicationManagement
    {

    typedef RArray<TUid> RUidArray;

    class CAMAppHideUtil : public CBase
        {
        RUidArray iHiddenUids; // The hidden uid list
        TBool iChanged;
        /**
         * Constructor
         */
        CAMAppHideUtil();


        /**
         * Second phase constructor. Loads the uid list from centrep.
         */
        void ConstructL();



        void ParseUids( RUidArray &aUid, const TDesC8& aUidList );

        /**
         * Loads uids from centrep
         */
        void LoadUidsL();

        /**
         * Parses uid, returning KErrNone if successful
         */ 
        TInt ParseUid( TLex &aLex, TUid &aUid ) const;

        /**
         * Skips delimiter, returning ETrue if delimiter is expected one.
         */
        TBool SkipDelim( TLex &aLex ) const;

        /**
         * Puts aUid to aBuf and if aHasMore also adds delimiter
         */
        void PersistUid( const TUid &aUid, TDes &aBuf, TBool aHasMore ) const;
    public:
        /**
         * Destructor
         */
        virtual ~CAMAppHideUtil();

        /**
         * Creates new CAMAppHideUtil instance
         */
        static CAMAppHideUtil* CAMAppHideUtil::NewL();

        /**
         * Adds uid to list, and updates the repository if needed
         */
        void AddUidL(const TUid& aUid, TBool aPersist = EFalse );

        /**
         * Removed uid from list, and updates the repository if needed
         */
        void RemoveUidL( const TUid& aUid, TBool aPersist = EFalse );

        /**
         * Writes uids to centrep
         */
        void PersistUidsL();

        /**
         * Resets the hidden uid array
         */
        void Reset();
        };

    }

#endif // #ifdef __AMAPPHIDEUTIL_H__

//  End of File
