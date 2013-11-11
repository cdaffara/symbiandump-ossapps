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
* Description:  Imports a m3u playlist file
*
*/


// INCLUDE FILES
#include <bautils.h>
#include <utf.h>
#include <charconv.h>
#include <mpxlog.h>
#include <mpxmediaarray.h>
#include <mpxattribute.h>
#include <mpxmediageneraldefs.h>
#include <mpxmediacontainerdefs.h>
#include "mpxm3uplaylistdefs.h"
#include "mpxm3uplaylistimporter.h"


// ============================ CONSTANTS ==============================
const TUint KUnicodeBOM = 0xFEFF;
const TInt KMinimumConfidenceRequired = 60;
const TInt KPathStartingChars = 3;

// ============================ MEMBER FUNCTIONS ==============================
// ----------------------------------------------------------------------------
// Constructor. 
// ----------------------------------------------------------------------------
EXPORT_C CMPXM3uPlaylistImporter::CMPXM3uPlaylistImporter(
            RFs* aFs,
            MMPXPlaylistPluginObserver* aObserver,
            const CArrayFix<CCnvCharacterSetConverter::SCharacterSet>& aTopCharacterSet,
            const CArrayFix<CCnvCharacterSetConverter::SCharacterSet>& aAvailableCharacterSet,
            TRequestStatus& aStatus ) :
    CActive( EPriorityStandard ),
    iFs( aFs ),
    iObserver( aObserver ),
    iCallerStatus( &aStatus ),
    iEndLineNumber( KMPXM3UNumOfLinesToProcess ),
    iCurrentLineNumber( 0 ),
    iMoreToDo( ETrue ),
    iEndOfFile( EFalse ),
    iAutoEncodingInvalidItems( 0 ),
    iState( EMPXM3UReadBufferWithAutoDetectEncoding ),
    iTopCharacterSet( aTopCharacterSet ),
    iAvailableCharacterSet( aAvailableCharacterSet )
    {
    CActiveScheduler::Add( this );
    }

// ----------------------------------------------------------------------------
// 2nd phase constructor
// ----------------------------------------------------------------------------
EXPORT_C void CMPXM3uPlaylistImporter::ConstructL( const TDesC& aPlaylistUri )
    {
    MPX_DEBUG2("CMPXM3uPlaylistImporter::ConstructL(%S) entering", &aPlaylistUri);

    __ASSERT_DEBUG(aPlaylistUri.Length() != 0, User::Leave(KErrArgument));
    iPlaylistFilePath.Set(aPlaylistUri);

    iAutoEncodingPlaylistArray = CMPXMediaArray::NewL();
    
    *iCallerStatus = KRequestPending;

    TRequestStatus* status = &iStatus;
    *status = KRequestPending;
    User::RequestComplete(status, KErrNone);   
    SetActive();
    
    MPX_DEBUG1("CMPXM3uPlaylistImporter::ConstructL() exiting");
    }
    
// ----------------------------------------------------------------------------
// Two-phased constructor. 
// ----------------------------------------------------------------------------
EXPORT_C CMPXM3uPlaylistImporter* CMPXM3uPlaylistImporter::NewL(
            RFs* aFs,
            MMPXPlaylistPluginObserver* aObserver,
            const TDesC& aPlaylistUri,
            const CArrayFix<CCnvCharacterSetConverter::SCharacterSet>& aTopCharacterSet,
            const CArrayFix<CCnvCharacterSetConverter::SCharacterSet>& aAvailableCharacterSet,
            TRequestStatus& aStatus )            
    {
    CMPXM3uPlaylistImporter* self =
        new(ELeave)CMPXM3uPlaylistImporter(
            aFs, aObserver, aTopCharacterSet, aAvailableCharacterSet, aStatus );
    CleanupStack::PushL( self );
    self->ConstructL( aPlaylistUri );
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------------------------
// Destructor. 
// ----------------------------------------------------------------------------
//
EXPORT_C CMPXM3uPlaylistImporter::~CMPXM3uPlaylistImporter()
    {
    Cancel();
    Cleanup();
    }

// ----------------------------------------------------------------------------
// Handles request completion event
// ----------------------------------------------------------------------------
//
EXPORT_C void CMPXM3uPlaylistImporter::RunL()
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
        NotifyClient(iStatus.Int());      
        Cleanup();                    
        }
    }
    
// ----------------------------------------------------------------------------
// Implements cancellation of an outstanding request.
// ----------------------------------------------------------------------------
//
EXPORT_C void CMPXM3uPlaylistImporter::DoCancel()
    {
    MPX_DEBUG1("CMPXM3uPlaylistImporter::DoCancel");

    TInt error( KErrCancel );
    
    // notify client that the request has been cancelled
    NotifyClient(error);

    Cleanup();
    
    if ( iCallerStatus )
        {
        User::RequestComplete( iCallerStatus, error );
        }
    }

// ----------------------------------------------------------------------------
// Performs one step of the task
// ----------------------------------------------------------------------------
//
EXPORT_C void CMPXM3uPlaylistImporter::DoTaskStep()
    {
    MPX_DEBUG1("CMPXM3uPlaylistImporter::DoTaskStep()");
    
    TRequestStatus* status = &iStatus;
    *status = KRequestPending;

    TInt error( KErrNone );
    
    MPX_TRAP( error, DoTaskStepL() );

    User::RequestComplete( status, error );
    }
        
// ----------------------------------------------------------------------------
// Performs one step of the task. Leaves when an error is encountered
// ----------------------------------------------------------------------------
//
EXPORT_C void CMPXM3uPlaylistImporter::DoTaskStepL()
    {
    MPX_DEBUG1("CMPXM3uPlaylistImporter::DoTaskStepL()");
    
    switch( iState )
        {        
        case EMPXM3UReadBufferWithAutoDetectEncoding:
            {
            ReadPlaylistFileToBufferL();
            iState = EMPXM3UParseWithAutoDetectEncoding;
            }
            break;
            
        case EMPXM3UParseWithAutoDetectEncoding:
            {
            ParsePlaylistBufferL(
                *iAutoEncodingPlaylistArray, iAutoEncodingInvalidItems );
            
            // If at the moment, we know that there is at least one error parsing
            // with auto detect encoding, we don't need to proceed until end of
            // file anymore, this playlist file is concluded to be corrupted
            if ( iAutoEncodingInvalidItems > 0 )
                {
                delete iAutoEncodingPlaylistArray;
                iAutoEncodingPlaylistArray = NULL;
                
                User::Leave(KErrCorrupt);
                }

            // we've finished parsing with auto detect encoding we will return
            // the playlist parsed with auto encoding
            else if ( iEndOfFile )
                {               
                iState = EMPXM3UComposePlaylistMedia;
                }
            }       
            break;

        case EMPXM3UComposePlaylistMedia:
            {
            ComposePlaylistL();
            iMoreToDo = EFalse;
            }
            break;
            
        default:
            {
            User::Leave(KErrAbort);
            }
            break;
        }
    }

// -----------------------------------------------------------------------------
// CMPXM3uPlaylistImporter::ReadPlaylistFileToBufferL
// -----------------------------------------------------------------------------
//
void CMPXM3uPlaylistImporter::ReadPlaylistFileToBufferL()
    {
    MPX_DEBUG2("Before reading playlist to buffer: heap size = %d", User::Heap().Size());    

    delete iBuffer;
    iBuffer = NULL;
    iBufferPtr.Set(KNullDesC);    

    //
    // leave with KErrNotFound if the playlist file does not exist
    //    
    if (!BaflUtils::FileExists(*iFs, iPlaylistFilePath))
        {
        User::Leave(KErrNotFound);
        }
    
    TEntry entry;
    User::LeaveIfError(iFs->Entry(iPlaylistFilePath, entry));
    
    HBufC* buffer = HBufC::NewLC(entry.iSize);
    TPtr ptr = buffer->Des();

    HBufC8* buf8 = HBufC8::NewLC(entry.iSize);
    TPtr8 ptr8 = buf8->Des();

    // Read the first KPlaylistSampleLength bytes of the file
    TInt sampleLength(KPlaylistSampleLength);
    if (sampleLength > entry.iSize)
        {
        sampleLength = entry.iSize;
        }
    User::LeaveIfError(iFs->ReadFileSection(
                            iPlaylistFilePath, 0, ptr8, sampleLength));

    // auto detect character encoding
    TUint charSetId(0);
    TInt error = DetectCharacterSetL(*buf8, iTopCharacterSet, charSetId);
    MPX_DEBUG3("encoding detected using top character set is 0x%x, error %d", charSetId, error);
    
    // when we fail to detect the encoding, use all available character set in the
    // system to try again. If that also fails, abandon the operation.
    if (error)
        {
        User::LeaveIfError(DetectCharacterSetL(*buf8, iAvailableCharacterSet, charSetId));
        MPX_DEBUG2("encoding detected using available character set is 0x%x", charSetId);        
        }

    // read the whole file if the sample taken isn't the whole file
    if (sampleLength != entry.iSize)
        {
        User::LeaveIfError(iFs->ReadFileSection(
                                iPlaylistFilePath, 0, ptr8, entry.iSize));
        }
   
    // perform character conversion using the selected encoding
    TInt state(CCnvCharacterSetConverter::KStateDefault);
    TInt numOfUnconvertibleChars(0);
    CCnvCharacterSetConverter* charSetConv = CCnvCharacterSetConverter::NewLC();
    charSetConv->PrepareToConvertToOrFromL(charSetId, iAvailableCharacterSet, *iFs);
    TInt retVal = charSetConv->ConvertToUnicode(ptr, *buf8, state, numOfUnconvertibleChars);
    User::LeaveIfError(retVal);

    // try again if the character set wasn't detected using the whole file
    if ((retVal > 0 || numOfUnconvertibleChars > 0) && (sampleLength != entry.iSize))
        {
        MPX_DEBUG4("retVal = %d, numOfUnconvertibleChars = %d, entry.iSize = %d",
                    retVal, numOfUnconvertibleChars, entry.iSize);
        numOfUnconvertibleChars = 0;
        retVal = 0;
        User::LeaveIfError(DetectCharacterSetL(*buf8, iAvailableCharacterSet, charSetId));
        charSetConv->PrepareToConvertToOrFromL(charSetId, iAvailableCharacterSet, *iFs);
        retVal = charSetConv->ConvertToUnicode(ptr, *buf8, state, numOfUnconvertibleChars);
        }
        
    if (retVal > 0 || numOfUnconvertibleChars > 0)
        {
        MPX_DEBUG3("Unable to find character encoding for the playlist file. retVal = %d, numOfUnconvertibleChars = %d",
                    retVal, numOfUnconvertibleChars);        
        User::Leave(KErrNotSupported);
        }
    
    // remove the byte order mark (BOM) character prepended at the beginning
    // of the stream if encoded with unicode as per Unicode section 2.4
    if ((charSetId == KCharacterSetIdentifierUnicodeLittle ||
         charSetId == KCharacterSetIdentifierUnicodeBig) &&
        ptr.Length() > 0 &&
        ptr[0] == KUnicodeBOM)
        {
        ptr.Delete(0,1);
        }
        
    iBuffer = buffer;
    iBufferPtr.Set(*iBuffer);
    
    CleanupStack::PopAndDestroy(2, buf8); // charSetConv & buf8    
    CleanupStack::Pop(buffer);
        
    // brand new buffer which hasn't been read, reset iCurrentLineNumber and
    // iEndLineNumber, and iEndOfFile
    iCurrentLineNumber = 0;
    iEndLineNumber = KMPXM3UNumOfLinesToProcess;
    iEndOfFile = EFalse;

    MPX_DEBUG2("After reading playlist to buffer: heap size = %d", User::Heap().Size());        
    }

// -----------------------------------------------------------------------------
// CMPXM3uPlaylistImporter::DetectCharacterSetL
// copied and revised based on CMetaDataParserID3v1::DetectCharacterSetL version 4
// -----------------------------------------------------------------------------
//
TInt CMPXM3uPlaylistImporter::DetectCharacterSetL(
            const TDesC8& aSample,
            const CArrayFix<CCnvCharacterSetConverter::SCharacterSet>& aCharacterSet,
            TUint& aCharSetId)
	{
	// CCnvCharacterSetConverter::ConvertibleToCharSetL hangs if sample is too big
	if (aSample.Size() > KPlaylistMaxSampleLength)
	    {
	    User::Leave(KErrNotSupported);
	    }
	    
	TInt confidence(0);
	TInt highestConfidence(0);
	TUint charSetId(0);
	TUint highestConfidencecharSetId(0);

	CCnvCharacterSetConverter* charSetConv = CCnvCharacterSetConverter::NewLC();
	TInt count = aCharacterSet.Count();
	for ( TInt i=0; i < count; i++)
		{
		charSetId = aCharacterSet.At(i).Identifier();
		charSetConv->ConvertibleToCharSetL(confidence, charSetId, aCharacterSet, aSample);
		if ( confidence > highestConfidence )
			{
			highestConfidence = confidence;
			highestConfidencecharSetId = charSetId;
			}
		}
	CleanupStack::PopAndDestroy(charSetConv);
	MPX_DEBUG3("CMPXM3uPlaylistImporter::DetectCharacterSetL :-> Confidence[%d] CharSetId[0x%x]",
   		confidence, aCharSetId);
	if ( highestConfidence == 0 || highestConfidence < KMinimumConfidenceRequired )
		{
		return KErrNotFound;
		}
	else
		{
		aCharSetId = highestConfidencecharSetId;
		return KErrNone;
		}
	}
	
// -----------------------------------------------------------------------------
// CMPXM3uPlaylistPlugin::ParsePlaylistBufferL
// -----------------------------------------------------------------------------
//
void CMPXM3uPlaylistImporter::ParsePlaylistBufferL(
            CMPXMediaArray& aPlaylist,
            TInt& aInvalidItemCount)
    {
    // Read and process all the lines in the file
    //
    // the order of the following conditions is important. ReadNextLineL
    // should be called last to avoid skipping one line
    while (iCurrentLineNumber < iEndLineNumber &&
           aPlaylist.Count() < KMPXM3UPlaylistMaxItemCount &&
           ReadNextLineL())
        {
        ProcessLineL(aPlaylist, aInvalidItemCount);  
        }

    if ( aPlaylist.Count() == KMPXM3UPlaylistMaxItemCount )
        {
        User::Leave(KErrOverflow);
        }

    //
    // haven't finished processing all lines in the file, but have processed
    // KMPXM3UNumOfLinesToProcess number of lines. Set up iEndLineNumber for
    // the next iteration
    //        
    if ( !iEndOfFile && iCurrentLineNumber == iEndLineNumber )
        {
        iEndLineNumber += KMPXM3UNumOfLinesToProcess;
        }
    }

// -----------------------------------------------------------------------------
// CMPXM3uPlaylistPlugin::ReadNextLineL
// -----------------------------------------------------------------------------
//
TBool CMPXM3uPlaylistImporter::ReadNextLineL()
    {
    // iBuffer should exist when this function is called
    __ASSERT_DEBUG(iBuffer, User::Leave(KErrBadDescriptor));

    if (!iBufferPtr.Length())
        {
        // File end reached.
        iEndOfFile = ETrue;
        return EFalse;
        }

    delete iLine;
    iLine = NULL;

    // Try to find line change
    TInt offset = iBufferPtr.FindF(KMPXM3ULineChange);
  
    if (offset == KErrNotFound)
        {
        // No line change was found --> last line had no line change
        iLine = iBufferPtr.AllocL();
        // Set iBufferPtr to the end of buffer
        iBufferPtr.Set(iBufferPtr.Right(0)); //magic
        }
    else
        {
        // Found line change
        TInt length(offset);
        if ((offset > KMPXM3UNoOffset) && 
            (iBufferPtr[length - 1] == KMPXM3UCarriageReturn)) // magic
            {
            --length;
            }

        iLine = iBufferPtr.Left(length).AllocL();

        // Move past the line feed
        iBufferPtr.Set(iBufferPtr.Mid(++offset));
        }

    // Remove leading and trailing space characters from iLine's data.
    TPtr ptr = iLine->Des();
    ptr.Trim();

    iCurrentLineNumber++;
    return ETrue;
    }
    
// -----------------------------------------------------------------------------
// CMPXM3uPlaylistImporter::ProcessLineL()
// -----------------------------------------------------------------------------
//
void CMPXM3uPlaylistImporter::ProcessLineL(
            CMPXMediaArray& aPlaylist,
            TInt& aInvalidItemCount)
    {
    if ( iCurrentLineNumber == 1 ) // first line
        {
        // Check whether the file is in the extented format
        TInt offset = iLine->Find(KMPXM3UTagExtm3u);
        if (offset == KErrNotFound || offset != KMPXM3UNoOffset ||
            iLine->Length() != KMPXM3UTagExtm3u().Length())
            {
            // The file is not in the extented format
            iExtendedFormat = EFalse;
            }
        else
            {
            // The file is in the extented format
            iExtendedFormat = ETrue;
            return;
            }        
        }

    if (!iItem)
        {
        iItem = CMPXMedia::NewL();
        iItem->SetTObjectValueL(KMPXMediaGeneralType, EMPXItem);
        iItem->SetTObjectValueL(KMPXMediaGeneralCategory, EMPXSong);
        }
    
    // Parse line and then decide what to do with it
    switch (ParseLineL(iItem, aInvalidItemCount))
        {
        case EMPXM3UPlaylistLineTypeExtinf:
            // Continue to next round
            break;
            
        case EMPXM3UPlaylistLineTypePath:
            {
            // Line was a path => add item to playlist
            aPlaylist.AppendL(iItem);
            iItem = NULL; // item now owned by aPlaylist
            }
            break; 
                       
        case EMPXM3UPlaylistLineTypeNotSupported:
        case EMPXM3UPlaylistLineTypeCorrupted:
        default:
            {
            // Line has unsupported extension tag or undefined has error
            // occurred -> delete item
            delete iItem;
            iItem = NULL;
            }
            break;
        }
    }
    
// -----------------------------------------------------------------------------
// CMPXM3uPlaylistImporter::ParseLineL
// -----------------------------------------------------------------------------
//
TInt CMPXM3uPlaylistImporter::ParseLineL(
            CMPXMedia* aItem,
            TInt& aInvalidItemCount)
    {
    // There should be always aItem & iLine when this function is called
    __ASSERT_DEBUG(aItem && iLine, User::Leave(KErrAbort));

    if (!iLine->Length())
        {
        // Empty line => line is invalid
        return EMPXM3UPlaylistLineTypeNotSupported;
        }

    if (iExtendedFormat)
        {
        // File is in the extented format => check whether there is extented
        // info in this line.
        TInt offset = iLine->Find(KMPXM3UTagExtinf);
        if (offset != KErrNotFound && offset == KMPXM3UNoOffset)
            {
            // Extented info found
            aItem->SetTextValueL(KMPXMediaGeneralTitle, KNullDesC);

            offset = iLine->Find(KMPXM3UPoint);

            if(offset != KErrNotFound)
                {

                // Title and length found from extented info
                // Parse length
                TLex16 lex(iLine->Mid(KMPXM3UTagExtinf().Length(),
                                offset - KMPXM3UTagExtinf().Length()));
                TInt length;
                if (lex.Val(length))
                    {
                    // Error has occurred => legth is set to be ignored
                    length = KMPXM3UIgnoreTimeEntry;
                    }

                aItem->SetTObjectValueL(KMPXMediaGeneralDuration, length);
                MPX_DEBUG2("    duration %d", length);

                // Parse title
                HBufC* title = iLine->Mid(offset + 1).AllocLC();
                aItem->SetTextValueL(KMPXMediaGeneralTitle, *title);
                MPX_DEBUG2("    title %S", title);
                CleanupStack::PopAndDestroy( title );

                return EMPXM3UPlaylistLineTypeExtinf; // line type extinf
                }    
            }
        }

    // File is not in the extented format or supported info not found from this
    // line.
    switch (iLine->Find(KMPXM3UTagExt))
        {
        case KMPXM3UNoOffset:
            // Unsupported extended info tag found from this line
            return EMPXM3UPlaylistLineTypeNotSupported;
            
        case KErrNotFound:
        default:
            // Extended info not found from the beginning of line => line is
            // a path.
            {
            // Get absolute path
            TInt error(KErrNone);
            HBufC* uri = ParseAbsolutePathLC(*iLine, error);
        
            if (error)
                {
                if (error == KErrPathNotFound)
                    {
                    TUint flag(KMPXMediaGeneralFlagsSetOrUnsetBit | KMPXMediaGeneralFlagsIsInvalid);
                    aItem->SetTObjectValueL(KMPXMediaGeneralFlags, flag);
                    }
                else
                    {
                    if( uri )
                        {
                        CleanupStack::PopAndDestroy( uri );
                        }
                    
                    ++aInvalidItemCount;
                                    
                    // All other errors are considered to mean playlist is
                    // corrupt.
                    return EMPXM3UPlaylistLineTypeCorrupted;
                    }
                }

            aItem->SetTextValueL(KMPXMediaGeneralUri, *uri);
            MPX_DEBUG2("    uri %S", uri);
                
            // if title isn't supplied by the m3u file, extract file name from
            // URI as the title
            if (!aItem->IsSupported(KMPXMediaGeneralTitle))
                {
                TParsePtrC parser(*uri);
                TPtrC title = parser.Name();
                aItem->SetTextValueL(KMPXMediaGeneralTitle, title);
                MPX_DEBUG2("    title %S", &title);
                }

            CleanupStack::PopAndDestroy( uri );
            
            return EMPXM3UPlaylistLineTypePath; // line type path
            }
        }
    }

// -----------------------------------------------------------------------------
// CMPlayerM3UPlaylistParser::ParseAbsolutePathL
// -----------------------------------------------------------------------------
//
HBufC* CMPXM3uPlaylistImporter::ParseAbsolutePathLC(
            const TDesC& aPath,
            TInt& aError)
    {
    HBufC* path = NULL;
    
    TBool isAbsolute( EFalse );
    
    if (aPath.Length() > KPathStartingChars && 
        !aPath.Mid(1, 2).CompareF(KMPXM3UAbsPath)) // magic: the 2nd and 3rd chars
                                               // are always ":\"
                                               // for absolute paths
        {
        isAbsolute = ETrue;
        }

    if (aPath.Length() > KMaxFileName) // Test if path is too long
        {
        aError = KErrCorrupt;
        }
    else if ( isAbsolute )
        {
        aError = KErrNone;
        aError = iFs->IsValidName(aPath) ? KErrNone : KErrBadName;
        path = aPath.AllocLC();
        }
    else
        {
        // Given path could be relative => create absolute path and test it
        // Playlist file path
        TParse playlistPath;
        playlistPath.Set(iPlaylistFilePath, NULL, NULL);
        // Path to the folder, where playlist file is located to
        TPtrC currentFolder = playlistPath.DriveAndPath();
        // Create absolute path
        path = HBufC::NewLC(currentFolder.Length() + aPath.Length());

        TPtr tmpPtr(path->Des());
        tmpPtr = currentFolder;
        tmpPtr += aPath;

        aError = iFs->IsValidName(*path) ? KErrNone : KErrBadName;
        }
    
    // It is possible that a song exists in the filesystem but isn't added to
    // the database because it's not a supported type. If such song is included
    // in a playlist, it will be added to the database when the playlist is added.
    // Because of this, we cannot rely on whether the song exists in the database
    // to conclude whether the song is a broken link. We need to check for file
    // existence here. For the unsupported songs included in the playlist, they
    // will then be marked as corrupted when user initiates playback of those
    // songs.
    if (!aError &&
        !BaflUtils::FileExists(*iFs, *path))
        {
        aError = KErrPathNotFound;
        }

    return path;
    }
    
// -----------------------------------------------------------------------------
// CMPlayerM3UPlaylistParser::ComposePlaylistL
// -----------------------------------------------------------------------------
//
void CMPXM3uPlaylistImporter::ComposePlaylistL()
    {
    //
    // instantiate a CMPXMedia that represent the playlist which will
    // contain the CMPXMediaArray
    //
    iPlaylist = CMPXMedia::NewL();
    
    // set playlist title
    TParsePtrC parser(iPlaylistFilePath);
    iPlaylist->SetTextValueL(KMPXMediaGeneralTitle, parser.Name());
    
    // set playlist URI
    iPlaylist->SetTextValueL(KMPXMediaGeneralUri, iPlaylistFilePath);
        
    // set type
    iPlaylist->SetTObjectValueL(KMPXMediaGeneralType, EMPXItem);
        
    // set category
    iPlaylist->SetTObjectValueL(KMPXMediaGeneralCategory, EMPXPlaylist);        

    // set playlist array
    iPlaylist->SetCObjectValueL(KMPXMediaArrayContents, iAutoEncodingPlaylistArray);
        
    // set array acount
    iPlaylist->SetTObjectValueL(KMPXMediaArrayCount, iAutoEncodingPlaylistArray->Count());
        
    // playlist makes a copy of the array, we can now free the medias
    // array
    delete iAutoEncodingPlaylistArray;
    iAutoEncodingPlaylistArray = NULL;
    }
    
// ----------------------------------------------------------------------------
// Cleanup. 
// ----------------------------------------------------------------------------
//
void CMPXM3uPlaylistImporter::Cleanup()
    {
    delete iBuffer;
    iBuffer = NULL;
    
    delete iLine;
    iLine = NULL;   

    delete iItem;
    iItem = NULL;
   
    delete iAutoEncodingPlaylistArray;
    iAutoEncodingPlaylistArray = NULL;
    
    delete iPlaylist;
    iPlaylist = NULL;
    }

// ----------------------------------------------------------------------------
// Handles notifications to the client
// ----------------------------------------------------------------------------
//
void CMPXM3uPlaylistImporter::NotifyClient( TInt aError )
    {
    MPX_DEBUG3("CMPXM3uPlaylistImporter::NotifyClient - iAutoEncodingInvalidItems=%d error=%d",
        iAutoEncodingInvalidItems, aError);
        
    if ( iObserver )
        {
        if (aError)
            {
            // we don't need the playlist media to be passed back to the client
            // in case of an error, delete it now
            delete iAutoEncodingPlaylistArray;
            iAutoEncodingPlaylistArray = NULL;

            // to-do: change HandlePlaylistL to HandlePlaylist
            TRAP_IGNORE(iObserver->HandlePlaylistL( NULL, aError, ETrue ));
            }
        else
            {
            // notify client. return the playlist media
            CMPXMedia* playlist = iPlaylist;
            iPlaylist = NULL; // client takes over the ownership
            
            // to-do: change HandlePlaylistL to HandlePlaylist
            TRAP_IGNORE(iObserver->HandlePlaylistL( playlist, aError, ETrue ));
            }
        }        
    }
        
// End of file
