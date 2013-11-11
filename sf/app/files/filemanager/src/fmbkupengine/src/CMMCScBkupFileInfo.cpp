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
* Description: CMMCScBkupFileInfo implementation
*
*
*/

#include "CMMCScBkupFileInfo.h"




// ========================= MEMBER FUNCTIONS ================================

// ---------------------------------------------------------------------------
// CMMCScBkupFileInfo::CMMCScBkupFileInfo()
// 
// C++ constructor.
// ---------------------------------------------------------------------------
CMMCScBkupFileInfo::CMMCScBkupFileInfo()
    {
    }


// ---------------------------------------------------------------------------
// CMMCScBkupFileInfo::CMMCScBkupFileInfo()
// 
// C++ constructor.
// ---------------------------------------------------------------------------
CMMCScBkupFileInfo::CMMCScBkupFileInfo( const TEntry& aEntry, TSecureId aAssociatedSID )
:   iSecureId(aAssociatedSID)
    {
    iSize = aEntry.iSize;
    iDateTime = aEntry.iModified;
    iAttributes = aEntry.iAtt;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupFileInfo::~CMMCScBkupFileInfo()
// 
// Destructor.
// ---------------------------------------------------------------------------
CMMCScBkupFileInfo::~CMMCScBkupFileInfo()
    {
    delete iFileName;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupFileInfo::ConstructL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupFileInfo::ConstructL( const TEntry& aEntry )
    {
    iFileName = aEntry.iName.AllocL();
    }


// ---------------------------------------------------------------------------
// CMMCScBkupFileInfo::NewL()
// 
// 
// ---------------------------------------------------------------------------
CMMCScBkupFileInfo* CMMCScBkupFileInfo::NewL( const TEntry& aEntry, TSecureId aAssociatedSID )
    {
    CMMCScBkupFileInfo* self = NewLC( aEntry, aAssociatedSID );
    CleanupStack::Pop(self);
    return self;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupFileInfo::NewLC()
// 
// 
// ---------------------------------------------------------------------------
CMMCScBkupFileInfo* CMMCScBkupFileInfo::NewLC( const TEntry& aEntry, TSecureId aAssociatedSID )
    {
    CMMCScBkupFileInfo* self = new(ELeave) CMMCScBkupFileInfo( aEntry, aAssociatedSID );
    CleanupStack::PushL(self);
    self->ConstructL(aEntry);
    return self;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupFileInfo::NewLC()
// 
// 
// ---------------------------------------------------------------------------
CMMCScBkupFileInfo* CMMCScBkupFileInfo::NewLC( RReadStream& aStream )
    {
    CMMCScBkupFileInfo* self = new(ELeave) CMMCScBkupFileInfo();
    CleanupStack::PushL(self);
    aStream >> *self;
    return self;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupFileInfo::Drive()
// 
// 
// ---------------------------------------------------------------------------
TDriveNumber CMMCScBkupFileInfo::Drive() const
    {
    const TDriveUnit driveUnit( FileName() );
    const TDriveNumber drive = static_cast< TDriveNumber > ( driveUnit.operator TInt() );
    //
    return drive;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupFileInfo::InternalizeL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupFileInfo::InternalizeL(RReadStream& aStream)
    {
    aStream.ReadInt32L(); // EStreamFormatVersion1 
    aStream.ReadInt32L(); // spare1
    aStream.ReadInt32L(); // spare2
    aStream.ReadInt32L(); // spare3
    //
    HBufC* fileName = HBufC::NewL(aStream, KMaxFileName);
    delete iFileName;
    iFileName = fileName;
    //
    TInt64 intermediateDateTime;
    aStream >> intermediateDateTime;
    iDateTime = TTime(intermediateDateTime);
    //
    TUid intermediateSID;
    aStream >> intermediateSID;
    iSecureId = intermediateSID;
    //
    iSize = aStream.ReadInt32L();
    iAttributes = aStream.ReadUint32L();
    }


// ---------------------------------------------------------------------------
// CMMCScBkupFileInfo::ExternalizeL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupFileInfo::ExternalizeL(RWriteStream& aStream) const
    {
    aStream.WriteInt32L( EStreamFormatVersion1 );
    aStream.WriteInt32L( 0 ); // spare1
    aStream.WriteInt32L( 0 ); // spare2
    aStream.WriteInt32L( 0 ); // spare3
    //
    aStream << *iFileName;
    aStream << iDateTime.Int64();
    aStream << iSecureId.operator TUid();
    aStream.WriteInt32L(iSize);
    aStream.WriteUint32L(iAttributes);
    }


// ---------------------------------------------------------------------------
// CMMCScBkupFileInfo::OrderByFileName()
// 
// 
// ---------------------------------------------------------------------------
TInt CMMCScBkupFileInfo::OrderByFileName(const CMMCScBkupFileInfo& aLeft, const CMMCScBkupFileInfo& aRight)
    {
    const TPtrC pLeft(aLeft.FileName());
    const TPtrC pRight(aRight.FileName());
    //
    const TInt ret = pLeft.Compare(pRight);
    //
    return ret;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupFileInfo::CompareByFileName()
// 
// 
// ---------------------------------------------------------------------------
TBool CMMCScBkupFileInfo::CompareByFileName(const CMMCScBkupFileInfo& aLeft, const CMMCScBkupFileInfo& aRight)
    {
    const TPtrC pLeft(aLeft.FileName());
    const TPtrC pRight(aRight.FileName());
    //
    const TBool same = (pLeft.Compare(pRight) == 0);
    return same;
    }

