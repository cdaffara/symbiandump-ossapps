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
* Description: CMMCScBkupFileListCollection implementation
*
*
*/

#include "CMMCScBkupFileListCollection.h"

#include <bldvariant.hrh>
#include <pathinfo.h>

// User includes
#include "MMCScBkupLogger.h"
#include "CMMCScBkupDataOwnerInfo.h"
#ifdef RD_FILE_MANAGER_BACKUP
#include <driveinfo.h>
#include "BkupEngine.hrh"
#include "pathconfiguration.hrh"
#endif 

// Constants
const TInt KMMCScBkupDataOwnerGranularity = 50;




// ========================= MEMBER FUNCTIONS ================================

// ---------------------------------------------------------------------------
// CMMCScBkupFileListCollection::CMMCScBkupFileListCollection()
// 
// C++ constructor.
// ---------------------------------------------------------------------------
CMMCScBkupFileListCollection::CMMCScBkupFileListCollection( TBitFlags aCategory, 
    RFs& aFsSession )
    :iEntries(KMMCScBkupDataOwnerGranularity), 
    iCategory( aCategory ),
    iFsSession( aFsSession )
    {
    }

// ---------------------------------------------------------------------------
// CMMCScBkupFileListCollection::~CMMCScBkupFileListCollection()
// 
// Destructor.
// ---------------------------------------------------------------------------
CMMCScBkupFileListCollection::~CMMCScBkupFileListCollection()
    {
    Reset();
    iEntries.Close();
    }


// ---------------------------------------------------------------------------
// CMMCScBkupFileListCollection::ConstructL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupFileListCollection::ConstructL()
    {
    }


// ---------------------------------------------------------------------------
// CMMCScBkupFileListCollection::NewL()
// 
// 
// ---------------------------------------------------------------------------
CMMCScBkupFileListCollection* CMMCScBkupFileListCollection::NewL( TBitFlags aCategory,
    RFs& aFsSession )
    {
    CMMCScBkupFileListCollection* self = new(ELeave) CMMCScBkupFileListCollection( aCategory, aFsSession );
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupFileListCollection::AppendL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupFileListCollection::AppendL( const RArray<TEntry>& aArray, RArray< const CMMCScBkupFileInfo* >& aAddedItems, TSecureId aAssociatedSID )
    {
    const TInt count = aArray.Count();

    __LOG2("CMMCScBkupFileListCollection::AppendL() - Number of files %d to check for category %x", count, Category().Value() );
    
    for(TInt i=0; i<count; i++)
        {
        const TEntry& entry = aArray[i];
#ifdef RD_FILE_MANAGER_BACKUP
        TBool skipFile = EFalse;

        if(Category().Value() != EBUCatUserFiles && Category().Value() != EBUCatAllInOne)
            {
            // Check whether public file in system user data folder. If it is and not part of
            // user file category, then discard file from collection
            const TDesC& path = PathInfo::PhoneMemoryRootPath();
            TInt error = entry.iName.FindF(path);
            if(error != KErrNotFound)
                {
                __LOG1("CMMCScBkupFileListCollection::AppendL() - Skipping file %S", &entry.iName );
                skipFile = ETrue;
                }
            }
        else if(Category().Value() == EBUCatUserFiles)
            {
            // Check also if public file is from mass-storage. If yes and user-files category in
            // question, prevent file getting backed up, because otherwise it is probable 2 GB 
            // get exceeded and we cannot save larger than 2 GB archive. Allow public files being 
            // backed up in other categories.
            TUint drvStatus( 0 );
            TInt drive( KErrNotFound );
            
            if( RFs::CharToDrive( entry.iName[0], drive ) != KErrNone )
                {
                drive = KErrNotFound;
                }
            
            TInt err = DriveInfo::GetDriveStatus(iFsSession, drive, drvStatus );
            
            if ( err == KErrNone && ( drvStatus & DriveInfo::EDriveInternal ) &&
                 ( drvStatus & DriveInfo::EDriveExternallyMountable ) )
                {
                __LOG1("CMMCScBkupFileListCollection::AppendL() - Skipping file %S due it in mass storage", &entry.iName );
                skipFile = ETrue;
                }
            }

        if(!skipFile)
#endif // RD_FILE_MANAGER_BACKUP
            {
            // Check whether the file is already present before adding it (avoid duplicate 
            // public files)
            CMMCScBkupFileInfo* fileInfo = CMMCScBkupFileInfo::NewLC( entry, aAssociatedSID );

            TLinearOrder<CMMCScBkupFileInfo> comparer( CMMCScBkupFileInfo::OrderByFileName );
            const TInt indexIfFound = iEntries.FindInOrder( fileInfo, comparer );
            if  (indexIfFound < 0)
                {
                // Item wasn't found - insert it in sorted order
                AppendL( fileInfo );
                CleanupStack::Pop( fileInfo );
                
                __LOG1("CMMCScBkupFileListCollection::AppendL() - Adding file %S", &fileInfo->FileName() );

                // Add a copy to our return array.
                aAddedItems.AppendL( fileInfo );
                }
            else
                {
                // Duplicate, discard
                __LOG1("CMMCScBkupFileListCollection::AppendL() - Duplicate file %S", &fileInfo->FileName() );
                CleanupStack::PopAndDestroy( fileInfo );
                }
            }
        }
    }


// ---------------------------------------------------------------------------
// CMMCScBkupFileListCollection::AppendL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupFileListCollection::AppendL( CMMCScBkupFileInfo* aFileInfo )
    {
    TLinearOrder<CMMCScBkupFileInfo> comparer( CMMCScBkupFileInfo::OrderByFileName );
    iEntries.InsertInOrderL( aFileInfo, comparer );
    }


// ---------------------------------------------------------------------------
// CMMCScBkupFileListCollection::Count()
// 
// 
// ---------------------------------------------------------------------------
TInt CMMCScBkupFileListCollection::Count() const
    {
    return iEntries.Count();
    }


// ---------------------------------------------------------------------------
// CMMCScBkupFileListCollection::Entry()
// 
// 
// ---------------------------------------------------------------------------
const CMMCScBkupFileInfo& CMMCScBkupFileListCollection::Entry(TInt aIndex) const
    {
    const CMMCScBkupFileInfo& entry = *iEntries[aIndex];
    return entry;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupFileListCollection::Remove()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupFileListCollection::Remove( TInt aIndex )
    {
    CMMCScBkupFileInfo* entry = iEntries[aIndex];
    delete entry;
    iEntries.Remove(aIndex);
    }


// ---------------------------------------------------------------------------
// CMMCScBkupFileListCollection::Reset()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupFileListCollection::Reset()
    {
    iEntries.ResetAndDestroy();
    }

