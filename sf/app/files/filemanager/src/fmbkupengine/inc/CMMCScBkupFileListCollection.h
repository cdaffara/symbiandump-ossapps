/*
* Copyright (c) 2005-2008 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0""
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: Declaration for CMMCScBkupFileListCollection
*     
*
*/

#ifndef __CMMCSCBKUPFILELISTCOLLECTION_H__
#define __CMMCSCBKUPFILELISTCOLLECTION_H__

// System includes
#include <f32file.h>

// User includes
#include "RMMCScBkupPointerArray.h"
#include "CMMCScBkupFileInfo.h"
#include <babitflags.h>


/**
*
*
* @since 3.0
*/
NONSHARABLE_CLASS(CMMCScBkupFileListCollection) : public CBase
    {
    public:

        /**
        * Static constructor
        */
        static CMMCScBkupFileListCollection* NewL( TBitFlags aCategory, RFs& aFsSession );

        /**
        * C++ destructor
        */
        ~CMMCScBkupFileListCollection();

    private:

        /**
        * C++ default constructor
        */
        CMMCScBkupFileListCollection( TBitFlags aCategory, RFs& aFsSession );

        /**
        *
        */
        void ConstructL();

    public: // API

        /**
        *
        */
        void AppendL( const RArray<TEntry>& aArray, RArray< const CMMCScBkupFileInfo* >& aAddedItems, TSecureId aAssociatedSID );

        /**
        *
        */
        void AppendL( CMMCScBkupFileInfo* aFileInfo );

        /**
        *
        */
        TInt Count() const;

        /**
        *
        */
        const CMMCScBkupFileInfo& Entry(TInt aIndex) const;

        /**
        *
        */
        void Remove( TInt aIndex );

        /**
        *
        */
        void Reset();

        /**
        *
        */
        TBitFlags Category() const { return iCategory; }

    private: // Member data

        // Array of data owners
        RMMCScBkupPointerArray<CMMCScBkupFileInfo> iEntries;
        //
        TBitFlags iCategory;
        //
        RFs& iFsSession;
    };




#endif // __CMMCSCBKUPFILELISTCOLLECTION_H__

//End of File
