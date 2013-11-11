/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Decoding of binary multimedia message
*
*/



#include    <apparc.h>
#include    <s32mem.h>
#include    <utf.h>
#include    <msvids.h>
#include    <escapeutils.h>
#include    <e32math.h>
#include    <badesca.h>
#include    <cmsvmimeheaders.h>
#include    <mmsvattachmentmanager.h>
#include    <mmsvattachmentmanagersync.h>
#include	<fileprotectionresolver.h>
#include    <apmstd.h>
// Features
#include    <featmgr.h>    
#include    <bldvariant.hrh>

#include    "mmsheaders.h"
#include    "mmsconst.h"
#include    "mmsdecode.h"
#include    "mmscodec.h"
#include    "mmsservercommon.h"
#include    "mmssession.h"
#include    "mmsgenutils.h"
#include    "mmsentrywrapper.h"
#include    "mmserrors.h"
#include    "mmsmmboxmessageheaders.h"
#include    "mmsmmboxviewheaders.h"
#include    "mmselementdescriptor.h"

enum TMmsMachineStates
    {
    EMmsIdle,
    EMmsDecodingHeaders,
    EMmsDecodingAttachments,
    EMmsFinished
    };

// These are the stages for chunked encoding.
// The chunked encoding is synchronous, so the stages are different
// from the machine states.    
enum TMmsChunkedDecodingStages
    {
    EMmsNotApplicable, // stage used when not doing chunks
    EMmsHeaders,
    EMmsAttachmentHeaders,
    EMmsAttachmentDataStart, // create attachment file and add data
    EMmsAttachmentDataAppend, // append data to existing attachment
    EMmsDone
    };

static const CMmsDecode::TMmsExtensionLookup KMmsFileExtensionMatchTable[] =
    {
        { _S8( "jpeg" ), _S16( "*.jpg" ) },
        { _S8( "gif" ), _S16( "*.gif" ) },
        { _S8( "text" ), _S16( "*.txt" ) },
        { _S8( "smil" ), _S16( "*.smil" ) },
        { _S8( "wbmp" ), _S16( "*.wbmp" ) },
        { _S8( "png" ), _S16( "*.png" ) },
        { _S8( "amr" ), _S16( "*.amr" ) },
        { _S8( "vcard" ), _S16( "*.vcf" ) },
        { _S8( "calendar" ), _S16( "*.vcs" ) },
        { _S8( "drm.message" ), _S16( "*.dm" ) },
        { _S8( "drm.content" ), _S16( "*.dcf" ) },
        { _S8( "drm.rights+xml" ), _S16( "*.dr" ) },
        { _S8( "drm.rights+wbxml" ), _S16( "*.drc" ) },
        { _S8( "drm.dcf" ), _S16( "*.odf" ) },
        // last entries in the table are empty so that
        // we can find the end of the table
        { _S8( "" ), _S16( "" ) }
    };
    
// application specific headers with known handling
static const CMmsDecode::TMmsExtensionHeaderLookup KMmsExtensionHeaderLookup[] =
    {
        { _S8( "X-Mms-Vodafone-Notif-Text" ), &KMmsAssignedExtendedNotificationText },
        { _S8( "X-Mms-Vodafone-Notif-EOL" ), &KMmsAssignedExtendedNotificationEOL },
        // end of table
        { _S8( "" ), 0 }
    };

_LIT8( KMmsWapMultipartReport, "application/vnd.wap.multipart.report" );
// fake assigned number - last one possible
const TUint8 KMmsAssignedApplicationVndWapMultipartReport = 0x7F;

// a header that is a text string, not assigned number
const TUint8 KMmsTextHeader = 0x7F;

_LIT8( KMmsExtension, "X-" );

#ifndef _NO_MMSS_LOGGING_
_LIT16( KLogYes, "-- Yes" );
_LIT16( KLogNo, "-- No" );
_LIT16( KLogUnknown, "-- Unknown %d" );
#endif

_LIT8( KmmsSmilMimeType,"application/smil");
_LIT8( KMmsSmilSubtype, "smil" );
_LIT8( KMmsAudioSubtype, "amr" );
_LIT8( KMmsAudioSubtype2, "x-amr" ); // old subtype - someone may send this
// main type for audio, don't mark for example "application/amr" as audiomessage
// as audiomessage UI can't handle it. UI only handles "audio/amr"
_LIT8( KMmsAudioType, "audio" );

_LIT8( KMessageClassPersonal, "Personal" );
_LIT8( KMessageClassAdvertisement, "Advertisement" );
_LIT8( KMessageClassInformational, "Informational" );
_LIT8( KMessageClassAuto, "Auto" );
         
#ifndef _NO_MMSS_LOGGING_
const TInt KMmsHalfByteShift = 4;
const TUint8 KMms0x0F = 0x0F;
const TInt KMmsDateFormatLength = 30; // length for date string buffer for logging
const TInt KMmsMicroToSeconds = 1000000;
#endif
const TUint8 KMms0x20 = 0x20; // lowest ascii, space
const TInt KMmsMinHeaderLength = 2; // no header can be shorter than this
const TInt KMmsBitsInByte = 8;
const TInt KMmsMaxIntegerLength = 4;
const TInt KMmsMaxLongIntegerLength = 8;
const TUint8 KMms0x80 = 0x80; // 128
const TUint8 KMms0x7F = 0x7F; // 127
const TInt KMms2 = 2; // used to calculate length when converting 8 bit to 16 bit encoding
const TUint8 KMms31 = 31; // uintvar indicator
const TUint8 KMms32 = 32; // low limit for text string encoding
const TUint8 KMms30 = 30; // upper limit for short length encoding
const TInt KMmsUintvarShift = 7; // bit shift for decoding uintvars
const TInt KMms14 = 14; // atta name length for self-generated names
const TInt KMms2k = 2000;
const TInt KMmsTestIllegalValue = 255;
// ==================== LOCAL FUNCTIONS ====================

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that
// might leave.
// All member variables are automatically zeroed in the constructor
// of a class derived from CBase.
// ---------------------------------------------------------------------------
//
CMmsDecode::CMmsDecode()
    :CMsgActive ( KMmsActiveObjectPriority ),
    iState ( EMmsIdle )
    {
    }

// ---------------------------------------------------------------------------
// Symbian OS default constructor can leave.
// ---------------------------------------------------------------------------
//
void CMmsDecode::ConstructL(RFs& aFs)
    {
    iFs = aFs;
    iMimeHeaders = CMsvMimeHeaders::NewL();
    // This is obsolete. Everything is always logged in debug version
    iLogAllDecoded = ETrue; 
    CActiveScheduler::Add(this);
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
EXPORT_C CMmsDecode* CMmsDecode::NewL(RFs& aFs)
    {
    CMmsDecode* self = new (ELeave) CMmsDecode;
    
    CleanupStack::PushL( self );
    self->ConstructL(aFs);
    CleanupStack::Pop( self );

    return self;
    }

    
// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CMmsDecode::~CMmsDecode()
    {
    Cancel();
    delete iMimeHeaders;
    delete iStore; // the store should be committed earlier, not anymore
    delete iRootContentIdBuffer;

    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
EXPORT_C void CMmsDecode::StartL(
    MMmsEntryWrapper& aEntryWrapper,
    CMmsHeaders& aMmsHeaders,
    CBufFlat& aDecodeBuffer,
    TRequestStatus& aStatus,
    TInt aStartPosition /* = 0 */,
    TInt aLength /* = 0 */ )
    {

    Reset();

    iEntryWrapper = &aEntryWrapper;
    iDecodeBuffer = &aDecodeBuffer;
    iMmsHeaders = &aMmsHeaders;
    iDRMFlags = 0;
    // length will be checked later when we start actual decoding
    iPosition = aStartPosition;
    if ( aLength == 0 )
        {
        iLength = aDecodeBuffer.Size();
        }
    else
        {
        iLength = aLength + aStartPosition;
        }

    iDumpIncoming = iEntryWrapper->GetDumpFlag();

    DumpL();

    Queue(aStatus);
    
    CompleteSelf( iError );

    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
EXPORT_C void CMmsDecode::DecodeHeadersL(
    MMmsEntryWrapper& aEntryWrapper,
    CMmsHeaders& aMmsHeaders,
    CBufFlat& aDecodeBuffer,
    TInt aStartPosition /* = 0 */,
    TInt aLength /* = 0 */,
    TInt* aNumberOfAttachments /* = 0 */,
    TInt* aDataStart /* = 0 */ )
    {
    // The initial settings are the same as for asynchronous
    // methods, but decoding just headers is fast enough to
    // be done synchronously

    Reset();

    iEntryWrapper = &aEntryWrapper;
    iDecodeBuffer = &aDecodeBuffer;
    iMmsHeaders = &aMmsHeaders;
    iPosition = aStartPosition;
    iNumberOfAttachments = 0;
    iLastHeader = EFalse;
    
    if ( aLength == 0 )
        {
        iLength = aDecodeBuffer.Size();
        }
    else
        {
        iLength = aLength + aStartPosition;
        }
    if ( aDataStart != 0 )
        {
        // Originally point beyound buffer
        // If data is found, this will be updated
        *aDataStart = iLength;
        }
    if ( aNumberOfAttachments )
        {
        // initialize to no attachments
        *aNumberOfAttachments = 0;
        }

    iMmsHeaders->Reset();

    // Save the time the entry was received into the MMS headers
    // the value in TMsvEntry may change, but this remains
    TTime now;      
    now.UniversalTime();
    iMmsHeaders->SetReceivingTime( now );
        
    iDumpIncoming = iEntryWrapper->GetDumpFlag();
    DumpL();

    if ( iLength < KMmsMinHeaderLength )
        {
        // cannot decode if nothing to decode
        // each header is at least 2 bytes. 
        // If not at least 2 bytes, no can do.
        iState = EMmsIdle;
        iError = KErrCorrupt;
        User::LeaveIfError( iError );
        }

    // each header byte must be followed by at least one byte value.
    // otherwise the message is corrupted
    // when we reach the header that indicates the start of the attachment
    // structure, we expect that no headers follow the attachment part
    // we could handle even headers following the attachments if we
    // wanted to, but if the specification is followed strictly, the 
    // attachments are the last part in the message.

    // each header is at least 2 bytes - header name and value
    while ( ( iPosition <= iLength - KMmsMinHeaderLength ) &&
        ( iError != KErrCorrupt ) &&
        ( !iLastHeader ) )
        {
        DecodeOneHeaderL();
        }
        
    iLength = iDecodeBuffer->Size();
    if ( iPosition >= iLength ||
        ( iMmsHeaders->MessageType() == KMmsMessageTypeMboxViewConf &&
          iMmsHeaders->MultipartType() == 0 ) )
        {
        // Only content type header, but no actual attachments
        iNumberOfAttachments = 0;
        }

    // If the caller is interested, return the number of attachments we found
    if ( aNumberOfAttachments )
        {
        *aNumberOfAttachments = iNumberOfAttachments;
        }

    if ( aDataStart != 0 )
        {
        *aDataStart = iPosition;
        }

    // WE DON'T SAVE THE MESSAGE HERE.
    // THE CALLER CONTROLS THE SAVING OF THE MESSAGE
    // AS WE ONLY DECODE HEADERS, AND DON'T HANDLE
    // ATTACHMENTS HERE
    // THERE MAY BE NO ENTRY AVAILABLE FOR SAVING THE HEADERS.

    // We are done - may be called again...
    iState = EMmsIdle;
    User::LeaveIfError( iError );

    }


// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
EXPORT_C void CMmsDecode::DecodeAttachmentsL(
    MMmsEntryWrapper& aEntryWrapper,
    CMmsHeaders& aMmsHeaders,
    CBufFlat& aDecodeBuffer,
    TInt aNumberOfAttachments,
    TInt& aStartPosition,
    TRequestStatus& aStatus,
    TBool aDoNotUpdateParentEntry /* = EFalse*/,
    TInt aLength /* = 0 */ )
    {

    // The parent entry is updated after specified number of attachments
    // has been handled.
    // Normally all attachments should be handled together.
    // Handling only some attachments is a special case
    
    // We must get an edit store when we are called the first time.
    if ( !iStore )
        {
        iStore = iEntryWrapper->EditStoreL();
        }

    iError = KErrNone; // start with clean slate
    iDataStart = aStartPosition;
    iNextStart = aStartPosition;
    iUpdatedPosition = &aStartPosition;
    iDoNotUpdateParentEntry = aDoNotUpdateParentEntry;

    iEntryWrapper = &aEntryWrapper;
    iMmsHeaders = &aMmsHeaders;
    iDecodeBuffer = &aDecodeBuffer;
    iNumberOfAttachments = aNumberOfAttachments;
    iPosition = aStartPosition;
    if ( aLength == 0 )
        {
        iLength = iDecodeBuffer->Size();
        }
    else
        {
        iLength = aStartPosition + aLength;
        }
    iState = EMmsDecodingAttachments;

    Queue(aStatus);
    
    CompleteSelf( iError );
    
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
EXPORT_C CMsvMimeHeaders& CMmsDecode::ExtractNextDataPartL(
    CBufFlat& aDecodeBuffer,
    TInt& aStartPosition,
    TInt& aStartOfData,
    TInt& aLengthOfData)
    {
    iMimeHeaders->Reset();
    iDecodeBuffer = &aDecodeBuffer;
    iPosition = aStartPosition;
    iUpdatedPosition = &aStartPosition;
    iLength = iDecodeBuffer->Size();

    TInt headersLength = 0;
    TInt dataLength = 0;
    
    // This function is only used to extract next data part from multipart structure

    headersLength = GetUintvar();
    dataLength = GetUintvar();

    aStartOfData = iPosition + headersLength;
    iDataStart = aStartOfData;
    aLengthOfData = dataLength;
    aStartPosition = iDataStart + dataLength;

    // Extract mime headers
    // iPosition points to the start of headers
    // First is content type without actual header field, just value
    GetAttachmentContentTypeL();

    while ( iPosition < iDataStart )
        {
        DecodeOneContentHeaderL();
        }

    return *iMimeHeaders;
    }
    
// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
EXPORT_C void CMmsDecode::CommitStoreL()
    {
    if ( iStore )
        {
        iStore->CommitL();
        }
    }
    
// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
EXPORT_C void CMmsDecode::InitializeChunkedMode(
    MMmsEntryWrapper& aEntryWrapper,
    CMmsHeaders& aMmsHeaders,
    CBufFlat& aDecodeBuffer
    )
    {
    Reset();
    iEntryWrapper = &aEntryWrapper;
    iMmsHeaders = &aMmsHeaders;
    iDecodeBuffer = &aDecodeBuffer;
    iDRMFlags = 0;
    iPosition = 0;
    iLength = 0;
    iRemoveDrm = EFalse;
    iDumpIncoming = iEntryWrapper->GetDumpFlag();
    iDecodingStage = EMmsHeaders;
    
    TRAP_IGNORE(DumpL());
    }
    
// ---------------------------------------------------------
// from class MMmsCodecDataSink
//
// ---------------------------------------------------------
//
TInt CMmsDecode::NextDataPart(
        CBufFlat& aBuffer,
        TInt& aPosition,
        TBool aLastDataChunk )
    {
    if ( iDecodingStage == EMmsNotApplicable )
        {
        aPosition = 0; // if not chunked, nothing handled by data sink -> keep all.
        return KErrNone;
        }
        
    TMsvEntry entry;    
    iEntryWrapper->GetIndexEntry( entry );
    if ( entry.Id() == KMsvNullIndexEntryId )
        {
        // We have no entry, behave as if in non-chunked mode.
        // We are getting a response that will not be saved
        // in a message entry, but may be decoded in its entirety
        // later.
        aPosition = 0; // if not chunked, nothing handled by data sink -> keep all.
        return KErrNone;
        }
        
    iDecodeBuffer = &aBuffer;
    
    iPosition = aPosition;
    // Use iLength to tell how much data must be dumped
    iLength = iDecodeBuffer->Size() - iPosition;
    TRAP_IGNORE(DumpL());
    // Use this to check when end of buffer has been reached
    iLength = iDecodeBuffer->Size();
    
    // We must get an edit store when we are called the first time.
    if ( !iStore && iError == KErrNone )
        {
        TRAP( iError, iStore = iEntryWrapper->EditStoreL() );
        }
    
    if ( iError != KErrNone )
        {
        return iError;
        }
    
    // clear this to make sure it does not point beyond buffer the next time
    iOldData = 0;
    
    TBool moreDataNeeded = EFalse;
    if ( iDecodingStage == EMmsHeaders )
        {
        // Decode headers and tell if there was enough data to do that
        moreDataNeeded = SinkHeaders( aLastDataChunk );
        }
        
    // If we have decoded all headers, we must see if we have enough data left
    // to decode the attachments.    
    
    // If we already ran out of data while decoding the MMS headers
    // we must get more data before even trying the attachments
    
    while ( !moreDataNeeded && !( aLastDataChunk && iPosition == iLength ) &&
        !( iDecodingStage == EMmsDone ) )
        {
        if ( iDecodingStage == EMmsAttachmentHeaders )
            {
            moreDataNeeded = SinkAttachmentHeaders( aLastDataChunk );
            }
            
        if ( iDecodingStage == EMmsAttachmentDataStart )
            {
            SinkAttachmentDataStart();
            }
            
        // If we have a monoblock, we don't know the length - we just write everything we've got    
        if ( iDecodingStage == EMmsAttachmentDataAppend  &&
            ( iAttaDataWritten < iCurrentAttaLength  || iCurrentAttaLength < 0 ) )
            {
            SinkAttachmentData();
            }
            
        if ( iDecodingStage == EMmsAttachmentDataAppend  &&
            ( ( iAttaDataWritten < iCurrentAttaLength ) ||
            ( iCurrentAttaLength < 0 && !aLastDataChunk ) ) )
            {
            moreDataNeeded = ETrue;
            }
            
        if ( ( iDecodingStage == EMmsAttachmentDataAppend &&
            iAttaDataWritten == iCurrentAttaLength ) || aLastDataChunk )
            {
            if ( iCurrentAttaLength > 0 )
                {
                FinishSinkingAttachment( aLastDataChunk );
                }
            }
            
        if ( iError != KErrNone && iError != KMmsErrorRemoveDRM )
            {
            if ( iDecodingStage == EMmsAttachmentDataAppend )
                {
                // We try to clean up.
                // We ignore the error as we have an error already
                TRAP_IGNORE( iEntryWrapper->FinalizeAttachmentL(
                    *iStore, iCurrentAttaLength, iDRMFlags ) );
                }
            // We cannot continue
            iDecodingStage = EMmsDone;
            }
        }

    // clear this in case it was set - it's not valid in chunked mode
    iFakeSubject = 0;
    iTextPlainLength = 0;
    
    if ( aLastDataChunk )
        {
        FinalizeSinkingLastChunk();
        }
        
    // remember how much data was left over
    if ( iPosition > iDecodeBuffer->Size() )
        {
        // this is paranoid, but we make sure that the caller will not panic
        iPosition = iDecodeBuffer->Size();
        }
    iOldData = iDecodeBuffer->Size() - iPosition;
    aPosition = iPosition;
    
    if ( iError != KErrNone )
        {
#ifndef _NO_MMSS_LOGGING_
        TMmsLogger::Log( _L("Chunked decode - error %d, deleting store"), iError );
#endif
        delete iStore;
        iStore = NULL;
        }
    
    return iError;
    }
    
// ---------------------------------------------------------
// from class MMmsCodecDataSink
//
// ---------------------------------------------------------
//
void CMmsDecode::RelaseDataSink()
    {
    // store must be released even in case of error
    delete iStore;
    iStore = NULL;
    }

// ---------------------------------------------------------
// from class MMmsCodecDataSink
//
// ---------------------------------------------------------
//
void CMmsDecode::ResetDataSink()
    {
    if ( iDecodingStage == EMmsNotApplicable )
        {
        // not chunked mode - do nothing
        return;
        }
        
    TMsvEntry entry;    
    if ( iEntryWrapper->GetIndexEntry( entry ) == KErrNone && 
         entry.Id() != KMsvNullIndexEntryId )
        {
        if ( !iStore )
            {
            TRAP_IGNORE(iStore = iEntryWrapper->EditStoreL());
            }
        // This should fail only if we run out of memory.
        // In that case the whole message entry will be deleted later,
        // so it does not make sense to return an error here
        // We just do our best and let the rest of the code to continue
        TRAP_IGNORE({
            MMsvAttachmentManager& attaMan = iStore->AttachmentManagerL();
            MMsvAttachmentManagerSync& attaManSync = iStore->AttachmentManagerExtensionsL();
            TInt count = attaMan.AttachmentCount();
            TInt i;
            // delete all old attachments as we are starting from the beginning
            for ( i = count - 1; i >= 0; --i )
               {
               attaManSync.RemoveAttachmentL( i );
               }
            iStore->CommitL();   
            });
        }
    
    Reset();
    iMmsHeaders->Reset();
    iDRMFlags = 0;
    iPosition = 0;
    iLength = 0;
    iDumpIncoming = iEntryWrapper->GetDumpFlag();
    iDecodingStage = EMmsHeaders;
    if ( entry.Id() == KMsvNullIndexEntryId )
        {
        // no entry - cannot use chunked mode.
        iDecodingStage = EMmsNotApplicable;
        }
    else    
        {
         TRAP_IGNORE(DumpL());
        }
    }

// ---------------------------------------------------------
// From class CMsgActive
//
// ---------------------------------------------------------
//
void CMmsDecode::DoRunL()
    {

    // This routine takes the state machine through the states
    // until an error is encountered or the cycle completes.

    if ( iError != KErrNone )
        {
        // We encountered an error, and cannot continue
        // For the time being we just give up without trying
        // to do any decent cleanup.
        iStatus = iError;
        iState = EMmsIdle;
        // If we return from DoRunL without becoming active again,
        // RunL completes.
        return;
        }

    SelectNextState();

    if ( iState != EMmsFinished )
        {
        // If appropriate, ChangeStateL makes us active again
        ChangeStateL();
        // If we return from DoRunL without becoming active again,
        // RunL completes.
        }
    else
        {
        iUpdatedPosition = NULL; // no longer needed
        if ( !iDoNotUpdateParentEntry )
            {
            FinishL();
            }
        else
            {
            // store must be released in any case
            CommitStoreL();
            delete iStore;
            iStore = NULL;
            }
        iDoNotUpdateParentEntry = EFalse; // default value
        iState = EMmsIdle;
        iStatus = iError;
        // If we return from DoRunL without becoming active again,
        // RunL completes.
        }

    }

// ---------------------------------------------------------
// From class CMsgActive
//
// ---------------------------------------------------------
//
void CMmsDecode::DoComplete( TInt& /* aStatus */ )
    {
    // We are exiting the loop - we say we are idle now
    iState = EMmsIdle;
    // store must be released in any case - if it is NULL already, nothing happens
    // Can't leave here anymore
    TInt error;
    TRAP( error, CommitStoreL() );
    delete iStore;
    iStore = NULL;
    if ( iError == KErrNone )
        {
        iError = error;
        }
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CMmsDecode::SelectNextState()
    {

    // If appropriate, the functions called within the switch statement
    // will make us active again. If all is done, the asynchronous request
    // will complete

    switch ( iState )
        {
        case EMmsIdle:
            // start the loop
            iState = EMmsDecodingHeaders;
            break;
        case EMmsDecodingHeaders:
            if ( iNumberOfAttachments > 0 )
                {
                iState = EMmsDecodingAttachments;
                }
            else
                {
                iState = EMmsFinished;
                }
            break;
        case EMmsDecodingAttachments:
            if ( iNumberOfAttachments > 0 )
                {
                iState = EMmsDecodingAttachments;
                }
            else
                {
                iState = EMmsFinished;
                }
            break;
        case EMmsFinished:
            // No more states
            break;
        default:
            // Illegal state
#ifndef _NO_MMSS_LOGGING_
            TMmsLogger::Log( _L("Decode - illegal state %d"), iState );
#endif
            break;
        }
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CMmsDecode::ChangeStateL()
    {
    switch ( iState )
        {
        case EMmsDecodingHeaders:
            DecodeHeadersL();
            break;
        case EMmsDecodingAttachments:
            if ( iNumberOfAttachments > KMmsMaxAttachments )
                {
                // If we have too many attachments, we cannot
                // handle the message. This is a temporary
                // error code that takes care of denying retries
                iError = KMmsErrorEMRUExceeded;
                CompleteSelf( iError );
                return;
                }
            DecodeOneAttachmentL();
            iNumberOfAttachments--;
            if ( iUpdatedPosition )
                {
                // The user wants to know where to continue
                *iUpdatedPosition = iPosition;
                }
            break;
        default:
            iState = EMmsIdle;
            break;
        }
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CMmsDecode::Reset()
    {
    iError = KErrNone; // start with clean slate
    iMimeHeaders->Reset();
    delete iStore;
    iStore = NULL; // this is a new message, cannot use old store
    iUpdatedPosition = NULL;
    iPosition = 0;
    iDataStart = 0;
    iNextStart = 0;
    iNumberOfAttachments = 0;
    iLastHeader = EFalse;
    iTotalSize = 0;
    iFakeSubject = 0;
    iCharacterSet = KMmsUsAscii; // if no character set, default is US_ASCII
    iTextPlainLength = 0;
    iMultipartType = 0;
    iRootContentId.Set( TPtrC8() );
    delete iRootContentIdBuffer;
    iRootContentIdBuffer = NULL;
    iRootAttachmentId = 0;
    iFirstAttachmentId = 0;
    iFirstTextPlain = 0;
    iUseForSubject = EFalse;
    iAttaNumber = 0;
    iSmilCount = 0;
    iAudioCount = 0;
    iPlainTexts = 0;
    iDoNotUpdateParentEntry = EFalse;
    iDecodingStage = EMmsNotApplicable;
    iOldData = 0;
    iLogAllDecoded = ETrue;
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
TBool CMmsDecode::SinkHeaders( TBool aLastDataChunk )
    {
    TBool enoughData = ETrue;
#ifndef _NO_MMSS_LOGGING_
        TMmsLogger::Log( _L("Chunked decode - MMS headers"));
#endif
    TInt error = KErrNone;
    // See if we have a whole header
    // We must have enough data to contain both the header and its value.
    // Otherwise we must ask for more data.
    TRAP ( error,
        {
        enoughData = CheckHeaderLength();
        while ( enoughData &&
            ( iError != KErrCorrupt ) &&
            ( !iLastHeader ) )
            {
            DecodeOneHeaderL();
            if ( ( iError != KErrCorrupt ) &&
                ( !iLastHeader ) )
                {
                enoughData = CheckHeaderLength();
                }
            }
        });
        
    if ( iError == KErrNone )
        {
        // we should get an error in decoding only if memory runs out
        // or the message is corrupted
        iError = error;
        }
    // check if we reached the end of the headers and can continue
    // with attachments
    if ( iError == KErrNone )
        {
        if ( iLastHeader )
            {
            // Headers decoded successfully, can continue with attachments
            // The uintvar specifying then number of attachments was decoded
            // at the end of the MMS headers in connection with multipart header.
            // This leaves only if out of disk space or out of memory
            TRAP( iError, SaveMMSHeadersL() );
            iDecodingStage = EMmsAttachmentHeaders;
            }
        else if ( aLastDataChunk && iPosition >= iLength )
            {
            // Headers decoded successfully, but no attachment present
            // This is an empty message
            TRAP( iError, SaveMMSHeadersL() );
            iDecodingStage = EMmsAttachmentHeaders;
            }
        else if ( aLastDataChunk && !enoughData )
            {
            // we have got something that cannot be decoded
            iError = KErrCorrupt;
            iDecodingStage = EMmsDone;
            }
        else 
            {
            // Do nothing - keep LINT happy
            }
        }
    // return ETrue if more data is needed.    
    return !enoughData;
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
TBool CMmsDecode::SinkAttachmentHeaders( TBool aLastDataChunk )
    {
    TBool moreDataNeeded = EFalse;
    TInt error = KErrNone;
    
    // We try to get all attachment headers at one go
    // It is easier than decoding the attachment headers one by one
    // As we have headers that tell the total length of attachment
    // headers we can ask for more data if needed.
    // The attachment headers are not normally very long, and if they
    // are, the message is probably corrupted anyway
    
#ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L("Chunked decode - Attachment headers"));
#endif
    iAttaDataWritten = 0; // no data yet    
    iCurrentAttaLength = 0;
    TInt currentPosition = iPosition;
    TInt uintvarStart = iPosition;
    iRemoveDrm = EFalse; // no DRM so far
    if ( iMultipartType && CheckUintvarLength() )
        {
        // restore position to start of Uintvar
        iPosition = uintvarStart;
        TUint attaHeaderLength = GetUintvar();
        // got atta header length. Do we still have more data.
        uintvarStart = iPosition;
        if ( CheckUintvarLength() )
            {
            iPosition = uintvarStart;
            iCurrentAttaLength = GetUintvar();
            // attaHeaderLength is the length of content type header and other
            // headers combined.
            // Do we still have that much data.
            if ( iLength - iPosition >= attaHeaderLength )
                {
                // decode headers
                iPosition = currentPosition;
                TRAP( error, DecodeAttachmentHeadersL() );
                if ( iError == KErrNone )
                    {
                    iError = error;
                    }
                if ( iCurrentAttaLength == 0 )
                    {
                    // if the attachment is empty, we do not create an attachment
                    // we should check here if there is any data left
                    iDecodingStage = EMmsAttachmentHeaders;
                    }
                else if ( iError == KErrNone )
                    {
                    iDecodingStage = EMmsAttachmentDataStart;
                    }
                else
                    {
                    // do nothing - keep LINT happy
                    }
                }
            else     
                {
                // not enough data for decoding headers - give up
                // and ask for more data
                iPosition = currentPosition;
                moreDataNeeded = ETrue;
#ifndef _NO_MMSS_LOGGING_
                TMmsLogger::Log( _L("Chunked decode - need more data for atta headers"));
#endif
                }
            }
        else
            {
            // not enough data for determining attachment data length - give up
            // and ask for more data
            iPosition = currentPosition;
            moreDataNeeded = ETrue;
            }
        }
    else if ( iMultipartType == 0 && !( iLength - iPosition < KMms2k && !aLastDataChunk ) )
        {
        // One chunk, no multipart, very bad, try anyway
        // We can only guess how much of the data is headers.
        TRAP( error, DecodeAttachmentHeadersL() );
        if ( iError == KErrNone )
            {
            iError = error;
            }
        iCurrentAttaLength = -1; // Don't know
        if ( !( aLastDataChunk && iPosition == iLength ) && iError == KErrNone )
            {
            // There is some data
            iDecodingStage = EMmsAttachmentDataStart;
            }
        else
            {
            iCurrentAttaLength = 0; // no data
            }
        }
    else
        {
        // not enough data for determining attachment header length - give up
        // and ask for more data
        iPosition = currentPosition;
        moreDataNeeded = ETrue;
        }

    return moreDataNeeded;
    }
    
// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CMmsDecode::SinkAttachmentDataStart()
    {
    TInt error = KErrNone;
    
#ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L("Chunked decode - Attachment data start"));
#endif
    // Now the attachment file must be created.
    // We should also have enough data
    
    // First an empty attachment is created. No data is needed for it.
    TRAP ( error, iEntryWrapper->CreateEmptyFileAttachmentL(
            *iStore,
            iParse.NameAndExt(),
            *iMimeHeaders,
            iCurrentAttachment )
        );
        
    if ( iError == KErrNone && error != KErrNone )
        {
        iError = error;
        }
    if ( iError == KErrNone )
        {
        if ( iUseForSubject )
            {
            iFirstTextPlain = iCurrentAttachment;
            iUseForSubject = EFalse;
            }

#ifndef _NO_MMSS_LOGGING_
        TRAP_IGNORE({
            MMsvAttachmentManager& attaMan = iStore->AttachmentManagerL();
            CMsvAttachment* attaInfo = attaMan.GetAttachmentInfoL( iCurrentAttachment );
            TMmsLogger::Log( _L("- Attachment created %S"), &attaInfo->FilePath() );
            delete attaInfo;
            attaInfo = NULL;
            });
#endif
        }
    if ( iError == KErrNone )
        {
        iDecodingStage = EMmsAttachmentDataAppend;
        }
    }
    
// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CMmsDecode::SinkAttachmentData()
    {
    TInt error = KErrNone;
    // no problem with signed/unsigned comparison - size cannot be signed
    if ( iPosition < iDecodeBuffer->Size() )
        {
        TPtr8 attaDataPtr = iDecodeBuffer->Ptr( iPosition );
        if ( attaDataPtr.Length() > iCurrentAttaLength - iAttaDataWritten &&
            iCurrentAttaLength > 0 )
            {
            // If we don't know the amount of attachment data, we write the
            // whole buffer
            attaDataPtr.SetLength( iCurrentAttaLength - iAttaDataWritten );
            }
        if ( iError == KMmsErrorRemoveDRM )
            {
            iRemoveDrm = ETrue;
            iError = KErrNone;
            }
        if ( !iRemoveDrm )
            {
            // we have data that can be written to the file
            TRAP( error,
                iError = iEntryWrapper->ProcessAttachmentDataL( attaDataPtr, iDRMFlags )
                );
            if ( iError == KErrNone )
                {
                iError = error;
                }
            if ( iError == KMmsErrorRemoveDRM )
                {
                iRemoveDrm = ETrue;
                iError = KErrNone;
                }
            }
        // if attachment is DRM protected and must be removed, just increase pointers
        iPosition += attaDataPtr.Length();
        iAttaDataWritten += attaDataPtr.Length();
#ifndef _NO_MMSS_LOGGING_
        TMmsLogger::Log( _L("Chunked decode - Attachment data %d, cumulative %d"),
        attaDataPtr.Length(), iAttaDataWritten );
#endif
        }
    }
    
// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CMmsDecode::FinishSinkingAttachment( TBool aLastDataChunk )
    {
    TInt error = KErrNone;
#ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L("Chunked decode - Attachment ready"));
#endif
    // end of attachment - close file
    // This must be called even if the error indicates the DRM file must be removed
    // This call does all cleanup and actually removes the DRM attachment if needed.
    TRAPD( error2,
        error = iEntryWrapper->FinalizeAttachmentL( *iStore, iCurrentAttaLength, iDRMFlags )
        );
    if ( iError == KErrNone )
        {
        iError = error;
        }
    if ( iError == KErrNone )
        {
        iError = error2;
        }
    // If DRM handling changed file size, iCurrentAttaLength gets adjusted
    if ( iError == KMmsErrorRemoveDRM )
        {
        // if error == KMmsErrorRemoveDRM all is well, and we can continue
        // but we don't have an attachment
        iError = KErrNone;
        // DRM already removed - flag can be cleared now
        iRemoveDrm = EFalse;
        }
    else if ( iError == KErrNone )
        {
        iTotalSize += iMimeHeaders->Size() + iCurrentAttaLength;
        if ( ( iMultipartType == KMmsAssignedApplicationVndWapMultipartRelated ) &&
            ( iRootContentId.Length() > 0 ) &&
            ( iRootAttachmentId == 0 ) )
            {
            // check if content-id's match
            if ( iRootContentId.Compare( iMimeHeaders->ContentId() ) == 0 )
                {
                iRootAttachmentId = iCurrentAttachment;
                }
            }
        }
    else
        {
        // do nothing - keep LINT happy
        }
    // no problem with signed/unsigned    
    if ( !( aLastDataChunk && iPosition == iLength ) &&
        ( iAttaNumber < iNumberOfAttachments ) )
        {
        iDecodingStage = EMmsAttachmentHeaders;
        }
    else
        {
        iDecodingStage = EMmsDone;
        }
    }
    
// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CMmsDecode::FinalizeSinkingLastChunk()
    {
    TInt error = KErrNone;
    
#ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L("Chunked decode - last chunk"));
#endif
    // If we have reached the last chunk, all data should be handled by now
    iDecodingStage = EMmsDone;
    
    // Read possible data from plain text file into encode buffer
    // and set iFake subject to point to the buffer
    // Must start from 1 because data would never start from 0
    // and value 0 indicates no fake subject available.

    // Fake subject handling        
    // Our store should still be valid.
    // The fake subject handling and multipart/report handling are "best effort"
    // Even if the operations fail, the code continues as if no error was encountered.
    TInt oldSize = iDecodeBuffer->Size();
    TRAP_IGNORE(
        {
        // if we have no subject but we do have only one text attachment
        // we take text from the beginning as subject
        // 
        if ( iFirstTextPlain && iPlainTexts == 1 && iMmsHeaders->Subject().Length() == 0 )
            {
            MMsvAttachmentManager& attaMan = iStore->AttachmentManagerL();
            RFile textAtta = attaMan.GetAttachmentFileL( iFirstTextPlain );
            CleanupClosePushL( textAtta );
            iDecodeBuffer->ResizeL( KMmsMaxDescription + 1 );
            // We must start from any position > 0
            TPtr8 pointer = iDecodeBuffer->Ptr( 1 );
            // read text into buffer
            error = textAtta.Read( pointer ); 
            if ( error == KErrNone )
                {
                iFakeSubject = 1;
                iTextPlainLength = pointer.Length();
                }
            CleanupStack::PopAndDestroy( &textAtta ); // textAtta file handle closed
            }
            
        // Multipart/report handling
        if ( iMultipartType == KMmsAssignedApplicationVndWapMultipartReport )
            {
            // we only keep the first part from multipart/report
            MMsvAttachmentManager& attaMan = iStore->AttachmentManagerL();
            MMsvAttachmentManagerSync& attaManSync = iStore->AttachmentManagerExtensionsL();
            TInt count = attaMan.AttachmentCount();
            TInt i;
            for ( i = count - 1; i > 0; --i )
                {
                attaManSync.RemoveAttachmentL( i );
                }
            }
        });
    
    error = KErrNone;
    iLength = iDecodeBuffer->Size();
    TRAP( error, FinishL() );
    TRAP_IGNORE( CommitStoreL() );
    delete iStore;
    iStore = NULL;
    if ( iError == KErrNone )
        {
        iError = error;
        }
    // Restore the original buffer size
    // This should actually never leave because we are just  restoring the original size
    TRAP_IGNORE( iDecodeBuffer->ResizeL( oldSize ));
    iLength = iDecodeBuffer->Size();
    }
    
// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CMmsDecode::DecodeHeadersL()
    {

    if ( iLength < KMmsMinHeaderLength )
        {
        // cannot decode if nothing to decode
        // each header is at least 2 bytes. 
        // If not at least 2 bytes, no can do.
        iError = KErrCorrupt;
        }

    iMmsHeaders->Reset();
    
    // Save the time the entry was received into the MMS headers
    // the value in TMsvEntry may change, but this remains
    TTime now;      
    now.UniversalTime();
    iMmsHeaders->SetReceivingTime( now );

    // each header byte must be followed by at least one byte value.
    // otherwise the message is corrupted
    // when we reach the header that indicates the start of the attachment
    // structure, we expect that no headers follow the attachment part.
    
    // each header is at least 2 bytes - header name and value
    while ( ( iPosition <= iLength - KMmsMinHeaderLength ) &&
        ( iError != KErrCorrupt ) &&
        ( !iLastHeader ) )
        {
        DecodeOneHeaderL();
        }
      
    SaveMMSHeadersL();    

    TRequestStatus* status = &iStatus;
    *status = KRequestPending;
    SetActive();

    // Now we indicate we already did everything.
    User::RequestComplete( status, iError );

    }
    
// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CMmsDecode::SaveMMSHeadersL()
    {
    
    // We save the headers only in case we are decoding an incoming
    // message. In other cases we deliver the results back to the caller
    // in iMmsHeaders.
    // Only retrieve confirmation is supposed to contain body that
    // needs saving of attachments.

    // We save a message of type KMmsMessageTypeMSendReq as it were an
    // incoming message, as during tests we get our own
    // send requests back as new messages.
    // We also save notifications and delivery reports.
    // However, in normal case MMS server calls a different function
    // (CMmsDecode::DecodeHeadersL) to decode notifications and delivery
    // reports as they do not contain attachments, and delivery reports
    // are not saved on disk, but are logged and deleted.
    // For testing purposes upload request is saved as an incoming message

    if ( ( iMmsHeaders->MessageType() == KMmsMessageTypeMRetrieveConf ||
        iMmsHeaders->MessageType() == KMmsMessageTypeMSendReq ||
        iMmsHeaders->MessageType() == KMmsMessageTypeMNotificationInd ||
        iMmsHeaders->MessageType() == KMmsMessageTypeDeliveryInd ||
        iMmsHeaders->MessageType() == KMmsMessageTypeMBoxUploadReq ) &&
        iError == KErrNone )
        {
        // The entry wrapper must be pointing to our message entry
        if ( !iStore )
            {
            iStore = iEntryWrapper->EditStoreL();
            }
        iError = iEntryWrapper->DiskSpaceBelowCriticalLevelL( iMmsHeaders->Size() );
        if ( iError == KErrNone )
            {
            iMmsHeaders->StoreL( *iStore );
            iStore->CommitL();
            }
        // the store will be left open until all the attachments have been saved
        }

    // Afterwards we must calculate the total message size and
    // store it in the entry.
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CMmsDecode::DecodeOneHeaderL()
    {

    TInt header;
    header = GetHeaderName();

    if ( header == -1 )
        {
        return; // skip unknown
        }

    // Get the corresponding value
    
    switch ( header )
        {
        case KMmsAssignedFrom:
            DecodeFromHeaderL();
            break;
        case KMmsAssignedTo:
            DecodeToL();
            break;
        case KMmsAssignedCc:
            DecodeCcL();
            break;
        case KMmsAssignedBcc:
            DecodeBccL();
            break;
        case KMmsAssignedContentLocation:
            DecodeContentLocationHeaderL();
            break;
        case KMmsAssignedDate:
            DecodeDateHeaderL();
            break;
        case KMmsAssignedDeliveryReport:
            DecodeDeliveryReportHeader();
            break;
        case KMmsAssignedDeliveryTime:
            DecodeDeliveryTimeL();
            break;
        case KMmsAssignedExpiry:
            DecodeExpiryL();
            break;
        case KMmsAssignedMessageClass:
            DecodeMessageClass();
            break;
        case KMmsAssignedMessageId:
            DecodeMessageIdL();
            break;
        case KMmsAssignedMessageType:
            DecodeMessageType();
            break;
        case KMmsAssignedMmsVersion:
            DecodeMmsVersion();
            break;
        case KMmsAssignedMessageSize:
            DecodeMessageSize();
            break;
        case KMmsAssignedPriority:
            DecodePriority();
            break;
        case KMmsAssignedReadReply:
            DecodeReadReply();
            break;
        case KMmsAssignedReportAllowed:
            DecodeReportAllowed();
            break;
        case KMmsAssignedResponseStatus:
        case KMmsAssignedRetrieveStatus:
            DecodeResponseStatusL( header );
            break;
        case KMmsAssignedResponseText:
        case KMmsAssignedRetrieveText:
        case KMmsAssignedStatusText:
            DecodeResponseTextL( header );
            break;
        case KMmsAssignedSenderVisibility:
            DecodeSenderVisibility();
            break;
        case KMmsAssignedStatus:
            DecodeStatus();
            break;
        case KMmsAssignedSubject:
            DecodeSubjectL();
            break;
        case KMmsAssignedTID:
            DecodeTidL();
            break;
        case KMmsAssignedContentType:
            // Content type is the last header in the PDU
            // Number of attachments and last header indicator will be set 
            DecodeContentTypeL();
            break;
        case KMmsAssignedReadStatus:
            DecodeReadStatus();
            break;
        case KMmsAssignedReplyCharging:
            DecodeReplyCharging();
            break;
        case KMmsAssignedReplyChargingDeadline:
            DecodeReplyChargingDeadlineL();
            break;
        case KMmsAssignedReplyChargingID:
            DecodeReplyChargingIdL();
            break;
        case KMmsAssignedReplyChargingSize:
            DecodeReplyChargingSize();
            break;
        case KMmsAssignedPreviouslySentBy:
            DecodePreviousSenderL();
            break;
        case KMmsAssignedPreviouslySentDate:
            DecodePreviouslySentDateL();
            break;
        case KMmsAssignedMmsStore:
            DecodeStoreHeaderL();
            break;
        case KMmsAssignedMMState:
            DecodeMMBoxStateL();
            break;
        case KMmsAssignedMMFlags:
            GetKeywordL();
            break;
        case KMmsAssignedStoreStatus:
            DecodeMMBoxStoreStatusL();
            break;
        case KMmsAssignedStoreStatusText:
            DecodeMMBoxStoreStatusTextL();
            break;
        case KMmsAssignedStored:
            DecodeStoredInMMBoxHeaderL();
            break;
        case KMmsAssignedAttributes:
            DecodeAttributesHeaderL();
            break;
        case KMmsAssignedTotals:
            DecodeTotalsL();
            break;
        case KMmsAssignedMboxTotals:
            DecodeMboxTotalsL();
            break;
        case KMmsAssignedQuotas:
            DecodeQuotaHeaderL();
            break;
        case KMmsAssignedMboxQuotas:
            DecodeMBoxQuotasL();
            break;
        case KMmsAssignedMessageCount:
            DecodeMessageCountL();
            break;
        case KMmsAssignedStart:
            DecodeStartInMMBoxViewL();
            break;
        case KMmsAssignedDistributionIndicator:
            DecodeDistributionIndicator();
            break;
        case KMmsAssignedElementDescriptor:
            DecodeElementDescriptorL();
            break;
        case KMmsAssignedLimit:
            DecodeMessageLimitL();
            break;
        case KMmsAssignedExtendedNotificationText:
            DecodeExtNotifTextL();
            break;
        case KMmsAssignedExtendedNotificationEOL:
            DecodeExtNotifEolL();
            break;
        case KMmsAssignedContentClass:
            DecodeContentClass();
            break;
        case KMmsAssignedDrmContent:
            DecodeDrmContentHeader();
            break;
        case KMmsAssignedAdaptationAllowed:
            DecodeAdaptationAllowed();
            break;
        case KMmsAssignedApplicId:
            DecodeApplicationIdL();
            break;
        case KMmsAssignedReplyApplicId:
            DecodeReplyApplicationIdL();
            break;
        case KMmsAssignedAuxApplicInfo:
            DecodeApplicationInfoL();
            break;
        case KMmsAssignedRecommendedRetrievalMode:
            DecodeRecommendedRetrievalMode();
            break;
        case KMmsAssignedRecommendedRetrievalModeText:
            DecodeRecommendedRetrievalModeTextL();
            break;
        case KMmsAssignedReplaceId:
        case KMmsAssignedCancelId:
            DecodeCancelReplaceIdL( header );
            break;
        case KMmsAssignedCancelStatus:
            DecodeCancelStatus();
            break;
        default:
#ifndef _NO_MMSS_LOGGING_
            TMmsLogger::Log( _L("- Unsupported header : 0x%02X"), (header & KMms0x7F) );
#endif
            // This is an error.
            // This is an unknown header, just ignore it.
            SkipFieldValue();
            break;
        }

    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
TInt CMmsDecode::GetHeaderName()
    {

    TInt header;
    header = -1; // unknown

    TUint8 byte;

    if ( iPosition > iLength - KMmsMinHeaderLength )
        {
        iError = KErrCorrupt;
        return header;
        }

    iDecodeBuffer->Read(iPosition, &byte, 1);

    // byte cannot be bigger than 255. No need to test upper limit
    if ( byte >= KMms0x80 )
        {
        // well known header, advance pointer
        iPosition++;
        header = byte;
        return header;
        }

    // we cannot handle headers which are not well known.
    // however, we should be able to skip them and ignore them.
    // If a field name has no encoding, it must be encoded as text.

    // If the header is corrupted, the value may be something
    // illegal. We should have a mechanism of skipping
    // illegal codings, too, but if the header name is not a
    // short integer or a text string, we are so deep in trouble
    // that we cannot decode the whole message anyway.

    // We handle some known text headers. Here we check if possible
    // text header is one we know. If not, it is skipped.

    TPtrC8 byteString = GetByteString();

    // see if this is in our table
    TInt i = 0;
    TBool matchFound = EFalse;
    TPtrC8 matchTablePointer;
    matchTablePointer.Set( KMmsExtensionHeaderLookup[i].extensionHeader );
    while ( matchTablePointer.Length() > 0 && !matchFound )
        {
        if ( matchTablePointer.CompareF( byteString ) == 0 )
            {
            header = *KMmsExtensionHeaderLookup[i].assignedValue;
            matchFound = ETrue;
            }
        i++;
        matchTablePointer.Set( KMmsExtensionHeaderLookup[i].extensionHeader );
        }

#ifndef _NO_MMSS_LOGGING_
    // we do not leave because of logging.
    TInt error = KErrNone;
    TRAP( error,
        {
        // length - terminating zero
        HBufC16* buffer = NULL;
        buffer = HBufC16::NewLC( byteString.Length() );
        buffer->Des().Copy( byteString );
        TPtrC dummy;
        // we cannot log indefinitely long strings.
        // We get this long strings only if the message is corrupted.
        dummy.Set( buffer->Des().Left( KMmsMaxLogStringLength ) );
        TMmsLogger::Log( _L("- Text header : %S"), &dummy );
        CleanupStack::PopAndDestroy( buffer );
        });
    if ( error != KErrNone )
        {
        TMmsLogger::Log( _L("- Decode left when reading Text header, error: %d"), error );
        }
#endif

    if ( matchFound )
        {
        return header;
        }

    // If we could interpret text headers, it should be done here
    // For now we just skip the field value without trying to interpret it.
    // As we cannot interpret the header, we must skip the value.
    // If a text header match was found in our table, we can interpret the header
    // and we leave the value intact

    if ( iPosition < iLength && header == -1 )
        {
        SkipFieldValue();
        }
    else
        {
        iError = KErrCorrupt;
        }

    return header;
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
/*
// not used any more
void CMmsDecode::SkipTextString()
    {

    TUint8 byte;

    iDecodeBuffer->Read(iPosition, &byte, 1);
    iPosition++;

    // don't read beyond buffer
    while ( byte != 0 && iPosition < iLength )
        {
        iDecodeBuffer->Read(iPosition, &byte, 1);
        iPosition++;
        }

    return;
    }
*/        

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CMmsDecode::SkipFieldValue()
    {

    TUint length;
    length = GetValueLength();
#ifndef _NO_MMSS_LOGGING_
    // This is an error situation, log always if logging allowed
    TMmsLogger::Log( _L("- Skipped field value: %d bytes"), length);
#endif

#ifndef _NO_MMSS_LOGGING_
    // this may produce a long log - but only if the message is weird
    TUint i;
    TUint8 byte;
    TBuf<1> character;
    for ( i = iPosition; i < iPosition + length && i < iLength; ++i )
        {
        iDecodeBuffer->Read(i, &byte, 1);
        TUint16 word = byte;
        if ( byte >= KMms0x20 && byte < KMms0x7F )
            {
            character.Copy( &word, 1 );
            TMmsLogger::Log( _L("- 0x%02X %S"), byte, &character );
            }
        else
            {
            TMmsLogger::Log( _L("- 0x%02X"), byte );
            }
        }
#endif
    iPosition += length;
// if iPosition == iLength, this was the last header, and we are not necessaily corrupted
// If the PDU contains only headers, and the last one is a text header whose value will be
// skipped, we are all right if iPosition == iLength.
    if ( iPosition > iLength )
        {
        iError = KErrCorrupt;
        }

    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CMmsDecode::SkipParameterName()
    {

    TUint8 byte;

    if ( iPosition >= iLength )
        {
        return;
        }

    iDecodeBuffer->Read(iPosition, &byte, 1);


    // byte cannot be bigger than 255. No need to test upper limit
    if ( byte >= KMms0x80 )
        {
        // well known header, advance pointer and remove high bit
        iPosition++;
#ifndef _NO_MMSS_LOGGING_
        TMmsLogger::Log( _L("- Skipped parameter: 0x%02X"), byte);
#endif
        }
    else
        {
        TPtrC8 byteString = GetByteString();
#ifndef _NO_MMSS_LOGGING_
        // we don't leave because of logging
        TInt error = KErrNone;
        TRAP( error, 
            {
            HBufC16* buffer = NULL;
            buffer = HBufC16::NewLC( byteString.Length() );
            buffer->Des().Copy( byteString );
            TPtrC dummy;
            // we cannot log indefinitely long strings.
            // We get this long strings only if the message is corrupted.
            dummy.Set( buffer->Des().Left( KMmsMaxLogStringLength ) );
            TMmsLogger::Log( _L("- Skipped parameter : %S"), &dummy ); 
            CleanupStack::PopAndDestroy( buffer );
            });
        if ( error != KErrNone )
            {
            TMmsLogger::Log( _L("- Decode left when logging skipped parameter, error : %d"),
                error );
            }
#endif
        }

    }


// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
TUint32 CMmsDecode::GetUintvar()
    {
    
    TUint32 uintvar;
    uintvar = 0;
    TUint8 byte;

    if ( iPosition >= iLength )
        {
        return uintvar;
        }

    iDecodeBuffer->Read(iPosition, &byte, 1);
    
    while ( byte & KMms0x80  && iPosition < iLength - 1 )
        {
        uintvar += ( byte & KMms0x7F );
        uintvar <<= KMmsUintvarShift;
        iPosition++;
        iDecodeBuffer->Read(iPosition, &byte, 1);
        }

    // add last byte without shift
    uintvar += byte;
    iPosition++;
    
    // If the file is corrupted, the length may be so big it becomes negative
    // We must make sure the value is less than maximum signed 32-bit integer.
    // 
    if ( uintvar > TUint32( KMaxTInt32 ) )
        {
        uintvar = TUint32( KMaxTInt32 );
        }

    return uintvar;
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
HBufC16* CMmsDecode::DecodeFromL()
    {

    TUint8 byte = 0;
    TUint length = 0;

    TUint start;
    start = iPosition;
    length = GetValueLength();

    // Get next token to see, if we have the address present token
    if ( iPosition < iLength )
        {
        iDecodeBuffer->Read(iPosition, &byte, 1);
        }

    if ( byte > KMms0x7F )
        {
        // an encoded token
        // It must be address present or be insert address
        iPosition++;
        if ( iPosition >= iLength )
            {
            iError = KErrCorrupt;
            return HBufC16::NewL( 0 );
            }

        // These two are the only two valid ones.
        if ( byte != KMmsAddressPresentToken && byte != KMmsInsertAddressToken)
            {
            // this encoding is a mess.
            // try if it resolves as an encoded string without 
            // these tokens
            iPosition = start;
            }
        }

    if ( length > 1 )
        {
        return DecodeAddressL();
        }
    else
        {
        return HBufC16::NewL( 0 );
        }

    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
HBufC16* CMmsDecode::DecodeAddressL()
    {
       
    HBufC16* buffer = NULL; // will be allocated by called subroutine

    if ( iPosition >= iLength )
        {
        return HBufC16::NewL( 0 );
        }
        
    // always use GetEncodedTextStringL(), it handles simple text strings, too

    buffer = GetEncodedTextStringL();

    // If our address contains an appended type specification,
    // it must be removed

    TInt position;
    TPtr temp = buffer->Des();
    _LIT ( KDot, "." );
    
    position = buffer->FindF( KMmsPlmnu().Ptr(), KMmsPlmnLength );
    if ( position != KErrNotFound )
        {
        temp.SetLength( position );
        // remove dots from phone number in order to keep
        // plmn number and ipv4 number different.
        position = temp.Find( KDot );
        while ( position >= 0 )
            {
            temp.Delete( position, 1 );
            position = temp.Find( KDot );
            }
        }
    else
        {
        position = buffer->FindF( KMmsIpv4u().Ptr(), KMmsIpv4Length );
        if ( position != KErrNotFound )
            {
            temp.SetLength( position );
            }
        else
            {
            position = buffer->FindF( KMmsIpv6u().Ptr(), KMmsIpv4Length ); 
            if ( position != KErrNotFound )
                {
                temp.SetLength( position );
                }
            }
        }

    return buffer;
    }


// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
HBufC16* CMmsDecode::GetSimpleTextStringL( TBool aKeepQuote /*= EFalse*/  )
    {
    // we don't handle quoted strings.
    // they should be present only in application headers, and 
    // we don't handle those

    TUint8 byte;
    HBufC16* buffer = NULL; // we must know length before trying to allocate
    TUint stringLength;
    TChar character;

    if ( iPosition >= iLength )
        {
        return HBufC16::NewL(0);
        }
    iDecodeBuffer->Read( iPosition, &byte, 1 );

    if ( byte == 0 )
        {
        // empty string.
        iPosition++;
        return HBufC16::NewL(0);
        }

    if ( byte == KMmsQuote )
        {
        // skip quote
        iPosition++;
        if ( iPosition >= iLength )
            {
            return HBufC16::NewL(1);
            }
        iDecodeBuffer->Read( iPosition, &byte, 1 );
        }

    TBool foundQuote = EFalse;

    if ( byte == KMmsStringQuote && !aKeepQuote )
        {
        // skip quote
        iPosition++;
        foundQuote = ETrue;
        if ( iPosition >= iLength )
            {
            return HBufC16::NewL(0);
            }
        iDecodeBuffer->Read( iPosition, &byte, 1 );
        }

    stringLength = iPosition;

    while ( byte != 0 && stringLength < iLength )
        {
        iDecodeBuffer->Read( stringLength, &byte, 1 );
        stringLength++;
        }

    // The diffrence of pointers is the actual string length
    // including the terminating zero.
    stringLength -= iPosition;

    if ( iPosition >= iLength )
        {
        return HBufC16::NewL(0);
        }

    iDecodeBuffer->Read( iPosition, &byte, 1 );
    if ( byte == 0 )
        {
        // Empty string. Only terminating zero.
        // Must point past the terminating zero at exit
        iPosition++;
        }

    // Allocate buffer for unicode.
    // Check if this is too long,
    // (allocating just stringLength left or panicked...)
    buffer = HBufC16::NewL( stringLength*KMms2 + KMms2 );

    while ( byte != 0 && iPosition < iLength )
        {
        iDecodeBuffer->Read(iPosition, &byte, 1);
        if ( byte != 0 )
            {
            character = byte;
            buffer->Des().Append( character );
            }
        iPosition++;
        }

    if ( foundQuote && buffer->Des().Right( 1 ).Compare( KQuote16 ) == 0 )
        {
        // if we removed the leading quote, we remove the trailing quote, too.
        buffer->Des().Delete( buffer->Des().Length() - 1, 1 );
        }
    return buffer;
        
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
HBufC16* CMmsDecode::GetEncodedTextStringL()
    {

    TUint8 byte;
    HBufC16* buffer = NULL; // we must know length before trying to allocate
    TUint stringLength = 0;
    TUint32 charset = 0;
    TUint start;

    if (iPosition >= iLength )
        {
        iError = KErrCorrupt;
        buffer = HBufC16::NewL(0);
        return buffer;
        }

    iDecodeBuffer->Read(iPosition, &byte, 1);

    if ( ( byte > KMms31 && byte < KMms0x80 ) || byte == 0 )
        {
        // simple text string. Restart.
        // (includes empty strings)
        // Keep quote because these are strange strings that may contain any
        // number of quotes all over the place

		// coverity[size_error][buffer_alloc]
        buffer = GetSimpleTextStringL( ETrue );
        // check if this looks like it needs decoding of MIME stuff (RFC2047)
        if ( buffer->Des().FindF( KEqualsQuestion16 ) == KErrNotFound )
            {
            return buffer;
            }
        else
            {
            // contains the magic equals-question mark combination,
            // try if MIME decoding succeeds
            CleanupStack::PushL( buffer );
            HBufC8* buffer8 = HBufC8::NewL( buffer->Des().Length() );
            buffer8->Des().Copy( buffer->Des() );
            CleanupStack::PushL( buffer8 );
            TPtr pointer = buffer->Des();
            TRAPD( error,
                TMmsGenUtils::DecodeAndConvertMessageHeaderL( buffer8->Des(), pointer, iFs ) );
            if ( error != KErrNone )
                {
                // if did not succeed, keep original as is
                buffer->Des().Copy( buffer8->Des() );
                }
            CleanupStack::PopAndDestroy( buffer8 );
            CleanupStack::Pop( buffer );
            return buffer;
            }
        }

    iPosition++;

    if (iPosition >= iLength )
        {
        iError = KErrCorrupt;
        buffer = HBufC16::NewL(0);
        return buffer;
        }

    if ( byte >= KMms0x80 )
        {
        // the message is corrupted.
        iError = KErrCorrupt;
        buffer = HBufC16::NewL(0);
        return buffer;
        }

    if ( byte == KMms31 )
        {
        stringLength = GetUintvar();
        }
    else 
        {
        stringLength = byte;
        }

    // Get character set
    // Character set must be an integer, short or long

    // check if character set is missing
    charset = GetLongOrShortInteger();
#ifndef _NO_MMSS_LOGGING_
    if ( charset != 0 )
        {
        TMmsLogger::Log( _L("- Encoded string charset %d"), charset );
        }
#endif

    if ( iPosition >= iLength || iError == KErrCorrupt )
        {
        iError = KErrCorrupt;
        buffer = HBufC16::NewL(0);
        return buffer;
        }

    // Get text length whatever character set

    iDecodeBuffer->Read(iPosition, &byte, 1);

    if ( byte == KMmsQuote )
        {
        // skip quote
        iPosition++;
        if (iPosition < iLength)
            {
            iDecodeBuffer->Read(iPosition, &byte, 1);
            }
        }

    start = iPosition;
    stringLength = 0;
    TBool foundQuote = EFalse;

    // Leave quote as is.
    // It has turned out that we do not get quoted strings in the WSP format
    // with leading quote only. We tend to get either both the leading and
    // trailing quote or none at all, so we better not remove the quote.
    // If we start seeing WSP quoted strings, we must start checking that
    // leading and trailing quotes match.

    /*
    if ( byte == KMmsStringQuote )
        {
        // skip quote
        iPosition++;
        start++;
        foundQuote = ETrue;
        }
    */

    while ( byte != 0 && iPosition < iLength )
        {
        iDecodeBuffer->Read(iPosition, &byte, 1);
        iPosition++;
        stringLength++;
        }

    // stringLength includes the terminating zero

    // allocate buffer for unicode
    // this should be the maximum we'll ever need
    if ( stringLength == 0 )
        {
        // The length of the string == 0
        // This is an empty string
        // I'm not sure if it is legal or not - but we keep it anyway
        if ( iPosition == start )
            {
            // we did not read anything, the first byte was 0.
            // we must advance out pointer beyond it
            iPosition++;
            }
        buffer = HBufC16::NewL(0);
        return buffer;
        }

    stringLength--; // remove the terminating zero
    // coverity[incorrect_multiplication][buffer_alloc]
	buffer = HBufC16::NewL( stringLength * KMms2 );

    if ( stringLength == 0 )
        {
        // buffer has zero length
        // I assume this is an empty string
        // I'm not sure if it is legal or not
        return buffer;
        }

    if ( charset == KMmsUtf8 )
        {
        TPtr8 pointer( iDecodeBuffer->Ptr( start ) );
        pointer.SetLength( stringLength );
        TInt error;
        TPtr16 pointer16 = buffer->Des();
        error = CnvUtfConverter::ConvertToUnicodeFromUtf8( pointer16, pointer );
        // should check error
        if ( error != KErrNone )
            {
            buffer->Des().Copy( pointer ) ; // could not convert, just copy garbage
            }
        }
    else
        {
        // other character sets to be implemented...
        TPtr8 pointer( iDecodeBuffer->Ptr( start ) );
        pointer.SetLength( stringLength );
        buffer->Des().Copy( pointer ) ; // just copy without conversion
        }
 
    if ( foundQuote && buffer->Des().Right( 1 ).Compare( KQuote16 ) == 0 )
        {
        // if we removed the leading quote, we remove the trailing quote, too.
        buffer->Des().Delete( buffer->Des().Length() - 1, 1 );
        }
    return buffer;
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
TPtrC8 CMmsDecode::GetByteString()
    {
    
    TUint8 byte = 0;
    if ( iPosition < iLength )
        {
        iDecodeBuffer->Read( iPosition, &byte, 1 );
        }
    else
        {
        return TPtrC8();
        }

    TUint start = iPosition;
    TUint length = 0;

    if ( byte == 0 )
        {
        // Empty string, skip over terminating zero.
        // Empty string may or may not be legal - we must
        // be prepared to accept it.
        // The caller must decide if empty value is legal or not.
        iPosition++;
        return TPtrC8();
        }

    TBool foundQuote = EFalse;
    if ( ( byte >= KMms32 ) && ( byte <= KMms0x7F ) )
        {
        if ( byte == KMmsStringQuote )
            {
            // skip quote
            iPosition++;
            start++;
            foundQuote = ETrue;
            }
        else if ( byte == KMmsQuote )
            {
            // We are not supposed to have a string starting with quote here
            // but we must be careful and strip the quote in case one exists.
            // skip quote
            iPosition++;
            start++;
            }

        // don't read beyond buffer
        // byte is between 32 and 127 when we come here.
        while ( byte != 0 && iPosition < iLength )
            {
            iDecodeBuffer->Read(iPosition, &byte, 1);
            iPosition++;
            }
        // now iPosition points beyond terminating zero - if there is one
        
        length = iPosition - start;
        if ( length > 1 )
            {
            // remove terminating zero if there is one
            length--;
            }
        // If we got a end-of-string immediately after string quote,
        // we have nothing left here.
        // Or even worse - we have not even the terminating zero
        if ( length < 1 )
            {
            return TPtrC8();
            }
        TPtr8 pointer( iDecodeBuffer->Ptr( start ) );
        pointer.SetLength( length );
        // Length is at least one, it was just checked.
        if ( foundQuote && ( pointer.Right( 1 )[0] == KMmsStringQuote ) )
            {
            length--; // decrease length to remove end quote
            }
        pointer.SetLength( length );
        return TPtrC8( pointer );
        }
    else
        {
        SkipFieldValue();
        return TPtrC8();
        }

    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
TPtrC8 CMmsDecode::GetUtf8String()
    {
    TUint8 byte = 0;
    if ( iPosition < iLength )
        {
        iDecodeBuffer->Read( iPosition, &byte, 1 );
        }
    else
        {
        return TPtrC8();
        }

    TUint start = iPosition;
    TUint length = 0;

    if ( byte == 0 )
        {
        // Empty string, skip over terminating zero.
        // Empty string may or may not be legal - we must
        // be prepared to accept it.
        // The caller must decide if empty value is legal or not.
        iPosition++;
        return TPtrC8();
        }

    if ( byte == KMmsQuote )
        {
        // skip quote
        iPosition++;
        start++;
        }

    // don't read beyond buffer
    while ( byte != 0 && iPosition < iLength )
        {
        iDecodeBuffer->Read(iPosition, &byte, 1);
        iPosition++;
        }

    // now iPosition points beyond terminating zero
    length = iPosition - start - 1;
    // If we got a end-of-string immediately after string quote,
    // we have nothing left here.
    if ( length < 1 )
        {
        return TPtrC8();
        }
    TPtr8 pointer( iDecodeBuffer->Ptr( start ) );
    pointer.SetLength( length );
    return TPtrC8( pointer );
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
TUint32 CMmsDecode::GetLongOrShortInteger()
    {

    TUint8 byte;
    TUint32 integer = 0;

    if ( iPosition >= iLength )
        {
        return integer;
        }

    iDecodeBuffer->Read(iPosition, &byte, 1);
    iPosition++;
    if ( byte & KMms0x80 )
        {
        integer = byte & KMms0x7F;
        return integer;
        }

    if ( byte > KMms30 )
        {
        // Data is corrupted.
        iError = KErrCorrupt;
        return integer; 
        }

    if ( byte > KMmsMaxIntegerLength )
        {
        // too long.
        iError = KErrTooBig;
        iPosition--;
        // It the caller thinks retry is possible, he must store the
        // original starting position.
        // So far no case has been found where the retry with very long
        // integer would be needed.
        SkipFieldValue();
        return integer;
        }

    TUint i;
    TUint length;
    length = byte;
    for ( i = 0;  i < length && iPosition < iLength; ++i)
        {
        integer <<= KMmsBitsInByte;
        iDecodeBuffer->Read(iPosition, &byte, 1);
        iPosition++;
        integer += byte;
        }

    return integer;

    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
TInt64 CMmsDecode::GetVeryLongInteger()
    {

    TUint8 byte;
    TInt64 veryLongInteger;

    veryLongInteger = 0;

    iDecodeBuffer->Read(iPosition, &byte, 1);
    iPosition++;
    if ( byte & KMms0x80 )
        {
        veryLongInteger = byte & KMms0x7F;
        return veryLongInteger;
        }

    if ( byte > KMms30 )
        {
        // data is corrupted.
        iError = KErrCorrupt;
        return veryLongInteger; 
        }

    if ( byte > KMmsMaxLongIntegerLength )
        {
        // too long.
        iError = KErrTooBig;
        iPosition--;
        // caller may do whatever he wants, or skip whole field
        return veryLongInteger;
        }

    TUint i;
    TUint length;
    length = byte;

    for ( i = 0;  i < length && iPosition < iLength; ++i)
        {
        veryLongInteger <<= KMmsBitsInByte;
        iDecodeBuffer->Read(iPosition, &byte, 1);
        iPosition++;
        veryLongInteger += TUint( byte );
        }
    
    return veryLongInteger;
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
TUint8 CMmsDecode::GetWellKnownFieldValueOrSkip()
    {

    TUint8 byte;
    byte = 0;
    iDecodeBuffer->Read(iPosition, &byte, 1);
        
    // Interpret the length of the field

    // one byte encoded value, this field has no more data
    if ( byte >= KMms0x80 )
        {
        iPosition++;
        return byte;
        }

    // if not one byte token, skip it
    SkipFieldValue();
    return byte;  // this is < 128, which is never a legal token

    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
TUint8 CMmsDecode::GetRelativeOrAbsoluteTime( TInt64& aTime )
    {
    TUint8 byte;
    TInt length;

    // we don't use the length, we just skip it
    length = GetValueLength();
    if ( ( iPosition + length ) > iLength || iPosition >= iLength )
        {
        iPosition = iLength;
        aTime = 0;
        byte = 0;
        iError = KErrCorrupt;
        return byte;
        }
    
    iDecodeBuffer->Read(iPosition, &byte, 1);

    if ( ( byte != KMmsAbsoluteToken ) && ( byte != KMmsRelativeToken ) )
        {
        // illegal encoding, skip.
        iPosition += length;
        byte = 0;
        aTime = 0;
        return byte;
        }

    iPosition++;
    if ( iPosition >= iLength )
        {
        iError = KErrCorrupt;
        byte = 0;
        aTime = 0;
        return byte;
        }

    aTime = GetVeryLongInteger();

    return byte;
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
TUint CMmsDecode::GetValueLength()
    {
    TUint8 byte;
    TUint length = 0;

    if ( iPosition >= iLength )
        {
        return 0;
        }

    iDecodeBuffer->Read(iPosition, &byte, 1);
    
    // Interpret the length of the field

    // one byte encoded value, this field has no more data
    if ( byte >= KMms0x80 )
        {
        length = 1;
        return length;
        }

    // text string
    // ( byte >= 128 returned already, so byte must be <=127 if we are here )
    if ( byte >= KMms32 )
        {
        // find the end
        length = iPosition;

        while ( byte != 0 && length < iLength )
            {
            iDecodeBuffer->Read(length, &byte, 1);
            length++;
            }

        // The difference of pointers is the actual string length
        length -= iPosition;

        return length;
        }
    
    // octet followed by a uintvar
    if ( byte == KMms31 )
        {
        iPosition++;
        // uintvar can be max 5 bytes, max value of
        // result is 32 bits.
        // get uintvar
        if ( iPosition < iLength )
            {
            length = GetUintvar();
            }
        return length;
        }

    // remains: octets 0 - 30
    // octet followed by the indicated number of data octets
    iPosition++;
    length = byte;
    return length;

    }
    
    
// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
TBool CMmsDecode::CheckHeaderLength()
    {
    TInt oldPosition = iPosition;
    TBool retVal = ETrue; // optimistic
    TUint8 byte;
    
    TBool checkUintvarData = ETrue;
    
    if ( iPosition < iLength )
        {
        // read first byte for some special checks
        iDecodeBuffer->Read( iPosition, &byte, 1 );
        }
    else
        {
        return EFalse;
        }
    
    if ( iDecodingStage == EMmsAttachmentHeaders )
        {
        if ( ( byte & KMms0x80 ) && ( ( byte & KMms0x7F ) == KWspQValue ) )
            {
            // Quality factor in attachment headers has strange encoding
            checkUintvarData = EFalse;
            }
        }
    
    // check header first - uintvar data included
    retVal = CheckValueLength( ETrue );
    
    // The position of the pointer is at the end of the header name
    // if the whole header name is in the buffer
    
    // Check the length of the header
    if ( retVal )
        {
        retVal = CheckValueLength( checkUintvarData );
        }
        
    if ( iDecodingStage == EMmsHeaders )
        {
        if ( byte == KMmsAssignedContentType && retVal )
            {
            // we need to have an uintvar specifying the number of attachments
            // to follow content type header            
            retVal = CheckUintvarLength();
            }
        }
   
    // always restore the pointer for decoding
    iPosition = oldPosition;
    return retVal;
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
TBool CMmsDecode::CheckValueLength( TBool aCheckUintvarData )
    {
    TBool retVal = ETrue; // optimistic
    
    TUint8 byte;
    TUint length = 0;
    
    if ( iPosition >= iLength )
        {
        return EFalse;
        }
        
    iDecodeBuffer->Read(iPosition, &byte, 1);
    
    // Interpret the length of the field

    if ( byte >= KMms0x80 )
        {
        // one byte encoded value, this field has no more data
        iPosition++;
        }
    else if ( byte >= KMms32 )
        {
        // text string - find the end
        while ( byte != 0 && iPosition < iLength )
            {
            iDecodeBuffer->Read( iPosition, &byte, 1 );
            iPosition++;
            }
        if ( byte != 0 )
            {
            // we reached the end of the buffer without finding the terminating 0
            // there is not enough data
            retVal = EFalse;
            }
        }
    else if ( byte == KMms31 )
        {
        // octet followed by a uintvar
        iPosition++;
        // get all bytes belonging to uintvar first to see if we have them all.
        TInt uintvarStart = iPosition;
        if ( CheckUintvarLength() )
            {
            // we have an uintvar, do we have all the following bytes
            if ( aCheckUintvarData )
                {
                iPosition = uintvarStart;
                // we are asked to check the data, too
                length = GetUintvar();
                if ( ( iLength - iPosition ) < length )
                    {
                    retVal = EFalse;
                    }
                else
                    {
                    iPosition += length;
                    }
                }
            }
        else
            {
            // we did not even have the uintvar let alone the data following it
            retVal = EFalse;
            }
        }
    else
        {
        // remains: octets 0 - 30
        // octet followed by the indicated number of data octets
        iPosition++;
        length = byte;
        if ( ( iLength - iPosition ) < length )
            {
            retVal = EFalse;
            }
        else
            {
            iPosition += length;
            }
        }
        
    return retVal;
    }


// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
TBool CMmsDecode::CheckUintvarLength()
    {
    TBool retVal = ETrue; // optimistic
    TUint8 byte;
    
    if ( iPosition >= iLength )
        {
        return EFalse;
        }
    
    // get all bytes belonging to uintvar to see if we have them all.
    iDecodeBuffer->Read( iPosition, &byte, 1 );
    while ( byte & KMms0x80  && iPosition < iLength - 1 )
        {
        iPosition++;
        iDecodeBuffer->Read(iPosition, &byte, 1);
        }
    iPosition++;
    if ( byte & KMms0x80 )
        {
        // if byte still has high bit set, we have not found
        // the end of the uintvar -> there is not enough data
        retVal = EFalse;
        }
                
    return retVal;
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
TPtrC8 CMmsDecode::GetContentTypeL()
    {

    TUint8 byte;
    TPtrC8 pointer = TPtrC8();

    if ( iPosition >= iLength )
        {
        iError = KErrCorrupt;
        return pointer;
        }

    iDecodeBuffer->Read(iPosition, &byte, 1);

    // Test if it is a well-known media type
    if ( byte >= KMms0x80 )
        {
        iPosition++; // skip the content type byte
        byte &= KMms0x7F; // remove the short integer identifier bit
        if ( ( byte == KMmsAssignedTextPlain ) )
            {
            // The beginning of the first text/plain part is used
            // to generate a description of the message, if
            // no real subject is available.
            // Save the starting point of corresponding data.
            if ( iDataStart < iLength )
                {
                iFakeSubject = iDataStart;
                }
            iUseForSubject = ETrue;
            // count the number of plain text attas.
            // We use iFakeSubject only if iPlainTexts == 1
            iPlainTexts++;
            }
        // encoded value
        if ( byte < KNumberContentTypes )
            {
            pointer.Set( KContentTypeTable[byte] );
            }
        else
            {
            pointer.Set( KContentTypeTable[KMmsUnAssigned] );
            }
        return pointer;
        }
    
    // Test if it is a text string without parameters
    // ( byte >= 128 returned already, byte is <= 127 if we get here )
    if ( byte >= KMms32 )
        {
        // text string
        return GetByteString();
        }

    // We have the worst case left:
    // Content-General-Form = Value-length Media-Type

    TUint mediaLength = 0;
    mediaLength = GetValueLength();
    TUint start = iPosition;

    if ( iPosition < iLength )
        {
        pointer.Set( GetContentTypeL() ); // We call ourselves.
        }

    // Now we should get the parameters, but we know nothing of
    // parameters. For now we just throw them away.
    // The handling of parameters of specific media types needs more study
    
    // The only parameter we care about now is charset.
    // its coding should be well-known

    while ( iPosition < start + mediaLength && iPosition < iLength )
        {
        iDecodeBuffer->Read( iPosition, &byte, 1 );
        if ( ( byte & KMms0x80 ) && ( ( byte & KMms0x7F ) == KWspCharset ) )
            {
            iPosition++;
            TUint32 charset;
            charset = GetLongOrShortInteger();
            iMimeHeaders->SetMimeCharset( charset );
            // if this is the part possibly used to generate
            // our fake subject, save the character set.
            if ( iFakeSubject == iDataStart )
                {
                iCharacterSet = charset;
                }
            }
        else if ( ( byte & KMms0x80 ) && ( ( byte & KMms0x7F ) == KWspName ) )
            {
            iPosition++;
            // Get name parameter to be suggestion of filename
            iMimeHeaders->ContentTypeParams().AppendL( KWspNameString );
            iMimeHeaders->ContentTypeParams().AppendL( GetByteString() );
            }
        else if ( ( byte & KMms0x80 ) && ( ( byte & KMms0x7F ) == KWspQValue ) )
            {
            iPosition++;
            // value encoded as Uintvar, must skip differently than others.
            GetUintvar();
            }
        else
            {
            if ( byte >= KMms0x20 && byte <= KMms0x7F )
                {
                // text string, save as content type parameter
                // If we don have a pair of two text strings, the message is illegal
                // The code will probably leave at some point
                iMimeHeaders->ContentTypeParams().AppendL( GetByteString() );
                iMimeHeaders->ContentTypeParams().AppendL( GetByteString() );
                }
            else
                {
                // skip
                SkipParameterName();
                SkipFieldValue();
                }
            }
        }
        
    iPosition = start + mediaLength;

    return pointer;

    }

// ---------------------------------------------------------
// If the content type is multipart, pointer will be positioned
// to the start of the multipart block (The first attachment)
// ---------------------------------------------------------
//
TUint8 CMmsDecode::GetMultipartContentTypeL()
    {

    TUint8 byte;
#ifndef _NO_MMSS_LOGGING_
    TPtrC dummy;
#endif

    iMultipartRootType.Set( TPtrC8() ); // clear contents
    iDecodeBuffer->Read(iPosition, &byte, 1);
    // Test if it is a well-known media type.
    if ( byte >= KMms0x80 )
        {
        byte &= KMms0x7F; // remove the short integer identifier bit
        // Test if it is a multipart type.
        if ( ( byte >= KMmsAssignedApplicationVndWapMultipart &&
            byte <= KMmsAssignedApplicationVndWapMultipartAlternative ) ||
            ( byte == KMmsAssignedApplicationVndWapMultipartRelated ) )
            {
            iPosition++; // skip the content type byte
            // Get number of parts in multipart
            iNumberOfAttachments = GetUintvar();
            }
        else
            {
            byte = 0;
            iNumberOfAttachments = 1;
            }
        // No more data in this header.
        // For example multipart/mixed has no parameters
        return byte;
        }

    // if we get here byte is <= 127 
    if ( byte >= KMms32 )
        {
        // text string, we don't even try to analyze it.
        // Nobody should send a well-known media type as a text string,
        // and if it is not a well-known type, we don't know what
        // to do with it, it will be treated as a monoblock
        byte = 0;
        iNumberOfAttachments = 1;
        return byte;
        }

    // The most complicated case: Content-General Form
    // Content-General-Form = Value-length Media-Type

    // we save the position in case this is monoblock and
    // we must keep the old position to get the content type for
    // the attachment.
    TUint oldPosition = iPosition;

    TUint mediaLength = 0;
    mediaLength = GetValueLength();
    TUint start = iPosition;
    byte = 0;

    if ( iPosition < iLength )
        {
        iDecodeBuffer->Read(iPosition, &byte, 1);
        }
    else
        {
        iNumberOfAttachments = 0;
        return 0;
        }

    if ( byte >= KMms0x80 )
        {
        byte &= KMms0x7F; // remove the short integer identifier bit
        // Test if it is a multipart type.
        if ( ( byte >= KMmsAssignedApplicationVndWapMultipart &&
            byte <= KMmsAssignedApplicationVndWapMultipartAlternative ) ||
            ( byte == KMmsAssignedApplicationVndWapMultipartRelated ) )
            {
            iPosition++; // skip the content type byte
            }
        else
            {
            // Not a multipart we can handle
            byte = 0;
            iNumberOfAttachments = 1;
            // point back to start of header
            // parameters will belong to the attachment
            iPosition = oldPosition;
            return byte;
            }
        }
    else
        {
        // text string, we don't even try to analyze it.
        // Nobody should send a well-known media type as a text string,
        // and if it is not a well-known type, we don't know what
        // to do with it, it will be treated as a monoblock
        TPtrC8 mediatype = GetByteString();
        if ( mediatype.CompareF( KMmsWapMultipartReport ) == 0 )
            {
            byte = KMmsAssignedApplicationVndWapMultipartReport;
            }
        else
            {
            byte = 0;
            iNumberOfAttachments = 1;
            // point back to start of header
            // parameters will belong to the attachment
            iPosition = oldPosition;
            return byte;
            }
        }

    // If we get here we have a WSP type multipart.
    // We must still extract the parameters of the content type
    // If we have multipart/related, we have start parameter.
    // We must also try to extract Application-Id and Reply-To-Application-ID
    //     in case Java has added them.
    // We don't care about the others now.

    TInt header;
    TPtrC8 contentHeader; // parameter name in case it is a string
   
    while ( iPosition < start + mediaLength && iPosition < iLength )
        {
        header = GetContentHeaderName( contentHeader );
        if ( header == KWspStart )
            {
            // Now we must get the content-id of the root part
            // and save it temporarily. We can get the actual
            // TMsvId for the attachment only by comparing the
            // Content-Id parameters of the attachments to the
            // saved root-id.
            TPtrC8 pointer = GetByteString();
            if ( pointer.Find( KMmsLeftAngle ) == 0 &&
                pointer.Find( KMmsRightAngle ) == pointer.Length() - 1 )
                {
                // remove angle brackets from cid, 2 characters removed
                pointer.Set( pointer.Mid( 1, pointer.Length() - KMms2 ) );
                }
            delete iRootContentIdBuffer;
            iRootContentIdBuffer = NULL;
            iRootContentIdBuffer = HBufC8::NewL( pointer.Length() );
            iRootContentIdBuffer->Des().Copy( pointer );
            iRootContentId.Set( iRootContentIdBuffer->Des() );
            }
        else if ( header == KWspQValue )
            {
            // value encoded as Uintvar, must skip differently than others.
            GetUintvar();
            }
        else if ( header == KWspRelatedType )
            {
            // get content type
            iMultipartRootType.Set( GetContentTypeL() ); 
            }
        else if ( header == KMmsTextHeader )
            {
            // see if this is Java application id
            if ( contentHeader.CompareF( KMmsJavaApplicationId ) == 0 )
                {
                DecodeApplicationIdL();
                }
            else if ( contentHeader.CompareF( KMmsJavaReplyApplicationId ) == 0 )
                {
                DecodeReplyApplicationIdL();
                }
            else
                {
                // something we don't handle
#ifndef _NO_MMSS_LOGGING_
                HBufC16* buffer = NULL;
                buffer = HBufC16::NewLC( contentHeader.Length() );
                buffer->Des().Copy( contentHeader );
                // we cannot log indefinitely long strings.
                // We get this long strings only if the message is corrupted.
                dummy.Set( buffer->Des().Left( KMmsMaxLogStringLength ) );
                TMmsLogger::Log( _L("- Text header: %S"), &dummy );
                CleanupStack::PopAndDestroy( buffer );
                buffer = NULL;
#endif
                SkipFieldValue();
                }
            }
        else
            {
            // skip
            // If the caller has messed things up and sent "start"
            // as a text string, we will miss it...
            // GetContentHeaderName always skips the header name.
            // if header < 0, the field has already been skipped
            if ( header >= 0 )
                {
                SkipFieldValue();
                }
            }
        }
        
    iPosition = start + mediaLength;
    iNumberOfAttachments = GetUintvar();

    return byte;
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CMmsDecode::GetAttachmentContentTypeL()
    {
    TPtrC8 byteString;
#ifndef _NO_MMSS_LOGGING_
    HBufC16* buffer = NULL;
    TPtrC dummy;
#endif

    // get content type
    byteString.Set( GetContentTypeL() );
    TInt semicolonPosition = 0;
    TInt position = 0;
    semicolonPosition = byteString.FindF( KSemicolon8 );
    // check if this content type has been sent to us as a string containing
    // parameters
    if ( semicolonPosition != KErrNotFound && semicolonPosition != 0 )
        {
        TInt endString = ( byteString.Left( semicolonPosition ).FindF( KSpace8 ) );
        if ( endString == KErrNotFound )
            {
            endString = semicolonPosition;
            }
        // First part of string is actual content-type header,
        // trailing blanks removed.
        // If the string was total garbage like "type huh;hei=hui",
        // the result will be wrong, but that cannot be helped if the input
        // is not legal.
        // The following should be OK
        // "application/type ;param1=value1
        TPtrC8 contentType = byteString.Left( endString );
        position = contentType.Find( KMmsSlash8 );
        if ( position <= 0 || position == contentType.Length() )
            {
            // weird string - no subtype, keep as is
            iMimeHeaders->SetContentTypeL( contentType );
            }
        else
            {
            iMimeHeaders->SetContentTypeL( contentType.Left( position ) );
            iMimeHeaders->SetContentSubTypeL( contentType.Mid( position + 1 ) );
            }
        // get the rest of the stuff as parameters
        ExtractContentTypeParametersL( byteString.Mid( semicolonPosition + 1 ) );
        }
    else
        {
        // If first character is semicolon, the content type is garbage anyway,
        // and we keep it "as is"
        position = byteString.Find( KMmsSlash8 );
        if ( position <= 0 || position == byteString.Length() )
            {
            // weird string - no subtype, keep as is
            iMimeHeaders->SetContentTypeL( byteString );
            }
        else
            {
            iMimeHeaders->SetContentTypeL( byteString.Left( position ) );
            iMimeHeaders->SetContentSubTypeL( byteString.Mid( position + 1 ) );
            }
        }
        
// if attachment subtype is "smil" or "amr", increase respective counter.


    if ( iMimeHeaders->ContentSubType().CompareF( KMmsSmilSubtype ) == 0 )
        {
        iSmilCount++;
        }
    else if ( iMimeHeaders->ContentType().CompareF( KMmsAudioType ) == 0 )
        {
        if ( iMimeHeaders->ContentSubType().CompareF( KMmsAudioSubtype ) == 0 )
            {
            iAudioCount++;
            }
        else if ( iMimeHeaders->ContentSubType().CompareF( KMmsAudioSubtype2 ) == 0 )
            {
            // Change the subtype because the x-amr is not supported
            iMimeHeaders->SetContentSubTypeL( KMmsAudioSubtype );
            iAudioCount++;
            }
        else
            {
            // do nothing - keep LINT happy
            }
        }
    else
        {
        // do nothing - keep LINT happy
        }
            
#ifndef _NO_MMSS_LOGGING_
    if ( semicolonPosition > 0 )
        {
        byteString.Set( byteString.Left( semicolonPosition ) );
        }
    buffer = HBufC16::NewLC( byteString.Length() );
    buffer->Des().Copy( byteString );
    // we cannot log indefinitely long strings.
    // We get this long strings only if the message is corrupted.
    dummy.Set( buffer->Des().Left( KMmsMaxLogStringLength ) );
    TMmsLogger::Log( _L("- Content Type: %S"), &dummy );
    CleanupStack::PopAndDestroy( buffer );
    if ( iMimeHeaders->MimeCharset() != 0 )
        {
        TMmsLogger::Log( _L("- Character set: %d"), iMimeHeaders->MimeCharset() );
        }
    if ( iMimeHeaders->ContentTypeParams().MdcaCount() > 0 )
        {
        TInt i;
        // we increment by 2 because every other value is parameter name
        // and every othe one the value
        for ( i = 0; i < iMimeHeaders->ContentTypeParams().MdcaCount(); i += KMms2 )
            {
            HBufC16* buffer2 = NULL;
            buffer = HBufC16::NewLC(
                iMimeHeaders->ContentTypeParams().MdcaPoint( i ).Length() );
            buffer->Des().Copy( iMimeHeaders->ContentTypeParams().MdcaPoint( i ) );
            // we cannot log indefinitely long strings.
            // We get this long strings only if the message is corrupted.
            dummy.Set( buffer->Des().Left( KMmsMaxLogStringLength ) );
            if ( i + 1 < iMimeHeaders->ContentTypeParams().MdcaCount() )
                {
                buffer2 = HBufC16::NewLC(
                    iMimeHeaders->ContentTypeParams().MdcaPoint( i + 1 ).Length() );
                TPtrC dummy2;
                buffer2->Des().Copy( iMimeHeaders->ContentTypeParams().MdcaPoint( i + 1 ) );
                dummy2.Set( buffer2->Des().Left( KMmsMaxLogStringLength ) );
                TMmsLogger::Log( _L("- %S: %S"), &dummy, &dummy2 );
                CleanupStack::PopAndDestroy( buffer2 );
                }
            else
                {
                TMmsLogger::Log( _L("- %S"), &dummy );
                }
            CleanupStack::PopAndDestroy( buffer );
            }
        }
#endif

    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CMmsDecode::DecodeOneContentHeaderL()
    {

    TInt header;
    HBufC16* buffer = NULL;
    TUint8 byte = 0;
    TPtrC8 contentHeader; // content header in case it is a string
#ifndef _NO_MMSS_LOGGING_
    TPtrC dummy;
#endif
    header = GetContentHeaderName( contentHeader );

    if ( header < 0 )
        {
        return; // skip unknown
        }

    // Get the corresponding value
    
    switch ( header )
        {
        case KWspContentLocation:
            {
            // I hope this is a simple text string. we cannot handle
            // different character sets in this context.
			// coverity[size_error][buffer_alloc]
            buffer = GetSimpleTextStringL(); // we might try to convert from utf8 to unicode
            CleanupStack::PushL( buffer );
            iMimeHeaders->SetContentLocationL( buffer->Des() );
            CleanupStack::PopAndDestroy( buffer );
            buffer = NULL;
            
#ifndef _NO_MMSS_LOGGING_
            buffer = HBufC16::NewLC( iMimeHeaders->ContentLocation().Length() );
            buffer->Des().Copy( iMimeHeaders->ContentLocation() );
            // we cannot log indefinitely long strings.
            // We get this long strings only if the message is corrupted.
            dummy.Set( buffer->Des().Left( KMmsMaxLogStringLength ) );
            TMmsLogger::Log( _L("- Content location: %S"), &dummy );
            CleanupStack::PopAndDestroy( buffer );
#endif
            break;
            }
        case KWspContentId:
            {
                TPtrC8 pointer = GetByteString();
                if ( pointer.Find( KMmsLeftAngle ) == 0 &&
                    pointer.Find( KMmsRightAngle ) == pointer.Length() - 1 )
                    {
                    // remove angle brackets from cid, 2 characters removed
                    pointer.Set( pointer.Mid( 1, pointer.Length() - KMms2 ) );
                    }
                iMimeHeaders->SetContentIdL( pointer );
                if ( iAttaNumber == 1 &&
                    iMultipartType == KMmsAssignedApplicationVndWapMultipartRelated &&
                    iRootContentId.Length() == 0 )
                    {
                    iRootContentId.Set( pointer );
                    }
#ifndef _NO_MMSS_LOGGING_
                buffer = HBufC16::NewLC( iMimeHeaders->ContentId().Length() );
                buffer->Des().Copy( iMimeHeaders->ContentId() );
                // we cannot log indefinitely long strings.
                // We get this long strings only if the message is corrupted.
                dummy.Set( buffer->Des().Left( KMmsMaxLogStringLength ) );
                TMmsLogger::Log( _L("- Content Id: %S"), &dummy );
                CleanupStack::PopAndDestroy( buffer );
#endif
            break;
            }
        case KWspContentDisposition:
            {
            TUint length;
            length = GetValueLength();
            TUint currentPosition = iPosition;

            if ( iPosition < iLength )
                {
                iDecodeBuffer->Read(iPosition, &byte, 1);
                }
            if ( byte >= KMms0x80 )
                {
                // well known header
                iPosition++;
                byte &= KMms0x7F;
                if ( byte == KWspAttachment )
                    {
                    iMimeHeaders->SetContentDispositionL( KWspAttachmentString );
                    }
                else if ( byte == KWspInline )
                    {
                    iMimeHeaders->SetContentDispositionL( KWspInlineString );
                    }
                // never mind others - might be form-data or something unknown
                else
                    {
                    // do nothing - keep LINT happy
                    }
                }
            else
                {
                // string - use as is
                TPtrC8 pointer = GetByteString();
                iMimeHeaders->SetContentDispositionL( pointer );
                }
#ifndef _NO_MMSS_LOGGING_
            buffer = HBufC16::NewLC( iMimeHeaders->ContentDisposition().Length() );
            buffer->Des().Copy( iMimeHeaders->ContentDisposition() );
            // we cannot log indefinitely long strings.
            // We get this long strings only if the message is corrupted.
            dummy.Set( buffer->Des().Left( KMmsMaxLogStringLength ) );
            TMmsLogger::Log( _L("- Content Disposition: %S"), &dummy );
            CleanupStack::PopAndDestroy( buffer );
#endif
            // now try to see if we have filename parameter
            while ( iPosition < ( currentPosition + length ) && iPosition < iLength )
                {
                byte = 0; // clean out old data
                iDecodeBuffer->Read(iPosition, &byte, 1);

                if ( byte >= KMms0x80 )
                    {
                    // well known parameter
                    iPosition++;
                    byte &= KMms0x7F;
                    if ( byte == KWspFileName )
                        {
                        // this might contain some strange encodings (against specs.)
                        // should be checked
                        TPtrC8 pointer = GetByteString();
                        iMimeHeaders->ContentDispositionParams().AppendL( KWspFilenameString );
                        iMimeHeaders->ContentDispositionParams().AppendL( pointer );
#ifndef _NO_MMSS_LOGGING_
                        buffer = HBufC16::NewLC( pointer.Length() );
                        buffer->Des().Copy( pointer );
                        // we cannot log indefinitely long strings.
                        // We get this long strings only if the message is corrupted.
                        dummy.Set( buffer->Des().Left( KMmsMaxLogStringLength ) );
                        TMmsLogger::Log( _L("- filename: %S"), &dummy );
                        CleanupStack::PopAndDestroy( buffer );
#endif
                        }
                    else
                        {
                        SkipFieldValue();
                        }
                    }
                else
                    {
                    // if "filename" has been sent as string, we skip it
                    if ( byte >= KMms0x20 && byte <= KMms0x7F )
                        {
                        // text string, save as content type parameter
                        // If we don have a pair of two text strings, the message is illegal
                        // The code will probably leave at some point
                        iMimeHeaders->ContentDispositionParams().AppendL( GetByteString() );
                        iMimeHeaders->ContentDispositionParams().AppendL( GetByteString() );
                        }
                    else
                        {
                        // skip
                        SkipParameterName();
                        SkipFieldValue();
                        }
                    }
                }

            iPosition = currentPosition + length;
            if ( iPosition >= iLength )
                {
                iError = KErrCorrupt;
                }
            break;
            }
        case KMmsTextHeader:
            {
            // contentHeader points to the textual header
#ifndef _NO_MMSS_LOGGING_
            // we don't leave just for logging
            TInt error = KErrNone;
            TRAP( error,
                {
                buffer = NULL;
                buffer = HBufC16::NewLC( contentHeader.Length() );
                buffer->Des().Copy( contentHeader );
                // we cannot log indefinitely long strings.
                // We get this long strings only if the message is corrupted.
                dummy.Set( buffer->Des().Left( KMmsMaxLogStringLength ) );
                TMmsLogger::Log( _L("- Textual content header: %S"), &dummy );
                CleanupStack::PopAndDestroy( buffer );
                };);
            if ( error != KErrNone )
                {
                TMmsLogger::Log( _L("- Decode left when logging content header, error : %d"),
                    error ); 
                }
#endif
            if ( contentHeader.FindF( KMmsExtension ) == 0 )
                {
                // extension header, store to mime headers, get corresponding parameter
                // and store to MIME headers
                TPtrC8 pointer = GetByteString();
                iMimeHeaders->XTypeParams().AppendL( contentHeader ); // header name 
                iMimeHeaders->XTypeParams().AppendL( pointer ); // header value
#ifndef _NO_MMSS_LOGGING_
                buffer = HBufC16::NewLC( pointer.Length() );
                buffer->Des().Copy( pointer );
                // we cannot log indefinitely long strings.
                // We get this long strings only if the message is corrupted.
                dummy.Set( buffer->Des().Left( KMmsMaxLogStringLength ) );
                TMmsLogger::Log( _L("- extension header value: %S"), &dummy );
                CleanupStack::PopAndDestroy( buffer );
#endif
                }
            else
                {
                SkipFieldValue();
                }
                
            break;
            }
        default:
#ifndef _NO_MMSS_LOGGING_
            TMmsLogger::Log( _L("- Unknown content header: 0x%02X"), header );
#endif
            SkipFieldValue();
            break;
        }

    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
TInt CMmsDecode::GetContentHeaderName( TPtrC8& aTextHeader )
    {

    TInt header;
    header = -1; // unknown
    aTextHeader.Set( TPtrC8() ); // empty string
    
    TUint8 byte;

    if ( iPosition < iLength )
        {
        iDecodeBuffer->Read(iPosition, &byte, 1);
        }
    else
        {
        return header;
        }

    // byte cannot be bigger than 255. No need to test upper limit
    if ( byte >= KMms0x80 )
        {
        // well known header, advance pointer and remove high bit
        iPosition++;
        header = (byte & KMms0x7F);
        return header;
        }
    else
        {
        TPtrC8 pointer = GetByteString();
        aTextHeader.Set( pointer );
        header = KMmsTextHeader; // this will cover X-headers
        if ( pointer.CompareF( KWspContentIdString ) == 0 )
            {
            header = KWspContentId;
            }
        }

    // To handle X-headers and Java apllication id parameters,
    // the text string is offered to caller.
    // The header has been set to KMmsTextHeader to indicate that the field value
    // is still available.


    return header;
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
//
void CMmsDecode::DecodeOneAttachmentL()
    {
    
    DecodeAttachmentHeadersL();
    
    // We must be able to handle an empty message without panic
    if ( iCurrentAttaLength == 0 )
        {
        // if attachment length == 0, do not create an attachment
        return;
        }
    
    TMsvAttachmentId attachmentId = 0;
    TInt32 drmFlags = 0;
    TPtrC8 attaData;
    if ( iPosition >= iLength )
        {
        iCurrentAttaLength = 0;
        }
    attaData.Set( iDecodeBuffer->Ptr( iPosition ).Ptr(), iCurrentAttaLength );
   
    TInt size = 0;
    
#ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L("- Attachment creation starts %S"), &iParse.FullName() );
#endif
    if ( iError == KErrNone )
        {
        iError = iEntryWrapper->CreateFileAttachmentL(
            *iStore,
            iParse.NameAndExt(),
            attaData,
            *iMimeHeaders,
            attachmentId,
            size,
            drmFlags);
        }

#ifndef _NO_MMSS_LOGGING_
    if ( iError == KErrNone )
        {
        TRAP_IGNORE({
            MMsvAttachmentManager& attaMan = iStore->AttachmentManagerL();
            CMsvAttachment* attaInfo = attaMan.GetAttachmentInfoL( attachmentId );
            TMmsLogger::Log( _L("- Attachment created %S"), &attaInfo->FilePath() );
            delete attaInfo;
            attaInfo = NULL;
            });
        }
#endif

    iDRMFlags |= drmFlags;
    
    if ( iError == KMmsErrorRemoveDRM )
        {
        // The attachment was not saved, but we can continue
        iError = KErrNone;
#ifndef _NO_MMSS_LOGGING_
        TMmsLogger::Log( _L("- DRM attachment removed") );
#endif
        }
    else if ( iError == KErrNone )
        {
        if ( ( iMultipartType == KMmsAssignedApplicationVndWapMultipartRelated ) &&
            ( iRootContentId.Length() > 0 ) &&
            ( iRootAttachmentId == 0 ) )
            {
            // check if content-id's match
            if ( iRootContentId.Compare( iMimeHeaders->ContentId() ) == 0 )
                {
                iRootAttachmentId = attachmentId;
                }
            }
        iTotalSize += size;
        }
    else
        {
        // do nothing - keep LINT happy
        }
 
    if ( iDataStart == iFakeSubject )
        {
        iTextPlainLength = iCurrentAttaLength;
        }

    // if doing dumping, also dump the smil part to a file
    // Note: this modifies iFileName, but our original file was closed already
    
#ifndef _NO_MMSS_LOGGING_
    DumpSmil( iCurrentAttaLength );
#endif
 
    iPosition = iNextStart;

    if ( iMultipartType == KMmsAssignedApplicationVndWapMultipartReport )
        {
        // If we have a multipart/report, we ignore the rest of attachments
        iNumberOfAttachments = 1;
        iPosition = iLength;
        }
     
    CompleteSelf( iError );    
        
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CMmsDecode::DecodeAttachmentHeadersL()
    {
    TUint32 headersLength = 0;
    iCurrentAttaLength = 0;
    iAttaNumber++; // starts fron zero, is 1 after first increment
    TInt error = KErrNone; // temporary error
#ifndef _NO_MMSS_LOGGING_
    TPtrC dummy;
#endif

#ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L("- Attachment #%d"), iAttaNumber );
#endif
    // we may have a multipart structure or a single monoblock data body
    // we handle both here.
    // The momoblock structure probably won't work correctly in chunked mode
    // but it is not supposed to be supported anyway.
    
    // Before this function is called in the chunked mode we have checked
    // that we have enough data to cover all the headers (the length of the
    // headers is given in the beginning)
    
    if ( iMultipartType != 0 )
        {
        if ( iPosition < iLength )
            {
            headersLength = GetUintvar();
            }
        else
            {
            iError = KErrCorrupt;
            if ( iDecodingStage == EMmsNotApplicable )
                {
                CompleteSelf( iError );
                }
            return;
            }

        if ( iPosition < iLength )
            {
            iCurrentAttaLength = GetUintvar();
            }
        else
            {
            iError = KErrCorrupt;
            if ( iDecodingStage == EMmsNotApplicable )
                {
                CompleteSelf( iError );
                }
            return;
            }
            
        // Data start can be calculated now.
        // Also the start of next attachment.

        iDataStart = iPosition + headersLength;

        if ( iDataStart > iLength && iDecodingStage == EMmsNotApplicable )
            {
            // This is an error only if we are handling all data, not chunks
            iError = KErrCorrupt;
            CompleteSelf( iError );
            return;
            }

        if ( iDataStart + iCurrentAttaLength > iLength && iDecodingStage == EMmsNotApplicable )
            {
            // if we are doing chunked decoding there may not be enough data in our buffer
            iCurrentAttaLength = iLength - iDataStart;
            }
        iNextStart = iDataStart + iCurrentAttaLength;
        }
    else
    // monoblock - this probably won't work in chunked mode
    // but then MMS engine is not supposed to support this kind of structure anyway
    // The data should always be wrapped in multipart structure even if there is 
    // only one part.
    // If the message is so small that everything fits into the buffer at one go,
    // then this will work in chunked mode, too.
        {
        // save old position
        TUint oldPosition = iPosition;
        headersLength = GetValueLength();
        iDataStart = iPosition + headersLength;
        iCurrentAttaLength = iLength - iDataStart;
        iNextStart = iDataStart + iCurrentAttaLength;
        // Header's length is part of content type header.
        // we cannot remove it, or we cannot interpret the
        // content type header correctly.
        iPosition = oldPosition;
        }

#ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L("- Headers length: %d"), headersLength );
    TMmsLogger::Log( _L("- Data length: %d"), iCurrentAttaLength );
#endif

    
    // don't touch iMmsHeaders, they may still be needed.
    // Just reset mime headers, attachments don't need anything else.
    iMimeHeaders->Reset();

    // We must be able to handle an empty message without panic
    if ( iCurrentAttaLength == 0 )
        {
        // empty attachment, don't create entry
        iPosition = iNextStart;
        if ( iDecodingStage == EMmsNotApplicable )
            {
            // chunked mode is not an active object
            CompleteSelf( iError );
            }
        return;
        }
    
    // attachment creation cannot be done before we have all the headers
    
    GetAttachmentContentTypeL();
    
    // Now we must analyze the content-headers
    // We only try to find Content-Location to get the filename
    // We should also save the Content-ID:s in case the SMIL part uses them
    // We also need to check if the Content-ID of some part matches
    // the content id given as the start parameter for multipart/related type
    // Some terminals put filename as parameter of content-disposition,
    // so that should be checked, too.

    while ( iPosition < iDataStart )
        {
        DecodeOneContentHeaderL();
        }

    if ( ( iMultipartType == KMmsAssignedApplicationVndWapMultipartRelated ) &&
        ( iRootContentId.Length() == 0 ) && iAttaNumber == 1 )
        {
        // First part must be root if start parameter not defined
        if ( iMimeHeaders->ContentId().Length() == 0 )
            {
            TTime now;
            now.UniversalTime();
            TInt random;
            TInt64 seed = now.Int64();
            // no problem with conversions - just creating a magic number for content id
            random = Math::Rand( seed );
            iTempBuffer.Des().Num( random );
            iMimeHeaders->SetContentIdL( iTempBuffer.Des() );
            iRootContentId.Set( iTempBuffer.Des() );
            }
        }
    
    // Now we have a root content id if the message is multipart/related.
    // we can set up the id for the root part after the attachment has been
    // created
    
    iPosition = iDataStart; // binary data starts here
    
    // Now we must try to generate a filename for the attachment.
    // A name must be generated before the actual attachment is saved
    
    TPtrC temp = TPtrC();
    TPtrC ptr;

    TBool isContentLocationFileName = EFalse;
    TPtrC8 dummy8;

    // Try name parameter of content location first
    if ( !isContentLocationFileName )
        {
        dummy8.Set( iMimeHeaders->GetContentTypeValue( KWspNameString ) );
        isContentLocationFileName =  MakeFilenameL( dummy8, iParse, TPtrC() );
        }

    // Try filename parameter of content-disposition header next
    TInt i;
    // every other string is parameter, every other one is value
    for ( i = 0;
        i < iMimeHeaders->ContentDispositionParams().MdcaCount() && !isContentLocationFileName;
        i += KMms2 )
        {
        // Try to generate a filename from the filename parameter
        if ( iMimeHeaders->ContentDispositionParams().MdcaPoint( i ).CompareF(
            KWspFilenameString ) == 0 )
            {
            dummy8.Set( iMimeHeaders->ContentDispositionParams().MdcaPoint( i + 1 ) );
            isContentLocationFileName = MakeFilenameL( dummy8, iParse, TPtrC() );
            }
        }

    HBufC16* uri = NULL;
    if ( !isContentLocationFileName &&
        iMimeHeaders->ContentLocation().Length() > 0 )
        {
        // Try to generate a filename from the content location
        error = KErrNone;
        TRAP( error, ( uri = EscapeUtils::EscapeDecodeL( iMimeHeaders->ContentLocation() ) ) );
        if ( error == KErrNone )
            {
            temp.Set( uri->Des().Right( KMmsMaxFileName ) );
            }
        else
            {
            temp.Set( iMimeHeaders->ContentLocation().Right( KMmsMaxFileName ) );
            }
        error = iParse.Set( TPtrC(), &temp, NULL );
        if ( error == KErrNone )
            {
            isContentLocationFileName = iEntryWrapper->IsValidFilename( iParse.NameAndExt() );
            }
        }
    CleanupStack::PushL( uri );

    // if content location not specified, use name parameter
    // as content location. FIX for /// originated messages.
    if ( iMimeHeaders->ContentLocation().Length() == 0 )
        {
        dummy8.Set( iMimeHeaders->GetContentTypeValue( KWspNameString ) );
        // This may use some strange charset. Should be decoded.
        // We only use it if it appears to be ascii.
        if ( dummy8.Length() > 0 && IsStringSafe( dummy8 ) )
            {
            HBufC* name = HBufC::NewL( dummy8.Length() );
            CleanupStack::PushL( name );
            name->Des().Copy( dummy8 );
            iMimeHeaders->SetContentLocationL( name->Des() );
#ifndef _NO_MMSS_LOGGING_
            // we cannot log indefinitely long strings.
            // We get this long strings only if the message is corrupted.
            dummy.Set( name->Des().Left( KMmsMaxLogStringLength ) );
            TMmsLogger::Log( _L("- Content location from name parameter: %S"), &dummy );
#endif
            CleanupStack::PopAndDestroy( name );
            name = NULL;
            }
        }

    TBuf<KMms14> attaName; // max 4294967295 attas.
    _LIT( KRelated, "att");

    if ( !isContentLocationFileName )
        {
        // We don't have content location, we must invent a filename
        // We should try to generate a sensible extension based on content type
        // Now we will have no extension
        attaName.Num( iAttaNumber, EDecimal );
        attaName.Insert( 0, KRelated );

        i = 0;
        TBool matchFound = EFalse;
        TPtrC8 matchTablePointer;
        matchTablePointer.Set( KMmsFileExtensionMatchTable[i].tag );
        while ( matchTablePointer.Length() > 0 && !matchFound )
            {
            if ( iMimeHeaders->ContentSubType().FindF(
                TPtrC8( KMmsFileExtensionMatchTable[i].tag ) ) != KErrNotFound )
                {
                temp.Set( KMmsFileExtensionMatchTable[i].extension );
                matchFound = ETrue;
                }
            i++;
            matchTablePointer.Set( KMmsFileExtensionMatchTable[i].tag );
            }
        ptr.Set( attaName );
        if ( !matchFound )
            {
            // unknown type, no extension
            iParse.Set( TPtrC(), &ptr, NULL );
            }
        else
            {
            iParse.Set( TPtrC(), &ptr, &temp );
            }
        }
        
    // We are through with filename, and URI may go if it ever was allocated
    CleanupStack::PopAndDestroy( uri );
    

    // We have to use iParse.NameAndExt as a guess for filename
    // we cannot call CApaApplication::GenerateFileName without a path
    
    // The next thing to do is to create an empty file ready to accept
    // the attachment data.
    
    
    
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CMmsDecode::ExtractContentTypeParametersL( const TPtrC8& aBuffer )
    {
    // we get here the parameter part of content type in the format
    // parameter1=value1;parameter2=value2
    TInt start = 0;
    TInt end = 0;
    TInt tokenEnd = 0;
    while ( start < aBuffer.Length() )
        {
        // remove leading spaces
        while ( start < aBuffer.Length() && aBuffer.Mid( start ).FindF( KSpace8 ) == 0 )
            {
            start++;
            }
        if ( start >= aBuffer.Length() )
            {
            return; // done, nothing left
            }
        // extract token
        end = aBuffer.Mid( start ).FindF( KEquals8 );
        if ( end == KErrNotFound )
            {
            end = aBuffer.Length(); 
            }
        else
            {
            end += start; // remember the original offset
            }
        tokenEnd = end - 1;
        // remove trailing spaces from token
        while ( tokenEnd - start > start && aBuffer.Mid( start, tokenEnd - start + 1 ).FindF( KSpace8 ) == tokenEnd - start )
            {
            tokenEnd--;
            }
        if ( aBuffer.Mid( start, tokenEnd - start + 1 ).Length() == 0 )
            {
            return; // garbage, give up
            }
        else
            {
            iMimeHeaders->ContentTypeParams().AppendL(
                aBuffer.Mid( start, tokenEnd - start + 1 ) );
            }
        // extract value
        start = end + 1;
        if ( start >= aBuffer.Length() )
            {
            iMimeHeaders->ContentTypeParams().AppendL( TPtrC8() );
            return;
            }
        // remove leading spaces
        while ( start < aBuffer.Length() && aBuffer.Mid( start ).FindF( KSpace8 ) == 0 )
            {
            start++;
            }
        if ( start >= aBuffer.Length() )
            {
            // there was no value for the parameter - must add an empty value
            iMimeHeaders->ContentTypeParams().AppendL( TPtrC8() );
            return; // done, nothing left
            }
        // A string or a quoted string left.
        // If quoted string, search for end quote.
        // If not quoted, search for next semicolon (delimiter between parameters)
        if ( aBuffer.Mid( start ).FindF( &KMmsStringQuote, 1 ) == 0 )
            {
            start++; // discard the quotes
            end = aBuffer.Mid( start ).FindF( &KMmsStringQuote, 1 );
            if ( end != KErrNotFound )
                {
                end += start; // remember the original offset
                }
            else
                {
                end = aBuffer.Length();
                }
            tokenEnd = end - 1;
            // skip to next delimiter
            end = aBuffer.Mid( tokenEnd + 1 ).FindF( KSemicolon8 );
            if ( end == KErrNotFound )
                {
                end = aBuffer.Length();
                }
            else
                {
                end += tokenEnd + 1; // remember the original offset
                }
            }
        else
            {
            end = aBuffer.Mid( start ).FindF( KSemicolon8 );
            if ( end == KErrNotFound )
                {
                end = aBuffer.Length();
                }
            else
                {
                end += start; // remember the original offset
                }
            tokenEnd = end - 1;
            // Remove trailing spaces from value.
            // If the value was a quoted string, spaces are part of value,
            // and must not be removed.
            while ( aBuffer.Mid( start, tokenEnd - start + 1 ).FindF( KSpace8 ) ==
                tokenEnd - start )
                {
                tokenEnd--;
                }
            }
        iMimeHeaders->ContentTypeParams().AppendL( aBuffer.Mid( start, tokenEnd - start + 1 ) );
        // keep trying 
        start = end + 1;
        }
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
TBool CMmsDecode::MakeFilenameL( TPtrC8& aSource, TParse& aDestination, const TFileName& aPath )
    {
    if ( aSource.Length() == 0 )
        {
        return EFalse;
        }
    TBool retValue = EFalse;
    TInt error = KErrNone;
    HBufC* name = NULL;
    name = HBufC::NewL( aSource.Length() );
    CleanupStack::PushL( name );
    TPtr pointer = name->Des();
    pointer.Copy( aSource );

    if ( pointer.FindF( KEqualsQuestion16 ) != KErrNotFound )
        {
        // Try to resolve MIME encoding
        TRAP( error, TMmsGenUtils::DecodeAndConvertMessageHeaderL( aSource, pointer, iFs ) );
        }
    else
        {
        // not MIME, error is fake, this may be just fine
        error = KErrCorrupt;
        }

    if ( error != KErrNone )
        {
        // Try to resolve plain utf8
        error = CnvUtfConverter::ConvertToUnicodeFromUtf8( pointer, aSource );
        }

    if ( error != KErrNone )
        {
        // use as is
        name->Des().Copy( aSource );
        }

    TPtrC ptr;

    if ( pointer.Length() > 0 )
        {
#ifndef _NO_MMSS_LOGGING_
        ptr.Set( name->Des().Left( KMmsMaxLogStringLength ) );
        TMmsLogger::Log( _L("- decoded name: %S"), &ptr );
#endif
        // We limit the length in case the filename is way too long (max is 100 characters).
        // We take the rightmost part to get the extension.
        // If we drop something from the beginning, it may be path or beginning of an overly
        // long name. As the too long name does not make sense anyway, we are free to drop
        // any part we like. No filenames should be 100 characters long.
        ptr.Set( name->Des().Right( KMmsMaxFileName ) );
        error = aDestination.Set( aPath, &ptr, NULL );
        if ( error == KErrNone )
            {
            // If the string we are trying to set to TParse is not valid,
            // we cannot check for filename
            retValue = iEntryWrapper->IsValidFilename( aDestination.NameAndExt() );
            }
        }
        
    CleanupStack::PopAndDestroy( name );
    return retValue;
    }


// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CMmsDecode::FinishL()
    {

    // We don't check the critical disk space level here.
    // If we got this far, we cannot balk now,
    // we have to continue to the bitter end.

    // If root attachment id == 0, the root has been deleted.
    // In that case the message structure has been messed up
    // and we can only show separate objects
    
    // use iStore member
    if ( !iStore )
        {
        iStore = iEntryWrapper->EditStoreL();
        }
    
    iMmsHeaders->RestoreL( *iStore );
    
    if ( iRootContentId.Length() > 0 &&
        iRootAttachmentId != 0 )
        {
        iMmsHeaders->SetRootContentIdL( iRootContentId );
        iMmsHeaders->SetMessageRoot( iRootAttachmentId );
        iMmsHeaders->StoreL( *iStore );
        }

    // this also commits all the attachments
    iStore->CommitL();
    
    // If we are in chunked mode we must get the possible fake subject from a file
    // because buffer gets reused.
    
    // now everything should be committed, and store can go
    delete iStore;
    iStore = NULL;

    // calculate the total size of message
    iTotalSize += iMmsHeaders->Size();

    TPtrC details = TPtrC();
    TPtrC description = TPtrC();

    // Neither sender nor subject might be present.
    // We ought to extract a few characters from the beginning of the
    // first text/plain part for the subject, if no separate subject is available

    // if the sender is a phone number add alias.
    // We don't add alias for email addresses here, as the comms
    // database search for email addresses is very slow
    // if there are lots of contacts.

    HBufC* buffer = HBufC::NewL( KMmsMaxDescription );
    CleanupStack::PushL( buffer );
    TPtr pBuffer = buffer->Des();
    HBufC* descriptionBuffer = NULL; // we may or may not need this

    if ( TMmsGenUtils::GenerateDetails( iMmsHeaders->Sender(),
        pBuffer, KMmsMaxDescription, iFs ) == KErrNone )
        {
        details.Set( pBuffer );
        }
    else
        {
        // We come here only if there was a fatal error in GenerateDetails.
        // Even if we don't find the alias, we have something in the string
        details.Set( iMmsHeaders->Sender() );
        }

    // the buffer cannot be reused, because tEntry only contains
    // a pointer to our data, not a copy of it.
    if ( iMmsHeaders->Subject().Length() > 0 )
        {
        description.Set( iMmsHeaders->Subject() );
        }
    else
        {
        // Check if we have found a text/plain part to be used as description.
        // Only use it if there is only one alternative, otherwise we don't
        // know which one to use.
        if ( iFakeSubject > 0 && iPlainTexts == 1 && iFakeSubject < iDecodeBuffer->Size() )
            {
            // we have a pointer to text/plain part
            // we need a new buffer as our old buffer should not be destroyed

            descriptionBuffer = HBufC::NewL( KMmsMaxDescription );
            CleanupStack::PushL( descriptionBuffer );
            pBuffer.Set( descriptionBuffer->Des() );

            if ( iCharacterSet == KMmsIso10646Ucs2 ||
                iCharacterSet == KMmsUTF16 ||
                iCharacterSet == KMmsUTF16BE ||
                iCharacterSet == KMmsUTF16LE)
                {
                // This may be big-endian or little-endian
                TBool nativeEndian = EFalse; // pessimist.
                TInt length = iTextPlainLength;
                if ( length > KMmsMaxDescription * KMms2 )
                    {
                    length = KMmsMaxDescription * KMms2; // this should be enough
                    }
                TInt i;
                TUint start = 0;
                TUint16 word = 0;
                iPosition = iFakeSubject;
                // must be able to read at least 2 bytes
                if ( iPosition < iLength - 1 )
                    {
                    iDecodeBuffer->Read( iPosition, &word, KMmsMinHeaderLength );
                    }

                // Check if we have a BOM (byte order mark).
                if ( word == KMmsByteOrderMark )
                    {
                    nativeEndian = ETrue;
                    iPosition += KMms2;
                    start += KMms2; // skip the endianness byte from the subject
                    }
                else if ( word == KMmsReversedByteOrderMark )
                    {
                    nativeEndian = EFalse;
                    iPosition += KMms2;
                    start += KMms2; // skip the endianness byte from the subject
                    }
                else
                    {
                    // coverity[size_error][buffer_alloc]
					CBufFlat* shortBuffer = CBufFlat::NewL( KMms2 );
                    CleanupStack::PushL( shortBuffer );
                    shortBuffer->ResizeL( KMms2 );
                    // no need to put it into cleanupstack... we don't leave before we delete it
                    TUint8 byte;
                    // put little-endian BOM to buffer
                    byte = 0xFF;
                    shortBuffer->Write( 0, &byte, 1 );
                    byte = 0xFE;
                    shortBuffer->Write( 1, &byte, 1 );
                    // get the result into a word, and see if it came out straight or reversed
                    shortBuffer->Read( 0, &word, KMms2 );
                    CleanupStack::PopAndDestroy( shortBuffer );
                    shortBuffer = NULL;
                    if ( iCharacterSet == KMmsUTF16LE )
                        {
                        // I think we are little endian, better check to be sure
                        if ( word == KMmsByteOrderMark )
                            {
                            // we are little endian, charset is little endian
                            nativeEndian = ETrue;
                            }
                        else
                            {
                            nativeEndian = EFalse;
                            }
                        }
                    else if ( iCharacterSet == KMmsUTF16BE )
                        {
                        if ( word == KMmsByteOrderMark )
                            {
                            // we are little endian, charset is big endian
                            nativeEndian = EFalse;
                            }
                        else
                            {
                            nativeEndian = ETrue;
                            }
                        }
                    // if NO BOM and character set does not define byte ordering,
                    // we don't really know.
                    // Native endian is left as false - may not be the correct guess.
                    else
                        {
                        // do nothing - keep LINT happy
                        }
                    }

                if ( nativeEndian )
                    {
                    for ( i = start; i < length - 1; i += KMms2 )
                        {
                        iDecodeBuffer->Read(iPosition, &word, KMms2 );
                        iPosition += KMms2;
                        pBuffer.Append( word );
                        }
                    }
                else
                    {
                    for ( i = start; i < length - 1; i += KMms2 )
                        {
                        iDecodeBuffer->Read(iPosition, &word, KMms2 );
                        iPosition += KMms2;
                        TUint16 temp;
                        temp = TUint16 ( word & 0xff ) ; //lower byte
                        word >>= KMmsBitsInByte; // higher to lower
                        word = TUint16 ( ( temp << KMmsBitsInByte ) + word ); 
                        pBuffer.Append( word );
                        }
                    }

                }
            else if ( iCharacterSet == KMmsUsAscii )
                {
                TInt length = iTextPlainLength;
                if ( length > KMmsMaxDescription )
                    {
                    length = KMmsMaxDescription; // this should be enough
                    }
                TPtrC8 attaData;
                attaData.Set( iDecodeBuffer->Ptr( iFakeSubject ).Ptr(), length );
                pBuffer.Copy( attaData );
                }
            else if ( iCharacterSet == KMmsUtf8 )
                {
                TInt length = iTextPlainLength;
                if ( length > KMmsMaxDescription )
                    {
                    length = KMmsMaxDescription; // this should be enough
                    }
                // just in case our message is corrupted...
                if ( length > (TInt) ( iDecodeBuffer->Size() - iFakeSubject ) )
                    {
                    length = iDecodeBuffer->Size() - iFakeSubject;
                    }
                TPtrC8 attaData;
                attaData.Set( iDecodeBuffer->Ptr( iFakeSubject ).Ptr(), length );
                if ( length > 0 )
                    {
                    CnvUtfConverter::ConvertToUnicodeFromUtf8( pBuffer, attaData );
                    if ( pBuffer.Length() > 0 && pBuffer.Find( &KMmsByteOrderMark, 1 ) == 0 )
                        {
                        // remove the BOM
                        pBuffer.Delete( 0, 1 );
                        }
                    }
                }
            else
                {
                // we have some unknown character set.
                // we should call some converter
                }
            TInt position;
            for ( position = 0; position < pBuffer.Length(); position++ )
                {
                if ( pBuffer.Mid( position, 1 ) < KSpace16 ||
                    pBuffer.Mid( position, 1 ) == KMmsUnicodeLineSeparator ||
                    pBuffer.Mid( position, 1 ) == KMmsUnicodeParagraphSeparator ||
                    pBuffer.Mid( position, 1 ) == KMmsIdeographicSpace ||
                    ((TChar)pBuffer[position]).IsControl() )
                    {
                    pBuffer.Replace( position, 1, KSpace16 );
                    }
                }
                
            
            pBuffer.TrimAll();
            description.Set( pBuffer );
            }
        }
     
    // don't set the response status text to description if the status is OK.
    // In that case we have received an empty message that should have no description.
    // In OK case the text is mostly "OK" or something equally uninformative    
    if ( description.Length() == 0 && iMmsHeaders->ResponseText().Length() > 0 &&
        iMmsHeaders->ResponseStatus() > KMmsResponseStatusOK )
        {
        description.Set( iMmsHeaders->ResponseText() );
        }

    TUint32 messageType = 0;

    switch ( iMmsHeaders->MessageType() )
        {
        case KMmsMessageTypeMSendReq:
            messageType = KMmsMessageMSendReq;
            break;
        case KMmsMessageTypeMNotificationInd:
            messageType = KMmsMessageMNotificationInd;
            break;
        case KMmsMessageTypeMRetrieveConf:
            messageType = KMmsMessageMRetrieveConf;
            break;
        case KMmsMessageTypeDeliveryInd:
            messageType = KMmsMessageDeliveryInd;
            break;
        default:
            // Unrecognized type - or one we are not interested in.
            // Messages that should not normally be stored on disk
            // have the type "unrecognized"
            // The type in TMsvEntry should be used for showing icons
            // or other special purposes only
            messageType = KMmsMessageUnrecognized;
            break;
        }

    // All data to iMtmData1 is set to an empty base.
    // If caller has more flags to set, they must be set afterwards
    TInt32 mtmData = 0;
    mtmData |= messageType;
    mtmData |= iDRMFlags;
    if ( iMmsHeaders->MessageClass() == EMmsClassAdvertisement )
        {
        mtmData |= KMmsMessageAdvertisement;
        }
    else if ( iMmsHeaders->MessageClass() == EMmsClassInformational )
        {
        mtmData |= KMmsMessageInformational;
        }
    else
        {
        // do nothing - keep LINT happy
        }

    TMsvEntry entry;
    
    iEntryWrapper->GetIndexEntry( entry );
    
    entry.iDetails.Set( details );
    entry.iDescription.Set( description );
    entry.iMtmData1 = mtmData;
    entry.iSize = iTotalSize;
    // Set the date.
    // This can in principle be either the arrival time of the message
    // or the original time the message was received by MMSC
    
    entry.iDate = iMmsHeaders->ReceivingTime();
    
    switch ( iMmsHeaders->MessagePriority() )
        {
        case KMmsPriorityNormal:
            entry.SetPriority( EMsvMediumPriority );
            break;
        case KMmsPriorityLow:
            entry.SetPriority( EMsvLowPriority );
            break;
        case KMmsPriorityHigh:
            entry.SetPriority( EMsvHighPriority );
            break;
        default:            
            // if not defined default is normal
            entry.SetPriority( EMsvMediumPriority );
            break;
        }
        
    // set the bio type to audio message if the message contains nothing but one
    // amr part in addition to possible one smil
    
    if ( iSmilCount <= 1 && iAudioCount == 1 &&
       ( iAttaNumber == iSmilCount + iAudioCount ) )
        {
         if(!CheckDRMContent())
             {
                TBool audioSupported = EFalse;
                TRAP_IGNORE(
                    {
                    FeatureManager::InitializeLibL();
                    audioSupported = FeatureManager::FeatureSupported( KFeatureIdAudioMessaging );
                    FeatureManager::UnInitializeLib();
                    });
                
                if ( audioSupported )
                    {
                    entry.iBioType = KUidMsgSubTypeMmsAudioMsg.iUid;
                    }
             }
        }

    //Since the changes have been committed to the store , we should get exact attachment count from the store. 
    //The message type check is not required as the only messagtypes decoded are retrieve-conf in global mode and send-req in local mode
    TInt attachmentCount = 0;
    CMsvStore* store = iEntryWrapper->ReadStoreL();
    CleanupStack::PushL( store );

    // Only new attachment structure is supported    
    MMsvAttachmentManager& attachMan = store->AttachmentManagerL();
    attachmentCount = attachMan.AttachmentCount();

    CleanupStack::PopAndDestroy( store );
    if ( attachmentCount > 0 )
        {
        entry.SetAttachment(ETrue);
        }
       
    iEntryWrapper->ChangeIndexEntry( entry );    

    // now that we have saved our index entry, the buffers can go
    if ( descriptionBuffer )
        {
        // if we have allocated a separate buffer for the
        // description, it sits on cleanups stack too.
        CleanupStack::PopAndDestroy( descriptionBuffer );
        }
    CleanupStack::PopAndDestroy( buffer );

    }

// ---------------------------------------------------------
// CMmsDecode::CheckDRMContent
// ---------------------------------------------------------
//
TBool CMmsDecode::CheckDRMContent()
    {
    CFileProtectionResolver* resolver = CFileProtectionResolver::NewLC( iFs);

    CMsvStore* store = iEntryWrapper->ReadStoreL();
    CleanupStack::PushL( store );
    MMsvAttachmentManager& attaManager = store->AttachmentManagerL();    
    
    TInt attaCount = attaManager.AttachmentCount();
    TInt flCount = 0;
    TInt sdCount = 0;

    for ( TInt i = 0; i < attaCount; ++i )
        {
        CMsvAttachment* info = attaManager.GetAttachmentInfoL( i );
        CleanupStack::PushL( info );
        TDataType dataType( info->MimeType() );
        if(dataType.Des8().Match(KmmsSmilMimeType) != KErrNotFound)
            {
            CleanupStack::PopAndDestroy( info );
            continue;
            }
        RFile file = attaManager.GetAttachmentFileL( info->Id() );
        CleanupClosePushL( file );
        
        TInt status = resolver->ProtectionStatusL( file, dataType );
        if ( status & EFileProtForwardLocked ||
             status & EFileProtClosedContent )
            {
            flCount++;
            }
        else if ( status & EFileProtSuperDistributable )
            {
            sdCount++;
            }

        CleanupStack::PopAndDestroy( 2, info ); // file, info
        }

    CleanupStack::PopAndDestroy( 2, resolver ); //store, resolver
#ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L("- CheckDRMContent:Total count :%d,FlCount: %d"),attaCount, flCount );
    TMmsLogger::Log( _L("- CheckDRMContent:SDCount: %d"), sdCount );
#endif
    if(flCount ||sdCount )
    return ETrue;
    else
    return EFalse;
    }
 
// ---------------------------------------------------------
//
// ---------------------------------------------------------
void CMmsDecode::DumpL()
    {
    // no dump if not logging
#ifndef _NO_MMSS_LOGGING_
    TInt error = KErrNone;
    RFile file;
    TUint att;
    _LIT( KRelated, "Rec.mms");
    // if no can do, sorry.
    TRAP( error,
        {
        if ( ( iDumpIncoming ) && iDecodeBuffer )
            {
            if ( iDecodeBuffer->Size() > 0 && iDecodingStage == EMmsNotApplicable )
                {
                // non-chunked mode. All data is in buffer, and can be dumped in one go
                iFileName = KMmsDefaultLogDirectory;
                // Check if directory exists - no directory, no dump
                User::LeaveIfError( iFs.Att( iFileName, att ) );
                iParse.Set( iFileName, &KRelated, NULL );
                iFileName = iParse.FullName();
                User::LeaveIfError( CApaApplication::GenerateFileName(
                    iFs, iFileName ) );
                User::LeaveIfError( file.Create( iFs, iFileName,
                    EFileWrite | EFileShareExclusive ) );
                // for message id generation
                iParse.Set( iFileName, NULL, NULL );

                // the data is supposed to be in the encode buffer
                TPtr8 ptr = iDecodeBuffer->Ptr( 0 );
                file.Write( ptr );
                file.Flush();

                // done - close files
                file.Close();
                }
            // chunked mode start - no data yet, initialize file
            else if ( iLength == 0 && iDecodingStage == EMmsHeaders )
                {
                // This is the initialization of the dump file
                iFileName = KMmsDefaultLogDirectory;
                // create an empty file
                // The name of the file will remain in iFilename
                // Later data will be appended to the same file
                User::LeaveIfError( iFs.Att( iFileName, att ) );
                iParse.Set( iFileName, &KRelated, NULL );
                iFileName = iParse.FullName();
                User::LeaveIfError( CApaApplication::GenerateFileName(
                    iFs, iFileName ) );
                User::LeaveIfError( file.Create( iFs, iFileName,
                    EFileWrite | EFileShareExclusive ) );
                // for message id generation
                iParse.Set( iFileName, NULL, NULL );
                file.Close();
                }
            else if ( iDecodingStage != EMmsNotApplicable )
                {
                User::LeaveIfError( file.Open( iFs, iFileName,
                    EFileWrite | EFileShareExclusive ) );
                TInt position = 0;    
                User::LeaveIfError( file.Seek( ESeekEnd, position ) );
                // the data is supposed to be in the encode buffer
                // But only write the amount of data that has been processed.
                // Some of the data may be pushed back to the beginning of the buffer
                // and we do not want to write such data twice.
                // iOldData tells how much data was left over from previous round
                // We always write as much as we have got so that we can analyze
                // a possible error
                TPtr8 ptr = iDecodeBuffer->Ptr( iOldData );
                // If there is any data, write it    
                if ( ptr.Length() > 0 )
                    {
                    file.Write( ptr );
                    file.Flush();
                    }

                // done - close files
                file.Close();
                }
            else
                {
                // do nothing
                }
            }
        }
    );
    if ( error != KErrNone )
        {
        TMmsLogger::Log( _L("- Dump left, error : %d"), error ); 
        }
#endif
    }
    
// ---------------------------------------------------------
//
// ---------------------------------------------------------
void CMmsDecode::DumpSmil( TInt aDataLength )
    {
    // no dump if not logging
#ifndef _NO_MMSS_LOGGING_

    TBool lowMemory = EFalse;
    
    TRAP_IGNORE({
        lowMemory = iEntryWrapper->DiskSpaceBelowCriticalLevelL( aDataLength );
        });
        
    // Only dump smil part if not decoding in chunks.
    // In chunked mode it would be too complicated.
    if ( iDumpIncoming && iMimeHeaders->ContentType().CompareF( KMmsApplicationSmil ) == 0
        && !lowMemory && iDecodingStage == EMmsNotApplicable)
        {
        RFile file;
        iFileName = KMmsDefaultLogDirectory;
        TUint att;
        if ( iFs.Att( iFileName, att ) == KErrNone )
            {
            _LIT( KRelated2, "smil.txt");
            iParse.Set( iFileName, &KRelated2, NULL );
            iFileName = iParse.FullName();
            TInt error = CApaApplication::GenerateFileName( iFs, iFileName );
            if ( error == KErrNone )
                {
                error = file.Create( iFs, iFileName, EFileWrite | EFileShareExclusive );
                // for message id generation
                iParse.Set( iFileName, NULL, NULL );

                if ( error == KErrNone )
                    {
                    // the data is supposed to be in the encode buffer
                    TPtrC8 ptrx;
                    ptrx.Set( iDecodeBuffer->Ptr( iPosition ).Ptr(), aDataLength );
                    file.Write( ptrx );
                    file.Flush();
                    }

                // done - close files
                file.Close();
                }
            }
        }
#endif
    }
    

// ---------------------------------------------------------
//
// ---------------------------------------------------------
void CMmsDecode::LogYesNo( TRefByValue<const TDesC> aTitle, TInt aValue )
    {
#ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( aTitle );
    switch ( aValue )
        {
        case ( KMmsYes ):
            TMmsLogger::Log( KLogYes );
            break;
        case ( KMmsNo ):
            TMmsLogger::Log( KLogNo );
            break;
        default:
            TMmsLogger::Log( KLogUnknown, aValue );
            break;
        }
#endif
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
void CMmsDecode::GetKeywordL()
    {
    GetValueLength(); // must be read just to get it out of way
    TUint8 byte;            // for simple reads
    byte = GetWellKnownFieldValueOrSkip();
    TUint temp;
    temp = byte; // should be add, remove or filter
    HBufC16* buffer = NULL;
    buffer = GetEncodedTextStringL();
    CleanupStack::PushL( buffer ); // keyword
    TPtrC dummy;
    dummy.Set( buffer->Des() );

    iMmsHeaders->MMBoxMessageHeadersL().AppendKeywordItemL( temp, dummy );
#ifndef _NO_MMSS_LOGGING_
    dummy.Set( buffer->Des().Left( KMmsMaxLogStringLength ) );
    TMmsLogger::Log( _L("- Keyword: %S"), &dummy );
    switch ( temp )
        {
        case KMmsAddToken:
            TMmsLogger::Log( _L("-- Add") );
            break;
        case KMmsRemoveToken:
            TMmsLogger::Log( _L("-- Remove") );
            break;
        case KMmsFilterToken:
            TMmsLogger::Log( _L("-- Filter") );
            break;
        default:
            TMmsLogger::Log( KLogUnknown, temp );
            break;
        }
#endif
    CleanupStack::PopAndDestroy( buffer );
    }
    
// ---------------------------------------------------------
//
// ---------------------------------------------------------
void CMmsDecode::CompleteSelf( TInt aError )
    {
    // completed own status to get back to the RunL
    iStatus = KRequestPending;
    SetActive();
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, aError );
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
TBool CMmsDecode::IsStringSafe( const TDesC8& aString )
    {

    // Very simple check to see if string is "safe" ASCII
    // Used for headers, which are short strings

    TInt i;
    TBool safe = ETrue;
    for ( i = 0; i < aString.Length() && safe; ++i )
        {
        if ( aString[i] < KMmsLowestAscii || aString[i] >= KMmsHighestAscii )
            {
            safe = EFalse;
            }
        }
    return safe;
    }
    
// ---------------------------------------------------------
//
// ---------------------------------------------------------
void CMmsDecode::DecodeFromHeaderL()
    {
    HBufC16* buffer = NULL;
    buffer = DecodeFromL();
    CleanupStack::PushL( buffer );
    iMmsHeaders->SetSenderL( buffer->Des() );
#ifndef _NO_MMSS_LOGGING_
    // we cannot log indefinitely long strings.
    // We get this long strings only if the message is corrupted.
    TPtrC dummy;
    dummy.Set( buffer->Des().Left( KMmsMaxLogStringLength ) );
    TMmsLogger::Log( _L("- From: %S"), &dummy );
#endif
    CleanupStack::PopAndDestroy( buffer );
    }
    
// ---------------------------------------------------------
//
// ---------------------------------------------------------
void CMmsDecode::DecodeToL()
    {
    HBufC16* buffer = NULL;
    buffer = DecodeAddressL();
    CleanupStack::PushL( buffer );
    iMmsHeaders->AddTypedAddresseeL( buffer->Des(), EMmsTo );
#ifndef _NO_MMSS_LOGGING_
    // we cannot log indefinitely long strings.
    // We get this long strings only if the message is corrupted.
    TPtrC dummy;
    dummy.Set( buffer->Des().Left( KMmsMaxLogStringLength ) );
    TMmsLogger::Log( _L("- To: %S"), &dummy );
#endif
    CleanupStack::PopAndDestroy( buffer );
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
void CMmsDecode::DecodeCcL()
    {
    HBufC16* buffer = NULL;
    buffer = DecodeAddressL();
    CleanupStack::PushL( buffer );
    iMmsHeaders->AddTypedAddresseeL( buffer->Des(), EMmsCc );
#ifndef _NO_MMSS_LOGGING_
    // we cannot log indefinitely long strings.
    // We get this long strings only if the message is corrupted.
    TPtrC dummy;
    dummy.Set( buffer->Des().Left( KMmsMaxLogStringLength ) );
    TMmsLogger::Log( _L("- Cc: %S"), &dummy );
#endif
    CleanupStack::PopAndDestroy( buffer );
    }
        
// ---------------------------------------------------------
//
// ---------------------------------------------------------
void CMmsDecode::DecodeBccL()
    {
    HBufC16* buffer = NULL;
    buffer = DecodeAddressL();
    CleanupStack::PushL( buffer );
    iMmsHeaders->AddTypedAddresseeL( buffer->Des(), EMmsBcc );
#ifndef _NO_MMSS_LOGGING_
    // we cannot log indefinitely long strings.
    // We get this long strings only if the message is corrupted.
    TPtrC dummy;
    dummy.Set( buffer->Des().Left( KMmsMaxLogStringLength ) );
    TMmsLogger::Log( _L("- Bcc: %S"), &dummy );
#endif
    CleanupStack::PopAndDestroy( buffer );
    }
    
// ---------------------------------------------------------
//
// ---------------------------------------------------------
void CMmsDecode::DecodeContentLocationHeaderL()
    {
    TPtrC8 byteString;
    TUint32 size = 0;
    // Request headers are handled for testing purposes only
    // Only PDUs that we should decode are the confirmation types
    if ( iMmsHeaders->MessageType() != KMmsMessageTypeMBoxDeleteConf &&
        iMmsHeaders->MessageType() != KMmsMessageTypeDeleteConf )
        {
        byteString.Set( GetByteString() );
        // for the MMBox view type PDUs the content locations go into the array
        if ( iMmsHeaders->MessageType() == KMmsMessageTypeMboxViewConf ||
             iMmsHeaders->MessageType() == KMmsMessageTypeMboxViewReq ||
             iMmsHeaders->MessageType() == KMmsMessageTypeMBoxDeleteReq ||
             iMmsHeaders->MessageType() == KMmsMessageTypeDeleteReq )
            {
            iMmsHeaders->MMBoxMessageHeadersL().ContentLocationList().AppendL(
                byteString );
            }
        else if ( iMmsHeaders->ContentLocation().Length() == 0 )
            {
            iMmsHeaders->SetContentLocationL( byteString );
            }
        else
            {
            // This is some message type that allows more than one content location
            // The only one should be M-Mbox-Delete.req.
            if ( iMmsHeaders->MMBoxMessageHeadersL().ContentLocationList().MdcaCount()
                 == 0 )
                {
                // nothing in the list, append the old one, too
                iMmsHeaders->MMBoxMessageHeadersL().ContentLocationList().AppendL(
                    iMmsHeaders->ContentLocation() );
                }
            iMmsHeaders->MMBoxMessageHeadersL().ContentLocationList().AppendL(
                byteString );
            }
        }
    else // KMmsMessageTypeMBoxDeleteConf has different format
        {
        GetValueLength();
        size = GetLongOrShortInteger(); // this is actually index
        byteString.Set( GetByteString() );
        iMmsHeaders->InsertDeleteContentLocationL( size, byteString );
#ifndef _NO_MMSS_LOGGING_
        TMmsLogger::Log( _L("- Content Location index: %d"), size );
#endif
        }
#ifndef _NO_MMSS_LOGGING_
    HBufC16* buffer = NULL;
    buffer = HBufC16::NewLC( byteString.Length() );
    buffer->Des().Copy( byteString );
    // we cannot log indefinitely long strings.
    // We get this long strings only if the message is corrupted.
    TPtrC dummy;
    dummy.Set( buffer->Des().Left( KMmsMaxLogStringLength ) );
    TMmsLogger::Log( _L("- Content Location: %S"), &dummy );
    CleanupStack::PopAndDestroy( buffer );
#endif
    }
    
// ---------------------------------------------------------
//
// ---------------------------------------------------------
void CMmsDecode::DecodeDateHeaderL()
    {
    TInt64 date;
    TUint8 byte;
    date = GetVeryLongInteger();
    if ( iError == KErrTooBig )
        {
        // we get here only if we have at least one byte to read
        iDecodeBuffer->Read(iPosition, &byte, 1);
        iPosition++;
        // skip rest of field if too long
        iPosition += byte;
        }
    if ( iPosition >= iLength )
        {
        iError = KErrCorrupt;
        return;
        }
    iMmsHeaders->SetDate( date );
#ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L("- Date:") );
    LogDateL( date );
#endif
    }
    
// ---------------------------------------------------------
//
// ---------------------------------------------------------
void CMmsDecode::DecodeDeliveryReportHeader()
    {
    TUint8 byte;
    TUint temp;
    byte = GetWellKnownFieldValueOrSkip();
    temp = byte;
    iMmsHeaders->SetDeliveryReport( temp );
#ifndef _NO_MMSS_LOGGING_
    LogYesNo( _L("- DeliveryReport:"), temp );
#endif
    }
    
// ---------------------------------------------------------
//
// ---------------------------------------------------------
void CMmsDecode::DecodeDeliveryTimeL()
    {
    TUint8 byte;
    TInt64 date;
    byte = GetRelativeOrAbsoluteTime( date );
    if ( byte == KMmsRelativeToken )
        {
        iMmsHeaders->SetDeliveryTimeInterval( I64LOW( date ) );
#ifndef _NO_MMSS_LOGGING_
        TMmsLogger::Log( _L("- Delivery Time, interval %d s"), I64LOW( date ) );
#endif
        }
    if ( byte == KMmsAbsoluteToken )
        {
        // keep as universal time, don't convert to local time
        iMmsHeaders->SetDeliveryDate( date );
#ifndef _NO_MMSS_LOGGING_
        TMmsLogger::Log( _L("- Delivery Time, absolute ") );
        LogDateL( date );
#endif
        }
    // If could not interpret field, don't set anything.
    }
    
// ---------------------------------------------------------
//
// ---------------------------------------------------------
void CMmsDecode::DecodeExpiryL()
    {
    TUint8 byte;
    TInt64 date;
    byte = GetRelativeOrAbsoluteTime( date );
    if ( byte == KMmsRelativeToken )
        {
        iMmsHeaders->SetExpiryInterval( I64LOW( date ) );
#ifndef _NO_MMSS_LOGGING_
        TMmsLogger::Log( _L("- Expiry interval: %d s"), I64LOW( date ) );
#endif
        }
    if ( byte == KMmsAbsoluteToken )
        {
        // keep as universal time, don't convert to local time
        iMmsHeaders->SetExpiryDate( date );
#ifndef _NO_MMSS_LOGGING_
        TMmsLogger::Log( _L("- Expiry, absolute:") );
        LogDateL( date );
#endif
        }
    // If could not interpret field, don't set anything.
    }
    
// ---------------------------------------------------------
//
// ---------------------------------------------------------
void CMmsDecode::DecodeMessageClass()
    {
    TUint8 byte = 0;
    TUint temp;

    iDecodeBuffer->Read(iPosition, &byte, 1);
        
    if ( byte >= KMms0x80 )
        {
        iPosition++;
        temp = byte;
        iMmsHeaders->SetMessageClass( temp );
#ifndef _NO_MMSS_LOGGING_
        TMmsLogger::Log( _L("-- MessageCalss encoded as Class-identifier") );
#endif
        }
    else    
        {
        TPtrC8 byteString = GetByteString();
#ifndef _NO_MMSS_LOGGING_
        TMmsLogger::Log( _L("-- MessageCalss encoded as Token-text") );
        // we don't leave because of logging
        TInt error = KErrNone;
        TRAP( error, 
            {
            HBufC16* buffer = NULL;
            buffer = HBufC16::NewLC( byteString.Length() );
            buffer->Des().Copy( byteString );
            TPtrC dummy;
            // we cannot log indefinitely long strings.
            // We get this long strings only if the message is corrupted.
            dummy.Set( buffer->Des().Left( KMmsMaxLogStringLength ) );
            TMmsLogger::Log( _L("- Message class : %S"), &dummy ); 
            CleanupStack::PopAndDestroy( buffer );
            });
        if ( error != KErrNone )
            {
            TMmsLogger::Log( _L("- Decode left when logging Message class, error : %d"),
                error );
            }
#endif   
         temp = KMmsTestIllegalValue;
         if ( byteString.CompareF( KMessageClassPersonal ) == 0 )
             {
             temp = KMmsMessageClassPersonal;
             }
         else if ( byteString.CompareF( KMessageClassAdvertisement ) == 0 )
             {
             temp = KMmsMessageClassAdvertisement;
             }  
         else if ( byteString.CompareF( KMessageClassInformational ) == 0 )
             {
             temp = KMmsMessageClassInformational;
             }
         else if ( byteString.CompareF( KMessageClassAuto ) == 0 )
             {
             temp = KMmsMessageClassAuto;
             }
             iMmsHeaders->SetMessageClass( temp );
         }
#ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L("- Message Class:") );
    switch ( temp )
        {
        case KMmsMessageClassPersonal:
            TMmsLogger::Log( _L("-- Personal") );
            break;
        case KMmsMessageClassAdvertisement:
            TMmsLogger::Log( _L("-- Advertisement") );
            break;
        case KMmsMessageClassInformational:
            TMmsLogger::Log( _L("-- Informational") );
            break;
        case KMmsMessageClassAuto:
            TMmsLogger::Log( _L("-- Auto") );
            break;
        default:
            TMmsLogger::Log( KLogUnknown, temp );
            break;
        }
#endif
    }
    
// ---------------------------------------------------------
//
// ---------------------------------------------------------
void CMmsDecode::DecodeMessageIdL()
    {
    TPtrC8 byteString;
    byteString.Set( GetByteString() );
    iMmsHeaders->SetMessageIdL( byteString );
#ifndef _NO_MMSS_LOGGING_
    HBufC16* buffer = NULL;
    buffer = HBufC16::NewLC( byteString.Length() );
    buffer->Des().Copy( byteString );
    // we cannot log indefinitely long strings.
    // We get this long strings only if the message is corrupted.
    TPtrC dummy;
    dummy.Set( buffer->Des().Left( KMmsMaxLogStringLength ) );
    TMmsLogger::Log( _L("- Message Id: %S"), &dummy );
    CleanupStack::PopAndDestroy( buffer );
#endif
    }
    
// ---------------------------------------------------------
//
// ---------------------------------------------------------
void CMmsDecode::DecodeMessageType()
    {
    TUint8 byte;
    TUint temp;
    byte = GetWellKnownFieldValueOrSkip();
    temp = byte;
    iMmsHeaders->SetMessageType( temp );
#ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L("- MessageType:") );
    switch ( temp )
        {
        case KMmsMessageTypeMSendReq:
            TMmsLogger::Log( _L("-- send-req") );
            break;
        case KMmsMessageTypeMSendConf:
            TMmsLogger::Log( _L("-- send-conf") );
            break;
        case KMmsMessageTypeMNotificationInd:
            TMmsLogger::Log( _L("-- notification-ind") );
            break;
        case KMmsMessageTypeMNotifyRespInd:
            TMmsLogger::Log( _L("-- notifyresp-ind") );
            break;
        case KMmsMessageTypeMRetrieveConf:
            TMmsLogger::Log( _L("-- retrieve-conf") );
            break;
        case KMmsMessageTypeAcknowledgeInd:
            TMmsLogger::Log( _L("-- acknowledge-ind") );
            break;
        case KMmsMessageTypeDeliveryInd:
            TMmsLogger::Log( _L("-- delivery-ind") );
            break;
        case KMmsMessageTypeReadRecInd:
            TMmsLogger::Log( _L("-- read-rec-ind") );
            break;
        case KMmsMessageTypeReadOrigInd:
            TMmsLogger::Log( _L("-- read-orig-ind") );
            break;
        case KMmsMessageTypeForwardReq:
            TMmsLogger::Log( _L("-- forward-req") );
            break;
        case KMmsMessageTypeForwardConf:
            TMmsLogger::Log( _L("-- forward-conf") );
            break;
        case KMmsMessageTypeMboxStoreReq:
            TMmsLogger::Log( _L("-- m-mbox-store-req") );
            break;
        case KMmsMessageTypeMboxStoreConf:
            TMmsLogger::Log( _L("-- m-mbox-store-conf") );
            break;
        case KMmsMessageTypeMboxViewReq:
            TMmsLogger::Log( _L("-- m-mbox-view-req") );
            break;
        case KMmsMessageTypeMboxViewConf:
            TMmsLogger::Log( _L("-- m-mbox-view-conf") );
            break;
        case KMmsMessageTypeMBoxUploadReq:
            TMmsLogger::Log( _L("-- m-mbox-upload-req") );
            break;
        case KMmsMessageTypeMBoxUploadConf:
            TMmsLogger::Log( _L("-- m-mbox-upload-conf") );
            break;
        case KMmsMessageTypeMBoxDeleteReq:
            TMmsLogger::Log( _L("-- m-mbox-delete-req") );
            break;
        case KMmsMessageTypeMBoxDeleteConf:
            TMmsLogger::Log( _L("-- m-mbox-delete-conf") );
            break;
        case KMmsMessageTypeMBoxDescr:
            TMmsLogger::Log( _L("-- m-mbox-descr") );
            break;
        case KMmsMessageTypeDeleteReq:
            TMmsLogger::Log( _L("-- m-delete-req") );
            break;
        case KMmsMessageTypeDeleteConf:
            TMmsLogger::Log( _L("-- m-delete-conf") );
            break;
        case KMmsMessageTypeCancelReq:
            TMmsLogger::Log( _L("-- m-cancel-req") );
            break;
        case KMmsMessageTypeCancelConf:
            TMmsLogger::Log( _L("-- m-cancel-conf") );
            break;
        default:
            TMmsLogger::Log( KLogUnknown, temp );
            break;
        }
#endif
    }
    
// ---------------------------------------------------------
//
// ---------------------------------------------------------
void CMmsDecode::DecodeMmsVersion()
    {
    TUint8 byte;
    byte = GetWellKnownFieldValueOrSkip();
    byte &= KMms0x7F;
    // if version major number is greater than 1,
    // we may not be compatible any more.
    // We save the version number, and check later
    // what we should do about it.
    iMmsHeaders->SetMmsVersion( byte );
#ifndef _NO_MMSS_LOGGING_
    // log version number always - even when decode logging is off
    TUint high = ( byte & KMms0x7F ) >> KMmsHalfByteShift;
    TUint low = byte & KMms0x0F;
    TMmsLogger::Log( _L("- MMS version: %d.%d"), high, low );
#endif
    }
    
// ---------------------------------------------------------
//
// ---------------------------------------------------------
void CMmsDecode::DecodeMessageSize()
    {
    TUint32 size = 0;
    size = GetLongOrShortInteger();
    iMmsHeaders->SetMessageSize( size );
#ifndef _NO_MMSS_LOGGING_
    if ( iError != KErrTooBig )
        {
        TMmsLogger::Log( _L("- Message Size: %d"), size );
        }
    else
        {
        TMmsLogger::Log( _L("- Message Size bigger than maxint") );
        }
#endif
    }
    
// ---------------------------------------------------------
//
// ---------------------------------------------------------
void CMmsDecode::DecodePriority()
    {
    TUint8 byte;
    byte = GetWellKnownFieldValueOrSkip();
    TUint temp;
    temp = byte;
    iMmsHeaders->SetMessagePriority( temp );
#ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L("- Priority:") );
    switch ( temp )
        {
        case KMmsPriorityLow:
            TMmsLogger::Log( _L("-- Low") );
            break;
        case KMmsPriorityNormal:
            TMmsLogger::Log( _L("-- Normal") );
            break;
        case KMmsPriorityHigh:
            TMmsLogger::Log( _L("-- High") );
            break;
        default:
            TMmsLogger::Log( KLogUnknown, temp );
            break;
        }
#endif
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
void CMmsDecode::DecodeReadReply()
    {
    TUint8 byte;
    byte = GetWellKnownFieldValueOrSkip();
    TUint temp;
    temp = byte;
    iMmsHeaders->SetReadReply( temp );
#ifndef _NO_MMSS_LOGGING_
    LogYesNo( _L("- Read Reply:"), temp );
#endif
    }
    
// ---------------------------------------------------------
//
// ---------------------------------------------------------
void CMmsDecode::DecodeReportAllowed()
    {
    TUint8 byte;
    byte = GetWellKnownFieldValueOrSkip();
    TUint temp;
    temp = byte;
    iMmsHeaders->SetReportAllowed( temp );
#ifndef _NO_MMSS_LOGGING_
    LogYesNo( _L("- Report Allowed:"), temp );
#endif
    }
    
// ---------------------------------------------------------
//
// ---------------------------------------------------------
#ifndef _NO_MMSS_LOGGING_
void CMmsDecode::DecodeResponseStatusL( TInt aHeader )
#else
void CMmsDecode::DecodeResponseStatusL( TInt /*aHeader*/ )
#endif
    {
    TUint8 byte;
    TUint temp;
    if ( iMmsHeaders->MessageType() != KMmsMessageTypeMBoxDeleteConf )
        {
        byte = GetWellKnownFieldValueOrSkip();
        temp = byte;
        iMmsHeaders->SetResponseStatus( temp );
        }
    else  // KMmsMessageTypeMBoxDeleteConf has different format
        {
        TUint32 size = 0;
        GetValueLength();
        size = GetLongOrShortInteger(); // this is actually index
        byte = GetWellKnownFieldValueOrSkip();
        temp = byte;
        iMmsHeaders->InsertDeleteStatusL( size, temp );
#ifndef _NO_MMSS_LOGGING_
        TMmsLogger::Log( _L("- Response Status Index: %d"), size );
#endif
        }
#ifndef _NO_MMSS_LOGGING_
    if ( aHeader == KMmsAssignedResponseStatus )
        {
        TMmsLogger::Log( _L("- Response Status:") );
        }
    else
        {
        TMmsLogger::Log( _L("- Retrieve Status:") );
        }
    switch ( temp )
        {
        case KMmsStatusOk:
            TMmsLogger::Log( _L("-- Ok") );
            break;
        case KMmsErrorUnspecified:
            TMmsLogger::Log( _L("-- ErrUnspecified") );
            break;
        case KMmsErrorServiceDenied:
            TMmsLogger::Log( _L("-- ErrServiceDenied") );
            break;
        case KMmsErrorMessageFormatCorrupt:
            TMmsLogger::Log( _L("-- ErrMessageFormatCorrupt") );
            break;
        case KMmsErrorSendingAddressUnresolved:
            TMmsLogger::Log( _L("-- ErrSendingAddressUnresolved") );
            break;
        case KMmsErrorMessageNotFound:
            TMmsLogger::Log( _L("-- ErrMessageNotFound") );
            break;
        case KMmsErrorNetworkProblem:
            TMmsLogger::Log( _L("-- ErrNetworkProblem") );
            break;
        case KMmsErrorNoContentAccepted:
            TMmsLogger::Log( _L("-- ErrNoContentAccepted") );
            break;
        case KMmsErrorUnsupportedMessage:
            TMmsLogger::Log( _L("-- ErrUnsupportedMessage") );
            break;
        default:
            if ( ( temp & KMmsErrorRangeMask ) == KMmsErrorTransient )
                {
                TMmsLogger::Log( _L("-- Transient error %d"), temp );
                }
            else if ( ( temp & KMmsErrorRangeMask ) == KMmsErrorPermanent )
                {
                TMmsLogger::Log( _L("-- Permanent error %d"), temp );
                }
            else
                {
                TMmsLogger::Log( KLogUnknown, temp );
                }
            break;
         }   
#endif
    }
    
// ---------------------------------------------------------
//
// ---------------------------------------------------------
void CMmsDecode::DecodeResponseTextL( TInt aHeader )
    {
    // The text may be response status text, retrieve status text
    // or status text depending on which PDU we are decoding.
    // Only one text string appears in each case, so we reuse the header
    HBufC16* buffer = NULL;
    if ( iMmsHeaders->MessageType() != KMmsMessageTypeMBoxDeleteConf )
        {
        // both notes go to the same variable, as both can never be present
        buffer = GetEncodedTextStringL();
        CleanupStack::PushL( buffer );
        iMmsHeaders->SetResponseTextL( buffer->Des() );
        }
    else // KMmsMessageTypeMBoxDeleteConf has different format
        {
        GetValueLength();
        TUint32 index = 0;
        index = GetLongOrShortInteger();
        buffer = GetEncodedTextStringL();
        CleanupStack::PushL( buffer );
        iMmsHeaders->InsertDeleteResponseTextL( index, buffer->Des() );
#ifndef _NO_MMSS_LOGGING_
        TMmsLogger::Log( _L("- Response Text Index: %d"), index );
#endif
        }
#ifndef _NO_MMSS_LOGGING_
    switch ( aHeader )
        {
        case KMmsAssignedResponseText:
            TMmsLogger::Log( _L("- Response text:") );
            break;           
        case KMmsAssignedRetrieveText:
            TMmsLogger::Log( _L("- Retrieve text:") );
            break;           
        case KMmsAssignedStatusText:
            TMmsLogger::Log( _L("- Status text:") );
            break;           
        }
    // we cannot log indefinitely long strings.
    // We get this long strings only if the message is corrupted.
    TPtrC dummy;
    dummy.Set( buffer->Des().Left( KMmsMaxLogStringLength ) );
    TMmsLogger::Log( _L("%S"), &dummy );
#endif
    CleanupStack::PopAndDestroy( buffer );
    }
    
// ---------------------------------------------------------
//
// ---------------------------------------------------------
void CMmsDecode::DecodeSenderVisibility()
    {
    TUint8 byte;
    byte = GetWellKnownFieldValueOrSkip();
    TUint temp;
    temp = byte;
    iMmsHeaders->SetSenderVisibility( temp );
#ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L("- Sender Visibility:") );
    switch ( temp )
        {
        case KMmsSenderHide:
            TMmsLogger::Log( _L("-- Hide") );
            break;
        case KMmsSenderShow:
            TMmsLogger::Log( _L("-- Show") );
            break;
        default:
            TMmsLogger::Log( KLogUnknown, temp );
            break;
        }
#endif
    }
    
// ---------------------------------------------------------
//
// ---------------------------------------------------------
void CMmsDecode::DecodeStatus()
    {
    TUint8 byte;
    byte = GetWellKnownFieldValueOrSkip();
    TUint temp;
    temp = byte;
    iMmsHeaders->SetStatus( temp );
#ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L("- Status:") );
    switch ( temp )
        {
        case KMmsMessageStatusExpired:
            TMmsLogger::Log( _L("-- Expired") );
            break;
        case KMmsMessageStatusRetrieved:
            TMmsLogger::Log( _L("-- Retrieved") );
            break;
        case KMmsMessageStatusRejected:
            TMmsLogger::Log( _L("-- Rejected") );
            break;
        case KMmsMessageStatusDeferred:
            TMmsLogger::Log( _L("-- Deferred") );
            break;
        case KMmsMessageStatusUnrecognized:
            TMmsLogger::Log( _L("-- Unrecognized") );
            break;
        case KMmsMessageStatusIndeterminate:
            TMmsLogger::Log( _L("-- Indeterminate") );
            break;
        case KMmsMessageStatusForwarded:
            TMmsLogger::Log( _L("-- Forwarded") );
            break;
        case KMmsMessageStatusUnreachable:
            TMmsLogger::Log( _L("-- Unreachable") );
            break;
        default:
            TMmsLogger::Log( KLogUnknown, temp );
            break;
        }
#endif
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
void CMmsDecode::DecodeSubjectL()
    {
    HBufC16* buffer = NULL;
    buffer = GetEncodedTextStringL();
    CleanupStack::PushL( buffer );
    // TMsvEntry.iDescription will be updated separately.
    iMmsHeaders->SetSubjectL( buffer->Des());
#ifndef _NO_MMSS_LOGGING_
    // we cannot log indefinitely long strings.
    // We get this long strings only if the message is corrupted.
    TPtrC dummy;
    dummy.Set( buffer->Des().Left( KMmsMaxLogStringLength ) );
    TMmsLogger::Log( _L("- Subject: %S"), &dummy );
#endif
    CleanupStack::PopAndDestroy( buffer );
    }
    
// ---------------------------------------------------------
//
// ---------------------------------------------------------
void CMmsDecode::DecodeTidL()
    {
    TPtrC8 byteString;
    byteString.Set( GetByteString() );
    iMmsHeaders->SetTidL( byteString );
#ifndef _NO_MMSS_LOGGING_
    HBufC16* buffer = NULL;
    buffer = HBufC16::NewLC( byteString.Length() );
    buffer->Des().Copy( byteString );
    TPtrC dummy;
    dummy.Set( buffer->Des() );
    // we cannot log indefinitely long strings.
    // We get this long strings only if the message is corrupted.
    dummy.Set( buffer->Des().Left( KMmsMaxLogStringLength ) );
    TMmsLogger::Log( _L("- Transaction Id: %S"), &dummy );
    CleanupStack::PopAndDestroy( buffer );
#endif
    }
    
// ---------------------------------------------------------
//
// ---------------------------------------------------------
void CMmsDecode::DecodeContentTypeL()
    {
    // Now we must analyze the content type in order to
    // handle the body correctly.
    // The body may be a multipart, or a single part.
    // The subroutine will set the number of Attachments.
    // For a monoblock body the number of attachments will be 1.
    iMultipartType = GetMultipartContentTypeL();
    // save the multipart type in case we must later make decisions
    // about the multipart/alternative type
    iMmsHeaders->SetMultipartType( iMultipartType );
    iLastHeader = ETrue; // Content type is always the last header
#ifndef _NO_MMSS_LOGGING_
    TPtrC8 byteString;
    HBufC16* buffer = NULL;
    TPtrC dummy;
    if ( iMultipartType != 0 )
        {
        if ( iMultipartType <= KNumberContentTypes )
            {
            byteString.Set( KContentTypeTable[ iMultipartType ] );
            }
        else if ( iMultipartType == KMmsAssignedApplicationVndWapMultipartReport )
            {
            byteString.Set( KMmsWapMultipartReport );
            }
        else
            {
            byteString.Set( KContentTypeTable[ KNumberContentTypes ] ); 
            }
        buffer = HBufC16::NewLC( byteString.Length() );
        // we cannot log indefinitely long strings.
        // We get this long strings only if the message is corrupted.
        buffer->Des().Copy( byteString );
        // no entries from our table are more than 200 bytes long.
        dummy.Set( buffer->Des() );
        TMmsLogger::Log( _L("- Content Type: %S"), &dummy );
        CleanupStack::PopAndDestroy( buffer );
        if ( iMultipartRootType.Length() > 0 )
            {
            buffer = HBufC16::NewLC( iMultipartRootType.Length() );
            buffer->Des().Copy( iMultipartRootType );
            // we cannot log indefinitely long strings.
            // We get this long strings only if the message is corrupted.
            dummy.Set( buffer->Des().Left( KMmsMaxLogStringLength ) );
            TMmsLogger::Log( _L("- Root content type: %S"), &dummy );
            CleanupStack::PopAndDestroy( buffer );
            }
        if ( iRootContentId.Length() > 0 )
            {
            buffer = HBufC16::NewLC( iRootContentId.Length() );
            buffer->Des().Copy( iRootContentId );
            // we cannot log indefinitely long strings.
            // We get this long strings only if the message is corrupted.
            dummy.Set( buffer->Des().Left( KMmsMaxLogStringLength ) );
            TMmsLogger::Log( _L("- Root content id: %S"), &dummy );
            CleanupStack::PopAndDestroy( buffer );
            }
        }
    TMmsLogger::Log( _L("- Number of attachments: %d"), iNumberOfAttachments );
#endif
    }
    
// ---------------------------------------------------------
//
// ---------------------------------------------------------
void CMmsDecode::DecodeReadStatus()
    {
    TUint8 byte;
    byte = GetWellKnownFieldValueOrSkip();
    TUint temp;
    temp = byte;
    iMmsHeaders->SetReadStatus( temp );
#ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L("- ReadStatus:") );
    switch ( temp )
        {
        case KMmsReadStatusRead:
            TMmsLogger::Log( _L("-- Read") );
            break;
        case KMmsReadStatusDeletedWithoutBeingRead:
            TMmsLogger::Log( _L("-- Deleted unread") );
            break;
        default:
            TMmsLogger::Log( KLogUnknown, temp );
            break;
        }
#endif
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
void CMmsDecode::DecodeReplyCharging()
    {
    TUint8 byte;
    byte = GetWellKnownFieldValueOrSkip();
    TUint temp;
    temp = byte;
    iMmsHeaders->SetReplyCharging( temp );
#ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L("- ReplyCharging:") );
    switch ( temp )
        {
        case KMmsReplyChargingRequested:
            TMmsLogger::Log( _L("-- Requested") );
            break;
        case KMmsReplyChargingRequestedTextOnly:
            TMmsLogger::Log( _L("-- Requested text only") );
            break;
        case KMmsReplyChargingAccepted:
            TMmsLogger::Log( _L("-- Accepted") );
            break;
        case KMmsReplyChargingAcceptedTextOnly:
            TMmsLogger::Log( _L("-- Accepted text only") );
            break;
        default:
            TMmsLogger::Log( KLogUnknown, temp );
            break;
        }
#endif
    }
    
// ---------------------------------------------------------
//
// ---------------------------------------------------------
void CMmsDecode::DecodeReplyChargingDeadlineL()
    {
    TUint8 byte;
    TInt64 date;
    byte = GetRelativeOrAbsoluteTime( date );
    if ( byte == KMmsRelativeToken )
        {
        iMmsHeaders->SetReplyChargingInterval( I64LOW( date ) );
#ifndef _NO_MMSS_LOGGING_
        TMmsLogger::Log( _L("- Reply charging interval: %d s"), I64LOW( date ) );
#endif
        }
    if ( byte == KMmsAbsoluteToken )
        {
        // keep as universal time, don't convert to local time
        iMmsHeaders->SetReplyChargingDate( date );
#ifndef _NO_MMSS_LOGGING_
        TMmsLogger::Log( _L("- Reply charging date, absolute:") );
        LogDateL( date );
#endif
        }
    }
    
// ---------------------------------------------------------
//
// ---------------------------------------------------------
void CMmsDecode::DecodeReplyChargingIdL()
    {
    TPtrC8 byteString;
    byteString.Set( GetByteString() );
    iMmsHeaders->SetReplyChargingIdL( byteString );
#ifndef _NO_MMSS_LOGGING_
    HBufC16* buffer = NULL;
    buffer = HBufC16::NewLC( byteString.Length() );
    buffer->Des().Copy( byteString );
    // we cannot log indefinitely long strings.
    // We get this long strings only if the message is corrupted.
    TPtrC dummy;
    dummy.Set( buffer->Des().Left( KMmsMaxLogStringLength ) );
    TMmsLogger::Log( _L("- Reply Charging Id: %S"), &dummy );
    CleanupStack::PopAndDestroy( buffer );
#endif
    }
    
// ---------------------------------------------------------
//
// ---------------------------------------------------------
void CMmsDecode::DecodeReplyChargingSize()
    {
    TUint32 size = 0;
    size = GetLongOrShortInteger();
    iMmsHeaders->SetReplyChargingSize( size );
#ifndef _NO_MMSS_LOGGING_
    if ( iError != KErrTooBig )
        {
        TMmsLogger::Log( _L("- Reply charging Size: %d"), size );
        }
    else
        {
        TMmsLogger::Log( _L("- Reply charging Size bigger than: %d"),
            TInt64 (0xffffffff) );
        }
#endif
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
void CMmsDecode::DecodePreviousSenderL()
    {
    GetValueLength(); // we don't need this
    TUint32 size = 0;
    size = GetLongOrShortInteger(); // This is actually index
    HBufC16* buffer = NULL;
    buffer = DecodeAddressL();
    CleanupStack::PushL( buffer );
    // If message is corrupted, the result may be garbage, but we do our best
    iMmsHeaders->InsertPreviouslySentByL( size, buffer->Des() );
#ifndef _NO_MMSS_LOGGING_
    // we cannot log indefinitely long strings.
    // We get this long strings only if the message is corrupted.
    TPtrC dummy;
    dummy.Set( buffer->Des().Left( KMmsMaxLogStringLength ) );
    TMmsLogger::Log( _L("- Prevously sent by: %S, order # %d"), &dummy, size );
#endif
    CleanupStack::PopAndDestroy( buffer );
    }
    
// ---------------------------------------------------------
//
// ---------------------------------------------------------
void CMmsDecode::DecodePreviouslySentDateL()
    {
    GetValueLength(); // we don't need this
    TUint32 size = 0;
    size = GetLongOrShortInteger();
    TInt64 date;
    date = GetVeryLongInteger();
    TUint8 byte;
    if ( iError == KErrTooBig )
        {
        // we get here only if we have at least one byte to read
        iDecodeBuffer->Read(iPosition, &byte, 1);
        iPosition++;
        // skip rest of field if too long
        iPosition += byte;
        // If this is too big, we just ignore it. It is no legal date.
        date = 0;
        iError = KErrNone;
        }
    // We insert the result no matter what
    iMmsHeaders->InsertPreviouslySentDateL( size, date );
#ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L("- Previously sent date: (order # %d)"), size );
    LogDateL( date );
#endif
    }
    
// ---------------------------------------------------------
//
// ---------------------------------------------------------
void CMmsDecode::DecodeStoreHeaderL()
    {
    TUint8 byte;
    byte = GetWellKnownFieldValueOrSkip();
    TUint temp;
    temp = byte;
    iMmsHeaders->MMBoxMessageHeadersL().SetMmsStore( temp );
#ifndef _NO_MMSS_LOGGING_
    LogYesNo( _L("- Store to MMBox:"), temp );
#endif
    }
    
// ---------------------------------------------------------
//
// ---------------------------------------------------------
void CMmsDecode::DecodeMMBoxStateL()
    {
    TUint8 byte;
    byte = GetWellKnownFieldValueOrSkip();
    TUint temp;
    temp = byte;
    if ( iMmsHeaders->MessageType() != KMmsMessageTypeMboxViewReq &&
        iMmsHeaders->MessageType() != KMmsMessageTypeMboxViewConf )
        {
        iMmsHeaders->MMBoxMessageHeadersL().SetMMState( temp );
        }
    else
        {
        // For the MMBox view PDU types we must put the states into
        // an array as they are used for filtering, and there may be
        // more than one state headers
        iMmsHeaders->MMBoxViewHeadersL().MMStateArray().InsertInOrder( temp );
        }
#ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L("- State in MMBox:") );
    switch ( temp )
        {
        case KMmsDraft:
            TMmsLogger::Log( _L("-- Draft") );
            break;
        case KMmsSent:
            TMmsLogger::Log( _L("-- Sent") );
            break;
        case KMmsNew:
            TMmsLogger::Log( _L("-- New") );
            break;
        case KMmsRetrieved:
            TMmsLogger::Log( _L("-- Retrieved") );
            break;
        case KMmsForwarded:
            TMmsLogger::Log( _L("-- Forwarded") );
            break;
        default:
            TMmsLogger::Log( KLogUnknown, temp );
            break;
        }
#endif
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
void CMmsDecode::DecodeMMBoxStoreStatusL()
    {
    TUint8 byte;
    byte = GetWellKnownFieldValueOrSkip();
    TUint temp;
    temp = byte;
    iMmsHeaders->MMBoxMessageHeadersL().SetMmsStoreStatus( temp );
#ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L("- MMBox Store Status:") );
    switch ( temp )
        {
        case KMmsStatusOk:
            TMmsLogger::Log( _L("-- Ok") );
            break;
        default:
            if ( ( temp & KMmsErrorRangeMask ) == KMmsErrorTransient )
                {
                TMmsLogger::Log( _L("-- Transient error %d"), temp );
                }
            else if ( ( temp & KMmsErrorRangeMask ) == KMmsErrorPermanent )
                {
                TMmsLogger::Log( _L("-- Permanent error %d"), temp );
                }
            else
                {
                TMmsLogger::Log( KLogUnknown, temp );
                }
            break;
        }
#endif
    }
    
// ---------------------------------------------------------
//
// ---------------------------------------------------------
void CMmsDecode::DecodeMMBoxStoreStatusTextL()
    {
    HBufC16* buffer = NULL;
    buffer = GetEncodedTextStringL();
    CleanupStack::PushL( buffer );
    iMmsHeaders->MMBoxMessageHeadersL().SetMmsStoreStatusTextL( buffer->Des() );
#ifndef _NO_MMSS_LOGGING_
    // we cannot log indefinitely long strings.
    // We get this long strings only if the message is corrupted.
    TPtrC dummy;
    dummy.Set( buffer->Des().Left( KMmsMaxLogStringLength ) );
    TMmsLogger::Log( _L("- MMBox Store Status Text: %S"), &dummy );
#endif
    CleanupStack::PopAndDestroy( buffer );
    }
    
// ---------------------------------------------------------
//
// ---------------------------------------------------------
void CMmsDecode::DecodeStoredInMMBoxHeaderL()
    {
    TUint8 byte;
    byte = GetWellKnownFieldValueOrSkip();
    TUint temp;
    temp = byte;
    iMmsHeaders->MMBoxMessageHeadersL().SetMmsStored( temp );
#ifndef _NO_MMSS_LOGGING_
    LogYesNo( _L("- Stored to MMBox:"), temp );
#endif
    }
    
// ---------------------------------------------------------
//
// ---------------------------------------------------------
void CMmsDecode::DecodeAttributesHeaderL()
    {
    TUint8 byte;
    byte = GetWellKnownFieldValueOrSkip();
    TUint temp;
    temp = byte;
    User::LeaveIfError(iMmsHeaders->MMBoxViewHeadersL().AttributeArray().Append( temp ));
#ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L("- MMBox Attribute: %d"), temp );
#endif
    }
    
// ---------------------------------------------------------
//
// ---------------------------------------------------------
void CMmsDecode::DecodeTotalsL()
    {
    TUint8 byte;
    byte = GetWellKnownFieldValueOrSkip();
    TUint temp;
    temp = byte;
    iMmsHeaders->MMBoxViewHeadersL().SetMmsTotals( temp );
#ifndef _NO_MMSS_LOGGING_
    LogYesNo( _L("- MMBox Totals requested:"), temp );
#endif
    }
    
// ---------------------------------------------------------
//
// ---------------------------------------------------------
void CMmsDecode::DecodeMboxTotalsL()
    {
    GetValueLength(); // we don't need this
    // get message quota or size quota
    TUint8 byte;
    byte = GetWellKnownFieldValueOrSkip();
    // actual value of the quota
    // We only get normal integer (not a 64 bit integer) as a 32 bit integer 
    // will give quota 4294967295 bytes or messages (which should be plenty)
    TUint32 size = 0;
    size = GetLongOrShortInteger();
    // save message quota
    if ( byte == KMmsMessageCountToken )
        {
        iMmsHeaders->MMBoxViewHeadersL().SetMMBoxTotalNumber( size );
#ifndef _NO_MMSS_LOGGING_
        TMmsLogger::Log( _L("- MMBox total: %d messages"), size );
#endif
        }
    else if ( byte == KMmsMessageSizeToken )
        {
        iMmsHeaders->MMBoxViewHeadersL().SetMMBoxTotalSize( size );
#ifndef _NO_MMSS_LOGGING_
        TMmsLogger::Log( _L("- MMBox total: %d bytes"), size );
#endif
        }
    // illegal token value is ignored.
    else
        {
#ifndef _NO_MMSS_LOGGING_
        TUint temp;
        temp = byte;
        TMmsLogger::Log( _L("- MMBox total: illegal token %d"), temp );
#endif
        }
    }
          
// ---------------------------------------------------------
//
// ---------------------------------------------------------
void CMmsDecode::DecodeQuotaHeaderL()
    {
    TUint8 byte;
    byte = GetWellKnownFieldValueOrSkip();
    TUint temp;
    temp = byte;
    iMmsHeaders->MMBoxViewHeadersL().SetMmsQuotas( temp );
#ifndef _NO_MMSS_LOGGING_
        LogYesNo( _L("- MMBox Quotas requested:"), temp );
#endif
    }
    
// ---------------------------------------------------------
//
// ---------------------------------------------------------
void CMmsDecode::DecodeMBoxQuotasL()
    {
    TUint8 byte;
    GetValueLength(); // we don't need this
    // get message quota or size quota
    byte = GetWellKnownFieldValueOrSkip();
    // actual value of the quota
    // We only get normal integer (not a 64 bit integer) as a 32 bit integer 
    // will give quota 4294967295 bytes or messages (which should be plenty)
    TUint32 size = 0;
    size = GetLongOrShortInteger();
    // save message quota
    if ( byte == KMmsMessageCountToken )
        {
        iMmsHeaders->MMBoxViewHeadersL().SetMMBoxQuotaNumber( size );
#ifndef _NO_MMSS_LOGGING_
        TMmsLogger::Log( _L("- MMBox quota: %d messages"), size );
#endif
        }
    else if ( byte == KMmsMessageSizeToken )
        {
        iMmsHeaders->MMBoxViewHeadersL().SetMMBoxQuotaSize( size );
#ifndef _NO_MMSS_LOGGING_
        TMmsLogger::Log( _L("- MMBox quota: %d bytes"), size );
#endif
        }
    // illegal token value is ignored.
    else
        {
#ifndef _NO_MMSS_LOGGING_
        TUint temp;
        temp = byte;
        TMmsLogger::Log( _L("- MMBox quota: illegal token %d"), temp );
#endif
        }
    }
    
// ---------------------------------------------------------
//
// ---------------------------------------------------------
void CMmsDecode::DecodeMessageCountL()
    {
    TUint32 size = 0;
    size = GetLongOrShortInteger();
    iMmsHeaders->MMBoxViewHeadersL().SetMmsMessageCount( size );
#ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L("- Message count: %d"), size );
#endif
    }
    
// ---------------------------------------------------------
//
// ---------------------------------------------------------
void CMmsDecode::DecodeStartInMMBoxViewL()
    {
    TUint32 size = 0;
    size = GetLongOrShortInteger();
    iMmsHeaders->MMBoxViewHeadersL().SetMmsStart( size );
#ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L("- Start of messages: %d"), size );
#endif
    }


// ---------------------------------------------------------
//
// ---------------------------------------------------------
void CMmsDecode::DecodeDistributionIndicator()
    {
    TUint8 byte;
    byte = GetWellKnownFieldValueOrSkip();
    TUint temp;
    temp = byte;
    iMmsHeaders->SetDistributionIndicator( temp );
#ifndef _NO_MMSS_LOGGING_
    LogYesNo( _L("- May be distributed further:"), temp );
#endif
    }
    
// ---------------------------------------------------------
//
// ---------------------------------------------------------
void CMmsDecode::DecodeElementDescriptorL()
    {
#ifndef _NO_MMSS_LOGGING_
    TPtrC dummy;
#endif
    TUint32 size = 0;
    // remember where we are
    size = GetValueLength();
    if ( size > iLength - iPosition )
        {
        // If the message is corrupted and indicates incorrect length,
        // make sure we don't read beyond the buffer
        // We don't declare that the message is corrupted because of this.
        // We just read what we can, we don't use this header anyway.
        // If there is data following a corrupted header, it will be lost.
        size = iLength - iPosition;
        }
    TUint endPosition = iPosition + size;
    HBufC16* buffer = NULL;
    // coverity[size_error][buffer_alloc]
	buffer = GetSimpleTextStringL(); // I hope this is correct.
    CleanupStack::PushL( buffer );
    iMmsHeaders->ElementDescriptorL().SetContentReferenceL( buffer->Des() );
#ifndef _NO_MMSS_LOGGING_
    dummy.Set( buffer->Des().Left( KMmsMaxLogStringLength ) );
    TMmsLogger::Log( _L("- Content reference: %S"), &dummy );
#endif
    CleanupStack::PopAndDestroy( buffer );
    buffer = NULL;
    TUint8 byte;
    while ( iPosition < endPosition )
        {
        byte = GetWellKnownFieldValueOrSkip();
        if ( byte != KMmsAssignedTopLevelContentType )
            {
            // Unknown parameter, ignore
            SkipFieldValue();
            }
        else
            {
            // the value of the parameter is content-type of the top-level message content
            TPtrC8 contType = GetContentTypeL();
            iMmsHeaders->ElementDescriptorL().SetContentTypeL( contType );
#ifndef _NO_MMSS_LOGGING_
            buffer = HBufC16::NewL( contType.Length() );
            CleanupStack::PushL( buffer );
            buffer->Des().Copy( contType );
            dummy.Set( buffer->Des().Left( KMmsMaxLogStringLength ) );
            TMmsLogger::Log( _L("-- Top level content type : %S"), &dummy );
            CleanupStack::PopAndDestroy( buffer );
            buffer = NULL;
#endif
            }
        }
    iPosition = endPosition;
    }
    
// ---------------------------------------------------------
//
// ---------------------------------------------------------
void CMmsDecode::DecodeMessageLimitL()
    {
    TUint32 size = 0;
    size = GetLongOrShortInteger();
    iMmsHeaders->MMBoxViewHeadersL().SetMmsLimit( size );
#ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L("- Max number of messages to list: %d"), size );
#endif
    }
   
// ---------------------------------------------------------
//
// ---------------------------------------------------------
void CMmsDecode::DecodeExtNotifTextL()
    {
    TPtrC8 byteString;
    HBufC16* buffer = NULL;
    byteString.Set( GetUtf8String() );
    // There will be at most one unicode character per one utf-8 character
	// coverity[incorrect_multiplication][buffer_alloc]
    buffer = HBufC16::NewL( byteString.Length() * KMms2 );
    TPtr16 pointer16 = buffer->Des();
    CleanupStack::PushL( buffer );
    // We convert what we can and forget the rest.
    CnvUtfConverter::ConvertToUnicodeFromUtf8( pointer16, byteString );
    iMmsHeaders->SetExtendedNotificationL( pointer16 );
#ifndef _NO_MMSS_LOGGING_
    // we cannot log indefinitely long strings.
    // We get this long strings only if the message is corrupted.
    TPtrC dummy;
    dummy.Set( buffer->Des().Left( KMmsMaxLogStringLength ) );
    TMmsLogger::Log( _L("- Extend. notif. txt: %S"), &dummy );
#endif
    CleanupStack::PopAndDestroy( buffer );
    }
   
// ---------------------------------------------------------
//
// ---------------------------------------------------------
void CMmsDecode::DecodeExtNotifEolL()
    {
    HBufC16* buffer = NULL;
	// coverity[size_error][buffer_alloc]
	buffer = GetSimpleTextStringL();
    CleanupStack::PushL( buffer );
    iMmsHeaders->SetMessageComplete( ( buffer->Des() )[0] );
#ifndef _NO_MMSS_LOGGING_
    TUint temp;
    temp = ( buffer->Des() )[0];
    TMmsLogger::Log( _L("- End of ext. notif: 0x%02X"), temp );
#endif
    CleanupStack::PopAndDestroy( buffer );
    
    }
    
// ---------------------------------------------------------
//
// ---------------------------------------------------------
void CMmsDecode::DecodeContentClass()
    {
    TUint8 byte;            // for simple reads
    TUint temp;
    byte = GetWellKnownFieldValueOrSkip();
    temp = byte;
    iMmsHeaders->SetContentClass( temp );
#ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L("- Content Class:") );
    switch ( temp )
        {
        case KMmsContentClassText:
            TMmsLogger::Log( _L("-- Text") );
            break;
        case KMmsContentClassImageBasic:
            TMmsLogger::Log( _L("-- Image Basic") );
            break;
        case KMmsContentClassImageRich:
            TMmsLogger::Log( _L("-- Image Rich") );
            break;
        case KMmsContentClassVideoBasic:
            TMmsLogger::Log( _L("-- Video Basic") );
            break;
        case KMmsContentClassVideoRich:
            TMmsLogger::Log( _L("-- Video Rich") );
            break;
        case KMmsContentClassMegaPixel:
            TMmsLogger::Log( _L("-- Megapixel") );
            break;
        case KMmsContentClassContentBasic:
            TMmsLogger::Log( _L("-- Content Basic") );
            break;
        case KMmsContentClassContentRich:
            TMmsLogger::Log( _L("-- Content Rich") );
            break;
        default:
            TMmsLogger::Log( _L("-- Unknown %d"), temp );
            break;
        }
#endif
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CMmsDecode::DecodeDrmContentHeader()
    {
    TUint8 byte;            // for simple reads
    TUint temp;
    byte = GetWellKnownFieldValueOrSkip();
    temp = byte;
    iMmsHeaders->SetDrmContent( temp );
#ifndef _NO_MMSS_LOGGING_
    LogYesNo( _L("- DRM Content:"), temp );
#endif
    }
    
// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CMmsDecode::DecodeAdaptationAllowed()
    {
    TUint8 byte;            // for simple reads
    TUint temp;
    byte = GetWellKnownFieldValueOrSkip();
    temp = byte;
    iMmsHeaders->SetAdaptationAllowed( temp );
#ifndef _NO_MMSS_LOGGING_
    LogYesNo( _L("- Adaptation allowed:"), temp );
#endif
    }
    
// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CMmsDecode::DecodeApplicationIdL()
    {
    HBufC16* buffer = NULL;
	
	// coverity[size_error][buffer_alloc]
    buffer = GetSimpleTextStringL();
    CleanupStack::PushL( buffer );
    iMmsHeaders->SetApplicIdL( buffer->Des() );
#ifndef _NO_MMSS_LOGGING_
    TPtrC dummy;
    dummy.Set( buffer->Des().Left( KMmsMaxLogStringLength ) );
    TMmsLogger::Log( _L("- Application-ID: %S"), &dummy );
#endif
    CleanupStack::PopAndDestroy( buffer );
    }
    
// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CMmsDecode::DecodeReplyApplicationIdL()
    {
    HBufC16* buffer = NULL;
	
	// coverity[size_error][buffer_alloc]
	buffer = GetSimpleTextStringL();
    CleanupStack::PushL( buffer );
    iMmsHeaders->SetReplyApplicIdL( buffer->Des() );
#ifndef _NO_MMSS_LOGGING_
    TPtrC dummy;
    dummy.Set( buffer->Des().Left( KMmsMaxLogStringLength ) );
    TMmsLogger::Log( _L("- Reply-To-Application-ID: %S"), &dummy );
#endif
    CleanupStack::PopAndDestroy( buffer );
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CMmsDecode::DecodeApplicationInfoL()
    {
    TPtrC8 byteString;
    byteString.Set( GetByteString() );
    iMmsHeaders->SetAuxApplicInfoL( byteString );
#ifndef _NO_MMSS_LOGGING_
    TPtrC dummy;
    HBufC16* buffer = NULL;
    buffer = HBufC16::NewLC( byteString.Length() );
    buffer->Des().Copy( byteString );
    dummy.Set( buffer->Des().Left( KMmsMaxLogStringLength ) );
    TMmsLogger::Log( _L("- Aux-Applic-Info: %S"), &dummy );
    CleanupStack::PopAndDestroy( buffer );
#endif
    }
    
// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CMmsDecode::DecodeRecommendedRetrievalMode()
    {
    TUint8 byte;
    TUint temp;
    byte = GetWellKnownFieldValueOrSkip();
    temp = byte;
    iMmsHeaders->SetRecommendedRetrievalMode( temp );
#ifndef _NO_MMSS_LOGGING_
    if ( temp == KMmsRecommendedRetrievalModeManual )
        {
        TMmsLogger::Log( _L("- Recommended retrieval mode: Manual") );
        }
    else
        {
        TMmsLogger::Log( _L("- Recommended retrieval mode unknown %d"), temp );
        }
#endif
    }
    
// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CMmsDecode::DecodeRecommendedRetrievalModeTextL()
    {
    HBufC16* buffer = NULL;
    buffer = GetEncodedTextStringL();
    CleanupStack::PushL( buffer );
    iMmsHeaders->SetRecommendedRetrievalModeTextL( buffer->Des() );
#ifndef _NO_MMSS_LOGGING_
    // we cannot log indefinitely long strings.
    // We get this long strings only if the message is corrupted.
    TPtrC dummy;
    dummy.Set( buffer->Des().Left( KMmsMaxLogStringLength ) );
    TMmsLogger::Log( _L("- Response text: %S"), &dummy );
#endif
    CleanupStack::PopAndDestroy( buffer );
    }
    
// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CMmsDecode::DecodeCancelReplaceIdL( TInt aHeader )
    {
    TPtrC8 byteString;
    byteString.Set( GetByteString() );
    iMmsHeaders->SetReplaceCancelIdL( byteString );
#ifndef _NO_MMSS_LOGGING_
    HBufC16* buffer = NULL;
    buffer = HBufC16::NewLC( byteString.Length() );
    buffer->Des().Copy( byteString );
    // we cannot log indefinitely long strings.
    // We get this long strings only if the message is corrupted.
    TPtrC dummy;
    dummy.Set( buffer->Des().Left( KMmsMaxLogStringLength ) );
    if ( aHeader == KMmsAssignedReplaceId )
        {
        TMmsLogger::Log( _L("- Replace Id: %S"), &dummy );
        }
    if ( aHeader == KMmsAssignedCancelId )
        {
        TMmsLogger::Log( _L("- Cancel Id: %S"), &dummy );
        }
    CleanupStack::PopAndDestroy( buffer );
#endif
    }
    
// ---------------------------------------------------------
//
// ---------------------------------------------------------
void CMmsDecode::DecodeCancelStatus()
    {
    TUint8 byte;
    byte = GetWellKnownFieldValueOrSkip();
    TUint temp;
    temp = byte;
    iMmsHeaders->SetCancelStatus( temp );
#ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L("- Cancel Status:") );
    switch ( temp )
        {
        case KMmsCancelRequestSuccessfullyReceived:
            TMmsLogger::Log( _L("-- successful") );
            break;
        case KMmsCancelRequestCorrupted:
            TMmsLogger::Log( _L("-- corrupted") );
            break;
        default:
            TMmsLogger::Log( KLogUnknown, temp );
            break;
        }
#endif
    }
    
#ifndef _NO_MMSS_LOGGING_
// ---------------------------------------------------------
//
// ---------------------------------------------------------
void CMmsDecode::LogDateL( const TInt64& aDate )
    {
    TBuf<KMmsDateFormatLength> dateString;

    TMmsLogger::Log( _L("%d seconds from 1.1.1970 (UTC)"), aDate );
    TTime time = TTime( KMmsYear1970String ) +
        TTimeIntervalMicroSeconds( aDate * KMmsMicroToSeconds ); 
    time.FormatL(dateString,(_L("%*E%*D%X%*N%Y %1 %2 '%3")));
    TMmsLogger::Log( _L("date %S"), &dateString );
    time.FormatL(dateString,(_L("%-B%:0%J%:1%T%:2%S%:3%+B")));
    TMmsLogger::Log( _L("time %S"), &dateString );
    }
#endif


// ================= OTHER EXPORTED FUNCTIONS ==============

//  End of File  

