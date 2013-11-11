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
* Description: Declaration of CMMCScBkupDataOwnerCollection
*     
*
*/

#ifndef __CMMCSCBKUPDATAOWNERCOLLECTION_H__
#define __CMMCSCBKUPDATAOWNERCOLLECTION_H__

// System includes
#include <connect/sbtypes.h>
#include <s32strm.h>
#include <babitflags.h>
#include <bldvariant.hrh>

// User includes
#include "RMMCScBkupPointerArray.h"

// Namespaces
using namespace conn;

// Classes referenced
class MMMCScBkupDriver;
class CMMCScBkupDriveSizer;
class CMMCScBkupDataOwnerInfo;

#ifndef RD_FILE_MANAGER_BACKUP
// Type definitions
typedef RPointerArray<CDataOwnerInfo> RDataOwnerInfoArray;
#endif

/**
*
*
* @since 3.0
*/
NONSHARABLE_CLASS(CMMCScBkupDataOwnerCollection) : public CBase
    {
    public:

        /**
        * Static constructor
        */
        static CMMCScBkupDataOwnerCollection* NewL( MMMCScBkupDriver& aDriver, TBitFlags aCategory );

        /**
        * C++ destructor
        */
        ~CMMCScBkupDataOwnerCollection();

    private:

        /**
        * C++ default constructor
        */
        CMMCScBkupDataOwnerCollection( MMMCScBkupDriver& aDriver, TBitFlags aCategory );

        /**
        *
        */
        void ConstructL();

    public: // API

        /**
        *
        */
#ifdef RD_FILE_MANAGER_BACKUP
        TBool AssignL( const CMMCScBkupDataOwnerInfo& dataOwnersInfo );
#else
        void AssignL( RDataOwnerInfoArray& aArray );
#endif

        /**
        *
        */
        void AppendL( CMMCScBkupDataOwnerInfo* aNewEntry );

        /**
        *
        */
        TInt Count() const;

        /**
        *
        */
        CMMCScBkupDataOwnerInfo& Owner( TInt aIndex );

        /**
        *
        */
        const CMMCScBkupDataOwnerInfo& Owner( TInt aIndex ) const;

        /**
        *
        */
        CMMCScBkupDataOwnerInfo& OwnerL( TSecureId aSID );

        /**
        *
        */
        const CMMCScBkupDataOwnerInfo& OwnerL( TSecureId aSID ) const;

        /**
        *
        */
        CMMCScBkupDataOwnerInfo& OwnerL( TUid aPackageId );

        /**
        *
        */
        const CMMCScBkupDataOwnerInfo& OwnerL( TUid aPackageId ) const;

        /**
        *
        */
        CMMCScBkupDataOwnerInfo& OwnerL( const TDesC& aHash );

        /**
        *
        */
        const CMMCScBkupDataOwnerInfo& OwnerL( const TDesC& aHash ) const;

        /**
        *
        */
        void Remove( TInt aIndex );

        /**
        *
        */
        void Reset();

        /**
        * The amount of space required spanning all drives.
        * This is used as the total progress amount during
        * restore operations.
        */
        TInt64 TotalOperationalSizeL() const;

        /**
        * Whether one or more data owners require a reboot
        * after restore has completed.
        */
        TBool RebootRequired() const;

        /**
        * 
        */
        TInt64 DiskSpaceRequiredForRestore( TDriveNumber aDrive ) const;

        /**
        *
        */
        void CalculateDiskSpaceRequiredForRestoreL();

        /**
        *
        */
        TBitFlags Category() const { return iCategory; }
        
    public:

        /**
        * Internalize 'iRestoreSizer' from stream
        */
        void InternalizeL( RReadStream& aStream );

        /**
        * Externalize 'iRestoreSizer' to stream
        */
        void ExternalizeL( RWriteStream& aStream ) const;

    private:
        
#ifdef RD_FILE_MANAGER_BACKUP
        /**
        *
        */
        TBool BelongsToL(const CMMCScBkupDataOwnerInfo& aInfo, TBitFlags aFlags, TBitFlags aExcludedFlags,
            const RArray<TSecureId> aSecureIds, const RArray<TSecureId> aExcludedSecureIds) const;
#endif
    
    private: // Internal enumerations
        enum
            {
            EStreamFormatVersion1 = 1
            };

    private: // Member data

        //
        MMMCScBkupDriver& iDriver;
        // Array of data owners
        RMMCScBkupPointerArray<CMMCScBkupDataOwnerInfo> iOwners;
        //
        CMMCScBkupDriveSizer* iRestoreSizer;
        //
        TBitFlags iCategory;
    };




#endif // __CMMCSCBKUPDATAOWNERCOLLECTION_H__

//End of File
