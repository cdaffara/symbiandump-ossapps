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
* Description: Declaration of CMMCScBkupDataOwnerInfo
*     
*
*/

#ifndef __CMMCSCBKUPDATAOWNERINFO_H__
#define __CMMCSCBKUPDATAOWNERINFO_H__

// System includes
#include <e32std.h>
#include <s32strm.h>
#include <connect/sbtypes.h>

// User includes
#include "TMMCScBkupOwnerDataType.h"

// Classes referenced
class TMMCScBkupDriveAndSize;
class CMMCScBkupDataTypeSizer;
class MMMCScBkupArchiveDataInterface;

// Namespaces
using namespace conn;


/**
*
*
* @since 3.0
*/
NONSHARABLE_CLASS(CMMCScBkupDataOwnerInfo) : public CBase
    {
    public:

        /**
        * Static constructor
        */
        static CMMCScBkupDataOwnerInfo* NewLC( CDataOwnerInfo* aOwner );

        /**
        * Static constructor
        */
        static CMMCScBkupDataOwnerInfo* NewLC( RReadStream& aStream );

        /**
        * Static constructor
        */
        static CMMCScBkupDataOwnerInfo* New( TSecureId aSecureId );

        /**
        * C++ destructor
        */
        ~CMMCScBkupDataOwnerInfo();

    private:

        /**
        * C++ default constructor
        */
        CMMCScBkupDataOwnerInfo( CDataOwnerInfo* aOwner = NULL );

        /**
        *
        */
        void ConstructL();

    public: // API

        /**
        *
        */
        CDataOwnerInfo& Owner();

        /**
        *
        */
        const CDataOwnerInfo& Owner() const;

        /**
        *
        */
        TDataOwnerStatus Status() const;

        /**
        *
        */
        void SetStatus( TDataOwnerStatus aStatus );

        /**
        *
        */
        void AddToOperationalSizeL( TMMCScBkupOwnerDataType aType, TDriveNumber aDrive, TInt64 aSize );

        /**
        *
        */
        void SetOperationalSizeL( TMMCScBkupOwnerDataType aType, TDriveNumber aDrive, TInt64 aSize );

        /**
        *
        */
        void ResetOperationalSize( TMMCScBkupOwnerDataType aType );

        /**
        *
        */
        TInt64 OperationalSize( TMMCScBkupOwnerDataType aType ) const;

        /**
        *
        */
        TInt64 OperationalSize( TMMCScBkupOwnerDataType aType, TDriveNumber aDrive ) const;

        /**
        *
        */
        TInt64 OperationalSize( TDriveNumber aDrive ) const;

        /**
        *
        */
        void OperationalSizesL( RArray<TMMCScBkupDriveAndSize>& aSizes ) const;

        /**
        *
        */
        inline TSecureId SecureId() const { return iSecureId; }

        /**
        *
        */
        TInt NumberOfOperationsRequiredL() const;

        /**
        *
        */
        void SetCompletionStatus( TMMCScBkupOwnerDataType aType, TBool aCompleted );

        /**
        *
        */
        TBool CompletionStatus( TMMCScBkupOwnerDataType aType ) const;

        /**
        *
        */
        TInt ActiveDataRetryCount() const;

        /**
        *
        */
        void SetActiveDataRetryCount( TInt aCount );

        /**
        *
        */
        inline TInt Version() const { return iVersion; }

    public: // Store/Restore

        /**
        *
        */
        void InternalizeL( RReadStream& aStream );

        /**
        *
        */
        void ExternalizeL( RWriteStream& aStream ) const;

    public: // Comparison support

        /**
        *
        */
        static TBool CompareDataOwnerBySIDL( const CMMCScBkupDataOwnerInfo& aLeft, const CMMCScBkupDataOwnerInfo& aRight );
        
        /**
        *
        */
        TBool HasJavaDataL() const;

        /**
        *
        */
        TBool HasActiveDataL() const;

        /**
        *
        */
        TBool HasPassiveDataL() const;

        /**
        *
        */
        TBool HasPublicDataL() const;

        /**
        *
        */
        TBool HasSystemDataL() const;

    private: // Internal enumerations
        enum
            {
            EStreamFormatVersion1 = 1,
            EStreamFormatVersion2,
            EStreamFormatVersionLatest
            };

    public: // Public enumerations
        enum
            {
            EStreamFormatVersionFirst = EStreamFormatVersion1
            };

    private: // Member data

        // Underlying data owner - owned by this object
        CDataOwnerInfo* iDataOwner;
        // Associated secure id of the data owner (if it has one)
        TSecureId iSecureId;
        // Ready status of the data owner
        TDataOwnerStatus iStatus;
        // Size of data that the owner has to backup/restore
        CMMCScBkupDataTypeSizer* iOperationalSize;
        // The completion status of each individual element
        TFixedArray<TBool, EMMCScBkupOwnerDataTypeCount> iCompletionStatus;
        // For active data, we record how many times the SID has returned "not ready"
        TInt iActiveDataRetryCount; 
        // Version information
        TInt iVersion;
    };




#endif // __CMMCSCBKUPDATAOWNERINFO_H__

//End of File
