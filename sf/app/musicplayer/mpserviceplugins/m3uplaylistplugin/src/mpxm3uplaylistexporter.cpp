/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  externalizes a m3u playlist
*
*  CMPXM3uPlaylistExporter is a self-completing CActiveObject. It starts
*  its exporting process by invoking DoTaskStep as soon as it's instantiated.
*  Although it doesn't have a state machine, it will still process the request
*  in steps. In each step, a fixed number of medias, KMPXM3UNumOfMediasToProcess,
*  are processed. These medias will be converted into M3U line array,
*  M3U line array will be transformed into buffer, and then the buffer will be
*  flashed into the file. Once done, it completes its own request and RunL is
*  called where DoTaskStep is called again to process another
*  KMPXM3UNumOfMediasToProcess until all medias have been processed.
*  After all medias have been processed, it notifies its caller of the
*  results through MMPXPlaylistPluginObserver interface. Once its caller
*  completes processing of these results, caller's request is completed.
*
*
*/


// INCLUDE FILES
#include <bautils.h>
#include <utf.h>
#include <charconv.h>
#include <sysutil.h>
#ifdef RD_MULTIPLE_DRIVE
#include <driveinfo.h>
#endif //RD_MULTIPLE_DRIVE
#include <mpxlog.h>
#include <mpxmediaarray.h>
#include <mpxattribute.h>
#include <mpxmediageneraldefs.h>
#include <mpxmediacontainerdefs.h>
#include "mpxm3uplaylistdefs.h"
#include "mpxm3uplaylistexporter.h"


// ============================ MEMBER FUNCTIONS ==============================
// ----------------------------------------------------------------------------
// Constructor.
// ----------------------------------------------------------------------------
EXPORT_C CMPXM3uPlaylistExporter::CMPXM3uPlaylistExporter(
            RFs* aFs,
            MMPXPlaylistPluginObserver* aObserver,
            const CMPXMedia& aPlaylist,
            TRequestStatus& aStatus ) :
    CActive( EPriorityStandard ),
    iPlaylist( aPlaylist ),
    iDriveNumber( EDriveE ),
    iPlaylistFileCreated( EFalse ),
    iFs( aFs ),
    iObserver( aObserver ),
    iCurrentMedia( -1 ),
    iMoreToDo( ETrue ),
    iCallerStatus( &aStatus )
    {
    CActiveScheduler::Add(this);
    }

// ----------------------------------------------------------------------------
// 2nd phase constructor
// ----------------------------------------------------------------------------
EXPORT_C void CMPXM3uPlaylistExporter::ConstructL(
            const TDesC& aFilePath )
    {
    MPX_DEBUG1("CMPXM3uPlaylistExporter::ConstructL() entering");

    // aFilePath should exist
    __ASSERT_DEBUG(aFilePath.Length(), User::Leave(KErrCorrupt));

#ifdef RD_MULTIPLE_DRIVE
    // Use the default MMC drive
    User::LeaveIfError( DriveInfo::GetDefaultDrive(
        DriveInfo::EDefaultRemovableMassStorage,
        iDriveNumber ) );
#endif // RD_MULTIPLE_DRIVE

    // retrieve playlist medias
    CMPXMediaArray* ary = iPlaylist.Value<CMPXMediaArray>(KMPXMediaArrayContents);
    User::LeaveIfNull( ary );
    iMedias = CMPXMediaArray::NewL( *ary );

    // check if the playlist name contains invalid characters for the file system
    // if so, use filename provided in the URI if provided; otherwise leave with
    // KErrBadName
    const TDesC& playlistName =
        iPlaylist.ValueText(KMPXMediaGeneralTitle);

    HBufC* playlistFilename(NULL);

    if (iFs->IsValidName(playlistName))
        {
        playlistFilename = playlistName.AllocLC();
        }
    else if (iPlaylist.IsSupported(KMPXMediaGeneralUri))
        {
        TParsePtrC parse(iPlaylist.ValueText(KMPXMediaGeneralUri));
        playlistFilename = parse.Name().AllocLC();
        }
    else
        {
        User::Leave(KErrBadName);
        }

    // set playlist file path
    iPlaylistFilePath =
        HBufC::NewL( playlistFilename->Length() +
                     aFilePath.Length() +
                     KMPXM3UExtension().Length() );

    TPtr playlistFilePath = iPlaylistFilePath->Des();

    playlistFilePath.Append(aFilePath);
    playlistFilePath.Append(*playlistFilename);
    playlistFilePath.Append(KMPXM3UExtension);

    CleanupStack::PopAndDestroy(playlistFilename);

    // determine designation drive number
    TParsePtrC parse(*iPlaylistFilePath);
    TPtrC drive = parse.Drive();
    User::LeaveIfError(iFs->CharToDrive(TChar(drive.Ptr()[0]), iDriveNumber));

    *iCallerStatus = KRequestPending;

    TRequestStatus* status = &iStatus;
    *status = KRequestPending;
    User::RequestComplete(status, KErrNone);
    SetActive();

    MPX_DEBUG1("CMPXM3uPlaylistExporter::ConstructL() exiting");
    }

// ----------------------------------------------------------------------------
// Two-phased constructor.
// ----------------------------------------------------------------------------
EXPORT_C CMPXM3uPlaylistExporter* CMPXM3uPlaylistExporter::NewL(
            RFs* aFs,
            MMPXPlaylistPluginObserver* aObserver,
            const CMPXMedia& aPlaylist,
            const TDesC& aFilePath,
            TRequestStatus& aStatus )
    {
    CMPXM3uPlaylistExporter* self =
        new(ELeave)CMPXM3uPlaylistExporter(aFs, aObserver, aPlaylist, aStatus );
    CleanupStack::PushL(self);
    self->ConstructL( aFilePath );
    CleanupStack::Pop(self);
    return self;
    }

// ----------------------------------------------------------------------------
// Destructor.
// ----------------------------------------------------------------------------
EXPORT_C CMPXM3uPlaylistExporter::~CMPXM3uPlaylistExporter()
    {
    Cancel();
    Cleanup();

    delete iMedias;
    delete iPlaylistFilePath;
    }

// ----------------------------------------------------------------------------
// Handles request completion event
// ----------------------------------------------------------------------------
//
EXPORT_C void CMPXM3uPlaylistExporter::RunL()
    {
    MPX_DEBUG1("CMPXM3uPlaylistImporter::RunL");

    if ( iMoreToDo && iStatus.Int() == KErrNone )
        {
        DoTaskStep();
        SetActive();
        }
    else
        {
        User::RequestComplete( iCallerStatus, iStatus.Int() );
        Cleanup();
        NotifyClient(iStatus.Int());
        }
    }

// ----------------------------------------------------------------------------
// Implements cancellation of an outstanding request.
// ----------------------------------------------------------------------------
//
EXPORT_C void CMPXM3uPlaylistExporter::DoCancel()
    {
    MPX_DEBUG1("CMPXM3uPlaylistExporter::DoCancel");

    TInt error( KErrCancel );
    Cleanup();

    // notify client. return the playlist with the least invalid paths
    NotifyClient(error);

    if ( iCallerStatus )
        {
        User::RequestComplete( iCallerStatus, error );
        }
    }

// ----------------------------------------------------------------------------
// Performs one step of the task.
// ----------------------------------------------------------------------------
//
EXPORT_C void CMPXM3uPlaylistExporter::DoTaskStep()
    {
    MPX_DEBUG1("CMPXM3uPlaylistExporter::DoTaskStep()");

    TRequestStatus* status = &iStatus;
    *status = KRequestPending;

    TInt error( KErrNone );

    MPX_TRAP( error, ExternalizeL() );

    User::RequestComplete( status, error );
    }

// -----------------------------------------------------------------------------
// CMPXM3uPlaylistExporter::ExternalizeL
// -----------------------------------------------------------------------------
//
void CMPXM3uPlaylistExporter::ExternalizeL()
    {
    PopulateLineArrayL();
    FlushLineArrayToBufferL();
    WritePlaylistToFileL();
    Cleanup();

    if (iMedias->Count() == 0)
        {
        iMoreToDo = EFalse;
        }
    }

// -----------------------------------------------------------------------------
// CMPXM3uPlaylistExporter::PopulateLineArrayL
// -----------------------------------------------------------------------------
//
void CMPXM3uPlaylistExporter::PopulateLineArrayL()
    {
    MPX_DEBUG2("Before PopulateLineArrayL: heap size = %d", User::Heap().Size());

    // Parse lowercase path to the current folder
    // Convert playlist file path to lower case
    HBufC* plTmp = iPlaylistFilePath->AllocLC();

    TPtr ptr(plTmp->Des());
    ptr.LowerCase();

    TParse playlistPath;
    playlistPath.Set(*plTmp, NULL, NULL);
    // Path to the folder, where playlist file is located to
    TPtrC currentFolder = playlistPath.DriveAndPath();

    TInt count = iMedias->Count();
    TBool addNewLine(ETrue);
    for (TInt i = 0; i < KMPXM3UNumOfMediasToProcess && i < count; i++)
        {
        CMPXMedia* item = (*iMedias)[0];

        // Create and write Extented format tag if this is
        // the first media being processed
        if (++iCurrentMedia == 0)
            {
            CreateHeaderLineL();
            AddLineToArrayL();
            }

        // Create and write extendted info line
        CreateExtentedInfoLineL(*item);
        AddLineToArrayL();

        // Create and write path line
        if (i == count-1)
            {
            // This is last item => no new line
            addNewLine = EFalse;
            }

        CreatePathLineL(*item, currentFolder, addNewLine);

        // delete media now to save memory consumption
        iMedias->Remove(0);

        AddLineToArrayL();
        }

    CleanupStack::PopAndDestroy(plTmp ); // plTmp

    MPX_DEBUG2("After PopulateLineArrayL: heap size = %d", User::Heap().Size());
    }

// -----------------------------------------------------------------------------
// CMPXM3uPlaylistExporter::AddLineToArrayL
// -----------------------------------------------------------------------------
//
void CMPXM3uPlaylistExporter::AddLineToArrayL()
    {
    if (iLine)
        {
        // Convert line from Unicode to UTF-8
        // UTF-8 encoded character may consume several bytes =>
        // multiply the descriptor length in order to prevent overflow.
        // coverity[incorrect_multiplication]
        // coverity[buffer_alloc]
        HBufC8* line = HBufC8::NewLC(iLine->Length() * KMPXM3UUtf8ConvMultiplier);

        TPtr8 ptr = line->Des();
         // According to current knowledge, this should not be ASCII
        CnvUtfConverter::ConvertFromUnicodeToUtf8(ptr, *iLine);

        iLines.AppendL(line);
        CleanupStack::Pop( line );

        delete iLine;
        iLine = NULL;
        }
    }

// -----------------------------------------------------------------------------
// CMPXM3uPlaylistExporter::CreateHeaderLineL
// -----------------------------------------------------------------------------
//
void CMPXM3uPlaylistExporter::CreateHeaderLineL()
    {
    delete iLine;
    iLine = NULL;

    iLine = HBufC::NewL(KMPXM3UTagExtm3u().Length() + KMPXM3ULengthOfLineChange);
    TPtr ptr = iLine->Des();

    ptr.Append(KMPXM3UTagExtm3u);
    ptr.Append(KMPXM3ULineChange);
    }

// -----------------------------------------------------------------------------
// CMPXM3uPlaylistExporter::CreateExtentedLineL
// -----------------------------------------------------------------------------
//
void CMPXM3uPlaylistExporter::CreateExtentedInfoLineL(
            const CMPXMedia& aMedia)
    {
    delete iLine;
    iLine = NULL;

    const TDesC& title =
        (aMedia.IsSupported(KMPXMediaGeneralTitle)) ?
            aMedia.ValueText(KMPXMediaGeneralTitle) : KNullDesC();

    if (title.Length())
        {
        TInt timeEntry(0);

        if (aMedia.IsSupported(KMPXMediaGeneralDuration))
            {
            timeEntry = aMedia.ValueTObjectL<TInt>(KMPXMediaGeneralDuration);
            }

        if (timeEntry < KMPXM3UIgnoreTimeEntry || timeEntry > KMPXM3UMaxTimeEntry)
            {
            timeEntry = KMPXM3UIgnoreTimeEntry;
            }

        iLine = HBufC::NewL(KMPXM3UMaxLengthOfExtinfStaticPart + title.Length());

        TPtr ptr = iLine->Des();

        ptr.Append(KMPXM3UTagExtinf);
        ptr.AppendNum(timeEntry);
        ptr.Append(KMPXM3UPoint);
        ptr.Append(title);
        ptr.Append(KMPXM3ULineChange);
        }
    }

// -----------------------------------------------------------------------------
// CMPXM3uPlaylistExporter::CreatePathLineL
// -----------------------------------------------------------------------------
//
void CMPXM3uPlaylistExporter::CreatePathLineL(
            const CMPXMedia& aMedia,
            const TDesC& aCurrentFolder,
            TBool aAddNewLine )
    {
    if (!aMedia.IsSupported(KMPXMediaGeneralUri))
        {
        User::Leave(KErrArgument);
        }

    // Temporary pointer to item path
    const TDesC& itemPath = aMedia.ValueText(KMPXMediaGeneralUri);

    delete iLine;
    iLine = NULL;
    iLine = HBufC::NewL(itemPath.Length() + KMPXM3ULengthOfLineChange);
    TPtr ptr = iLine->Des();

    // Create temporary copy of the path and set it lowercase
    HBufC* path = itemPath.AllocLC();
    path->Des().LowerCase();
    // Try to find current folder path from the item path
    TInt offset = path->Find(aCurrentFolder);

    if (offset != KErrNotFound)
        {
        // File is under the current folder => append relative path
        ptr.Append(itemPath.Mid(aCurrentFolder.Length()));
        }
    else
        {
        // File is not under the current path => append absolute path
        ptr.Append(itemPath);
        }

    CleanupStack::PopAndDestroy(path); // path

    if (aAddNewLine)
        {
        ptr.Append(KMPXM3ULineChange);
        }
    }

// -----------------------------------------------------------------------------
// CMPXM3uPlaylistExporter::FlushLineArrayToBufferL
// -----------------------------------------------------------------------------
//
void CMPXM3uPlaylistExporter::FlushLineArrayToBufferL()
    {
    MPX_DEBUG2("Before FlushLineArrayToBufferL: heap size = %d", User::Heap().Size());

    TInt length(0);
    TInt count = iLines.Count();
    for (TInt ii = 0; ii < count; ++ii)
        {
        length += iLines[ii]->Length();
        }

    delete iPlaylistBuf;
    iPlaylistBuf = NULL;
    iPlaylistBuf = HBufC8::NewL(length);
    TPtr8 ptr = iPlaylistBuf->Des();

    for (TInt jj = 0; jj < count; ++jj)
        {
        ptr.Append(*iLines[0]);
        // delete the line from the lines array now to save memory consumption
        delete iLines[0];

        // remove the appended element from the array
        iLines.Remove(0);
        }

    MPX_DEBUG2("After FlushLineArrayToBufferL: heap size = %d", User::Heap().Size());
    }

// -----------------------------------------------------------------------------
// CMPXM3uPlaylistExporter::WritePlaylistToFileL
// -----------------------------------------------------------------------------
//
void CMPXM3uPlaylistExporter::WritePlaylistToFileL()
    {
    // Open playlist file
    RFile file;
    if (!iPlaylistFileCreated)
        {
        User::LeaveIfError(file.Replace(*iFs, *iPlaylistFilePath, EFileWrite));
        iPlaylistFileCreated = ETrue;
        }
    else
        {
        User::LeaveIfError(file.Open(*iFs, *iPlaylistFilePath, EFileWrite));
        }
    CleanupClosePushL(file);

    // Calculate the increase in the playlist file size
    TInt oldSize = 0;
    User::LeaveIfError(file.Size(oldSize));
    TInt sizeIncr = iPlaylistBuf->Size() - oldSize;

    if (sizeIncr > 0 &&
        SysUtil::DiskSpaceBelowCriticalLevelL(iFs, sizeIncr, iDriveNumber))
        {
        // Don't show any own notes here
        User::Leave(KErrDiskFull);
        }

    // Write file to permanent memory
    User::LeaveIfError(file.Write(oldSize, *iPlaylistBuf));

    file.Flush();
    CleanupStack::PopAndDestroy(&file); // file
    }

// ----------------------------------------------------------------------------
// Cleanup.
// ----------------------------------------------------------------------------
void CMPXM3uPlaylistExporter::Cleanup()
    {
    iLines.ResetAndDestroy();

    delete iLine;
    iLine = NULL;

    delete iPlaylistBuf;
    iPlaylistBuf = NULL;
    }

// ----------------------------------------------------------------------------
// Handles notifications to the client
// ----------------------------------------------------------------------------
void CMPXM3uPlaylistExporter::NotifyClient( TInt aError )
    {
    MPX_DEBUG2("CMPXM3uPlaylistExporter::NotifyClient(%d)", aError);

    if ( iObserver )
        {
        // notify client. return the playlist with the least invalid paths
        TRAP_IGNORE(iObserver->HandlePlaylistL( *iPlaylistFilePath, aError ));
        }

    if ( aError )
        {
        // delete the partial playlist file.
        iFs->Delete(*iPlaylistFilePath);
        }
    }

// End of file
