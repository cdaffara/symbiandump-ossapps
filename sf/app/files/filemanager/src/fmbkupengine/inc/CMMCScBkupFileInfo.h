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
* Description: Declaration for CMMCScBkupFileInfo
*     
*
*/

#ifndef __CMMCSCBKUPFILEINFO_H__
#define __CMMCSCBKUPFILEINFO_H__

// System includes
#include <f32file.h>
#include <s32strm.h>


/**
* Represents a public file within an archive. Each public file
* has an associated data owner. This is encapsulated by the SID
* of the owning process.
*
* @since 3.0
*/
NONSHARABLE_CLASS(CMMCScBkupFileInfo) : public CBase
    {
    public:

        /**
        * Static constructor
        */
        static CMMCScBkupFileInfo* NewL( const TEntry& aEntry, TSecureId aAssociatedSID );

        /**
        * Static constructor
        */
        static CMMCScBkupFileInfo* NewLC( const TEntry& aEntry, TSecureId aAssociatedSID );

        /**
        * Static constructor
        */
        static CMMCScBkupFileInfo* NewLC( RReadStream& aStream );

        /**
        * C++ destructor
        */
        ~CMMCScBkupFileInfo();

    private:

        /**
        * C++ default constructor
        */
        CMMCScBkupFileInfo();

        /**
        * C++ default constructor
        */
        CMMCScBkupFileInfo( const TEntry& aEntry, TSecureId aAssociatedSID );

        /**
        *
        */
        void ConstructL( const TEntry& aEntry );

    public: // API - Getters

        /**
        *
        */
        inline const TDesC& FileName() const { return *iFileName; }

        /**
        *
        */
        inline TInt Size() const { return iSize; }

        /**
        *
        */
        inline const TTime& DateTime() const { return iDateTime; }

        /**
        *
        */
        inline TUint Attributes() const { return iAttributes; }

        /**
        *
        */
        inline TSecureId SecureId() const { return iSecureId; }

        /**
        *
        */
        TDriveNumber Drive() const;

    public: // Store/Restore

        /**
        *
        */
        void InternalizeL(RReadStream& aStream);

        /**
        *
        */
        void ExternalizeL(RWriteStream& aStream) const;

    private: // Internal enumerations
        enum
            {
            EStreamFormatVersion1 = 1
            };

    public: // Comparison support

        /**
        *
        */
        static TInt OrderByFileName(const CMMCScBkupFileInfo& aLeft, const CMMCScBkupFileInfo& aRight);

        /**
        *
        */
        static TBool CompareByFileName(const CMMCScBkupFileInfo& aLeft, const CMMCScBkupFileInfo& aRight);

    private: // Member data

        // File name
        HBufC* iFileName;
        //
        TInt iSize;
        //
        TTime iDateTime;
        //
        TUint iAttributes;
        //
        TSecureId iSecureId;
    };




#endif // __CMMCSCBKUPFILEINFO_H__

//End of File
