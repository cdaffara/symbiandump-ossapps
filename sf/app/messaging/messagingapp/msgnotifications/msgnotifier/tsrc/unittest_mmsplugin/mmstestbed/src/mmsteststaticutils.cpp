/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 * This component and the accompanying materials are made available
 * under the terms of "Eclipse Public License v1.0"
 * which accompanies this distribution, and is available
 * at the URL "http://www.eclipse.org/legal/epl-v10.html".
 *
 * Initial Contributors:
 * Nokia Corporation - initial contribution.
 *
 * Contributors:
 *
 * Description:
 */

#include <f32file.h>
#include <centralrepository.h>
#include <apparc.h>
#include <msvapi.h>
#include <msvids.h>

#include "mmsteststaticutils.h"
#include "mmsconst.h"
#include <MmsEnginePrivateCRKeys.h>
#include "mmssettings.h"
#include <mmsheaders.h>
#include "mmsservercommon.h"
#include <mmsencode.h>


// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
TMmsJoinedArrays::TMmsJoinedArrays(CDesCArray* const & aArray1, CDesCArray* const & aArray2)
: iArray1(aArray1), iArray2(aArray2)
    {
    }

TInt TMmsJoinedArrays::MdcaCount() const
    {
    TInt count=0;
    if (iArray1)
        count += iArray1->MdcaCount();
    if (iArray2)
        count += iArray2->MdcaCount();
    return count;
    }

TPtrC16 TMmsJoinedArrays::MdcaPoint(TInt aIndex) const
    {
    if (iArray1)
        {
        if (aIndex < iArray1->MdcaCount())
            return iArray1->MdcaPoint(aIndex);
        else
            aIndex -= iArray1->MdcaCount();
        }
    return iArray2->MdcaPoint(aIndex);
    }


// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
TMmsTestUtils::TMmsTestUtils()
    {

    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
HBufC* TMmsTestUtils::ReadContactFromFileL( TDesC& aFileName, RFs& aFs )
    {
    RFile file;
    TInt error = KErrNone;

    TInt textBufferSize = 256;
    HBufC* textBuffer = HBufC::NewL( textBufferSize );
    TPtr textPtr = textBuffer->Des();
    TFileText textFile;

    error = file.Open( aFs, aFileName,
            EFileRead|EFileShareReadersOnly );

    if ( error == KErrNone )
        {
        textFile.Set( file );
        error = textFile.Seek( ESeekStart );
        }

    if ( error == KErrNone )
        {
        error = textFile.Read( textPtr );
        }

    file.Close();

    if ( textBuffer->Des().Find( &KMmsByteOrderMark, 1 ) == 0 )
        {
        textBuffer->Des().Copy( textBuffer->Des().Mid( 1 ) );
        }

    return textBuffer;
    }

//----------------------------------------------------------------------------------------
// turn on detailed logging while decoding a message
//----------------------------------------------------------------------------------------
//
void TMmsTestUtils::DecodeLoggingOnL()
    {
    // CenRep for decodelogging
    CRepository* repository = NULL;
    TInt retval = KErrNone;
    TRAP( retval, repository = CRepository::NewL( KUidMmsServerMtm ) );
    if( retval == KErrNone )
        {
        // Best effort - if cannot access repository, no can do
        TInt temp = 1;
        repository->Set( KMmsEngineDecodeLog, temp );
        delete repository;
        }
    }

//----------------------------------------------------------------------------------------
// turn off detailed logging while decoding a message
//----------------------------------------------------------------------------------------
//
void TMmsTestUtils::DecodeLoggingOffL()
    {
    // CenRep for decodelogging
    CRepository* repository = NULL;
    TInt retval = KErrNone;
    TRAP( retval, repository = CRepository::NewL( KUidMmsServerMtm ) );
    if( retval == KErrNone )
        {
        // Best effort - if cannot access repository, no can do
        TInt temp = 0;
        repository->Set( KMmsEngineDecodeLog, temp );
        delete repository;
        }

    }

//----------------------------------------------------------------------------------------
// turn on binary dump of incoming messages
//----------------------------------------------------------------------------------------
//
void TMmsTestUtils::BinaryDumpOnL()
    {
    // CenRep for binarydump setting
    CRepository* repository = NULL;
    TInt retval = KErrNone;
    TRAP( retval, repository = CRepository::NewL( KUidMmsServerMtm ) );
    if( retval == KErrNone )
        {
        // Best effort - if cannot access repository, no can do
        TInt temp = 1;
        repository->Set( KMmsEngineBinaryDump, temp );
        delete repository;
        }

    }

//----------------------------------------------------------------------------------------
// turn off binary dump of incoming messages
//----------------------------------------------------------------------------------------
//
void TMmsTestUtils::BinaryDumpOffL()
    {
    // CenRep for binarydump setting
    CRepository* repository = NULL;
    TInt retval = KErrNone;
    TRAP( retval, repository = CRepository::NewL( KUidMmsServerMtm ) );
    if( retval == KErrNone )
        {
        // Best effort - if cannot access repository, no can do
        TInt temp = 0;
        repository->Set( KMmsEngineBinaryDump, temp );
        delete repository;
        }
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void TMmsTestUtils::CleanDirectoryL( RFs& aFs )
    {
    // Delete the files in the directory
    CFileMan* fileMan = CFileMan::NewL( aFs );
    CleanupStack::PushL( fileMan );
    fileMan->RmDir( KMmsDefaultLocalModeDir );
    fileMan->RmDir( KMmsMMBoxDirectory );
    fileMan->RmDir( KMmsMMBoxDescriptionDirectory );
    CleanupStack::PopAndDestroy(); // fileManager
    // we don't want to throw these away!
    aFs.MkDirAll( KMmsDefaultLocalModeDir );
    aFs.MkDirAll( KMmsMMBoxDirectory );
    aFs.MkDirAll( KMmsMMBoxDescriptionDirectory );
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void TMmsTestUtils::Dump( CBufFlat& aBuffer, TFileName& aFilename, TParse& aParse, RFs& aFs )
    {
    TInt error = KErrNone;
    aFs.MkDirAll( aFilename );
    TUint att;
    if ( aFs.Att( aFilename, att ) == KErrNone )
        {
        _LIT( KRelated, "dump.mms");
        aParse.Set( aFilename, &KRelated, NULL );
        aFilename = aParse.FullName();
        error = CApaApplication::GenerateFileName( aFs, aFilename );
        if ( error == KErrNone )
            {
            RFile file;
            error = file.Create( aFs, aFilename, EFileWrite | EFileShareExclusive );
            // for message id generation
            aParse.Set( aFilename, NULL, NULL );
            if ( error == KErrNone )
                {
                // the data is supposed to be in the encode buffer
                TPtr8 ptr = aBuffer.Ptr( 0 );
                file.Write( ptr );
                file.Flush();
                }

            // done - close files
            file.Close();
            }
        }
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void TMmsTestUtils::CleanBoxL(TMsvId aBoxId, CMsvSession& aSession)
    {
    CMsvEntry* cEntry = NULL;
    // delete all messages from the specified box
    cEntry = aSession.GetEntryL(KMsvRootIndexEntryId);
    CleanupStack::PushL(cEntry);
    cEntry->SetEntryL(aBoxId);
    // show invisible entries
    cEntry->SetSortTypeL( TMsvSelectionOrdering( KMsvNoGrouping, EMsvSortByNone, ETrue ) );
    CMsvEntrySelection* msvEntrySelection = cEntry->ChildrenWithMtmL(KUidMsgTypeMultimedia);
    CleanupStack::PushL(msvEntrySelection);

    CMsvEntrySelection* selection = NULL;
    cEntry->SetSortTypeL( TMsvSelectionOrdering( KMsvNoGrouping, EMsvSortByNone, ETrue ) );
    selection = cEntry->ChildrenWithMtmL(KUidMsgMMSNotification);
    CleanupStack::PushL( selection );
    if ( selection->Count() > 0 )
        {
        msvEntrySelection->AppendL( selection->Back( 0 ), selection->Count() );
        }
    CleanupStack::PopAndDestroy(); // selection

    int i;
    for (i = 0; i < msvEntrySelection->Count(); ++i)
        {
        CMsvEntry* entry = aSession.GetEntryL( msvEntrySelection->At(i) );
        CleanupStack::PushL( entry );
        TMsvEntry tEntry = entry->Entry();
        tEntry.SetReadOnly(EFalse);
        entry->ChangeL(tEntry);
        cEntry->DeleteL( msvEntrySelection->At(i) );
        CleanupStack::PopAndDestroy( entry );
        }

    CleanupStack::PopAndDestroy(msvEntrySelection);
    CleanupStack::PopAndDestroy(cEntry);
    }

//----------------------------------------------------------------------------------------
// turn on logging email recipients
//----------------------------------------------------------------------------------------
void TMmsTestUtils::EmailLoggingOnL()
    {
    // CenRep for binarydump setting
    CRepository* repository = NULL;
    TInt retval = KErrNone;
    TRAP( retval, repository = CRepository::NewL( KUidMmsServerMtm ) ); // ***
    if( retval == KErrNone )
        {
        TInt temp = 1;
        repository->Set( KMmsEngineLogEmailRecipients, temp );
        delete repository;
        }

    }

//----------------------------------------------------------------------------------------
// turn off logging email recipients
//----------------------------------------------------------------------------------------
void TMmsTestUtils::EmailLoggingOffL()
    {
    // CenRep for binarydump setting
    CRepository* repository = NULL;
    TInt retval = KErrNone;
    TRAP( retval, repository = CRepository::NewL( KUidMmsServerMtm ) ); // ***
    if( retval == KErrNone )
        {
        TInt temp = 0;
        repository->Set( KMmsEngineLogEmailRecipients, temp );
        delete repository;
        }

    }

//----------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------
TInt TMmsTestUtils::CountChildrenL(TMsvId aBoxId, CMsvEntrySelection*& aMsvEntrySelection, CMsvSession& aSession, TUid aMessageType )
    {
    CMsvEntry* cEntry = aSession.GetEntryL(KMsvRootIndexEntryId);
    CleanupStack::PushL(cEntry);
    cEntry->SetEntryL(aBoxId);
    if (aMsvEntrySelection != NULL)
        {
        delete aMsvEntrySelection;
        aMsvEntrySelection = NULL;
        }
    aMsvEntrySelection = cEntry->ChildrenWithMtmL(aMessageType);
    CleanupStack::PopAndDestroy(); // cEntry
    return aMsvEntrySelection->Count();
    }


// -----------------------------------------------------------------------------
// CreateFolderEntryL
//
// -----------------------------------------------------------------------------
//
void TMmsTestUtils::CreateFolderEntryL(
    CMsvSession& aSession,
    TMsvId aParentFolder,
    const TDesC& aFolderName,
    TMsvId& aFolderId )
    {
    aFolderId = KMsvNullIndexEntryId;
    CMsvEntry* cEntry = aSession.GetEntryL( aParentFolder );
    CleanupStack::PushL( cEntry );

    // Create a new folder.

    TMsvEntry entry;
    entry.iType = KUidMsvFolderEntry;
    entry.iMtm = KUidMsvLocalServiceMtm;
    entry.iDetails.Set( aFolderName );
    entry.SetVisible( EFalse );
    entry.SetInPreparation( EFalse );
    entry.iServiceId = KMsvLocalServiceIndexEntryId;
    cEntry->CreateL( entry );
    aFolderId = entry.Id();
    CleanupStack::PopAndDestroy( cEntry );

    }


//----------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------
TMsvId TMmsTestUtils::CreateNotificationEntryL(
        TMsvId aNotificationFolder,
        TMsvId aServiceId,
        CBufFlat* aEncodeBuffer,
        CMsvSession& aSession )
    {
    TMsvId entryId = KMsvNullIndexEntryId;
    if ( aNotificationFolder == KMsvNullIndexEntryId )
        {
        // no folder no entry
        return entryId;
        }

    CMsvEntry* cEntry = aSession.GetEntryL( aNotificationFolder );
    CleanupStack::PushL(cEntry);

    TMsvEntry entry;
    entry.iType = KUidMsvMessageEntry;
    entry.iMtm = KUidMsgTypeMultimedia;
    entry.SetVisible( ETrue );
    // If we want to put data here, InPreparation must be set to true first
    entry.SetInPreparation( EFalse );
    entry.iServiceId = KMsvLocalServiceIndexEntryId;
    entry.iRelatedId = aServiceId;
    entry.iMtmData2 = KMmsNotificationBinary;
    cEntry->CreateL( entry );
    entryId = entry.Id();

    //
    // Stream
    // 1) length of the data as 32 bit integer
    // 2) pushed message data
    // into created entry's stream
    //
    cEntry->SetEntryL( entryId );
    CMsvStore* store = cEntry->EditStoreL();
    CleanupStack::PushL( store );   // ***
    RMsvWriteStream outs;
    outs.AssignLC( *store, KUidBinaryNotificationStream ); // ***
    TPtrC8 ptr = aEncodeBuffer->Ptr( 0 );
    outs.WriteUint32L( ptr.Length() );
    outs.WriteL( ptr );
    outs.CommitL();
    outs.Close();
    store->CommitL();

    CleanupStack::PopAndDestroy( &outs ); // close outs
    CleanupStack::PopAndDestroy( store );
    CleanupStack::PopAndDestroy( cEntry );

    return entryId;

    }

//----------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------
void TMmsTestUtils::FormNotification(
        TDesC8& aUrl,
        TInt aSize,
        CMmsHeaders& aMmsHeaders,
        CMmsEncode& aMmsEncoder,
        CBufFlat* aEncodeBuffer )
    {
    // for test purposes aUrl will contain the filename.

    // Reset sets the default encapsulation version
    // The default version has been set from MmsSettings in NewL
    aMmsHeaders.Reset();

    // construct the notification into iMmsHeaders, and call encode

    aMmsHeaders.SetMessageType( KMmsMessageTypeMNotificationInd );

    TTime currentTime;
    currentTime.UniversalTime();
    currentTime.Int64();

    TPtrC8 tid;
    TBufC8<KMMSMAXTIDLENGTH> target;
    TInt random = 0;

    // we don't generate a true random TID: We generate the
    // TID from the URL so that if we generate a notification
    // twice from the same file, we get the same TID and the
    // same URL. This way we can test the pruning function in
    // server MTM

    TInt i;
    for ( i = 0; i < aUrl.Length(); ++i )
        {
        random += aUrl[ i ];
        }

    target.Des().Num( random );
    tid.Set( target.Des() );
    aMmsHeaders.SetTidL( tid );

    aMmsHeaders.SetMessageClass( EMmsClassPersonal );
    aMmsHeaders.SetMessageSize( aSize );
    const TInt KTenHours = 10 * 60 * 60; // 10 hours relative expiry
    aMmsHeaders.SetExpiryInterval( KTenHours );
    aMmsHeaders.SetContentLocationL( aUrl );

    aMmsEncoder.EncodeHeadersL( aMmsHeaders, *aEncodeBuffer );

    }


//----------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------
TBool TMmsTestUtils::IsFile(const TDesC& aFileName, RFs& aFs)
    {
    //Is the name a file?
    if (IsDrive(aFileName))
        return EFalse;
    return !(IsDir(aFileName, aFs));
    }

//----------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------
TBool TMmsTestUtils::IsDir(const TDesC& aFileName, RFs& aFs)
    {
    //Is the name a directory?
    TEntry entry;
    TInt err = aFs.Entry(aFileName, entry);
    if (err)
        return EFalse;
    else
        return entry.IsDir();
    }

//----------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------
TBool TMmsTestUtils::IsDrive(const TDesC& aFileName)
    {
    //horrible little function to figure if the path is just a drive
    TBool retVal = EFalse;
    if (aFileName.Length()==3) //eg "c:\"
        {
        if ((aFileName[1] == ':')  && (aFileName[2] == '\\'))
            retVal=ETrue;
        }
    else if (aFileName.Length()==2) //eg "c:"
        {
        if (aFileName[1] == ':')
            retVal=ETrue;
        }
    return retVal;
    }

// end of file
