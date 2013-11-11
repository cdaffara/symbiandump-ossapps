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
* Description: MMCScBkupArchiveUtils implementation
*
*
*/

#include "MMCScBkupArchiveUtils.h"

// System includes
#include <s32file.h>
#include <s32mem.h>

// User includes
#include "MMCScBkupLogger.h"
#include "MMCScBkupDllUids.h"
#include "MMCScBkupPhoneModelUtils.h"
#include "CMMCScBkupArchiveFooter.h"
#include "CMMCScBkupArchiveHeader.h"
#include "MMMCScBkupArchiveDataInterface.h"
#include "MMCScBkupArchiveFlags.h"

// Constants
const TInt8 KMMCScBkupArchiveFileFormatVersionMajor = 1;
#ifdef RD_FILE_MANAGER_BACKUP
const TInt8 KMMCScBkupArchiveFileFormatVersionMinor = 1;
#else
const TInt8 KMMCScBkupArchiveFileFormatVersionMinor = 0;
#endif
const TInt16 KMMCScBkupArchiveFileFormatVersionBuild = 1;
const TUid KMMCScBkupArchiveFileFormatUid1 = { KMMCAppEngUID3 };
const TUid KMMCScBkupArchiveFileFormatUid2 = { 0 };
const TUid KMMCScBkupArchiveFileFormatUid3 = { 0x0BACCCCC }; // FIX

    /**
     * Fixed Header
     * ============
     * 
     * 12 bytes = 3 uids
     *  4 bytes = 1 uid crc checksum
     *
     * ARCHIVE FILE FORMAT VERSION
     * {
     *    1 byte  = version major
     *    1 byte  = version minor
     *    2 bytes = version build
     * }
     * 
     * 4 bytes = size of footer in bytes - this is always the same
     *           fixed length, hence we can write the size here. 
     *           The value can then be used to work from the back of the
     *           archive to the starting position of the footer (where
     *           most of the juicy info is).
     *
     *  4 bytes = archive flags
     *
     *  4 bytes = archive payload CRC (activated when RD_FILE_MANAGER_BACKUP)
     * 
     *  4 bytes = archive category
     * 
     *  4 bytes = archive header CRC (activated when RD_FILE_MANAGER_BACKUP)
     * 
     * KMMCScSpareByteCount bytes = spare padding
     *
     *  1 byte = phone model version string length
     *  n bytes = phone model version string
     *
     *
     *
     * Note that we deliberately do not use the streaming chevrons
     * as we then risk the problem of running the string through
     * a unicode compressor first.
     */


// ========================= MEMBER FUNCTIONS ================================

// ---------------------------------------------------------------------------
// MMCScBkupArchiveUtils::ArchiveUidType()
// 
// 
// ---------------------------------------------------------------------------
void MMCScBkupArchiveUtils::ArchiveUidType(TUidType& aType)
    {
    aType = TUidType( KMMCScBkupArchiveFileFormatUid1,
                      KMMCScBkupArchiveFileFormatUid2,
                      KMMCScBkupArchiveFileFormatUid3 );
    }


// ---------------------------------------------------------------------------
// MMCScBkupArchiveUtils::ArchiveCheckedUid()
// 
// 
// ---------------------------------------------------------------------------
void MMCScBkupArchiveUtils::ArchiveCheckedUid(TCheckedUid& aCheckedUids)
    {
    TUidType uidType;
    ArchiveUidType( uidType );
    aCheckedUids.Set( uidType );
    }


// ---------------------------------------------------------------------------
// MMCScBkupArchiveUtils::PhoneModelFromArchiveLC()
// 
// 
// ---------------------------------------------------------------------------
HBufC8* MMCScBkupArchiveUtils::PhoneModelFromArchiveLC( MMMCScBkupArchiveDataInterface& aADI )
    {
    const TInt phoneModelOffset = OffsetOfModelInformation();
    //
    RReadStream stream( aADI.ADIReadStreamUncompressedLC( phoneModelOffset ) );
    HBufC8* modelInfo = PhoneModelFromArchiveLC( stream);
    CleanupStack::PopAndDestroy(); // stream
    //
    return modelInfo;  
    }


// ---------------------------------------------------------------------------
// MMCScBkupArchiveUtils::ReadPhoneValidityInformationL()
// 
// 
// ---------------------------------------------------------------------------
void MMCScBkupArchiveUtils::ReadPhoneValidityInformationL( RFs& aFsSession, const TDesC& aFileName, HBufC8*& aPhoneModelData, TBitFlags& aArchiveFlags, TVersion& aArchiveVersion )
    {
    __ASSERT_DEBUG( aPhoneModelData == NULL, User::Invariant() );
    //
    RFile64 file;
    TInt error = file.Open( aFsSession, aFileName, EFileShareReadersOnly | EFileStream | EFileRead );
    User::LeaveIfError( error );
    CleanupClosePushL(file);

    // First, read the archive flags
    aArchiveFlags = ReadArchiveFlagsL( file );
    
    // Read version
    ReadArchiveVersionL( file, aArchiveVersion );
    
    // Then, create a stream interface to the file
    const TInt phoneModelOffset = OffsetOfModelInformation();
    RFileReadStream stream( file, phoneModelOffset );
    CleanupStack::Pop( &file );
    CleanupClosePushL( stream ); // stream takes ownership of the file now
    //
    HBufC8* modelInfo = PhoneModelFromArchiveLC( stream );
    
    // Unfortunately we have to juggle the cleanup stack
    CleanupStack::Pop( modelInfo );
    CleanupStack::PopAndDestroy( &stream ); // also closes the file
    
    // Done
    aPhoneModelData = modelInfo;
    }


// ---------------------------------------------------------------------------
// MMCScBkupArchiveUtils::ArchiveRunTimeFileFormatVersion()
// 
// 
// ---------------------------------------------------------------------------
TVersion MMCScBkupArchiveUtils::ArchiveRunTimeFileFormatVersion()
    {
    return TVersion( KMMCScBkupArchiveFileFormatVersionMajor, 
                     KMMCScBkupArchiveFileFormatVersionMinor, 
                     KMMCScBkupArchiveFileFormatVersionBuild );
    }


// ---------------------------------------------------------------------------
// MMCScBkupArchiveUtils::WriteHeaderL()
// 
// 
// ---------------------------------------------------------------------------
const TMMCScBkupArchiveVector& MMCScBkupArchiveUtils::WriteHeaderL( MMMCScBkupArchiveDataInterface& aADI, 
    TBitFlags aCategory )
    {
    __LOG("MMCScBkupArchiveUtils::WriteHeaderL() - START");
    RWriteStream stream( aADI.ADIWriteStreamUncompressedLC() );

    // 16 bytes = uids + *uid* checksum
    __LOG("MMCScBkupArchiveUtils::WriteHeaderL() - [16] write uids + checksum");
    TCheckedUid checkedUid;
    ArchiveCheckedUid( checkedUid );
    const TPtrC8 pUidAndCRC( checkedUid.Des() );
    stream.WriteL( pUidAndCRC );
    
    // 4 bytes = file format version
    __LOG("MMCScBkupArchiveUtils::WriteHeaderL() - [ 4] write version");
    const TVersion version( ArchiveRunTimeFileFormatVersion() );
    stream.WriteInt8L( version.iMajor );
    stream.WriteInt8L( version.iMinor );
    stream.WriteInt16L( version.iBuild );

    // 4 bytes = Footer length - starts life as length of 0, and
    // is then written to again later on after the footer has
    // been exteranlised.
    __LOG("MMCScBkupArchiveUtils::WriteHeaderL() - [ 4] write size of footer (starts life as 0)");
    const TInt footerLength = 0;
    stream.WriteInt32L( footerLength );

    // 4 bytes = Archive flags
    __LOG("MMCScBkupArchiveUtils::WriteHeaderL() - [ 4] archive flags (archive starts life invalid)");
    const TUint32 defaultArchiveFlags = DefaultArchiveFlags();
    stream.WriteUint32L( defaultArchiveFlags );

    // 4 bytes = Initial payload CRC value, this will be updated with the final 
    // CRC once the entire archive has been prepared.
    __LOG("MMCScBkupArchiveUtils::WriteHeaderL() - [ 4] archive payload CRC");
    const TUint32 initialPayloadCRC = 0;
    stream.WriteUint32L( initialPayloadCRC );
    
    __LOG("MMCScBkupArchiveUtils::WriteHeaderL() - [ 4] archive category");
    stream.WriteUint32L( aCategory.iFlags );
    
    // 4 bytes = Initial header CRC value, this will be updated with the final 
    // CRC once the entire archive has been prepared.
    __LOG("MMCScBkupArchiveUtils::WriteHeaderL() - [ 4] archive header CRC");
    const TUint32 initialHeaderCRC = 0;
    stream.WriteUint32L( initialHeaderCRC );
    
    // KMMCScSpareByteCount = padding, for future use
    __LOG1("MMCScBkupArchiveUtils::WriteHeaderL() - [%d] padding/spare data", KMMCScSpareByteCount);
    for(TInt i = 0; i < KMMCScSpareByteCount/sizeof(TInt32); i++)
        {
        stream.WriteInt32L( 0 );
        }

    // 1 byte = Phone model version string length
    __LOG("MMCScBkupArchiveUtils::WriteHeaderL() - [ 1] phone model length");
    HBufC8* phoneModelString = MMCScBkupPhoneModelUtils::CurrentPhoneModelLC();
    stream.WriteInt8L( phoneModelString->Length() );

    // Then the version string itself
    __LOG("MMCScBkupArchiveUtils::WriteHeaderL() - [??] phone model string");
    stream.WriteL( *phoneModelString );

    // Tidy up
    __LOG("MMCScBkupArchiveUtils::WriteHeaderL() - committing stream...");
    stream.CommitL();
    __LOG("MMCScBkupArchiveUtils::WriteHeaderL() - destroying stream...");
    CleanupStack::PopAndDestroy( 2 ); // phoneModelString and stream

    __LOG("MMCScBkupArchiveUtils::WriteHeaderL() - END");
    return aADI.ADICurrentArchiveVectorInfo();
    }


// ---------------------------------------------------------------------------
// MMCScBkupArchiveUtils::ReadHeaderL()
// 
// 
// ---------------------------------------------------------------------------
const TMMCScBkupArchiveVector& MMCScBkupArchiveUtils::ReadHeaderL( MMMCScBkupArchiveDataInterface& aADI, CMMCScBkupArchiveHeader& aHeader )
    {
    __LOG("MMCScBkupArchiveUtils::ReadHeaderL() - START");
    RReadStream stream( aADI.ADIReadStreamUncompressedLC() );

    // 16 bytes = uids + *uid* checksum
    TBuf8<16> uidBuffer;
    stream.ReadL( uidBuffer, uidBuffer.MaxLength() );

    // Create a UID type objjec
    TCheckedUid checkedUid( uidBuffer );
    const TUidType uids( checkedUid.UidType()) ;
    __LOG3("MMCScBkupArchiveUtils::ReadHeaderL() - uids: 0x%08x/0x%08x/0x%08x", uids[0], uids[1], uids[2] );
    if  ( uids[0] == KNullUid && uids[1] == KNullUid && uids[2] == KNullUid )
        {
        // Apparently, TCheckedUid sets the uids to 0 when the CRC doesn't match
        // the value read from the file.
        User::Leave( KErrNotSupported );
        }
    aHeader.SetCheckedUid( checkedUid );
    
    // 4 bytes = file format version
    TVersion version;
    version.iMajor = stream.ReadInt8L();
    version.iMinor = stream.ReadInt8L();
    version.iBuild = stream.ReadInt16L();
    aHeader.SetVersion( version );
    __LOG3("MMCScBkupArchiveUtils::ReadHeaderL() - version: %3d.%3d.%6d", version.iMajor, version.iMinor, version.iBuild );

    // 4 bytes = Length of footer
    const TInt footerLength = stream.ReadInt32L();
    aHeader.SetFooterLength( footerLength );
    __LOG1("MMCScBkupArchiveUtils::ReadHeaderL() - footerLength: %d", footerLength);

    // 4 bytes = Archive flags
    const TUint32 archiveFlags = stream.ReadUint32L();
    aHeader.SetArchiveFlags( archiveFlags );
    __LOG1("MMCScBkupArchiveUtils::ReadHeaderL() - archiveFlags: %d", archiveFlags);

    // 4 bytes = Archive payload CRC
    const TUint32 payloadCRC = stream.ReadUint32L();
    __LOG1("MMCScBkupArchiveUtils::ReadHeaderL() - archive payload CRC: %d", payloadCRC);

    // 4 bytes = Archive Category
    const TUint32 archiveCategory = stream.ReadUint32L();
    __LOG1("MMCScBkupArchiveUtils::ReadHeaderL() - archiveCategory: %d", archiveCategory);

    // 4 bytes = Archive header CRC
    const TUint32 headerCRC = stream.ReadUint32L();
    __LOG1("MMCScBkupArchiveUtils::ReadHeaderL() - archive header CRC: %d", headerCRC);

    // KMMCScSpareByteCount of padding (spare) data. Skip for now
    for(TInt i = 0; i < KMMCScSpareByteCount/sizeof(TInt32); i++)
        {
        (void) stream.ReadInt32L();
        }

    // Then the version string itself - which is handled through a separate method:
    HBufC8* phoneModel = PhoneModelFromArchiveLC( stream );
    aHeader.SetPhoneModelIdentifierL( *phoneModel );
    CleanupStack::PopAndDestroy( phoneModel );

    // Clean up
    CleanupStack::PopAndDestroy(); // stream

    __LOG("MMCScBkupArchiveUtils::ReadHeaderL() - END");
    return aADI.ADICurrentArchiveVectorInfo();
    }


// ---------------------------------------------------------------------------
// MMCScBkupArchiveUtils::OffsetOfModelInformation()
// 
// 
// ---------------------------------------------------------------------------
TInt MMCScBkupArchiveUtils::OffsetOfModelInformation()
    {
    return EArchiveOffsetPhoneModelStringLength;
    }


// ---------------------------------------------------------------------------
// MMCScBkupArchiveUtils::PhoneModelFromArchiveLC()
// 
// 
// ---------------------------------------------------------------------------
HBufC8* MMCScBkupArchiveUtils::PhoneModelFromArchiveLC( RReadStream& aStream )
    {
    const TInt length = aStream.ReadInt8L();
    
    // Validate against preconditions
    const TInt maxLength = MMCScBkupPhoneModelUtils::MaximumPhoneModelIdentifierLength();
    if  ( length > maxLength || length < 0 )
        {
        User::Leave( KErrCorrupt );
        }

    // Now try to read the model identifier
    HBufC8* model = HBufC8::NewLC( length );
    TPtr8 pModel( model->Des() );
    aStream.ReadL( pModel, length );

    // All done
    return model;
    }


// ---------------------------------------------------------------------------
// MMCScBkupArchiveUtils::DefaultArchiveFlags()
// 
// 
// ---------------------------------------------------------------------------
TUint32 MMCScBkupArchiveUtils::DefaultArchiveFlags()
    {
    return EMMCScBkupArchiveFlagsDefault;
    }


// ---------------------------------------------------------------------------
// MMCScBkupArchiveUtils::SetArchiveContentAsValidL()
// 
// 
// ---------------------------------------------------------------------------
void MMCScBkupArchiveUtils::SetArchiveContentAsValidL( RFile64& aArchive )
    {
    TInt error = KErrNone;
    
    // Calculate the offset to the archive flags:
    const TUint archiveFlagsFileOffset = EArchiveOffsetArchiveFlags;
    
    // Flags are four bytes. Read the existing raw flag data
    TBuf8< KMMCScArchiveFlagsByteCount > flagData;
    error = aArchive.Read( static_cast<TInt64>( archiveFlagsFileOffset ), flagData );
    User::LeaveIfError( error );
    
    // Interpret the flag data as real flags
    RDesReadStream readStream( flagData );
    CleanupClosePushL( readStream );
    TUint32 archiveFlags = readStream.ReadUint32L();
    CleanupStack::PopAndDestroy( &readStream );
    
    // Twiddle the "archive is complete/valid" bits
    archiveFlags |= EMMCScBkupArchiveFlagsContentValid;
    
    // Write the flags back out the descriptor
    flagData.Zero();
    RDesWriteStream writeStream( flagData );
    CleanupClosePushL( writeStream );
    writeStream.WriteUint32L( archiveFlags );
    writeStream.CommitL();
    CleanupStack::PopAndDestroy( &writeStream );
    
    // Then write them to the file itself
    error = aArchive.Write( static_cast<TInt64>( archiveFlagsFileOffset ), flagData );
    User::LeaveIfError( error );
    }


// ---------------------------------------------------------------------------
// MMCScBkupArchiveUtils::SetFooterLengthL()
// 
// 
// ---------------------------------------------------------------------------
void MMCScBkupArchiveUtils::SetFooterLengthL( RFile64& aArchive, TInt aLength )
    {
    TInt error = KErrNone;
    
    // Calculate the offset to the footer length:
    const TUint archiveFooterLengthOffset = EArchiveOffsetFooterLength;
    
    // Prepare externalized representation of length
    TBuf8< KMMCScArchiveFooterLengthByteCount > footerLengthData;
    RDesWriteStream writeStream( footerLengthData );
    CleanupClosePushL( writeStream );
    writeStream.WriteInt32L( aLength );
    writeStream.CommitL();
    CleanupStack::PopAndDestroy( &writeStream );
    
    // Then write them to the file itself
    error = aArchive.Write( static_cast<TInt64>( archiveFooterLengthOffset ), footerLengthData );
    User::LeaveIfError( error );
    }

// ---------------------------------------------------------------------------
// MMCScBkupArchiveUtils::ReadBkUpCategoryInformationL()
// 
// 
// ---------------------------------------------------------------------------
TBitFlags MMCScBkupArchiveUtils::ReadBkUpCategoryInformationL( RFs& aFsSession, const TDesC& aFileName )
    {
    RFile64 file;
    TInt error = file.Open( aFsSession, aFileName, EFileShareReadersOnly | EFileStream | EFileRead );
    User::LeaveIfError( error );
    CleanupClosePushL(file);

    // Then, create a stream interface to the file
    RFileReadStream stream( file, EArchiveOffsetArchiveCategory );
    CleanupStack::Pop( &file );
    CleanupClosePushL( stream ); // stream takes ownership of the file now
    //
    TUint category = stream.ReadUint32L();

    CleanupStack::PopAndDestroy( &stream ); // also closes the file
    
    TBitFlags ret;
    ret.SetValue( category );
    
    return ret;
    }


#ifdef RD_FILE_MANAGER_BACKUP
// ---------------------------------------------------------------------------
// MMCScBkupArchiveUtils::SetArchiveCrcsL()
// 
// 
// ---------------------------------------------------------------------------
void MMCScBkupArchiveUtils::SetArchiveCrcsL( RFile64& aArchive, TUint32 aCrc )
    {
    TInt error = KErrNone;
    
    // Payload crc 
    SetArchiveCrcL( aArchive, aCrc, EArchiveOffsetArchivePayloadCRC );
    
    // Calculate and write header crc also to the file
    TBuf8< KMMCScArchivePhoneModelStringLength > length;
    TUint32 modelLength;
    TUint32 headerCrc = 0;
    
    error = aArchive.Read( static_cast<TInt64>( EArchiveOffsetPhoneModelStringLength ), length );
    User::LeaveIfError(error);
    RDesReadStream readStream( length );
    CleanupClosePushL( readStream );
    modelLength = readStream.ReadUint8L();
    CleanupStack::PopAndDestroy( &readStream );
    
	CalculateCrcFromArchive( headerCrc, aArchive, 0, EArchiveOffsetArchiveHeaderCRC);
	CalculateCrcFromArchive( headerCrc, aArchive, EArchiveOffsetArchiveHeaderCRC + KMMCScArchiveCrcByteCount, 
	     KMMCScSpareByteCount + KMMCScArchivePhoneModelStringLength + modelLength);
    SetArchiveCrcL( aArchive, headerCrc, EArchiveOffsetArchiveHeaderCRC );
    }


// ---------------------------------------------------------------------------
// MMCScBkupArchiveUtils::ValidateArchiveCrcsL()
// 
// 
// ---------------------------------------------------------------------------
TBool MMCScBkupArchiveUtils::ValidateArchiveCrcsL( RFs& aFsSession, const TDesC& aFileName )
    {
    TBool validCrc = EFalse;

    RFile64 file;
    TInt64 size;
    
    TInt error = file.Open( aFsSession, aFileName, EFileShareReadersOnly | EFileRead );
    
    if ( error == KErrNone && file.Size(size) == KErrNone)
        {
        TBuf8< KMMCScArchiveFlagsByteCount > headerCrc;
        TBuf8< KMMCScArchiveFlagsByteCount > payloadCrc;
        TBuf8< KMMCScArchivePhoneModelStringLength > length;
        TUint32 archivedHeaderCrc, calculatedHeaderCrc;
        TUint32 archivedPayloadCrc, calculatedPayloadCrc;
        TUint32 modelLength;
        
        // Read crcs from header
        error = file.Read( static_cast<TInt64>( EArchiveOffsetArchiveHeaderCRC ), headerCrc );
        User::LeaveIfError(error);
        error = file.Read( static_cast<TInt64>( EArchiveOffsetArchivePayloadCRC ), payloadCrc );
        User::LeaveIfError(error);
        error = file.Read( static_cast<TInt64>( EArchiveOffsetPhoneModelStringLength ), length );
        User::LeaveIfError(error);
        CleanupClosePushL( file );

        RDesReadStream readStream( headerCrc );
        CleanupClosePushL( readStream );
        archivedHeaderCrc = readStream.ReadUint32L();
        readStream.Close();
        readStream.Open(payloadCrc);
        archivedPayloadCrc = readStream.ReadUint32L();
        readStream.Close();
        readStream.Open(length);
        modelLength = readStream.ReadUint8L();
        CleanupStack::PopAndDestroy( &readStream );
        
        // Calculate crc from header in two parts
        calculatedHeaderCrc = 0;
    	CalculateCrcFromArchive( calculatedHeaderCrc, file, 0, EArchiveOffsetArchiveHeaderCRC);
    	CalculateCrcFromArchive( calculatedHeaderCrc, file, EArchiveOffsetArchiveHeaderCRC + KMMCScArchiveCrcByteCount, 
    	     KMMCScSpareByteCount + KMMCScArchivePhoneModelStringLength + modelLength);

        // Calculate crc from payload and footer
        TInt payloadSize = size - (EArchiveOffsetPhoneModelString + modelLength);
        calculatedPayloadCrc = 0;
    	CalculateCrcFromArchive( calculatedPayloadCrc, file, EArchiveOffsetPhoneModelString + modelLength, payloadSize);

    	if(archivedPayloadCrc == calculatedPayloadCrc && archivedHeaderCrc == calculatedHeaderCrc)
    	    {
    	    validCrc = ETrue;
    	    }
    	else
    	    {
    	    __LOG4("MMCScBkupArchiveUtils::ValidateArchiveCrcsL() - crc mismatch: %u vs. %u - %u vs. %u", 
    	        archivedPayloadCrc, calculatedPayloadCrc, archivedHeaderCrc, calculatedHeaderCrc);
    	    }
        
        CleanupStack::PopAndDestroy( &file );
        }
        
	return validCrc;
	}
#endif // RD_FILE_MANAGER_BACKUP


// ---------------------------------------------------------------------------
// MMCScBkupArchiveUtils::ReadArchiveFlagsL()
// 
// 
// ---------------------------------------------------------------------------
TBitFlags MMCScBkupArchiveUtils::ReadArchiveFlagsL( RFile64& aArchive )
    {
    TInt error = KErrNone;
    
    // Calculate the offset to the archive flags:
    const TUint archiveFlagsFileOffset = EArchiveOffsetArchiveFlags;
    
    // Flags are four bytes. Read the existing raw flag data
    TBuf8< KMMCScArchiveFlagsByteCount > flagData;
    error = aArchive.Read( static_cast<TInt64>( archiveFlagsFileOffset ), flagData );
    User::LeaveIfError( error );
    
    // Interpret the flag data as real flags
    RDesReadStream readStream( flagData );
    CleanupClosePushL( readStream );
    TUint32 archiveFlags = readStream.ReadUint32L();
    CleanupStack::PopAndDestroy( &readStream );

    // Done
    TBitFlags ret;
    ret.SetValue( archiveFlags );
    return ret;
    }


// ---------------------------------------------------------------------------
// MMCScBkupArchiveUtils::ReadArchiveVersionL()
// 
// 
// ---------------------------------------------------------------------------
void MMCScBkupArchiveUtils::ReadArchiveVersionL( RFile64& aArchive, TVersion& aVersion )
    {
    TInt error = KErrNone;
    
    // Calculate the offset to the archive flags:
    const TUint archiveVersionFileOffset = EArchiveOffsetFileFormatVersion;
    
    // Flags are four bytes. Read the existing raw flag data
    TBuf8< KMMCScArchiveVersionByteCount > versionData;
    error = aArchive.Read( static_cast<TInt64>( archiveVersionFileOffset ), versionData );
    User::LeaveIfError( error );
    
    // Interpret the flag data as real flags
    RDesReadStream readStream( versionData );
    CleanupClosePushL( readStream );
    aVersion.iMajor = readStream.ReadInt8L();
    aVersion.iMinor = readStream.ReadInt8L();
    aVersion.iBuild = readStream.ReadInt16L();
    CleanupStack::PopAndDestroy( &readStream );
    }


#ifdef RD_FILE_MANAGER_BACKUP
// ---------------------------------------------------------------------------
// MMCScBkupArchiveUtils::SetArchiveCrcL()
// 
// 
// ---------------------------------------------------------------------------
void MMCScBkupArchiveUtils::SetArchiveCrcL( RFile64& aArchive, TUint32 aCrc, TUint aOffset )
    {
    TInt error = KErrNone;
    
    // Prepare externalized representation of crc
    TBuf8< KMMCScArchiveCrcByteCount > crcData;
    RDesWriteStream writeStream( crcData );
    CleanupClosePushL( writeStream );
    writeStream.WriteInt32L( aCrc );
    writeStream.CommitL();
    CleanupStack::PopAndDestroy( &writeStream );
    
    // Then write crc to the file itself
    error = aArchive.Write( static_cast<TInt64>( aOffset ), crcData );
    User::LeaveIfError( error );
    }


// ---------------------------------------------------------------------------
// MMCScBkupArchiveUtils::CalculateCrcFromArchive()
// 
// 
// ---------------------------------------------------------------------------
void MMCScBkupArchiveUtils::CalculateCrcFromArchive( TUint32& aCrc, RFile64& aArchive, TUint32 aOffset, TUint32 aLength )
    {
    const TInt KBigBufSize=0x10000;
    const TInt KMediumBufSize=0x8000;
    const TInt KSmallBufSize=0x1000;

    // Allocate as large buffer as possible for crc validation, because 
    // need to read file content in chunks for crc calculation.
	HBufC8* bufPtr = HBufC8::New(KBigBufSize);
	
	if ( bufPtr == NULL )
		bufPtr = HBufC8::New(KMediumBufSize);
	if ( bufPtr == NULL )
		bufPtr = HBufC8::New(KSmallBufSize);
	
	if ( bufPtr != NULL)
	    {
        TPtr8 copyBuf = bufPtr->Des();
        TInt64 pos = aOffset;
        TInt size = aLength;
        
        // Loop through archive file skipping archive crc
    	while(size)
    		{
    		TInt s;
    		
		    s = Min( size, copyBuf.MaxSize() );
    		
    		TInt error = aArchive.Read( pos, copyBuf, s );
    		
    		if ( error == KErrNone && copyBuf.Length() != s )
    			break;
    		
    		pos += s;
    		size -= s;
    		Mem::Crc32(aCrc, copyBuf.Ptr(), copyBuf.Length());
    		}
    		
        delete bufPtr;
	    }
    }
#endif // RD_FILE_MANAGER_BACKUP

