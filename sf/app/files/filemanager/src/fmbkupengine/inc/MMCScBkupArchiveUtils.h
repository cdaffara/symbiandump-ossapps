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
* Description: Declaration for MMCScBkupArchiveUtils
*     
*
*/

#ifndef __MMCSCBKUPARCHIVEUTILS_H__
#define __MMCSCBKUPARCHIVEUTILS_H__

// System includes
#include <f32file.h>
#include <s32strm.h>
#include <babitflags.h>

// User includes
#include "TMMCScBkupArchiveVector.h"

// Classes referenced
class CMMCScBkupArchiveHeader;
class MMMCScBkupArchiveDataInterface;

// Constants
const TInt KMMCScBkupArchiveFileFormatFirstRestorableMajorVersion = 1;
const TInt KMMCScArchiveUidsByteCount = sizeof( TCheckedUid );
const TInt KMMCScArchiveVersionByteCount = sizeof( TInt8 ) + sizeof( TInt8 ) + sizeof( TInt16 );
const TInt KMMCScArchiveFooterLengthByteCount = sizeof( TUint32 );
const TInt KMMCScArchiveFlagsByteCount = sizeof( TUint32 );
const TInt KMMCScArchiveCrcByteCount = sizeof( TUint32 );
const TInt KMMCScArchiveCategoryByteCount = sizeof( TUint32 );
const TInt KMMCScSpareByteCount = 5 * sizeof(TInt32);
const TInt KMMCScArchivePhoneModelStringLength = sizeof(TUint8);

/**
*
*
* @since 3.0
*/
class MMCScBkupArchiveUtils
    {
    public: // API

        /**
        *
        */
        static void ArchiveUidType( TUidType& aType );

        /**
        *
        */
        static void ArchiveCheckedUid( TCheckedUid& aCheckedUids );

        /**
        *
        */
        static HBufC8* PhoneModelFromArchiveLC( MMMCScBkupArchiveDataInterface& aADI );

        /**
        *
        */
        static void ReadPhoneValidityInformationL( RFs& aFsSession, const TDesC& aFileName, HBufC8*& aPhoneModelData, TBitFlags& aArchiveFlags, TVersion& aArchiveVersion );
        
        /**
        *
        */
        static TVersion ArchiveRunTimeFileFormatVersion();

        /**
        *
        */
        static const TMMCScBkupArchiveVector& WriteHeaderL( MMMCScBkupArchiveDataInterface& aADI, TBitFlags aCategory );

        /**
        *
        */
        static const TMMCScBkupArchiveVector& ReadHeaderL( MMMCScBkupArchiveDataInterface& aADI, CMMCScBkupArchiveHeader& aHeader );

        /**
        *
        */
        static void SetArchiveContentAsValidL( RFile64& aArchive );

        /**
        *
        */
        static void SetFooterLengthL( RFile64& aArchive, TInt aLength );
        
        /**
        *
        */
        static TBitFlags ReadBkUpCategoryInformationL( RFs& aFsSession, const TDesC& aFileName );
        
#ifdef RD_FILE_MANAGER_BACKUP
        /**
        *
        */
        static void SetArchiveCrcsL( RFile64& aArchive, TUint32 aCrc );
        
        /**
        *
        */
        static TBool ValidateArchiveCrcsL( RFs& aFsSession, const TDesC& aFileName );
#endif

    private: // Offsets
        enum
            {
            EArchiveOffsetUids = 0,
            EArchiveOffsetFileFormatVersion = EArchiveOffsetUids + KMMCScArchiveUidsByteCount,
            EArchiveOffsetFooterLength = EArchiveOffsetFileFormatVersion + KMMCScArchiveVersionByteCount,
            EArchiveOffsetArchiveFlags = EArchiveOffsetFooterLength + KMMCScArchiveFooterLengthByteCount,
            EArchiveOffsetArchivePayloadCRC = EArchiveOffsetArchiveFlags + KMMCScArchiveFlagsByteCount,
            EArchiveOffsetArchiveCategory = EArchiveOffsetArchivePayloadCRC + KMMCScArchiveCrcByteCount,
            EArchiveOffsetArchiveHeaderCRC = EArchiveOffsetArchiveCategory + KMMCScArchiveCategoryByteCount,
            EArchiveOffsetPadding = EArchiveOffsetArchiveHeaderCRC + KMMCScArchiveCrcByteCount,
            EArchiveOffsetPhoneModelStringLength = EArchiveOffsetPadding +  KMMCScSpareByteCount,
            EArchiveOffsetPhoneModelString = EArchiveOffsetPhoneModelStringLength + KMMCScArchivePhoneModelStringLength
            };

    private: // Internal methods

        /**
        *
        */
        static TInt OffsetOfModelInformation();

        /**
        *
        */
        static HBufC8* PhoneModelFromArchiveLC( RReadStream& aStream );

        /**
        *
        */
        static TUint32 DefaultArchiveFlags();
        
        /**
        *
        */
        static TBitFlags ReadArchiveFlagsL( RFile64& aFile );
        
        /**
        *
        */
        static void ReadArchiveVersionL( RFile64& aFile, TVersion& aVersion );

#ifdef RD_FILE_MANAGER_BACKUP
        /**
        *
        */
        static void SetArchiveCrcL( RFile64& aArchive, TUint32 aCrc, TUint aOffset );

        /**
        *
        */
        static void CalculateCrcFromArchive( TUint32& aCrc, RFile64& aArchive, TUint32 aOffset, TUint32 aLength );
#endif
    };




#endif // __MMCSCBKUPARCHIVEUTILS_H__

//End of File
