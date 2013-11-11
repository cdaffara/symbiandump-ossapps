/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Declaration of CMMCScBkupArchiveInfo
*     
*
*/

#ifndef __CMMCSCBKUPARCHIVEINFO_H__
#define __CMMCSCBKUPARCHIVEINFO_H__

// System includes
#include <f32file.h>
#include <babitflags.h>
#include <barsread.h>

#include "CMMCScBkupFileInfo.h"

/**
* Class for encapsulating category specific information
* 
* @since 3.2
*/
NONSHARABLE_CLASS(CMMCScBkupArchiveInfo) : public CBase
    {
    public:

        /**
        * Static constructor
        */
        static CMMCScBkupArchiveInfo* NewL( TResourceReader& aReader );

        /**
        * Static constructor
        */
        static CMMCScBkupArchiveInfo* NewL( const TEntry& aEntry );

        /**
        * Static constructor
        */
        static CMMCScBkupArchiveInfo* NewLC( const TEntry& aEntry );

        /**
        * C++ destructor
        */
        ~CMMCScBkupArchiveInfo();

    private:

        /**
        * C++ default constructor
        */
        CMMCScBkupArchiveInfo();

        /**
        * C++ default constructor
        */
        //CMMCScBkupArchiveInfo( const TEntry& aEntry );

        /**
        *
        */
        void ConstructL( const TEntry& aEntry );

    public: // API - Setters

        /**
        *
        */
        void SetTargetDrive( TDriveUnit aDrive ) { iTargetDrive = aDrive; }
        
        /**
        *
        */
        void SetCategory(TBitFlags aCategories) { iCategory = aCategories; }
        
        /**
        *
        */
        void SetSpecialFlags(TBitFlags aFlags) { iSpecialFlags = aFlags; }
        
        /**
        *
        */
        void SetExcludedSpecialFlags(TBitFlags aFlags) { iExcludedSpecialFlags = aFlags; }
        
        /**
        *
        */
        void SetSIDs( RArray<TSecureId>& aSIDs );

        /**
        *
        */
        void SetExcludedSIDs( RArray<TSecureId>& aExcludedSIDs );

    public: // API - Getters

        /**
        *
        */
        IMPORT_C TBitFlags Category() const;
        
        /**
        *
        */
        IMPORT_C const TDesC& FileName() const;

        /**
        *
        */
        IMPORT_C const TTime& DateTime() const;

        /**
        *
        */
        IMPORT_C TDriveNumber Drive() const;

        /**
        *
        */
        TBitFlags SpecialFlags() const { return iSpecialFlags; }

        /**
        *
        */
        TBitFlags ExcludedSpecialFlags() const { return iExcludedSpecialFlags; }

        /**
        *
        */
        const RArray<TSecureId>& SIDs( TBitFlags aCategory ) const;

        /**
        *
        */
        const RArray<TSecureId>& ExcludedSIDs( TBitFlags aCategory ) const;

    private: // Member data

        // 
        TDriveUnit iTargetDrive;
        // 
        TBitFlags iCategory;
        // 
        TBitFlags iSpecialFlags;
        // 
        TBitFlags iExcludedSpecialFlags;
        // 
        CMMCScBkupFileInfo* iFileInfo;
        //
        RArray<TSecureId> iSecureIds;
        //
        RArray<TSecureId> iExcludedSecureIds;
    };




#endif // __CMMCSCBKUPARCHIVEINFO_H__

//End of File
