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
* Description: CMMCScBkupIndexPublicDataFiles implementation
*
*
*/

#include "CMMCScBkupIndexPublicDataFiles.h"

// User includes
#include "MMMCScBkupArchiveDataInterface.h"
#include "CMMCScBkupFileInfo.h"
#include "CMMCScBkupFileListCollection.h"


    /**
     * Public File Data
     * ================
     * 
     * This is the format of the data written by the
     * CMMCScBkupStateArchiveOpPublicDataFiles object
     * 
     *  PUB FILE DATA for file 0
     *  PUB FILE DATA for file 1
     *  PUB FILE DATA for file n
     * 
     * Public File Data Index
     * ======================
     * 
     * This is the format of the data written by this object.
     * The format allows the possibility of a future partial
     * restore (hopefully).
     * 
     *  4 bytes = count of data owners
     *
     *  FOR EACH PUBLIC FILE
     *  {
     *      externalised CMMCScBkupFileInfo object
     *      externalised TMMCScBkupArchiveVector (offset & compressed size) object
     *  }
     *
     **/



// ========================= MEMBER FUNCTIONS ================================

// ---------------------------------------------------------------------------
// CMMCScBkupIndexPublicDataFiles::CMMCScBkupIndexPublicDataFiles()
// 
// C++ constructor.
// ---------------------------------------------------------------------------
CMMCScBkupIndexPublicDataFiles::CMMCScBkupIndexPublicDataFiles()
:   CMMCScBkupIndexBase( EMMCScBkupOwnerDataTypePublicData )
    {
    }


// ---------------------------------------------------------------------------
// CMMCScBkupIndexPublicDataFiles::~CMMCScBkupIndexPublicDataFiles()
// 
// Destructor.
// ---------------------------------------------------------------------------
CMMCScBkupIndexPublicDataFiles::~CMMCScBkupIndexPublicDataFiles()
    {
    iEntries.Close();
    }


// ---------------------------------------------------------------------------
// CMMCScBkupIndexPublicDataFiles::NewLC()
// 
// 
// ---------------------------------------------------------------------------
CMMCScBkupIndexPublicDataFiles* CMMCScBkupIndexPublicDataFiles::NewLC()
    {
    CMMCScBkupIndexPublicDataFiles* self = new(ELeave) CMMCScBkupIndexPublicDataFiles();
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupIndexPublicDataFiles::AddIndexRecordL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupIndexPublicDataFiles::AddIndexRecordL( const TMMCScBkupArchiveVector& aInfo, const CMMCScBkupFileInfo& aFile )
    {
    // Create new entry
    TMMCScBkupPublicFileEntry entry( aInfo, aFile );

    // Add it to our index
    iEntries.AppendL( entry );
    }


// ---------------------------------------------------------------------------
// CMMCScBkupIndexPublicDataFiles::At()
// 
// 
// ---------------------------------------------------------------------------
const CMMCScBkupFileInfo& CMMCScBkupIndexPublicDataFiles::At( TInt aIndex, TMMCScBkupArchiveVector& aInfo ) const
    {
    const TMMCScBkupPublicFileEntry& entry = iEntries[ aIndex ];
    aInfo = entry.iInfo;
    return *entry.iFile;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupIndexPublicDataFiles::StoreL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupIndexPublicDataFiles::StoreL(MMMCScBkupDriver& aDriver)
    {
    MMMCScBkupArchiveDataInterface& archiveDataInterface = aDriver.DrvADI();
    RWriteStream stream(archiveDataInterface.ADIWriteStreamUncompressedLC());
    //    
    const TInt count = iEntries.Count();
    stream.WriteInt32L(count);
    //
    for(TInt i=0; i<count; i++)
        {
        const TMMCScBkupPublicFileEntry& entry = iEntries[i];
        stream << *entry.iFile;
        stream << entry.iInfo;
        }
    //
    stream.CommitL();
    CleanupStack::PopAndDestroy(); // stream

    // Update our base class info with the offset to the index
    SetVector( archiveDataInterface.ADICurrentArchiveVectorInfo() );
    }


// ---------------------------------------------------------------------------
// CMMCScBkupIndexPublicDataFiles::RestoreL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupIndexPublicDataFiles::RestoreL(MMMCScBkupDriver& aDriver)
    {
    CMMCScBkupFileListCollection& fileList = aDriver.DrvFileList();
    //
    MMMCScBkupArchiveDataInterface& archiveDataInterface = aDriver.DrvADI();
    RReadStream& stream = archiveDataInterface.ADIReadStreamUncompressedLC( Vector().Offset() );
    //
    const TInt count = stream.ReadInt32L();
    for(TInt i=0; i<count; i++)
        {
        TMMCScBkupPublicFileEntry entry;

        // Reconstruct the CMMCScBkupFileInfo entry in the stream
        CMMCScBkupFileInfo* fileInfo = CMMCScBkupFileInfo::NewLC( stream );

        // Give ownership to the file list collection
        fileList.AppendL( fileInfo );
        CleanupStack::Pop( fileInfo );
        entry.iFile = fileInfo;

        // Then read the offset to the file...
        stream >> entry.iInfo;

        // And finally add a new record to our index, so client's can
        // retrieve the info.
        iEntries.AppendL( entry );
        }
    //
    CleanupStack::PopAndDestroy(); // stream
    //
    const TMMCScBkupArchiveVector& readInfo = archiveDataInterface.ADICurrentArchiveVectorInfo();
    if  ( readInfo.Length() > Vector().Length() )
        {
        // We've read too much!
        User::Leave( KErrCorrupt );
        }
    }




