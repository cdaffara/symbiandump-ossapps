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
* Description: CMMCScBkupArchiveInfo Implementation
*
*
*/

#include "CMMCScBkupArchiveInfo.h"




// ========================= MEMBER FUNCTIONS ================================

// ---------------------------------------------------------------------------
// CMMCScBkupArchiveInfo::CMMCScBkupArchiveInfo()
// 
// C++ constructor.
// ---------------------------------------------------------------------------
CMMCScBkupArchiveInfo::CMMCScBkupArchiveInfo()
    {
    }

// ---------------------------------------------------------------------------
// CMMCScBkupArchiveInfo::~CMMCScBkupArchiveInfo()
// 
// Destructor.
// ---------------------------------------------------------------------------
CMMCScBkupArchiveInfo::~CMMCScBkupArchiveInfo()
    {
    delete iFileInfo;
    iSecureIds.Close();
    iExcludedSecureIds.Close();
    }


// ---------------------------------------------------------------------------
// CMMCScBkupArchiveInfo::ConstructL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupArchiveInfo::ConstructL( const TEntry& aEntry )
    {
    iFileInfo = CMMCScBkupFileInfo::NewL( aEntry, 0 );
    }


// ---------------------------------------------------------------------------
// CMMCScBkupArchiveInfo::NewL()
// 
// 
// ---------------------------------------------------------------------------
CMMCScBkupArchiveInfo* CMMCScBkupArchiveInfo::NewL( const TEntry& aEntry )
    {
    CMMCScBkupArchiveInfo* self = NewLC( aEntry );
    CleanupStack::Pop(self);
    return self;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupArchiveInfo::NewLC()
// 
// 
// ---------------------------------------------------------------------------
CMMCScBkupArchiveInfo* CMMCScBkupArchiveInfo::NewLC( const TEntry& aEntry )
    {
    CMMCScBkupArchiveInfo* self = new(ELeave) CMMCScBkupArchiveInfo( );
    CleanupStack::PushL(self);
    self->ConstructL(aEntry);
    return self;
    }

// ---------------------------------------------------------------------------
// CMMCScBkupArchiveInfo::SetSIDs()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupArchiveInfo::SetSIDs( RArray<TSecureId>& aSIDs )
    {
    TInt count = aSIDs.Count();
    
    for(TInt i = 0; i < count; i++ )
        {
        iSecureIds.Append(aSIDs[i]);
        }
    }

// ---------------------------------------------------------------------------
// CMMCScBkupArchiveInfo::SetExcludedSIDs()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupArchiveInfo::SetExcludedSIDs( RArray<TSecureId>& aExcludedSIDs )
    {
    TInt count = aExcludedSIDs.Count();
    
    for(TInt i = 0; i < count; i++ )
        {
        iExcludedSecureIds.Append(aExcludedSIDs[i]);
        }
    }

// ---------------------------------------------------------------------------
// CMMCScBkupArchiveInfo::SIDs()
// 
// 
// ---------------------------------------------------------------------------
const RArray<TSecureId>& CMMCScBkupArchiveInfo::SIDs( TBitFlags aCategory ) const
    {
    __ASSERT_ALWAYS(aCategory == iCategory, User::Invariant());
    
    return iSecureIds;
    }
    
// ---------------------------------------------------------------------------
// CMMCScBkupArchiveInfo::ExcludeSIDs()
// 
// 
// ---------------------------------------------------------------------------
const RArray<TSecureId>& CMMCScBkupArchiveInfo::ExcludedSIDs( TBitFlags aCategory ) const
    {
    __ASSERT_ALWAYS(aCategory == iCategory, User::Invariant());
    
    return iExcludedSecureIds;
    }
    
// ---------------------------------------------------------------------------
// CMMCScBkupArchiveInfo::Category()
// 
// 
// ---------------------------------------------------------------------------
EXPORT_C TBitFlags CMMCScBkupArchiveInfo::Category() const
    {
    return iCategory;
    }
        
// ---------------------------------------------------------------------------
// CMMCScBkupArchiveInfo::FileName()
// 
// 
// ---------------------------------------------------------------------------
EXPORT_C const TDesC& CMMCScBkupArchiveInfo::FileName() const
    {
    return iFileInfo->FileName();
    }

// ---------------------------------------------------------------------------
// CMMCScBkupArchiveInfo::DateTime()
// 
// 
// ---------------------------------------------------------------------------
EXPORT_C const TTime& CMMCScBkupArchiveInfo::DateTime() const
    {
    return iFileInfo->DateTime();
    }

// ---------------------------------------------------------------------------
// CMMCScBkupArchiveInfo::Drive()
// 
// 
// ---------------------------------------------------------------------------
EXPORT_C TDriveNumber CMMCScBkupArchiveInfo::Drive() const
    {
    return iFileInfo->Drive();
    }
