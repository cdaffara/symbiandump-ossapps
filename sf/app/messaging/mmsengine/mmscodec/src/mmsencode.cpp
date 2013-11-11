/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*     binary encoding of a multimedia message
*
*/



//#define CONTENT_DISPOSITION_TEST

// INCLUDE FILES
#include    <e32std.h>
#include    <e32hal.h>
#include    <apparc.h>
#include    <s32mem.h>
#include    <msventry.h>
#include    <utf.h>
#include    <e32math.h>
#include    <msvids.h>
#include    <escapeutils.h>
#include    <badesca.h>
#include    <mmsvattachmentmanager.h>
#include    <mmsvattachmentmanagersync.h>
#include    <cmsvmimeheaders.h>
#include    <imcvcodc.h>

#include    <msgtextutils.h>
#include    <msvstore.h>
#include    <charconv.h>

#include    <centralrepository.h>          // link against centralrepository.lib
#include    "MmsEnginePrivateCRKeys.h"

#include    "mmsheaders.h"
#include    "mmsconst.h"
#include    "mmscodec.h"
#include    "mmsencode.h"
#include    "mmsservercommon.h"
#include    "mmssession.h"
#include    "mmsgenutils.h"
#include    "mmserrors.h"
#include    "mmsentrywrapper.h"
#include    "mmsmmboxmessageheaders.h"
#include    "mmsmmboxviewheaders.h"
#include    "mmssendingchain.h"

const TInt KMmsExtra = 1024; // extra memory left for others
const TInt KMmsShortIntegerLimit127 = 127; // limit for short integer length
const TInt KMmsOneByteLimit = 0x100;
const TInt KMmsTwoByteLimit = 0x10000;
const TInt KMmsThreeByteLimit = 0x1000000;
const TInt KMms2 = 2;
const TInt KMms3 = 3;
const TInt KMms4 = 4;
const TInt KMms5 = 5;
const TInt KMms7 = 7;
const TInt KMms8 = 8;
const TInt KMms24 = 24; // shift of three half bytes
const TInt KMms30 = 30; // upper limit for short length encoding
const TUint8 KMms0x80 = 0x80; // 128
const TUint8 KMms0x7F = 0x7F; // 127
const TUint8 KMms0xFF = 0xFF;
const TInt KMmsMaxCidLength = 18;
// max filename length according to MMS conformance specs
const TInt KMmsMaxFileNameLength = 40;
// if 40 bytes are encoded in base 64, the result is 56 bytes
// (two bytes of padding are needed to make the number divisible by 3)
const TInt KMaxEncodingLength = 56;
// Maximum length of output buffer needed for encoding the filename
// Max 56 bytes for actual encoding and 12 bytes for character set (utf8)
// and encoding scheme indicator 68 for final result, but we allocate
// the maximum Mime header length to be sure everything fits.
const TInt KMaxNameBufferLength = 75;
const TInt KMmsEncodingExtraLength = 12;
const TInt KMmsPreambleLength = 10;
_LIT8( KMmsQuotedPreamble, "=?utf-8?Q?" );
_LIT8( KMmsBase64Preamble, "=?utf-8?B?" );
_LIT8( KMmsEncodingTrailer, "?=" );
const TInt KMmsIntUnderscore = 0x5F; // underscore


enum TMmsMachineStates
    {
    EMmsIdle,
    EMmsEncodingHeaders,
    EMmsEncodingAttachments,
    EMmsFinished
    };
    
// These are the stages for chunked encoding.
// The chunked encoding is synchronous, so the stages are different
// from the machine states.    
enum TMmsChunkedEncodingStages
    {
    EMmsHeaders,
    EMmsAttachmentHeaders,
    EMmsAttachmentData
    };
    
// ==================== LOCAL FUNCTIONS ====================

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that
// might leave.
//
// All member variables are automatically zeroed in the constructor
// of a class derived from CBase.
// Priority is set slightly above standard (1 instead of 0) to make sure
// that message is sent as soon as possible (user input has priority 10).
// Variables related to chunked encoding are initialized to indicate
// one complete chunk only.
// ---------------------------------------------------------------------------
//
CMmsEncode::CMmsEncode()
    :CMsgActive ( KMmsActiveObjectPriority ),
    iState ( EMmsIdle ),
    iOverallDataSize ( -1 ),
    iLastChunk ( ETrue ),
    iOnlyOneChunk( ETrue )
    {
    }

// ---------------------------------------------------------------------------
// Symbian OS default constructor can leave.
// ---------------------------------------------------------------------------
//
void CMmsEncode::ConstructL( RFs& aFs )
    {
    iFs = aFs;
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
EXPORT_C CMmsEncode* CMmsEncode::NewL( RFs& aFs )
    {
    CMmsEncode* self = new ( ELeave ) CMmsEncode;
    
    CleanupStack::PushL( self );
    self->ConstructL( aFs );
    CleanupStack::Pop( self );

    return self;
    }

    
// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CMmsEncode::~CMmsEncode()
    {

    Cancel();
    
    if ( iFileOpen )
        {
        iAttachFile.Close();
        iFileOpen = EFalse;
        }
    
    // Do not delete pointers that were presents from the caller.
    // The caller still owns that data
    delete iMimeHeaders; // this is ours too.
   
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
EXPORT_C void CMmsEncode::StartL(
    MMmsEntryWrapper& aEntryWrapper,
    CMmsHeaders& aMmsHeaders,
    CBufFlat& aEncodeBuffer,
    TRequestStatus& aStatus )
    {

    // This is the entry point for encoding all PDUs that contain both headers
    // and data.
    // For normal use the only PDUs in this category would be M-send.req and
    // M-MBox-Upload.req, but for testing purposes the following PDUs may
    // also be handled using this entry point:
    // M-retrieve.conf (for testing purposes)
    // M-Mbox-View.conf (for testing purposes)
    // M-Mbox-Descr (for testing purposes) 

    // Old asynchronous encoding is retained.
    // Reset sets all chunk-related variables to indicate one chunk which is the last one
    Reset();

    iEntryWrapper = &aEntryWrapper;
    iMmsHeaders = &aMmsHeaders;
    iEncodeBuffer = &aEncodeBuffer;

    if ( iMimeHeaders )
        {
        iMimeHeaders->Reset();
        }
    else
        {
        iMimeHeaders = CMsvMimeHeaders::NewL();
        }

    // We need to know the message entry ID
    // The wrapper originally points to the message entry
    
    TMsvEntry indexEntry;
    iError = iEntryWrapper->GetIndexEntry( indexEntry );
    iCurrentMessageId = indexEntry.Id();
    
    CMsvStore* store = iEntryWrapper->ReadStoreL();
    CleanupStack::PushL( store );

    // Only new attachment structure is supported    
    MMsvAttachmentManager& attachMan = store->AttachmentManagerL();
    iNumberOfAttachments = attachMan.AttachmentCount();
    
    CleanupStack::PopAndDestroy( store );
    
    Queue( aStatus );
    
    iStatus = KRequestPending;
    SetActive();
    // Pretend that we called an asynchronous service
    // in order to get into the state machine loop
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, iError );

    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
EXPORT_C void CMmsEncode::EncodeHeadersL(
    CMmsHeaders& aMmsHeaders,
    CBufFlat& aEncodeBuffer )
    {
    // This function is used to encode all PDUs that contain only headers,
    // not data. Some are responses sent to MMSC, some are requests that
    // contain headers only.
    // Nothing is read from storage, everything to be encoded is contained
    // in the headers.
    // No active object is invoked, this is a synchronous function
    
    // Old one-chunk encoding is retained for encoding headers only.
    // Reset sets all chunk-related variables to indicate one chunk which is the last one
    Reset(); // also sets iError to KErrNone

    iMmsHeaders = &aMmsHeaders;
    iEncodeBuffer = &aEncodeBuffer;

    // start from the beginning
    iPosition = 0;
    // we discard old contents of the buffer in case we must expand it
    iEncodeBuffer->Reset();
    
    // We are a bit greedy here so that we don't need
    // to change the code if there is a minor change in the specs.
    iEncodeBuffer->ResizeL( iMmsHeaders->Size() + KMMSBufferExtra );

    // encode message depending on type
    // these are all types that contain headers only, never data
    switch ( iMmsHeaders->MessageType() )
        {
        case KMmsMessageTypeMNotifyRespInd:
            EncodeNotifyResponse();
            break;
        case KMmsMessageTypeAcknowledgeInd:
            EncodeAcknowledgeIndication();
            break;
        case KMmsMessageTypeMNotificationInd:
            EncodeMmsNotificationL();
            break;
        case KMmsMessageTypeForwardReq:
            EncodeForwardRequestL();
            break;
        case KMmsMessageTypeReadRecInd:
            EncodeReadReplyL();
            break;
        case KMmsMessageTypeMboxStoreReq:
            EncodeMMBoxStoreRequestL();
            break;
        case KMmsMessageTypeMboxViewReq:
            // This may contain keywords. Reserve some extra space
            EncodeMMBoxViewRequestL();
            break;
        case KMmsMessageTypeMBoxDeleteReq:
        case KMmsMessageTypeDeleteReq:
            EncodeDeleteRequestL();
            break;
        case KMmsMessageTypeCancelConf:
            EncodeCancelResponse();
            break;
        case KMmsMessageTypeReadOrigInd:
            // This is for testing purposes.
            // identical to ReadRecInd except for PDU type
            // This would be the PDU sent to originator by MMSC
            EncodeReadReplyL();
            break;
        case KMmsMessageTypeMSendConf:
            // for testing purposes
            EncodeSendConfirmationL();
            break;
        case KMmsMessageTypeDeliveryInd:
            // for testing purposes
            EncodeDeliveryReportL();
            break;
        case KMmsMessageTypeForwardConf:
            // for testing purposes
            EncodeForwardConfirmationL();
            break;
        case KMmsMessageTypeMboxStoreConf:
            // for testing purposes
            EncodeMMBoxStoreConfirmationL();
            break;
        case KMmsMessageTypeMBoxUploadConf:
            // for testing purposes
            EncodeMMBoxUploadConfirmationL();
            break;
        case KMmsMessageTypeMBoxDeleteConf:
        case KMmsMessageTypeDeleteConf:
            // for testing purposes
            EncodeDeleteConfirmationL();
            break;
        case KMmsMessageTypeCancelReq:
            // for testing purposes
            EncodeCancelRequest();
            break;
#ifdef __WINS__
        case KMmsMessageTypeMSendReq:
            EncodeSendRequestHeadersL();
            break;
        case KMmsMessageTypeMBoxUploadReq:
            // This type has attachments and headers
            // This function encodes headers only
            EncodeMMBoxUploadRequestL();
            break;
        case KMmsMessageTypeMRetrieveConf:
            // for test purposes
            EncodeRetrieveConfirmationL();
            break;
        case KMmsMessageTypeMboxViewConf:
            // for test purposes
            EncodeMMBoxViewConfirmationL();
            break;
        case KMmsMessageTypeMBoxDescr:
            // for test purposes
            EncodeMMBoxDescriptionL();
            break;
#endif
        default:
            // Illegal message type.
            iEncodeBuffer->Reset();
            iPosition = 0;
            break;
        }

    // Remove slack to keep garbage out from the end of the file
    iEncodeBuffer->ResizeL( iPosition );
    // Dump the buffer contents into file if requested
    Dump();
    
    iOverallDataSize = iEncodeBuffer->Size();

    }
    
// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
EXPORT_C void CMmsEncode::StartChunkedL(
    MMmsEntryWrapper& aEntryWrapper,
    CMmsHeaders& aMmsHeaders,
    CBufFlat& aEncodeBuffer,
    TRequestStatus& aStatus )
    {
    
    // This is the entry point for chunked encoding all PDUs that contain both headers
    // and data.
    
    // Reset sets all chunk-related variables to indicate one chunk which is the last one
#ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L("CMmsEncode: Start chunked") );
#endif
    Reset(); // also sets iError to KErrNone
    iDataSupplierStage = EMmsHeaders;
    
    iEntryWrapper = &aEntryWrapper;
    iMmsHeaders = &aMmsHeaders;
    iEncodeBuffer = &aEncodeBuffer;

    if ( iMimeHeaders )
        {
        iMimeHeaders->Reset();
        }
    else
        {
        iMimeHeaders = CMsvMimeHeaders::NewL();
        }
        
    // We need to know the message entry ID
    // The wrapper originally points to the message entry
    
    TMsvEntry indexEntry;
    iError = iEntryWrapper->GetIndexEntry( indexEntry );
    iCurrentMessageId = indexEntry.Id();
    
    // Encode headers to the buffer
    // The rest will follow when next data chunk is requested.
    EncodeHeadersChunkedL();
    
	aStatus=KRequestPending;
    TRequestStatus* status = &aStatus;
    User::RequestComplete( status, iError );
    }
    
    
// ---------------------------------------------------------
// From class MMmsCodecDataSupplier
//
// ---------------------------------------------------------
//
TInt CMmsEncode::GetNextDataPart( TPtrC8& aDataPart, TBool& aLastDataChunk )
    {
    TInt error = KErrNone;
    if ( !iEncodeBuffer || iEncodeBuffer->Size() == 0 )
        {
        // called out of context or no message data available
        error = KErrNotFound;
        }
    else
        {
        aDataPart.Set( iEncodeBuffer->BackPtr( iPosition ) ); 
        }
    aLastDataChunk = iLastChunk;
    
    return error;
    }
    
// ---------------------------------------------------------
// From class MMmsCodecDataSupplier
//
// ---------------------------------------------------------
//
TInt CMmsEncode::ReleaseData()
    {
    if ( iOnlyOneChunk )
        {
        // if we have only one chunk we always point to the beginning
        // the buffer gets cleared when all data has been handled
        return KErrNone;
        }
    
    iPosition = 0;
    
    // encode next data part into the buffer
    
    if ( iDataSupplierStage == EMmsAttachmentHeaders && iNumberOfAttachments == 0 )
        {
        iLastChunk = ETrue;
        iDataSupplierStage = EMmsHeaders;
        }
    else if ( iDataSupplierStage == EMmsAttachmentHeaders )
        {
#ifndef _NO_MMSS_LOGGING_
        TMmsLogger::Log( _L("- data supplier stage: Attachment headers") );
#endif
        iError = iEntryWrapper->SetCurrentEntry( iCurrentMessageId );
        if ( iError != KErrNone )
            {
            return iError;
            }
        
        iCurrentFileSize = 0;
        CMsvStore* store = NULL;
        TRAP( iError, 
            {
            store = iEntryWrapper->ReadStoreL();
            CleanupStack::PushL( store );
            MMsvAttachmentManager& attachMan = store->AttachmentManagerL();
            iCurrentFileSize = EncodeHeadersAndGetFileL( attachMan );
            CleanupStack::PopAndDestroy( store );
            });
        if ( iError != KErrNone )
            {
            return iError;
            }
            
        // Now we have the attachment headers in our buffer.
        // If there is room, put data there, too
        
        if ( iBufferSize - iPosition >= iCurrentFileSize )
            {
#ifndef _NO_MMSS_LOGGING_
            TMmsLogger::Log( _L("- enough room left in buffer for attachment data") );
#endif
            EncodeAttachmentData( iAttachFile, iCurrentFileSize );
            iPosition += iCurrentFileSize;
            iAttachFile.Close();
            iFileOpen = EFalse;
            iCurrentAttachment++;
            if ( iCurrentAttachment >= iNumberOfAttachments )
                {
                iLastChunk = ETrue;
                iDataSupplierStage = EMmsHeaders;
                }
            else
                {
                // one attachment finished - move to next one
                iDataSupplierStage = EMmsAttachmentHeaders;
                }
            }
        else
            {
            iDataSupplierStage = EMmsAttachmentData;
            }
        }
    else if ( iDataSupplierStage == EMmsAttachmentData )
        {
#ifndef _NO_MMSS_LOGGING_
        TMmsLogger::Log( _L("- data supplier stage: Attachment data") );
#endif
        // Add as much attachment data as possible.
        // Attachment file is now open, check position
        TInt filePosition = 0;
        iError = iAttachFile.Seek( ESeekCurrent, filePosition );
        if ( iError != KErrNone )
            {
            return iError;
            }
        TInt dataLeft = iCurrentFileSize - filePosition;
        TInt roomLeft = iBufferSize - iPosition;
        if ( roomLeft >= dataLeft )
            {
            EncodeAttachmentData( iAttachFile, dataLeft );
            iPosition += dataLeft;
            iAttachFile.Close();
            iFileOpen = EFalse;
            iCurrentAttachment++;
            if ( iCurrentAttachment >= iNumberOfAttachments )
                {
                iLastChunk = ETrue;
                iDataSupplierStage = EMmsHeaders;
                }
            else
                {
                // one attachment finished - move to next one
                iDataSupplierStage = EMmsAttachmentHeaders;
                }
            }
        else
            {
            // we have more data than fits into buffer
            EncodeAttachmentData( iAttachFile, roomLeft );
            iPosition += roomLeft;
            // We don't close the file because we still have data.
            // We stay in EMmsAttachmentHeaders state
            }
        }
    else
        {
        // do nothing, keep LINT happy
        }
#ifndef _NO_MMSS_LOGGING_
    if ( iDataSupplierStage == EMmsHeaders )
        {
        TMmsLogger::Log( _L("- last chunk released") );
        TMmsLogger::Log( _L("- data supplier stage: MMS headers") );
        }
#endif
    // If we have reached the end and have been asked to release data
    // we have nothing left to do.    
    
    // dump what we got into file - if needed
    if ( iPosition > 0 )
        {
        DumpAppend();
        }
    return iError;
    }
    
// ---------------------------------------------------------
// From class MMmsCodecDataSupplier
//
// ---------------------------------------------------------
//
TInt CMmsEncode::OverallDataSize()
    {
    // If we have encoded all data, iOverallDataSize contains the 
    // actual amount of data.
    // If we are doing chunked encoding, it contains -1.
    return iOverallDataSize;
    }

    
// ---------------------------------------------------------
// From class MMmsCodecDataSupplier
//
// ---------------------------------------------------------
//
TInt CMmsEncode::ResetSupplier()
    {
#ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L("CMmsEncode: ResetSupplier") );
#endif
    if ( iOnlyOneChunk )
        {
        // if we have only one chunk we always point to the beginning
        // the buffer gets cleared when all data has been handled
        return KErrNone;
        }
    
    // start data from the beginning
    iError = KErrNone;
    iDataSupplierStage = EMmsHeaders;
    TRAP( iError, EncodeHeadersChunkedL() );
    
    return iError;
    }

// ---------------------------------------------------------
// From class CMsgActive
//
// ---------------------------------------------------------
//
void CMmsEncode::DoRunL()
    {

    // This routine takes the state machine through the states
    // until an error is encountered or the cycle completes.

    if ( iError != KErrNone )
        {
        // We encountered an error, and cannot continue
        // For the time being we just give up without trying
        // to do any decent cleanup.
        iStatus = iError;
        // If we return from DoRunL without becoming active again,
        // RunL completes.
        return;
        }

    if ( iState != EMmsFinished )
        {
        SelectNextState();
        // If appropriate, ChangeStateL makes us active again
        ChangeStateL();
        }
    else
        {
        // We are done, we must become idle again
        FinishL();
        iStatus = iError;
        // If we return from DoRunL without becoming active again,
        // RunL completes.
        }
    // As we are using standard Mentact RunL, we must leave
    // if we have encountered an error, and do not want to continue

    if ( iError != KErrNone && !IsActive() )
        {
        iPosition = 0;
        FinishL();
        User::Leave( iError );
        }
    }

// ---------------------------------------------------------
// From class CMsgActive
//
// ---------------------------------------------------------
//
void CMmsEncode::DoComplete( TInt& /* aStatus */ )
    {
    // We are exiting the loop - we say we are idle now
    iState = EMmsIdle;
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CMmsEncode::SelectNextState()
    {

    // If appropriate, the functions called within the switch statement
    // will make us active again. If all is done, the asynchronous request
    // will complete

    switch ( iState )
        {
        case EMmsIdle:
            // start the loop
            iState = EMmsEncodingHeaders;
            break;
        case EMmsEncodingHeaders:
            if ( iNumberOfAttachments > 0 )
                {
                iState = EMmsEncodingAttachments;
                }
            else
                {
                iState = EMmsFinished;
                }
            break;
        case EMmsEncodingAttachments:
            // if there are more attachments, don't change state
            if ( iCurrentAttachment >= iNumberOfAttachments )
                {
                iState = EMmsFinished;
                }
            break;
        case EMmsFinished:
            // No more states
            iState = EMmsIdle;
            break;
        default:
            // Illegal state
            break;
        }
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CMmsEncode::ChangeStateL()
    {
    switch ( iState )
        {
        case EMmsEncodingHeaders:
            EncodeHeadersL();
            break;
        case EMmsEncodingAttachments:
            EncodeAttachmentL();
            iCurrentAttachment++;
            break;
        case EMmsFinished:
            FinishL();
            break;
        default:
            // Should not be here anymore
            break;
        }
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CMmsEncode::Reset()
    {
    // close open file in case operation has ended with error
    // and the attachment file is still open
    if ( iFileOpen )
        {
        iAttachFile.Close();
        iFileOpen = EFalse;
        }

    iNumberOfAttachments = 0;
    iCurrentAttachment = 0;
    iError = KErrNone;
    iCurrentMessageId = KMsvNullIndexEntryId;
    iOverallDataSize = -1;
    iLastChunk = ETrue;
    iOnlyOneChunk = ETrue;
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CMmsEncode::EncodeHeadersL()
    {

    // start from the beginning
    iPosition = 0;
    // we discard old contents of the buffer in case we must expand it
    iEncodeBuffer->Reset();
    
    // calculate space needed and resize buffer
    TInt size = 0;
    size += iMmsHeaders->Size();
    
    User::LeaveIfError( iEntryWrapper->SetCurrentEntry( iCurrentMessageId ) );
    CMsvStore* store = iEntryWrapper->ReadStoreL();
    CleanupStack::PushL( store );
    // AttachmentsSize function asks actual size from files
    // in case the info in CMsvAttachment is not up to date.
    // This function leaves if the attachment file is not found.
    // A message cannot be sent if it refers to attachment files that no
    // longer exist.
    size += iEntryWrapper->AttachmentsSizeL( *store );
    CleanupStack::PopAndDestroy( store );
    store = NULL;
        
    size += KMMSBufferExtra; // this is just an estimate of header size...
    // extra needed if lots of attachments.
    size += KMMSAttachmentExtra * iNumberOfAttachments;
    TMemoryInfoV1Buf memory;
    UserHal::MemoryInfo( memory );
    TInt available = memory().iFreeRamInBytes;
    // check that buffer fits, leave a little memory for others too.
    if ( size > ( available - KMmsExtra ) )
        {
        // message is too big - don't even try
        iError = KMmsErrorMessageTooBig;
        }

    if ( iError == KErrNone )
        {
        iEncodeBuffer->ResizeL( size );
        EncodeRequestHeadersL();
        }

    iStatus = KRequestPending;
    SetActive();
  
    // We complete ourselves.
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, iError );

    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CMmsEncode::EncodeHeadersChunkedL()
    {
    // The first step of chunked encoding
#ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L("CMmsEncode::EncodeHeadersChunkedL:: start") );
#endif /* _NO_MMSS_LOGGING_ */
    User::LeaveIfError( iEntryWrapper->SetCurrentEntry( iCurrentMessageId ) );
    
    /* Korean req: 415-5434
     * Get the target encoding MIB enum from cenrep and encode MMS text objects using corresponding conversion plugins
     */
    TInt temp;
    iTargetEncodingType = 0;
    CRepository* repository = CRepository::NewLC( KUidMmsServerMtm );    
    if ( repository->Get( KMmsEncodingType, temp ) == KErrNone )
        {
        iTargetEncodingType = temp;
        }
    CleanupStack::PopAndDestroy( repository );
#ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L("CMmsEncode::EncodeHeadersChunkedL:: target encoding type: %d"), iTargetEncodingType );
#endif /* _NO_MMSS_LOGGING_ */

    if( iTargetEncodingType != 0 )
        {
        TRAPD( err, PreProcessAttachmentDataL() );

#ifndef _NO_MMSS_LOGGING_
    /* if any error, korean specific encoding failed, But this should not stop from sending MMS using
     * existing default encoding. Hence just log the error and continue in any case
     */
        if( err != KErrNone )
            {
            TMmsLogger::Log( _L("CMmsEncode::EncodeHeadersChunkedL:: PreProcessAttachmentDataL error: %d"), err );
            }
#endif /* _NO_MMSS_LOGGING_ */
        }
    
    //Open store for read-only purpose
    CMsvStore* store = iEntryWrapper->ReadStoreL();
    CleanupStack::PushL( store );
    
    // Only new attachment structure is supported    
    MMsvAttachmentManager& attachMan = store->AttachmentManagerL();
    iNumberOfAttachments = attachMan.AttachmentCount();
    
    TInt size = 0;
    size += iMmsHeaders->Size();
    size += KMMSBufferExtra; // this is just an estimate of header size...
    // The buffer must be at least big enough to hold all headers
    iBufferSize = Max( KMmsChunkedBufferSize, size );
    
    // extra needed if lots of attachments.
    // AttachmentsSize function asks actual size from files
    // in case the info in CMsvAttachment is not up to date.
    // This function leaves if the attachment file is not found.
    // A message cannot be sent if it refers to attachment files that no
    // longer exist.
    size += iEntryWrapper->AttachmentsSizeL( *store );
    size += KMMSAttachmentExtra * iNumberOfAttachments;
    CleanupStack::PopAndDestroy( store );
    store = NULL;

    if ( iBufferSize > size )
        {
        // Our message is small enough to be sent in one chunk.
        // It does not make sense to send the message in small chunks
        // it the total is only a few kilobytes
        iBufferSize = size;
        iOnlyOneChunk = ETrue;
        iLastChunk = ETrue;
        }
    else
        {
        // we need several chunks
        iOnlyOneChunk = EFalse;
        iLastChunk = EFalse;
        }
        
    // start from the beginning
    iPosition = 0;
    iEncodeBuffer->Reset();
    // This leaves if unable to resize.
    // We try to keep our buffer small enough so that this does not leave.
    iEncodeBuffer->ResizeL( iBufferSize );
#ifndef _NO_MMSS_LOGGING_
        TMmsLogger::Log( _L("CMmsEncode::EncodeHeadersChunkedL:: MMS buff Size(Approx): %d"), iBufferSize );
#endif /* _NO_MMSS_LOGGING_ */        // The message is small enough to be sent as one chunk

    EncodeRequestHeadersL();
    
    TInt i = 0;
    if ( iOnlyOneChunk )
        {
#ifndef _NO_MMSS_LOGGING_
        TMmsLogger::Log( _L("CMmsEncode::EncodeHeadersChunkedL:: only one chunk") );
#endif /* _NO_MMSS_LOGGING_ */        // The message is small enough to be sent as one chunk
        for ( i = 0; ( i < iNumberOfAttachments ) && ( iError == KErrNone ); ++i )
            {
            iCurrentAttachment = i;
            // encode attachments, too, as we have decided that our message
            // is small enough to fit into the buffer as one chunk
            // DoEncodeAttachment always encodes iCurrentAttachment
            // and updates buffer position as needed.
            DoEncodeAttachmentL();
            // If something goes wrong, DoEncodeAttachmentL() sets iError,
            // and StartChunkedL will complete the caller with error.
            }
        iEncodeBuffer->ResizeL( iPosition );
        iOverallDataSize = iEncodeBuffer->Size();
        }
    else
        {
#ifndef _NO_MMSS_LOGGING_
        TMmsLogger::Log( _L("CMmsEncode::EncodeHeadersChunkedL:: Multiple chunks") );
#endif /* _NO_MMSS_LOGGING_ */        // The message is small enough to be sent as one chunk
        // chunked sending - but it should be possible to calculate data size
        iOverallDataSize = iPosition; // This is the amount taken by headers
        // The message is small enough to be sent as one chunk
        for ( i = 0; ( i < iNumberOfAttachments ) && ( iError == KErrNone ); ++i )
            {
            iCurrentAttachment = i;
            // encode attachments, too, as we have decided that our message
            // is small enough to fit into the buffer as one chunk
            // DoEncodeAttachment always encodes iCurrentAttachment
            // and updates buffer position as needed.
            iOverallDataSize += DoGetAttachmentEncodingLengthL();
            // If something goes wrong, DoEncodeAttachmentL() sets iError,
            // and StartChunkedL will complete the caller with error.
            }
        iCurrentAttachment = 0;
        iDataSupplierStage = EMmsAttachmentHeaders;
        if ( iError != KErrNone )
            {
            iOverallDataSize = -1;
            }
        }
     
    // Dump headers. More data will follow    
    Dump();
#ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L("CMmsEncode::EncodeHeadersChunkedL:: end") );
#endif /* _NO_MMSS_LOGGING_ */        // The message is small enough to be sent as one chunk
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CMmsEncode::EncodeAttachmentL()
    {

    DoEncodeAttachmentL();
    
    if ( iError != KErrNone )
        {
        // If we return without becoming active again, RunL completes
        // we cannot send this message, because we cannot access attachments
        return;
        }

    iStatus = KRequestPending;
    SetActive();
  
    // Now we indicate we already did it.
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, iError );

    }
    
// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CMmsEncode::DoEncodeAttachmentL()
    {
    
    // Encode one part
    // We must calculate length of headers and content type
    
    User::LeaveIfError( iEntryWrapper->SetCurrentEntry( iCurrentMessageId ) );
        
    CMsvStore* store = iEntryWrapper->ReadStoreL();
    CleanupStack::PushL( store );
    MMsvAttachmentManager& attachMan = store->AttachmentManagerL();
    
    TInt size = 0;
    size = EncodeHeadersAndGetFileL( attachMan );
    if ( iError != KErrNone )
        {
        CleanupStack::PopAndDestroy( store );
        return;
        }
    
    // The data read function will not leave
    EncodeAttachmentData( iAttachFile, size );
    iPosition += size;
    iAttachFile.Close();

    CleanupStack::PopAndDestroy( store );

    }
    
// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
TInt CMmsEncode::DoGetAttachmentEncodingLengthL()
    {
    
    // calculate length of headers and content type
    
    User::LeaveIfError( iEntryWrapper->SetCurrentEntry( iCurrentMessageId ) );
        
    CMsvStore* store = iEntryWrapper->ReadStoreL();
    CleanupStack::PushL( store );
    MMsvAttachmentManager& attachMan = store->AttachmentManagerL();
    
    TInt size = 0;
    size = GetHeadersAndFileSizeL( attachMan );
    CleanupStack::PopAndDestroy( store );
    return size;

    }
    
// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
TInt CMmsEncode::EncodeHeadersAndGetFileL( MMsvAttachmentManager& aAttachMan )
    {
    
    CMsvAttachment* attachmentInfo = NULL;
    attachmentInfo = aAttachMan.GetAttachmentInfoL( iCurrentAttachment );
    CleanupStack::PushL( attachmentInfo );
    
    iMimeHeaders->RestoreL( *attachmentInfo );
        
    // We don't trust the info, we ask the file itself
    // Someone outside the messaging system may have changed the file
    // without changing the attachment info
    TInt size = 0;
    if ( iFileOpen )
        {
        // close file in case we have been reset while the file is still open
        iAttachFile.Close();
        iFileOpen = EFalse;
        }
    iAttachFile = aAttachMan.GetAttachmentFileL( iCurrentAttachment );
    iError = iAttachFile.Size( size );
    iAttachFile.Close();

    if ( iError != KErrNone )
        {
        CleanupStack::PopAndDestroy( attachmentInfo );
        return size;
        }

    // calculate the length of the headers
    TBool foundName = EFalse;
    TUint contentTypeSize = 0;
    TInt8 contentType = -1; // indicate not found
    TPtrC8 contentTypeString;

    TUint headerSize = 0;
    
    HBufC8* buf8 = CalculateAttachmentHeaderLengthL(
        *attachmentInfo,
        headerSize,    
        foundName,
        contentTypeSize,
        contentType,
        contentTypeString );
     
    CleanupStack::PushL( buf8 );
        
    // We have calculated the header length, now we can encode:

    TPtrC8 nameString;
    if ( !foundName && buf8 )
        {
        nameString.Set( buf8->Des() );
        }
        
    EncodeAttachmentHeadersL(
        size,
        headerSize,
        foundName,
        contentTypeSize,
        contentType,
        contentTypeString,
        nameString );

    // now we have put the name all over, we can destroy the buffer
    CleanupStack::PopAndDestroy( buf8 );
    buf8 = NULL;
    CleanupStack::PopAndDestroy( attachmentInfo );
    attachmentInfo = NULL;
    
    // Now just write all the data octets
    iAttachFile = aAttachMan.GetAttachmentFileL( iCurrentAttachment );
    iFileOpen = ETrue;
    
    return size;
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
TInt CMmsEncode::GetHeadersAndFileSizeL( MMsvAttachmentManager& aAttachMan )
    {
    
    CMsvAttachment* attachmentInfo = NULL;
    attachmentInfo = aAttachMan.GetAttachmentInfoL( iCurrentAttachment );
    CleanupStack::PushL( attachmentInfo );
    
    iMimeHeaders->RestoreL( *attachmentInfo );
        
    // We don't trust the info, we ask the file itself
    // Someone outside the messaging system may have changed the file
    // without changing the attachment info
    TInt size = 0;
    if ( iFileOpen )
        {
        // close file in case we have been reset while the file is still open
        iAttachFile.Close();
        iFileOpen = EFalse;
        }
    iAttachFile = aAttachMan.GetAttachmentFileL( iCurrentAttachment );
    iError = iAttachFile.Size( size );
    iAttachFile.Close();

    if ( iError != KErrNone )
        {
        CleanupStack::PopAndDestroy( attachmentInfo );
        return 0;
        }

    // calculate the length of the headers
    TBool foundName = EFalse;
    TUint contentTypeSize = 0;
    TInt8 contentType = -1; // indicate not found
    TPtrC8 contentTypeString;

    TUint headerSize = 0;
    
    HBufC8* buf8 = CalculateAttachmentHeaderLengthL(
        *attachmentInfo,
        headerSize,    
        foundName,
        contentTypeSize,
        contentType,
        contentTypeString );
        
    delete buf8; // This is not needed
    buf8 = NULL;     
     
    CleanupStack::PopAndDestroy( attachmentInfo );
    attachmentInfo = NULL;
    
    // We have calculated the header length, now we still need the
    // uintvar lengths:
    
    size += GetUintvarLength( size ); // encoding size for attachment length
    size += headerSize;
    size += GetUintvarLength( headerSize ); // encoding size for header length
 
    return size;
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
HBufC8* CMmsEncode::CalculateAttachmentHeaderLengthL(
    CMsvAttachment& aAttachmentInfo,
    TUint& aHeaderLength,
    TBool& aFoundName,
    TUint& aContentTypeSize,
    TInt8& aContentType,
    TPtrC8& aContentTypeString  )
    {
    
    TInt numNonSafe = 0; // number of characters in filename that would require encoding
    // Not all content types have parameters.
    // The only parameter we can handle to some extent
    // is the character set parameter for text content types.
    // The name parameter will always be added if not already present
    
    // Check if we have well-known media.
    aContentType = -1; // indicate not found
    aContentTypeString.Set( aAttachmentInfo.MimeType() );
    if ( aContentTypeString.Length() == 0 )
        {
        // take type from mime headers
        HBufC8* tempContentType = HBufC8::NewL(
            iMimeHeaders->ContentType().Length() +
            iMimeHeaders->ContentSubType().Length() + 1 );
        CleanupStack::PushL( tempContentType );
        tempContentType->Des().Copy( iMimeHeaders->ContentType() );
        if ( ( iMimeHeaders->ContentType().Find( KMmsSlash8 ) !=
            ( iMimeHeaders->ContentType().Length() - 1 ) ) &&
            ( iMimeHeaders->ContentSubType().Find( KMmsSlash8 ) != 0 ) &&
            ( iMimeHeaders->ContentSubType().Length() != 0 ) )
            {
            tempContentType->Des().Append( KMmsSlash8 );
            }
        tempContentType->Des().Append( iMimeHeaders->ContentSubType() );
        aAttachmentInfo.SetMimeTypeL( tempContentType->Des() );
        CleanupStack::PopAndDestroy( tempContentType );
        aContentTypeString.Set( aAttachmentInfo.MimeType() );
        }
        
    TInt8 i;
    for ( i = 0; i < KNumberContentTypes && aContentType < 0; ++i )
        {
        if ( aContentTypeString.CompareF(
            TPtrC8( KContentTypeTable[i] ) ) == 0 )
            {
            aContentType = i;
            }
        }
    if ( aContentTypeString.Length() == 0 )
        {
        aContentTypeString.Set( KMmsUnknownType );
        aContentType = -1;
        }
        
    // we always add the name parameter to content type, so we always
    // use the general form. If no name is given, we use the original filename
    // If the name parameter is defined, we use it.

    aFoundName = EFalse;
    iFileName.SetLength( 0 );

    aHeaderLength = 0;
    // add media type length
    if ( aContentType >= 0 )
        {
        // well-known media
        aHeaderLength += 1;
        }
    else
        {
        // Extension media
        aHeaderLength += aContentTypeString.Length();
        aHeaderLength += 1; // terminating zero
        }

    // If we have parameters, we must calculate their length.
    if ( iMimeHeaders->MimeCharset() != 0 )
        {
        // charset has well-known encoding
        aHeaderLength += 1;
        if ( iMimeHeaders->MimeCharset() <= KMmsShortIntegerLimit127 )
            {
            // short integer
            aHeaderLength += 1;
            }
        else if ( iMimeHeaders->MimeCharset() < KMmsOneByteLimit )
            {
            // long integer, short length + 1 byte of value
            aHeaderLength += KMms2;
            }
        else if ( iMimeHeaders->MimeCharset() < KMmsTwoByteLimit )
            {
            // long integer, short length + 2 bytes of value
            // so far I don't know any IANA numbers that would
            // take more than two hex bytes.
            aHeaderLength += KMms3;
            }
        else if ( iMimeHeaders->MimeCharset() < KMmsThreeByteLimit )
            {
            // long integer, short length + 3 bytes of value
            aHeaderLength += KMms4;
            }
        else
            {
            // long integer, short length + 4 bytes of value
            // cannot be longer than this in any case
            aHeaderLength += KMms5;
            }
        }
    // Then the rest of the parameters, unknown,
    // sent as text strings
    // If "name" attribute has been defined, we use it.
    // if it is not added, we must add it.
    // We must ensure that we don't add it twice...
    
    TInt mimeHeaderCount = iMimeHeaders->ContentTypeParams().MdcaCount();

    for ( i = 0; i < mimeHeaderCount; i++ )
        {
        // If we have corrupted parameters (length of name is zero)
        // we skip the name/value pair.
        // value may be zero, it will be encoded as just the terminating zero
        if ( i%2 == 0 &&
            iMimeHeaders->ContentTypeParams().MdcaPoint( i ).Length() == 0 )
            {
            i++;
            }
        else
            {
            // 8-byte string size + terminating zero
            // If parameter has no value, it still must have
            // the "no-value" indicator (just the terminating zero.)
            if ( iMimeHeaders->ContentTypeParams().MdcaPoint( i ).CompareF(
                KWspNameString ) == 0 && i%2 == 0 )
                {
                if ( ( i + 1 ) < mimeHeaderCount && 
                    IsStringSafe( iMimeHeaders->ContentTypeParams().MdcaPoint( i + 1 ),
                    numNonSafe ) )
                    {
                    // We always move the name to the first item.
                    // It may be this parameter or something else
                    
                    // Content type parameters are 8-bit strings.
                    // We can use the name only if it is safe, otherwise it may contain some
                    // unknown encoding (maybe utf-8).
                    iFileName.Copy( ( iMimeHeaders->ContentTypeParams().
                       MdcaPoint( i + 1 ) ).Right( KMmsMaxFileName ) );
                    i++;
                    }
                else
                    {
                    // Skip the next field as it can only be used if it is safe
                    i++;
                    }
                }
            else
                {
                aHeaderLength += iMimeHeaders->ContentTypeParams().MdcaPoint( i ).Length() + 1;
                }
            }
        }
    if ( ( iMimeHeaders->ContentTypeParams().MdcaCount() % 2 ) == 1 )
        {
        // Odd number. Obviously last parameter has no value.
        // Add the "no value" token
        aHeaderLength++;
        }

    // include name parameter to content-type

    // if the name was not among our mime headers,
    // we must add it.
    TInt nameLength = 0;

    // We prefer the suggested filename as the content type name parameter.
    // If version is 1.3 or later, we can encode a non-safe filename.
    // If suggested filename is not safe, but we already have a safe filename
    // in iFilename, we don't override it if version is 1.2 or lower
    if ( iMimeHeaders->SuggestedFilename().Length() > 0 )
        {
        if ( iMmsHeaders->MmsVersion() > KMmsVersion12 ||
             IsStringSafe( iMimeHeaders->SuggestedFilename() ) ||
             iFileName.Length() == 0 )
            {
            iFileName.Copy( iMimeHeaders->SuggestedFilename().Right( KMmsMaxFileName ) );
            }
        }
        
    TParse* parse = NULL;
    // If we don't have a filename suggestion so far, we just use the
    // actual filename the attachment manager has used to save the file.    
    if ( iFileName.Length() == 0 )
        {
        parse = new( ELeave )TParse;
        CleanupStack::PushL( parse );
        // aAttachmentInfo.FilePath() is an actual Symbian filename including path
        // so it never can be too long to fit into TParse or TFilename 
        parse->Set( aAttachmentInfo.FilePath(), NULL, NULL );
        iFileName.Copy( parse->NameAndExt() );
        CleanupStack::PopAndDestroy( parse );
        parse = NULL;
        }

    // Last effort:
    // If our version is 1.2 or lower and the filename suggestion so far
    // is not safe, we try content-location. It is always safe
        
    if ( iMmsHeaders->MmsVersion() <= KMmsVersion12 &&
        !IsStringSafe( iFileName ) )
        {
        if ( iMimeHeaders->ContentLocation().Length() > 0 )
            {
            // content location should be safe.
            // However, we must make sure it contains no URI escapes.
            // for example space is legal in filename, but not in URI.
            HBufC16* decoded = NULL;
            TRAPD( error, ( decoded = EscapeUtils::EscapeDecodeL(
                iMimeHeaders->ContentLocation() ) ) );
            if ( error == KErrNone )
                {
                iFileName.Copy( decoded->Des().Right( KMmsMaxFileName ) );
                delete decoded;
                decoded = NULL;
                }
            else
                {
                iFileName.Copy( iMimeHeaders->ContentLocation().Right( KMmsMaxFileName ) );
                }
            }
        }
        
    // remove leading and trailing spaces just in case...    
    iFileName.Trim();    
        
    // We should now make sure that the filename is not too long.
    // According to MMS conformance specs, the maximum should be 40 characters
    // It is unclear if the characters mean the total length or the length
    // after conversion to utf-8, but as a first approximation we try
    // to ensure that the total does not exceed 40 characters
    
    nameLength = iFileName.Length();
    
    if ( nameLength > KMmsMaxFileNameLength )
        {
        parse = new( ELeave )TParse;
        CleanupStack::PushL( parse );
        parse->Set( aAttachmentInfo.FilePath(), NULL, NULL );
        iFileName.Copy( parse->Name().Left( KMmsMaxFileNameLength - parse->Ext().Length() ) );
        iFileName.Append( parse->Ext() );
        CleanupStack::PopAndDestroy( parse );
        parse = NULL;
        nameLength = iFileName.Length();
        }

    // If encoding version is <=1.2, make filename safe,
    // otherwise use MIME encoding (either base64 or quoted printable)
    
    HBufC8* buf8 = NULL;

    TPtrC16 originalName;
    TBuf16<1> oneCharacter;
    originalName.Set( iFileName );

    TInt j;
    
    if ( IsStringSafe( iFileName ) )
        {
        // filename only contains us-ascii characters - use as is 
        buf8 = HBufC8::NewL( nameLength );
        CleanupStack::PushL( buf8 );
        buf8->Des().Copy( iFileName );      
        }
    else
        {
        if ( iMmsHeaders->MmsVersion() <= KMmsVersion12 )
            {
            // if version is <= 1.2, the filename must be us-ascii
            // replace all illegal characters by underscore
            buf8 = HBufC8::NewL( nameLength );
            CleanupStack::PushL( buf8 );
            for ( j = 0; j < nameLength; ++j )
                {
                oneCharacter.Copy( originalName.Mid( j, 1 ) );
                if ( !IsStringSafe( oneCharacter ) )
                    {
                    _LIT( KMmsUnderscore, "_" );
                    oneCharacter.Copy( KMmsUnderscore );
                    }
                buf8->Des().Append( oneCharacter );
                }
            }
        else
            {
            // MMS encapsulation version is 1.3 or later
            // Using non-ascii characters in filename parameter is allowed
            // The filename must be encoded either using base64 or quoted printable
            HBufC8* temp = NULL;
			// coverity[size_error][buffer_alloc]
            temp = CnvUtfConverter::ConvertFromUnicodeToUtf8L( originalName );
            CleanupStack::PushL( temp );
            // Now we have the filename in utf-8.
            // We must check if it contains non-ascii, and if it does,
            // we must use base64 or quoted-printable encoding.
            TInt numNonSafe;
            // Actually we know already that we are not safe. We just need
            // to get the number of non-ascii characters
            IsStringSafe( temp->Des(), numNonSafe );
            // We prefer quoted printable as it looks nicer in case we have mostly ascii,
            // and it leaves extension in cleartext anyway
            if ( ( temp->Des().Length() + ( KMms2 * numNonSafe ) ) <= KMaxEncodingLength )
                {
                // use quoted printable, but first convert spaces to underscores
                TInt spacePosition = temp->Des().Find( KSpace8 );
                while ( spacePosition != KErrNotFound )
                    {
                    temp->Des()[ spacePosition ] = KMmsIntUnderscore;
                    spacePosition = temp->Des().Find( KSpace8 );
                    }
                buf8 = EncodeQuotedPrintableWordL( temp->Des() );
                }
            else
                {
                // use base64
                // We have not restricted the filename length in utf-8 format,
                // so it may be way longer than 40 bytes.
                // However, we can calculate the length the buffer needs, so we
                // can try. However, we may end up with some soft line break in 
                // the middle of the parameter which may not be acceptable.
                buf8 = EncodeBase64WordL( temp->Des() );
                }
            
            CleanupStack::PopAndDestroy( temp );
            
            CleanupStack::PushL( buf8 );
            // we must change nameLength here
            }
        }

    // The name is now in buf8, either just copied or encoded
    nameLength = buf8->Des().Length();

    if ( aFoundName == EFalse )
        {
        // well-known encoding for the header itself
        aHeaderLength++;
        aHeaderLength += nameLength;
        aHeaderLength += 1; // terminating zero
        }

    aContentTypeSize = aHeaderLength;

    // As we have general form (at least name parameter), we need to indicate the length
    if ( aContentTypeSize <= KMms30 )
        {
        // short length
        aHeaderLength += 1;
        }
    else
        {
        // length quote + at least one byte of uintvar
        aHeaderLength += KMms2;
        // Expecting over 128 bytes is paranoid.
        if ( aContentTypeSize >= KMms0x7F )
            {
            aHeaderLength++;
            }
        }
        
    // Now the rest of the mime headers:

    // Content-location
    // just a short integer + a null terminated string

    if ( iMimeHeaders->ContentLocation().Length() > 0 )
        {
        aHeaderLength += 1; // encoding of header as short integer

        // If we want to make sure we are safe,
        // we must call IsStringSafe() function.

        // We just assume we are safe now.
        aHeaderLength += iMimeHeaders->ContentLocation().Length();
        aHeaderLength += 1; // terminating zero
        }

   // Content-ID

    if ( iMimeHeaders->ContentId().Length() > 0 )
        {
        aHeaderLength += 1; // encoding of header as short integer
        // add terminating zero and preceding quote.
        // (needed according to WSP 1.3)
        aHeaderLength += iMimeHeaders->ContentId().Length() + KMms2;
        if ( iMimeHeaders->ContentId().Find( KMmsLeftAngle ) != 0 )
            {
            aHeaderLength += KMms2; // we must add angle bracket
            }
        }
        
    // x-type parameters
    // Must be stored as name/value pairs like content-type parameters
    // X-parameter[i] = parameter name
    // X-parameter[i+1] = parameter value
    
    for ( i = 0; i < iMimeHeaders->XTypeParams().MdcaCount(); ++i )
        {
        // If we have corrupted parameters (length of name is zero)
        // we skip the name/value pair.
        // value may be zero, it will be encoded as just the terminating zero
        if ( i%2 == 0 &&
            ( iMimeHeaders->XTypeParams().MdcaPoint( i ).Length() == 0  ||
            iMimeHeaders->XTypeParams().MdcaPoint( i ).CompareF( 
            KMmsSeparateDeliveryOmaXHeader ) == 0 ) )
            {
            // We skip the header and its value if
            // a) the header length is 0
            // b) the header is X-Oma-Drm-Separate-Delivery
            i++;
            }
        else
            {
            aHeaderLength += iMimeHeaders->XTypeParams().MdcaPoint( i ).Length() + 1;
            }
        }
    if ( ( iMimeHeaders->XTypeParams().MdcaCount() % 2 ) == 1 )
        {
        // Odd number. Obviously last parameter has no value.
        // Add the "no value" token
        aHeaderLength++;
        }
        
    // THESE ARE NOT IMPLEMENTED

    // Content-disposition & parameters
    // Content-description
    // Content-base
    // Relative path
    // version

#ifdef CONTENT_DISPOSITION_TEST
// not supported

    // We generate a general header for content disposition
    // for tests we use the actual filename.
    // In real life the user's filename should not be
    // sent out to the world

    // Fixed coding: 
    // Content Disposition, value length, attachment, filename,
    // actual filename + terminating 0

    // attachment, filename, terminating zero
    // content disposition should be text-string, not quoted text string
    TInt contentDispositionLength = KMms3 + nameLength;

    TInt valueLengthLength = 1;
    if ( contentDispositionLength > KMms30 )
        {
        // lengths 0 - 30 are encoded using "short length"
        valueLengthLength++;
        // 
        if ( contentDispositionLength > ShortIntegerLimit127 )
            {
            valueLengthLength++;
            }
        // Should never be longer than this.
        // I don't think anybody has a filename that is longer than 16383 bytes.
        }

    aHeaderLength += valueLengthLength + contentDispositionLength + 1;
#endif

    CleanupStack::Pop( buf8 );
    return buf8; // caller will delete this afterwards
    
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CMmsEncode::EncodeAttachmentHeadersL(
    TUint aSize,
    TUint aHeaderSize,
    TBool aFoundName,
    TUint aContentTypeSize,
    TInt8 aContentType,
    TPtrC8& aContentTypeString,
    TPtrC8& aNameString )
    {
    
    EncodeUintvar( aHeaderSize );

    EncodeUintvar( aSize );

    TUint8 temp = 0; // used to set the high bit for integers

    // if we have character set (or other parameters),
    // we must use general encoding

    EncodeValueLength( aContentTypeSize );

    if ( aContentType >= 0 )
        {
        temp = aContentType | KMms0x80;
        iEncodeBuffer->Write( iPosition, &temp, 1 );
        iPosition++;
        }
    else
        {
        // we assume this is a safe string
        EncodeTextString( aContentTypeString );
        }
    
    // Then content type parameters. Mainly character set,
    // but maybe something else, too

    temp = KWspCharset | KMms0x80; // encode as short integer
    EncodeOptionalInteger( temp, iMimeHeaders->MimeCharset() );

    // if we didn't find "Name" parameter among our parameters, we
    // put it first.

    // we need the name later anyway...

    if ( !aFoundName )
        {
        temp = KWspName | KMms0x80; // encode as short integer
        EncodeHeaderAndTextString( temp, aNameString );
        }

    // Then the rest of content type parameters, just text strings.
    
    TInt i = 0;
    
    for ( i = 0; i < iMimeHeaders->ContentTypeParams().MdcaCount(); ++i )
        {
        // If we have corrupted parameters (length of name is zero)
        // we skip the name/value pair.
        // value may be zero, it will be encoded as just the terminating zero
        if ( i%2 == 0 &&
            iMimeHeaders->ContentTypeParams().MdcaPoint( i ).Length() == 0 )
            {
            i++;
            }
        else
            {
            // 8-byte string + terminating zero
            // If parameter has no value, it still must have
            // the "no-value" indicator (just the terminating zero.)
            // The ContentTypeParameters function provides
            // an empty string in this case.
            if ( iMimeHeaders->ContentTypeParams().MdcaPoint( i ).CompareF(
                KWspNameString ) == 0 && i%2 == 0 ) 
                {
                if ( aFoundName )
                    {
                    temp = KWspName | KMms0x80; // encode as short integer
                    iEncodeBuffer->Write( iPosition, &temp, 1 );
                    iPosition++;
                    }
                else
                    {
                    // we can use name from mime headers only if it is "safe"
                    // (contains only us-ascii characters)
                    // If the name is not safe, the parameter value is skipped
                    i++;
                    }
                }
            else
                {
                EncodeTextString(
                    iMimeHeaders->ContentTypeParams().MdcaPoint( i ) );
                }
            }
        }
    if ( ( iMimeHeaders->ContentTypeParams().MdcaCount() % 2 ) == 1 )
        {
        // Odd number. Obviously last parameter has no value.
        // Add the "no value" token
        iEncodeBuffer->Write( iPosition, &KMmsNull, 1 );
        iPosition++;
        }

    // content-location
    temp = KWspContentLocation | KMms0x80; // encode as short integer
    EncodeOptionalStringL( temp, iMimeHeaders->ContentLocation() );

    // content-id
    if ( iMimeHeaders->ContentId().Length() > 0 )
        {
        HBufC8* tempContentId = HBufC8::NewL( iMimeHeaders->ContentId().Length() + KMms2 );
        CleanupStack::PushL( tempContentId );
        if ( iMimeHeaders->ContentId().Find( KMmsLeftAngle ) != 0 )
            {
            tempContentId->Des().Copy( KMmsLeftAngle );
            tempContentId->Des().Append( iMimeHeaders->ContentId() );
            tempContentId->Des().Append( KMmsRightAngle );
            }
        else
            {
            tempContentId->Des().Copy( iMimeHeaders->ContentId() );
            }
        temp = KWspContentId | KMms0x80; // encode as short integer
        iEncodeBuffer->Write( iPosition, &temp, 1 );
        iPosition++;
        EncodeQuotedTextString( tempContentId->Des() );
        CleanupStack::PopAndDestroy( tempContentId );
        }

#ifdef CONTENT_DISPOSITION_TEST
// not supported
     temp = 0x2E | KMms0x80; // content disposition
     iEncodeBuffer->Write( iPosition, &temp, 1 );
     iPosition++;
     EncodeValueLength( contentDispositionLength );
     temp = 0x81; // attachment
     iEncodeBuffer->Write( iPosition, &temp, 1 );
     iPosition++;
     temp = 0x06 | KMms0x80; // filename
     // filename should be text-string, not quoted
     EncodeHeaderAndTextString( temp, buf8->Des() );
#endif

        // x-type parameters
    for ( i = 0; i < iMimeHeaders->XTypeParams().MdcaCount(); ++i )
        {
        // If we have corrupted parameters (length of name is zero)
        // we skip the name/value pair.
        // value may be zero, it will be encoded as just the terminating zero
        if ( i%2 == 0 &&
            ( iMimeHeaders->XTypeParams().MdcaPoint( i ).Length() == 0  ||
            iMimeHeaders->XTypeParams().MdcaPoint( i ).CompareF( 
            KMmsSeparateDeliveryOmaXHeader ) == 0 ) )
            {
            // We skip the header and its value if
            // a) the header length is 0
            // b) the header is X-Oma-Drm-Separate-Delivery
            i++;
            }
        else
            {
            // 8-byte string + terminating zero
            // If parameter has no value, it still must have
            // the "no-value" indicator (just the terminating zero.)
            // The ContentTypeParameters function provides
            // an empty string in this case.
            EncodeTextString( iMimeHeaders->XTypeParams().MdcaPoint( i ) );
            }
        }
    if ( ( iMimeHeaders->XTypeParams().MdcaCount() % 2 ) == 1 )
        {
        // Odd number. Obviously last parameter has no value.
        // Add the "no value" token
        iEncodeBuffer->Write( iPosition, &KMmsNull, 1 );
        iPosition++;
        }
        
    // End of MIME headers

    }
    
// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CMmsEncode::EncodeAttachmentData( RFile& aAttachFile, TInt aSize )
    {
    // read the attachment file
    // set up a pointer to point to iEncodeBuffer at iPosition
    TPtr8 pos = iEncodeBuffer->Ptr( iPosition );
    // Attachemnt may be a file attacment or a linked file.
    // The file pointer will point to unread part if file is read in chunks
    iError = aAttachFile.Read( pos, aSize );
    }
    

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CMmsEncode::FinishL()
    {

    // Remove slack to keep garbage out from the end of the file
    iEncodeBuffer->ResizeL( iPosition );
    iOverallDataSize = iEncodeBuffer->Size();
    iState = EMmsIdle;
    
    // Dump the buffer contents into file if requested
    Dump();

    }
    
// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CMmsEncode::EncodeRequestHeadersL()
    {
    // encode message depending on type
    switch ( iMmsHeaders->MessageType() )
        {
        // KMmsMessageTypeMNotifyRespInd and KMmsMessageTypeAcknowledgeInd and all other PDUs
        // that contain only headers and no data should be handled by
        // using function
        // EncodeHeadersL(CMmsHeaders& aMmsHeaders,CBufFlat& aEncodeBuffer)
        case KMmsMessageTypeMSendReq:
            EncodeSendRequestHeadersL();
            break;
        case KMmsMessageTypeMBoxUploadReq:
            // This type has attachments and headers
            EncodeMMBoxUploadRequestL();
            break;
        case KMmsMessageTypeMRetrieveConf:
            // for test purposes
            EncodeRetrieveConfirmationL();
            break;
        case KMmsMessageTypeMboxViewConf:
            // for test purposes
            EncodeMMBoxViewConfirmationL();
            break;
        case KMmsMessageTypeMBoxDescr:
            // for test purposes
            EncodeMMBoxDescriptionL();
            break;
        default:
            iError = KMmsErrorUnknownMessageType;
            break;
        }
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CMmsEncode::EncodeSendRequestHeadersL()
    {
        
    // Insert message type, TID and version number
    EncodeStartingHeaders( KMmsMessageTypeMSendReq,
        iMmsHeaders->Tid(), iMmsHeaders->MmsVersion() );

    // if we have set a date, insert it
    EncodeDate( iMmsHeaders->Date() );

    // If we have set the sender, encode it
    // If not, we ask MMSC to add the sender
    EncodeSenderL( iMmsHeaders->Sender() );

    // All recipient lists. If no recipients in some of the lists,
    // nothing is encoded, no need to check separately
    EncodeRecipientL( iMmsHeaders->ToRecipients(), EMmsTo );
    EncodeRecipientL( iMmsHeaders->CcRecipients(), EMmsCc );
    EncodeRecipientL( iMmsHeaders->BccRecipients(), EMmsBcc );

    // Subject
    EncodeOptionalStringL( KMmsAssignedSubject, iMmsHeaders->Subject() );

    // Message class
    EncodeOptionalByte( KMmsAssignedMessageClass,
        iMmsHeaders->MessageClass() );

    // Expiration time
    EncodeOptionalIntervalOrDate( KMmsAssignedExpiry,
        iMmsHeaders->ExpiryInterval(),
        iMmsHeaders->ExpiryDate() );

    // Delivery time
    EncodeOptionalIntervalOrDate( KMmsAssignedDeliveryTime,
        iMmsHeaders->DeliveryTimeInterval(),
        iMmsHeaders->DeliveryDate() );

    // Priority
    EncodeOptionalByte( KMmsAssignedPriority, iMmsHeaders->MessagePriority() );
    
    // Sender visibility
    EncodeOptionalByte( KMmsAssignedSenderVisibility,
        iMmsHeaders->SenderVisibility() );

    // Delivery report
    EncodeOptionalByte( KMmsAssignedDeliveryReport,
        iMmsHeaders->DeliveryReport() );

    // Read reply
    EncodeOptionalByte( KMmsAssignedReadReply, iMmsHeaders->ReadReply() );

    // MMS encapsulation 1.1 headers - optional
    if ( iMmsHeaders->MmsVersion() > KMmsVersion1 )
        {
        // Do we want to specify reply charging
        if ( iMmsHeaders->ReplyCharging() == KMmsReplyChargingRequested ||
            iMmsHeaders->ReplyCharging() == KMmsReplyChargingRequestedTextOnly )
            {
            // Reply charging value
            EncodeOptionalByte( KMmsAssignedReplyCharging,
                iMmsHeaders->ReplyCharging() );
            // Reply charging deadline
            EncodeOptionalIntervalOrDate( KMmsAssignedReplyChargingDeadline,
                iMmsHeaders->ReplyChargingInterval(),
                iMmsHeaders->ReplyChargingDate() );
            // Reply charging size
            EncodeReplyChargingSize( iMmsHeaders->ReplyChargingSize() );
            }
        // Reply charging ID
        EncodeOptionalString( KMmsAssignedReplyChargingID,
            iMmsHeaders->ReplyChargingId() );
        }

    if ( iMmsHeaders->MmsVersion() > KMmsVersion11 )
        {
        // MMBox related headers - optional
        if ( iMmsHeaders->ReadOnlyMMBoxMessageHeaders() )
            {
            // X-Mms-MM-Store
            EncodeOptionalByte( KMmsAssignedMmsStore,
                iMmsHeaders->ReadOnlyMMBoxMessageHeaders()->MmsStore() );
            if ( iMmsHeaders->ReadOnlyMMBoxMessageHeaders()->MmsStore() == KMmsYes )
                {
                // X-Mms-MM-State
                EncodeOptionalByte( KMmsAssignedMMState,
                    iMmsHeaders->ReadOnlyMMBoxMessageHeaders()->MmsMMState() );
                // X-MMs-MM-Flags
                // The keyword token should always be "add" or "remove"
                EncodeKeywordArrayL();
                }
            }
        }

    // Add encapsulation version 1.3 headers - optional
    if ( iMmsHeaders->MmsVersion() > KMmsVersion12 )
        {
        // X-Mms-Applic-ID, X-Mms-Reply-Applic-ID and X-Mms-Aux-Applic-Info
        EncodeApplicationHeadersL();
        // X-Mms-Content-Class
        EncodeOptionalByte( KMmsAssignedContentClass, iMmsHeaders->ContentClass() );
        // X-Mms-DRM-Content
        EncodeOptionalByte( KMmsAssignedDrmContent, iMmsHeaders->DrmContent() );
        // X-Mms-Adaptation-Allowed
        EncodeOptionalByte( KMmsAssignedAdaptationAllowed, iMmsHeaders->AdaptationAllowed() );
        }

    // Get Root TMsvId from headers if set
    TMsvAttachmentId rootId = iMmsHeaders->MessageRoot();

    TInt error = KErrNone;
    // If we are handling headers only, we may not have iEntryWrapper
    if ( iEntryWrapper )
        {
        CMsvStore* store = iEntryWrapper->ReadStoreL();
        CleanupStack::PushL( store );
        MMsvAttachmentManager& attachMan = store->AttachmentManagerL();
        TRAP( error,
            {
            CMsvAttachment* attachmentInfo = attachMan.GetAttachmentInfoL(
                (TMsvAttachmentId) rootId );
            delete attachmentInfo; // we just wanted to check that it was found
            }
            );
        CleanupStack::PopAndDestroy( store );
        }
        
    // If we have a valid root part, we send the message as multipart/related.
    // Otherwise we send it as multipart mixed.
    // We don't try to guess what the root is supposed to be, if it
    // is not specified or does not point to an existing attachment.

    if ( error == KErrNone && rootId != 0 && iEntryWrapper )
        {
        EncodeMultipartRelatedHeaderL( rootId );
        }
    else
        {
        EncodeMultipartMixedHeaderL();
        }
        
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CMmsEncode::EncodeNotifyResponse()
    {

    // version may be current version or 1.0
    // Only byte is used. iMmsHeaders controls the value.

    // Insert message type, TID and version number
    EncodeStartingHeaders( KMmsMessageTypeMNotifyRespInd, iMmsHeaders->Tid(),
        iMmsHeaders->MmsVersion() );

    // insert message status
    EncodeMandatoryByte( KMmsAssignedStatus, iMmsHeaders->Status() );

    // insert report allowed if present
    EncodeOptionalByte( KMmsAssignedReportAllowed,
        iMmsHeaders->ReportAllowed() );

    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CMmsEncode::EncodeAcknowledgeIndication()
    {
    // Insert message type, TID and version number
    EncodeStartingHeaders( KMmsMessageTypeAcknowledgeInd,
        iMmsHeaders->Tid(), iMmsHeaders->MmsVersion() );

    // insert report allowed if present
    EncodeOptionalByte( KMmsAssignedReportAllowed,
        iMmsHeaders->ReportAllowed() );
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CMmsEncode::EncodeMmsNotificationL()
    {

    // Insert message type, TID and version number
    EncodeStartingHeaders( KMmsMessageTypeMNotificationInd,
        iMmsHeaders->Tid(), iMmsHeaders->MmsVersion() );

    // for notification sender is optional - added only if it is known
    if ( iMmsHeaders->Sender().Length() > 0 )
        {
        EncodeSenderL( iMmsHeaders->Sender() );
        }

    EncodeOptionalStringL( KMmsAssignedSubject, iMmsHeaders->Subject() );

    EncodeOptionalByte( KMmsAssignedDeliveryReport,
        iMmsHeaders->DeliveryReport() );

    EncodeOptionalByte( KMmsAssignedMessageClass,
        iMmsHeaders->MessageClass() );

    iEncodeBuffer->Write( iPosition, &KMmsAssignedMessageSize, 1 );
    iPosition++;

    TInt64 size = iMmsHeaders->MessageSize();
    EncodeLongInteger( size );

    EncodeOptionalIntervalOrDate( KMmsAssignedExpiry,
        iMmsHeaders->ExpiryInterval(),
        iMmsHeaders->ExpiryDate() );

    // version 1.1 fields - optional
    // reply-charging
    // reply-charging deadline
    // reply-charging size
    // reply-charging ID
    if ( iMmsHeaders->MmsVersion() > KMmsVersion1 )
        {
        // Do we want to specify
        if ( iMmsHeaders->ReplyCharging() == KMmsReplyChargingAccepted ||
            iMmsHeaders->ReplyCharging() == KMmsReplyChargingAcceptedTextOnly )
            {
            // Reply charging value
            EncodeOptionalByte( KMmsAssignedReplyCharging,
                iMmsHeaders->ReplyCharging() );
            // Reply charging deadline
            EncodeOptionalIntervalOrDate( KMmsAssignedReplyChargingDeadline,
                iMmsHeaders->ReplyChargingInterval(),
                iMmsHeaders->ReplyChargingDate() );
            // Reply charging size
            EncodeReplyChargingSize( iMmsHeaders->ReplyChargingSize() );
            }
        // if this is a reply, we may specify the ID
        // Reply charging ID
        EncodeOptionalString( KMmsAssignedReplyChargingID,
            iMmsHeaders->ReplyChargingId() );
        }

    // version 1.2 headers - optional
    if ( iMmsHeaders->MmsVersion() > KMmsVersion11 )
        {
        // MMBox headers
        if ( iMmsHeaders->ReadOnlyMMBoxMessageHeaders() )
            {
            EncodeOptionalByte( KMmsAssignedStored,
                iMmsHeaders->ReadOnlyMMBoxMessageHeaders()->MmsStored() );
            }

        // Distribution indicator
        EncodeOptionalByte( KMmsAssignedDistributionIndicator,
            iMmsHeaders->DistributionIndicator() );
        }

    // Element descriptor
    // This is somewhat a mystery - not implemented
    
    // version 1.3 headers - optional
    if ( iMmsHeaders->MmsVersion() > KMmsVersion12 )
        {
        // X-Mms-Applic-ID, X-Mms-Reply-Applic-ID and X-Mms-Aux-Applic-Info
        EncodeApplicationHeadersL();
        // X-Mms-Recommended-Retrieval-Mode
        EncodeOptionalByte( KMmsAssignedRecommendedRetrievalMode,
            iMmsHeaders->RecommendedRetrievalMode() );
        // X-Mms-Recommended-Retrieval-Mode-Text
        EncodeOptionalStringL( KMmsAssignedRecommendedRetrievalModeText,
            iMmsHeaders->RecommendedRetrievalModeText() );
        // X-Mms-Content-Class
        EncodeOptionalByte( KMmsAssignedContentClass, iMmsHeaders->ContentClass() );
        // X-Mms-DRM-Content
        EncodeOptionalByte( KMmsAssignedDrmContent, iMmsHeaders->DrmContent() );
        // X-Mms-Replace-ID
        EncodeOptionalString( KMmsAssignedReplaceId, iMmsHeaders->ReplaceCancelId() );
        }

    // We keep this as the last one, though it does not really matter
    // Mandatory content-location
    // this must be USASCII or URL encoded
    EncodeHeaderAndTextString( KMmsAssignedContentLocation,
        iMmsHeaders->ContentLocation() );

    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CMmsEncode::EncodeSendConfirmationL()
    {
// implemented for test purposes
#ifdef __WINS__
    // Insert message type, TID and version number
    EncodeStartingHeaders( KMmsMessageTypeMSendConf,
        iMmsHeaders->Tid(), iMmsHeaders->MmsVersion() );

    EncodeMandatoryByte( KMmsAssignedResponseStatus,
        iMmsHeaders->ResponseStatus() );

    EncodeOptionalStringL( KMmsAssignedResponseText,
        iMmsHeaders->ResponseText() );

    // if send confirmation returns error, it might not contain message id
    EncodeOptionalString( KMmsAssignedMessageId, iMmsHeaders->MessageId() );

    // MMBox related headers - optional
    if ( iMmsHeaders->ReadOnlyMMBoxMessageHeaders() )
        {
        EncodeOptionalByte( KMmsAssignedStoreStatus,
            iMmsHeaders->MMBoxMessageHeadersL().MmsStoreStatus() );
        EncodeOptionalStringL( KMmsAssignedStoreStatusText,
            iMmsHeaders->MMBoxMessageHeadersL().MmsStoreStatusText() );
        if ( iMmsHeaders->MMBoxMessageHeadersL().MmsStoreStatus() ==
            KMmsStatusOk )
            {
            // this must be USASCII or URL encoded
            // If message was stored to MMBox, this is mandatory
            EncodeHeaderAndTextString( KMmsAssignedContentLocation,
                iMmsHeaders->ContentLocation() );
            }
        }
    
#endif

    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CMmsEncode::EncodeRetrieveConfirmationL()
    {
// implemented for test purposes
#ifdef __WINS__

    // Insert message type, TID and version number
    EncodeStartingHeaders( KMmsMessageTypeMRetrieveConf,
        iMmsHeaders->Tid(), iMmsHeaders->MmsVersion() );

    // message id
    EncodeOptionalString( KMmsAssignedMessageId, iMmsHeaders->MessageId() );

    // if we have set a date, insert it
    EncodeDate( iMmsHeaders->Date() );

    // If we have set the sender, encode it
    if ( iMmsHeaders->Sender().Length() > 0 )
        {
        EncodeSenderL( iMmsHeaders->Sender() );
        }

    // previously sent by array
    TInt i;
    TUint oldPosition;
    TUint length;
    for ( i = 0; i < iMmsHeaders->PreviouslySentList().Count(); ++i )
        {
        oldPosition = iPosition;
        length = 0;
        // Encode once just to find out filed length
        EncodeInteger( iMmsHeaders->PreviouslySentList()[i]->Order() );
        EncodeAddressL( iMmsHeaders->PreviouslySentList()[i]->Sender() );
        length += iPosition - oldPosition;
        iPosition = oldPosition;
        iEncodeBuffer->Write( iPosition, &KMmsAssignedPreviouslySentBy, 1 );
        iPosition++;
        EncodeValueLength( length );
        // now we have added length, do the actual encoding
        EncodeInteger( iMmsHeaders->PreviouslySentList()[i]->Order() );
        EncodeAddressL( iMmsHeaders->PreviouslySentList()[i]->Sender() );
        oldPosition = iPosition;
        length = 0;
        EncodeInteger( iMmsHeaders->PreviouslySentList()[i]->Order() );
        EncodeLongInteger( iMmsHeaders->PreviouslySentList()[i]->Date() );
        length += iPosition - oldPosition;
        iPosition = oldPosition;
        iEncodeBuffer->Write( iPosition, &KMmsAssignedPreviouslySentDate, 1 );
        iPosition++;
        EncodeValueLength( length );
        EncodeInteger( iMmsHeaders->PreviouslySentList()[i]->Order() );
        EncodeLongInteger( iMmsHeaders->PreviouslySentList()[i]->Date() );
        }

    // All recipient lists. If no recipients in some of the lists,
    // nothing is encoded, no need to check separately
    EncodeRecipientL( iMmsHeaders->ToRecipients(), EMmsTo );
    EncodeRecipientL( iMmsHeaders->CcRecipients(), EMmsCc );
    // no Bcc in retrieve confirmation

    // Subject
    EncodeOptionalStringL( KMmsAssignedSubject, iMmsHeaders->Subject() );

    // MMBox headers
    if ( iMmsHeaders->ReadOnlyMMBoxMessageHeaders() )
        {
        // X-Mms-MM-State
        EncodeOptionalByte( KMmsAssignedMMState,
            iMmsHeaders->ReadOnlyMMBoxMessageHeaders()->MmsMMState() );
        // X-MMs-MM-Flags
        // The keyword token should always be "add" or "remove"
        EncodeKeywordArrayL();
        }

    // Message class
    EncodeOptionalByte( KMmsAssignedMessageClass,
        iMmsHeaders->MessageClass() );

    // Priority
    EncodeOptionalByte( KMmsAssignedPriority, iMmsHeaders->MessagePriority() );
    
    // Delivery report
    EncodeOptionalByte( KMmsAssignedDeliveryReport,
        iMmsHeaders->DeliveryReport() );

    // Read reply
    EncodeOptionalByte( KMmsAssignedReadReply, iMmsHeaders->ReadReply() );

    // MMS encapsulation 1.1 headers - optional
    if ( iMmsHeaders->MmsVersion() > KMmsVersion1 )
        {
        // Do we want to specify reply charging
        if ( iMmsHeaders->ReplyCharging() == KMmsReplyChargingAccepted ||
            iMmsHeaders->ReplyCharging() == KMmsReplyChargingAcceptedTextOnly )
            {
            // Reply charging value
            EncodeOptionalByte( KMmsAssignedReplyCharging,
                iMmsHeaders->ReplyCharging() );
            // Reply charging deadline
            EncodeOptionalIntervalOrDate( KMmsAssignedReplyChargingDeadline,
                iMmsHeaders->ReplyChargingInterval(),
                iMmsHeaders->ReplyChargingDate() );
            // Reply charging size
            EncodeReplyChargingSize( iMmsHeaders->ReplyChargingSize() );
            }
        // Reply charging ID
        EncodeOptionalString( KMmsAssignedReplyChargingID,
            iMmsHeaders->ReplyChargingId() );
        // status of the operation
        EncodeOptionalByte( KMmsAssignedRetrieveStatus,
            iMmsHeaders->ResponseStatus() );

        EncodeOptionalStringL( KMmsAssignedRetrieveText,
            iMmsHeaders->ResponseText() );
        }

    // version 1.2 headers - optional
    if ( iMmsHeaders->MmsVersion() > KMmsVersion11 )
        {
        // Distribution indicator
        EncodeOptionalByte( KMmsAssignedDistributionIndicator,
            iMmsHeaders->DistributionIndicator() );
        }

    // version 1.3 headers - optional
    if ( iMmsHeaders->MmsVersion() > KMmsVersion12 )
        {
        // X-Mms-Applic-ID, X-Mms-Reply-Applic-ID and X-Mms-Aux-Applic-Info
        EncodeApplicationHeadersL();
        // X-Mms-Content-Class
        EncodeOptionalByte( KMmsAssignedContentClass, iMmsHeaders->ContentClass() );
        // X-Mms-DRM-Content
        EncodeOptionalByte( KMmsAssignedDrmContent, iMmsHeaders->DrmContent() );
        // X-Mms-Replace-ID
        EncodeOptionalString( KMmsAssignedReplaceId, iMmsHeaders->ReplaceCancelId() );
        }

    // Start pointer
    TUint ii = 0;
    // Get Root TMsvId from headers if set
    TMsvId rootId = iMmsHeaders->MessageRoot();

    TInt error = KErrNone;
    if ( iEntryWrapper )
        {
        CMsvStore* store = iEntryWrapper->ReadStoreL();
        CleanupStack::PushL( store );
        MMsvAttachmentManager& attachMan = store->AttachmentManagerL();
        TRAP( error,
            {
            CMsvAttachment* attachmentInfo = attachMan.GetAttachmentInfoL(
                (TMsvAttachmentId) rootId );
            delete attachmentInfo; // we just wanted to check that it was found
            }
            );
        CleanupStack::PopAndDestroy( store );
        }
        
    // If we have a valid root part, we send the message as multipart/related.
    // Otherwise we send it as multipart mixed.
    // We don't try to guess what the root is supposed to be, if it
    // is not specified or does not point to an existing attachment.

    if ( error == KErrNone && rootId != 0 && iEntryWrapper )
        {
        EncodeMultipartRelatedHeaderL( rootId );
        }
    else
        {
        EncodeMultipartMixedHeaderL();
        }

    
#endif
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CMmsEncode::EncodeDeliveryReportL()
    {
// implemented for test purposes
#ifdef __WINS__

    // Insert message type, TID and version number
    EncodeStartingHeaders( KMmsMessageTypeDeliveryInd, TPtrC8(),
        iMmsHeaders->MmsVersion() );

    EncodeHeaderAndTextString( KMmsAssignedMessageId,
        iMmsHeaders->MessageId() );

    EncodeRecipientL( iMmsHeaders->ToRecipients(), EMmsTo );

    // if we have set a date, insert it
    EncodeDate( iMmsHeaders->Date() );

    // insert message status
    EncodeMandatoryByte( KMmsAssignedStatus, iMmsHeaders->Status() );
    
    // version 1.3 headers - optional
    if ( iMmsHeaders->MmsVersion() > KMmsVersion12 )
        {
        // X-Mms-Status-Text
        // Status text is stored in the same place as response text and retrieve
        // text as only one of them can appear in any PDU
        EncodeOptionalStringL( KMmsAssignedStatusText,
            iMmsHeaders->ResponseText() );
        // X-Mms-Applic-ID, X-Mms-Reply-Applic-ID and X-Mms-Aux-Applic-Info
        EncodeApplicationHeadersL();
        }
    
#endif
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CMmsEncode::EncodeForwardRequestL()
    {
    // Insert message type, TID and version number
    EncodeStartingHeaders( KMmsMessageTypeForwardReq,
        iMmsHeaders->Tid(), iMmsHeaders->MmsVersion() );

    // if we have set a date, insert it
    EncodeDate( iMmsHeaders->Date() );

    // If we have set the sender, encode it
    // If not, we ask MMSC to add the sender
    EncodeSenderL( iMmsHeaders->Sender() );

    // All recipient lists. If no recipients in some of the lists,
    // nothing is encoded, no need to check separately
    EncodeRecipientL( iMmsHeaders->ToRecipients(), EMmsTo );
    EncodeRecipientL( iMmsHeaders->CcRecipients(), EMmsCc );
    EncodeRecipientL( iMmsHeaders->BccRecipients(), EMmsBcc );

    // Expiration time
    EncodeOptionalIntervalOrDate( KMmsAssignedExpiry,
        iMmsHeaders->ExpiryInterval(),
        iMmsHeaders->ExpiryDate() );

    // Delivery time
    EncodeOptionalIntervalOrDate( KMmsAssignedDeliveryTime,
        iMmsHeaders->DeliveryTimeInterval(),
        iMmsHeaders->DeliveryDate() );

    // Delivery report allowed
    EncodeOptionalByte( KMmsAssignedReportAllowed,
        iMmsHeaders->ReportAllowed() );

    // Delivery report
    EncodeOptionalByte( KMmsAssignedDeliveryReport,
        iMmsHeaders->DeliveryReport() );

    // Read reply
    EncodeOptionalByte( KMmsAssignedReadReply, iMmsHeaders->ReadReply() );

    // version 1.2 headers - optional
    if ( iMmsHeaders->MmsVersion() > KMmsVersion11 )
        {
        // MMBox related headers - optional
        if ( iMmsHeaders->ReadOnlyMMBoxMessageHeaders() )
            {
            // X-Mms-MM-Store
            EncodeOptionalByte( KMmsAssignedMmsStore,
                iMmsHeaders->ReadOnlyMMBoxMessageHeaders()->MmsStore() );
            if ( iMmsHeaders->ReadOnlyMMBoxMessageHeaders()->MmsStore() == KMmsYes ||
                 iMmsHeaders->ReadOnlyMMBoxMessageHeaders()->MmsStored() == KMmsYes )
                {
                // X-Mms-MM-State
                EncodeOptionalByte( KMmsAssignedMMState,
                    iMmsHeaders->ReadOnlyMMBoxMessageHeaders()->MmsMMState() );
                // X-MMs-MM-Flags
                // The keyword token should always be "add" or "remove"
                EncodeKeywordArrayL();
                }
            }
        }
        
    // version 1.3 headers - optional
    if ( iMmsHeaders->MmsVersion() > KMmsVersion12 )
        {
        // Do we want to specify reply charging
        if ( iMmsHeaders->ReplyCharging() == KMmsReplyChargingRequested ||
            iMmsHeaders->ReplyCharging() == KMmsReplyChargingRequestedTextOnly )
            {
            // Reply charging value
            EncodeOptionalByte( KMmsAssignedReplyCharging,
                iMmsHeaders->ReplyCharging() );
            // Reply charging deadline
            EncodeOptionalIntervalOrDate( KMmsAssignedReplyChargingDeadline,
                iMmsHeaders->ReplyChargingInterval(),
                iMmsHeaders->ReplyChargingDate() );
            // Reply charging size
            EncodeReplyChargingSize( iMmsHeaders->ReplyChargingSize() );
            }
        }

    // Mandatory content-location
    // this must be USASCII or URL encoded
    EncodeHeaderAndTextString( KMmsAssignedContentLocation,
        iMmsHeaders->ContentLocation() );

    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CMmsEncode::EncodeForwardConfirmationL()
    {
// implemented for test purposes
#ifdef __WINS__
    // Insert message type, TID and version number
    EncodeStartingHeaders( KMmsMessageTypeForwardConf,
        iMmsHeaders->Tid(), iMmsHeaders->MmsVersion() );

    EncodeMandatoryByte( KMmsAssignedResponseStatus,
        iMmsHeaders->ResponseStatus() );

    EncodeOptionalStringL( KMmsAssignedResponseText,
        iMmsHeaders->ResponseText() );

    EncodeOptionalString( KMmsAssignedMessageId, iMmsHeaders->MessageId() );

    // MMBox related headers - optional
    if ( iMmsHeaders->ReadOnlyMMBoxMessageHeaders() )
        {
        EncodeOptionalByte( KMmsAssignedStoreStatus,
            iMmsHeaders->MMBoxMessageHeadersL().MmsStoreStatus() );
        EncodeOptionalStringL( KMmsAssignedStoreStatusText,
            iMmsHeaders->MMBoxMessageHeadersL().MmsStoreStatusText() );
        if ( iMmsHeaders->MMBoxMessageHeadersL().MmsStoreStatus() ==
            KMmsStatusOk )
            {
            // this must be USASCII or URL encoded
            // If message was stored to MMBox, this is mandatory
            EncodeHeaderAndTextString( KMmsAssignedContentLocation,
                iMmsHeaders->ContentLocation() );
            }
        }
    
#endif
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CMmsEncode::EncodeReadReplyL()
    {
    // Insert message type, TID and version number
    EncodeStartingHeaders( iMmsHeaders->MessageType(), TPtrC8(), iMmsHeaders->MmsVersion() );

    // message id
    EncodeHeaderAndTextString( KMmsAssignedMessageId,
        iMmsHeaders->MessageId() );

    // originator of the original message
    EncodeRecipientL( iMmsHeaders->ToRecipients(), EMmsTo );

    // If we have set the sender, encode it
    // If not, we ask MMSC to add the sender
    EncodeSenderL( iMmsHeaders->Sender() );

    // if we have set a date, insert it
    EncodeDate( iMmsHeaders->Date() );

    EncodeMandatoryByte( KMmsAssignedReadStatus, iMmsHeaders->ReadStatus() );
    
    // version 1.3 headers - optional
    if ( iMmsHeaders->MmsVersion() > KMmsVersion12 )
        {
        // X-Mms-Applic-ID, X-Mms-Reply-Applic-ID and X-Mms-Aux-Applic-Info
        EncodeApplicationHeadersL();
        }
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CMmsEncode::EncodeMMBoxStoreRequestL()
    {
    // Insert message type, TID and version number
    EncodeStartingHeaders( KMmsMessageTypeMboxStoreReq, iMmsHeaders->Tid(),
        iMmsHeaders->MmsVersion() );

    // this must be USASCII or URL encoded
    EncodeHeaderAndTextString( KMmsAssignedContentLocation,
        iMmsHeaders->ContentLocation() );

    // MMBox headers
    if ( iMmsHeaders->ReadOnlyMMBoxMessageHeaders() )
        {
        // X-Mms-MM-State
        EncodeOptionalByte( KMmsAssignedMMState,
            iMmsHeaders->ReadOnlyMMBoxMessageHeaders()->MmsMMState() );
        // X-MMs-MM-Flags
        // The keyword token should always be "add" or "remove"
        EncodeKeywordArrayL();
        }
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CMmsEncode::EncodeMMBoxStoreConfirmationL()
    {
// implemented for test purposes
#ifdef __WINS__
    // Insert message type, TID and version number
    EncodeStartingHeaders( KMmsMessageTypeMboxStoreConf, iMmsHeaders->Tid(),
        iMmsHeaders->MmsVersion() );

    // this must be USASCII or URL encoded
    EncodeOptionalString( KMmsAssignedContentLocation,
        iMmsHeaders->ContentLocation() );

    EncodeMandatoryByte( KMmsAssignedStoreStatus,
        iMmsHeaders->MMBoxMessageHeadersL().MmsStoreStatus() );

    EncodeOptionalStringL( KMmsAssignedStoreStatusText,
        iMmsHeaders->MMBoxMessageHeadersL().MmsStoreStatusText() );

#endif
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CMmsEncode::EncodeMMBoxViewRequestL()
    {
    // Insert message type, TID and version number
    EncodeStartingHeaders( KMmsMessageTypeMboxViewReq, iMmsHeaders->Tid(),
        iMmsHeaders->MmsVersion() );

    // Insert content location header. There may be more than one
    EncodeContentLocationArray();

    if ( iMmsHeaders->ReadOnlyMMBoxMessageHeaders() )
        {
        // X-MMs-MM-Flags. The token should always be "filter"
        EncodeKeywordArrayL();
        }

    if ( iMmsHeaders->ReadOnlyMMBoxViewHeaders() )
        {
        CMmsMMBoxViewHeaders& viewHeaders = iMmsHeaders->MMBoxViewHeadersL();
        
        // states used as filter
        // X-Mms-MM-State may appear multiple times in view request
        EncodeMMBoxStates( viewHeaders.MMStateArray() ); 

        // start
        EncodeOptionalInteger( KMmsAssignedStart, viewHeaders.MmsStart() );
        // limit
        if ( viewHeaders.MmsLimit() != KMaxTUint32 )
            {
            // 0 means no message information, absence means all remaining
            // messages.
            // If header has value KMaxTUint32, it means that info about all
            // remaining messages is wanted.
            iEncodeBuffer->Write( iPosition, &KMmsAssignedLimit, 1 );
            iPosition++;
            EncodeInteger( viewHeaders.MmsLimit() );
            }

        EncodeAttributes( viewHeaders.AttributeArray() );
        // request totals
        EncodeOptionalByte( KMmsAssignedTotals, viewHeaders.MmsTotals() );
        // request quotas
        EncodeOptionalByte( KMmsAssignedQuotas, viewHeaders.MmsQuotas() );
        }
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CMmsEncode::EncodeMMBoxViewConfirmationL()
    {
// implemented for test purposes
#ifdef __WINS__
    // Insert message type, TID and version number
    EncodeStartingHeaders( KMmsMessageTypeMboxViewConf,
        iMmsHeaders->Tid(), iMmsHeaders->MmsVersion() );

    EncodeMandatoryByte( KMmsAssignedResponseStatus,
        iMmsHeaders->ResponseStatus() );

    EncodeOptionalStringL( KMmsAssignedResponseText,
        iMmsHeaders->ResponseText() );

    EncodeContentLocationArray();

    // MMBox headers
    if ( iMmsHeaders->ReadOnlyMMBoxMessageHeaders() )
        {
        // X-MMs-MM-Flags. The token should always be "filter"
        EncodeKeywordArrayL();
        }
    
    if ( iMmsHeaders->ReadOnlyMMBoxViewHeaders() )
        {
        CMmsMMBoxViewHeaders& viewHeaders = iMmsHeaders->MMBoxViewHeadersL();

        // states used as filter
        // X-Mms-MM-State may appear multiple times in view request
        EncodeMMBoxStates( viewHeaders.MMStateArray() ); 

        // start
        EncodeOptionalInteger( KMmsAssignedStart, viewHeaders.MmsStart() );
        // limit
        if ( viewHeaders.MmsLimit() != KMaxTUint32 )
            {
            // 0 means no message information, absence means all remaining
            // messages.
            // If header has value KMaxTUint32, it means that info about
            // all remaining messages is wanted.
            iEncodeBuffer->Write( iPosition, &KMmsAssignedLimit, 1 );
            iPosition++;
            EncodeInteger( viewHeaders.MmsLimit() );
            }
        EncodeAttributes( viewHeaders.AttributeArray() );

        // totals
        TUint oldPosition = iPosition;
        TUint length = 0;

        if ( viewHeaders.MMBoxTotalNumber() != KMaxTUint32 )
            {
            oldPosition = iPosition;
            EncodeInteger( viewHeaders.MMBoxTotalNumber() );
            length = iPosition - oldPosition;
            length += 1; // quota token
            iPosition = oldPosition;
            iEncodeBuffer->Write( iPosition, &KMmsAssignedMboxTotals, 1 );
            iPosition++;
            EncodeValueLength( length );
            iEncodeBuffer->Write( iPosition, &KMmsMessageCountToken, 1 );
            iPosition++;
            EncodeInteger( viewHeaders.MMBoxTotalNumber() );
            }

        if ( viewHeaders.MMBoxTotalSize() != KMaxTUint32 )
            {
            oldPosition = iPosition;
            EncodeInteger( viewHeaders.MMBoxTotalSize() );
            length = iPosition - oldPosition;
            length += 1; // quota token
            iPosition = oldPosition;
            iEncodeBuffer->Write( iPosition, &KMmsAssignedMboxTotals, 1 );
            iPosition++;
            EncodeValueLength( length );
            iEncodeBuffer->Write( iPosition, &KMmsMessageSizeToken, 1 );
            iPosition++;
            EncodeInteger( viewHeaders.MMBoxTotalSize() );
            }

        // quotas
        if ( viewHeaders.MMBoxQuotaNumber() != KMaxTUint32 )
            {
            oldPosition = iPosition;
            EncodeInteger( viewHeaders.MMBoxQuotaNumber() );
            length = iPosition - oldPosition;
            length += 1; // quota token
            iPosition = oldPosition;
            iEncodeBuffer->Write( iPosition, &KMmsAssignedMboxQuotas, 1 );
            iPosition++;
            EncodeValueLength( length );
            iEncodeBuffer->Write( iPosition, &KMmsMessageCountToken, 1 );
            iPosition++;
            EncodeInteger( viewHeaders.MMBoxQuotaNumber() );
            }

        if ( viewHeaders.MMBoxQuotaSize() != KMaxTUint32 )
            {
            oldPosition = iPosition;
            EncodeInteger( viewHeaders.MMBoxQuotaSize() );
            length = iPosition - oldPosition;
            length += 1; // quota token
            iPosition = oldPosition;
            iEncodeBuffer->Write( iPosition, &KMmsAssignedMboxQuotas, 1 );
            iPosition++;
            EncodeValueLength( length );
            iEncodeBuffer->Write( iPosition, &KMmsMessageSizeToken, 1 );
            iPosition++;
            EncodeInteger( viewHeaders.MMBoxQuotaSize() );
            }

        // Message count
        // This should in principle come from the viewHeaders.MmsMessageCount()
        // But in this context the number of messages will be the number of
        // attachments available in this entry

        // However, we will use the viewHeaders.MmsMessageCount() to generate
        // the header so that we can test that the decoding part can handle
        // possible conflict between message number and the number of parts
        // in the multipart structure. This header is optional. KMaxTUint32
        // will mean unspecified.

        if ( viewHeaders.MmsMessageCount() != KMaxTUint32 )
            {
            iEncodeBuffer->Write( iPosition, &KMmsAssignedMessageCount, 1 );
            iPosition++;
            EncodeInteger( viewHeaders.MmsMessageCount() );
            }
        }

    if ( iNumberOfAttachments == 0 )
        {
        EncodeMandatoryByte( KMmsAssignedContentType, KMmsAssignedAny );
        }
    else
        {
        EncodeMultipartMixedHeaderL();
        }
#endif
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CMmsEncode::EncodeMMBoxUploadRequestL()
    {
    // Insert message type, TID and version number
    EncodeStartingHeaders( KMmsMessageTypeMBoxUploadReq, iMmsHeaders->Tid(),
        iMmsHeaders->MmsVersion() );

    // MMBox headers
    if ( iMmsHeaders->ReadOnlyMMBoxMessageHeaders() )
        {
        // X-Mms-MM-State
        EncodeOptionalByte( KMmsAssignedMMState,
            iMmsHeaders->ReadOnlyMMBoxMessageHeaders()->MmsMMState() );
        // X-MMs-MM-Flags
        // The keyword token should always be "add" or "remove"
        EncodeKeywordArrayL();
        }

    // Get Root TMsvId from headers if set
    TMsvAttachmentId rootId = iMmsHeaders->MessageRoot();

    // Find the matching id from attachment list
    
    TInt error = KErrNone;
    if ( iEntryWrapper )
        {
        CMsvStore* store = iEntryWrapper->ReadStoreL();
        CleanupStack::PushL( store );
        // Only new attachment structure is supported    
        MMsvAttachmentManager& attachMan = store->AttachmentManagerL();
        TRAP( error,
            {
            CMsvAttachment* attachmentInfo = attachMan.GetAttachmentInfoL(
                (TMsvAttachmentId) rootId );
            delete attachmentInfo; // we just wanted to check that it was found
            }
            );
        CleanupStack::PopAndDestroy( store );
        }
    
    // If we have a valid root part, we send the message as multipart/related.
    // Otherwise we send it as multipart mixed.
    // We don't try to guess what the root is supposed to be, if it
    // is not specified or does not point to an existing attachment.

    // actually the content should be an MMS PDU as in M-MBox-Descr PDU
    // It is unclear if if should be wrapped in a multipart structure 
    // (containing only one header)
    // or if the content type should be application/vnd.wap.mms-message
    if ( error == KErrNone && rootId != 0 && iEntryWrapper )
        {
        EncodeMultipartRelatedHeaderL( rootId );
        }
    else
        {
        EncodeMultipartMixedHeaderL();
        }

    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CMmsEncode::EncodeMMBoxUploadConfirmationL()
    {
// implemented for test purposes
#ifdef __WINS__
    // Insert message type, TID and version number
    EncodeStartingHeaders( KMmsMessageTypeMBoxUploadConf, iMmsHeaders->Tid(),
        iMmsHeaders->MmsVersion() );

    // this must be USASCII or URL encoded
    EncodeOptionalString( KMmsAssignedContentLocation,
        iMmsHeaders->ContentLocation() );

    EncodeMandatoryByte( KMmsAssignedStoreStatus,
        iMmsHeaders->MMBoxMessageHeadersL().MmsStoreStatus() );

    EncodeOptionalStringL( KMmsAssignedStoreStatusText,
        iMmsHeaders->MMBoxMessageHeadersL().MmsStoreStatusText() );
#endif
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CMmsEncode::EncodeDeleteRequestL()
    {
    // Insert message type, TID and version number
    
    EncodeStartingHeaders( iMmsHeaders->MessageType(), iMmsHeaders->Tid(),
        iMmsHeaders->MmsVersion() );

    // Insert content location header. There may be more than one
    EncodeContentLocationArray();
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CMmsEncode::EncodeDeleteConfirmationL()
    {
// implemented for test purposes
#ifdef __WINS__
    // Insert message type, TID and version number
    EncodeStartingHeaders( iMmsHeaders->MessageType(), iMmsHeaders->Tid(),
        iMmsHeaders->MmsVersion() );

    // content location, response status and response text headers have
    // different format from other PDUs

    const RPointerArray<CMmsDeleteResultArray>& resultArray = 
        iMmsHeaders->DeleteResultArray();

    TInt i;
    TInt length = 0;
    TInt oldPosition = 0;
    TUint oldIndex = KMaxTUint32;

    for ( i = 0; i < resultArray.Count(); i++ )
        {
        TUint index = resultArray[i]->Order();
        if ( index != oldIndex )
            {
            // We add status and status text only once, there may be more
            // than one content location with the same status
            oldIndex = index;
            if ( resultArray[i]->ResponseStatus() != 0 )
                {
                oldPosition = iPosition;
                length = 0;
                // Encode once just to find out field length
                EncodeInteger( index );
                length += iPosition - oldPosition;
                length ++; // response status is only one byte
                iPosition = oldPosition;
                // now we have calculated length, do the actual encoding
                iEncodeBuffer->Write( iPosition, &KMmsAssignedResponseStatus, 1 );
                iPosition++;
                EncodeValueLength( length );
                EncodeInteger( index );
                TUint8 value = (TUint8) resultArray[i]->ResponseStatus();
                iEncodeBuffer->Write( iPosition, &value, 1 );
                iPosition++;
                }
            if ( resultArray[i]->ResponseText().Length() > 0 )
                {
                oldPosition = iPosition;
                length = 0;
                // Encode once just to find out field length
                EncodeInteger( index );
                EncodeTextStringL( resultArray[i]->ResponseText() );
                length += iPosition - oldPosition;
                iPosition = oldPosition;
                // now we have calculated length, do the actual encoding
                iEncodeBuffer->Write( iPosition, &KMmsAssignedResponseText, 1 );
                iPosition++;
                EncodeValueLength( length );
                EncodeInteger( index );
                EncodeTextStringL( resultArray[i]->ResponseText() );
                }
            }
        if ( resultArray[i]->ContentLocation().Length() > 0 )
            {
            oldPosition = iPosition;
            length = 0;
            // Encode once just to find out field length
            EncodeInteger( index );
            EncodeTextString( resultArray[i]->ContentLocation() );
            length += iPosition - oldPosition;
            iPosition = oldPosition;
            // now we have calculated length, do the actual encoding
            iEncodeBuffer->Write( iPosition, &KMmsAssignedContentLocation, 1 );
            iPosition++;
            EncodeValueLength( length );
            EncodeInteger( index );
            EncodeTextString( resultArray[i]->ContentLocation() );
            }
        }
#endif
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CMmsEncode::EncodeMMBoxDescriptionL()
    {
// implemented for test purposes
#ifdef __WINS__
    EncodeMandatoryByte( KMmsAssignedMessageType, KMmsMessageTypeMBoxDescr );

    // this must be USASCII or URL encoded
    EncodeOptionalString( KMmsAssignedContentLocation,
        iMmsHeaders->ContentLocation() );

    EncodeOptionalString( KMmsAssignedMessageId, iMmsHeaders->MessageId() );

    // MMBox headers
    if ( iMmsHeaders->ReadOnlyMMBoxMessageHeaders() )
        {
        // X-Mms-MM-State
        EncodeOptionalByte( KMmsAssignedMMState,
            iMmsHeaders->ReadOnlyMMBoxMessageHeaders()->MmsMMState() );
        // X-MMs-MM-Flags
        // The keyword token should always be "add" or "remove"
        EncodeKeywordArrayL();
        }

    // if we have set a date, insert it
    EncodeDate( iMmsHeaders->Date() );
    
    // sender is optional
    if ( iMmsHeaders->Sender().Length() > 0 )
        {
        EncodeSenderL( iMmsHeaders->Sender() );
        }

    // All recipient lists. If no recipients in some of the lists,
    // nothing is encoded, no need to check separately
    EncodeRecipientL( iMmsHeaders->ToRecipients(), EMmsTo );
    EncodeRecipientL( iMmsHeaders->CcRecipients(), EMmsCc );
    EncodeRecipientL( iMmsHeaders->BccRecipients(), EMmsBcc );

    // Message class
    EncodeOptionalByte( KMmsAssignedMessageClass,
        iMmsHeaders->MessageClass() );

    // Subject
    EncodeOptionalStringL( KMmsAssignedSubject, iMmsHeaders->Subject() );

    // Priority
    EncodeOptionalByte( KMmsAssignedPriority, iMmsHeaders->MessagePriority() );
    
    // Delivery time
    EncodeOptionalIntervalOrDate( KMmsAssignedDeliveryTime,
        iMmsHeaders->DeliveryTimeInterval(),
        iMmsHeaders->DeliveryDate() );

    // Expiration time
    EncodeOptionalIntervalOrDate( KMmsAssignedExpiry,
        iMmsHeaders->ExpiryInterval(),
        iMmsHeaders->ExpiryDate() );

    // Delivery report
    EncodeOptionalByte( KMmsAssignedDeliveryReport,
        iMmsHeaders->DeliveryReport() );

    // Read reply
    EncodeOptionalByte( KMmsAssignedReadReply, iMmsHeaders->ReadReply() );

    if ( iMmsHeaders->MessageSize() > 0 )
        {
        iEncodeBuffer->Write( iPosition, &KMmsAssignedMessageSize, 1 );
        iPosition++;
        TInt64 size = iMmsHeaders->MessageSize();
        EncodeLongInteger( size );
        }

    // MMBox description appears from 1.2 onwards - no use to check if we are
    // 1.1 or later.
    // Do we want to specify reply charging
    if ( iMmsHeaders->ReplyCharging() == KMmsReplyChargingRequested ||
        iMmsHeaders->ReplyCharging() == KMmsReplyChargingRequestedTextOnly ||
        iMmsHeaders->ReplyCharging() == KMmsReplyChargingAccepted ||
        iMmsHeaders->ReplyCharging() == KMmsReplyChargingAcceptedTextOnly )
        {
        // Reply charging value
        EncodeOptionalByte( KMmsAssignedReplyCharging,
            iMmsHeaders->ReplyCharging() );
        // Reply charging deadline
        EncodeOptionalIntervalOrDate( KMmsAssignedReplyChargingDeadline,
            iMmsHeaders->ReplyChargingInterval(),
            iMmsHeaders->ReplyChargingDate() );
        // Reply charging size
        EncodeReplyChargingSize( iMmsHeaders->ReplyChargingSize() );
        }
    // Reply charging ID
    EncodeOptionalString( KMmsAssignedReplyChargingID,
        iMmsHeaders->ReplyChargingId() );

    TInt i;
    TUint oldPosition;
    TUint length;
    for ( i = 0; i < iMmsHeaders->PreviouslySentList().Count(); ++i )
        {
        oldPosition = iPosition;
        length = 0;
        // Encode once just to find out field length
        EncodeInteger( iMmsHeaders->PreviouslySentList()[i]->Order() );
        EncodeAddressL( iMmsHeaders->PreviouslySentList()[i]->Sender() );
        length += iPosition - oldPosition;
        iPosition = oldPosition;
        iEncodeBuffer->Write( iPosition, &KMmsAssignedPreviouslySentBy, 1 );
        iPosition++;
        EncodeValueLength( length );
        // now we have added length, do the actual encoding
        EncodeInteger( iMmsHeaders->PreviouslySentList()[i]->Order() );
        EncodeAddressL( iMmsHeaders->PreviouslySentList()[i]->Sender() );
        oldPosition = iPosition;
        length = 0;
        EncodeInteger( iMmsHeaders->PreviouslySentList()[i]->Order() );
        EncodeLongInteger( iMmsHeaders->PreviouslySentList()[i]->Date() );
        length += iPosition - oldPosition;
        iPosition = oldPosition;
        iEncodeBuffer->Write( iPosition, &KMmsAssignedPreviouslySentDate, 1 );
        iPosition++;
        EncodeValueLength( length );
        EncodeInteger( iMmsHeaders->PreviouslySentList()[i]->Order() );
        EncodeLongInteger( iMmsHeaders->PreviouslySentList()[i]->Date() );
        }

    if ( iNumberOfAttachments > 0 )
        {
        EncodeMultipartMixedHeaderL();
        }

#endif
    }

// ---------------------------------------------------------
// 8-bit version (no conversions)
// ---------------------------------------------------------
//
void CMmsEncode::EncodeTextString( const TDesC8& aString )
    {
    // Only Content-Id needs a quoted string, for example
    // "<agjrfnjr>
    // content-id encoding uses a separate function that adds the quote.
    // EncodeQuotedTextString

    // Check if we need a Quote (This does not mean the quoted string.)
    TInt length = aString.Length();
    if ( length > 0 && aString[0] >= KMms0x80 )
        {
        iEncodeBuffer->Write( iPosition, &KMmsQuote, 1 );
        iPosition++;
        }

    iEncodeBuffer->Write( iPosition, aString, length );
    iPosition += length;

    iEncodeBuffer->Write( iPosition, &KMmsNull, 1 );
    iPosition++;

    }

// ---------------------------------------------------------
// 8-bit version (no conversions)
// ---------------------------------------------------------
//
void CMmsEncode::EncodeQuotedTextString( const TDesC8& aString )
    {
    // Before this function is called, the caller must check
    // that the length of the string is not 0. Otherwise this
    // makes no sense. You cannot quote a zero-length string

    // We need quoted string for content-id.
    // So far for nothing else.
    // (for the filename in MIME headers, too, just in case)

    // We add one quote to the beginning, nothing to the end.
    // As we start with a quote, we don't need to check for the special
    // Quote that is needed if a string starts with a character above 128
    TInt length = aString.Length();
    iEncodeBuffer->Write( iPosition, &KMmsStringQuote, 1 );
    iPosition++;

    iEncodeBuffer->Write( iPosition, aString, length );
    iPosition += length;

    iEncodeBuffer->Write( iPosition, &KMmsNull, 1 );
    iPosition++;

    }

// ---------------------------------------------------------
// unicode version
// ---------------------------------------------------------
//
void CMmsEncode::EncodeTextStringL( const TDesC& aString )
    {

    // What about a RFC2068 quoted strings - 
    // see explanation in EncodeTextString( const TDesC8& aString ) function

    TInt i = 0;
    TInt length = aString.Length();

    TBool safe = IsStringSafe( aString );

    // If the string can be encoded as ASCII, go ahead
    if ( safe )
        {
        TUint8 character;
        // No need to check if we need a quote - if we are safe, we have 
        // no characters >= 128.

        for ( i = 0; i < aString.Length(); ++i )
            {
            character = TUint8( aString[i] & KMms0xFF );
            iEncodeBuffer->Write( iPosition, &character, 1 );
            iPosition++;
            }
        iEncodeBuffer->Write( iPosition, &KMmsNull, 1 );
        iPosition++;
        }
    else
        {
        // if our length is 0, we are safe, no need to check the 
        // length here anymore
        // we must convert to utf-8
        
        //one ucs-2 character should never produce more than 4 bytes when converted to utf-8
        // coverity[incorrect_multiplication][buffer_alloc]
		HBufC8* buffer = HBufC8::NewL( aString.Length() * KMms4 ); // paranoid.
        // we don't leave while we need buffer
        TPtr8 buf8 = buffer->Des();

        // if conversion fails, something is really seriously wrong
        iError = CnvUtfConverter::ConvertFromUnicodeToUtf8( buf8, aString );

        length = buf8.Length();
        // utf8 character set encoding needs one byte (fits into short integer)
        length += KMms2; // add character set encoding byte and trailing NULL
        
        if ( buf8[0] >= KMms0x80 ) // 128
            {
            length++; // we will need a quote byte at the start...
            }
        
        EncodeValueLength( length );

        // add one byte of character set
        // this is a short integer, high bit must be set
        TUint8 charset = KMmsUtf8 | KMms0x80;

        iEncodeBuffer->Write( iPosition, &charset, 1 );
        iPosition++;

        // Check if we need a quote
        if ( buf8[0] >= KMms0x80 ) // 128
            {
            iEncodeBuffer->Write( iPosition, &KMmsQuote, 1 );
            iPosition++;
            }
        
        // Then write the string itself
        iEncodeBuffer->Write( iPosition, buf8, buf8.Length() );
        iPosition += buf8.Length();

        // Add terminating NULL
        iEncodeBuffer->Write( iPosition, &KMmsNull, 1 );
        iPosition++;

        delete buffer;
        buffer = NULL;
        }
    
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CMmsEncode::EncodeDate( const TInt64& aDate)
    {

    if ( aDate == 0 )
        {
        return;
        }

    iEncodeBuffer->Write( iPosition, &KMmsAssignedDate, 1 );
    iPosition++;

    EncodeLongInteger( aDate );

    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CMmsEncode::EncodeLongInteger( const TInt64& aLongInteger )
    {

    TUint8 length = 0; // number of bytes we will need
    // The long integer will take 8 bytes or less
    TUint8 array[KMms8];

    TInt64 temp = aLongInteger;
    TInt i = 0;
    TInt64 reminder = 0;

    for ( i = 7; i >= 0; --i )
        {
        reminder = temp % 0x100;
        temp = temp / 0x100;
        array[i] = TInt8 ( I64INT( reminder ) ) ;
        }

    length = KMms8;
    i = 0;
    // The array has 8 elements, so this is safe.
    while( ( i < KMms8 ) && ( array[i] == 0 ) )
        {
        i++;
        length--;
        }

    // a zero should be coded as short integer.
    // However, if there is a valid reason to code a zero as a long integer,
    // we allow it. The caller should know what he is doing.
    if ( length == 0 )
        {
        length = 1;
        }

    // write short length
    iEncodeBuffer->Write( iPosition, &length, 1 );
    iPosition++;

    // write as many bytes as were non-zero
    // array index will stay withing limits because of the way it was calculated
    iEncodeBuffer->Write( iPosition, &(array[KMms8 - length] ), length );
    iPosition+= length;

    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CMmsEncode::EncodeInteger( TUint aInteger )
    {
    // KMms1 = 1
    // KMms2 = 2
    // KMms3 = 3
    // KMms4 = 4
    // KMms5 = 5
    // etc.
    
    TUint8 byte = 0;

    if ( aInteger <= KMmsShortIntegerLimit127 )
        {
        byte = ( TInt8 ) aInteger;
        byte |= KMms0x80;
        iEncodeBuffer->Write( iPosition, &byte, 1);
        iPosition++;
        return; // this was easy
        }

    TUint8 length = KMms4; // number of bytes we will need
    TUint8 array[KMms4];

    TUint temp = aInteger;
    byte = TInt8( ( temp >> KMms24 ) & KMms0xFF );

    while ( byte == 0 && length > 0 )
        {
        length--;
        temp = temp << KMms8;
        byte = TInt8( ( temp >> KMms24 ) & KMms0xFF );
        }

    TUint i = 0;
    for ( i = 0; i < length; ++i )
        {
        array[i] = TInt8( ( temp >> ( KMms8 * ( KMms3 - i ) ) ) & KMms0xFF );
        }

    // write short length
    iEncodeBuffer->Write( iPosition, &length, 1 );
    iPosition++;

    // write as many bytes as were non-zero
    // aInteger was tested in the beginning - it needs at least one byte
    // So length is at least 1 and the array has been properly initialized.
    iEncodeBuffer->Write( iPosition, &array[0], length );
    iPosition+= length;

    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CMmsEncode::EncodeSenderL( const TPtrC& aSender )
    {
    
    iEncodeBuffer->Write( iPosition, &KMmsAssignedFrom, 1 );
    iPosition++;

    // We must insert value length.

    if ( aSender.Length() == 0 )
        {
        // The MMSC must insert our address
        TUint8 length;
        length = 1;
        iEncodeBuffer->Write( iPosition, &length, 1 );
        iPosition++;
        iEncodeBuffer->Write( iPosition, &KMmsInsertAddressToken, 1 );
        iPosition++;
        return;
        }
    
    // Now we must insert something
    TUint length = 1; // address present token

    if ( aSender.Find( KMiuMau ) != KErrNotFound )
        {
        TBool safe = IsStringSafe( aSender );
        if ( safe )
            {
            // this was easy, just ASCII, no conversion
            length += aSender.Length();
            length ++; // room for terminating zero
            }
        else // not USASCII, charset must be specified
            {
            // worst case scenario.
            // must be encoded as UTF-8, value length and character set
            // must be added
        
            // one ucs-2 character will not produce more than 4 bytes when converted to utf-8
			// coverity[incorrect_multiplication][buffer_alloc]
            HBufC8* buffer = HBufC8::NewL( aSender.Length() * KMms4 ); // paranoid.
            // we don't need to push buffer onto cleanup stack, as we don't
            // leave while we are using it
            TPtr8 buf8 = buffer->Des();

            // if we get error here, something is badly wrong
            iError = CnvUtfConverter::ConvertFromUnicodeToUtf8( buf8, aSender );

            length += buf8.Length();
            // utf8 character set encoding needs one byte (short integer)
            length += KMms2; // add character set encoding byte and trailing NULL
        
            if ( buf8[0] >= KMms0x80 ) // 128
                {
                length++; // we will need a quote byte at the start...
                }
            delete buffer;
            buffer = NULL;
            }
        }
    else // no miumau found
        {
        // phone number, must be ASCII
        length += aSender.Length();
        length ++; // room for terminating zero
        length += KMmsPlmnLength; // type indication
        // this should be the length, if we have a phone number (no alias.)
        }

    EncodeValueLength( length );

    iEncodeBuffer->Write( iPosition, &KMmsAddressPresentToken, 1 );
    iPosition++;

    // If the address contains some non-ascii characters,
    // it must be converted to utf-8
    
    EncodeAddressL( aSender );

    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CMmsEncode::EncodeAddressL( const TPtrC& aAddress )
    {
    // Supports only address types PLMN and email.
    // If the address string contains a @ character,
    // it is interpreted as an email

    // Internal alias is removed

    TMmsAddressType addressType = EMmsAddressTypeUnknown;
    HBufC* realAddress = HBufC::NewL( aAddress.Length() );
    CleanupStack::PushL( realAddress );

    TInt error = KErrNone;

    TPtr realAddressPointer = realAddress->Des();
    error = TMmsGenUtils::AddressTypeAndRealAddress(
        aAddress,
        addressType,
        realAddressPointer,
        aAddress.Length());

    if ( error != KErrNone || addressType == EMmsAddressTypeUnknown )
        {
        // could not resolve. Send unchanged
        realAddress->Des().Copy( aAddress );
        if ( aAddress.Find( KMiuMau ) != KErrNotFound )
            {
            addressType = EMmsAddressTypeEmail;
            }
        else
            {
            addressType = EMmsAddressTypeMobile;
            }
        }
        
    if ( addressType == EMmsAddressTypeEmail )
        {
        // email address
        // If the address contains only ASCII characters,
        // it can be sent as text string, if not, it must be sent as utf-8

        EncodeTextStringL( *realAddress );

        }
    else
        {
        // must be a phone number
        // We expect for now that the format is correct as is
        // All legal characters present in a phone number are ASCII

        TInt i = 0;
        TUint8 character = 0;
        realAddressPointer = realAddress->Des();
        for ( i = 0; i < realAddress->Length(); ++i )
            {
            // The array index is safe because i is always < realAddress->Length().
            character = TUint8( realAddressPointer[i] & KMms0xFF );
            iEncodeBuffer->Write( iPosition, &character, 1 );
            iPosition++;
            }
        iEncodeBuffer->Write( iPosition, KMmsPlmn, KMmsPlmnLength );
        iPosition += KMmsPlmnLength;
        iEncodeBuffer->Write( iPosition, &KMmsNull, 1 );
        iPosition++;
        }
    CleanupStack::PopAndDestroy( realAddress );

    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CMmsEncode::EncodeValueLength( TUint aLength )
    {

    TUint8 shortLength = 0;

    if ( aLength <= KMms30 )
        {
        // short length
        shortLength = TUint8( aLength ) ;
        iEncodeBuffer->Write( iPosition, &shortLength, 1 );
        iPosition++;
        }
    else
        {
        iEncodeBuffer->Write( iPosition, &KMmsLengthQuote, 1 );
        iPosition++;
        EncodeUintvar( aLength );
        }

    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CMmsEncode::EncodeUintvar( TUint aInteger )
    {

    // The value is split into 7 bit chunks, and continue bit set
    // when needed.

    // No more than 5 bytes will be produced
    TUint8 buffer[KMms5];
    TUint temp = aInteger;
    TInt i;

    for ( i = 0; i < KMms5; ++i )            
        {
        buffer[KMms4 - i] = TUint8( temp & KMms0x7F );
        temp >>= KMms7;
        }

    i = 0;
    // buffer indexes are safe because the buffer has been defined long enough.
    while ( i < KMms4 && buffer[i] == 0 )
        {
        i++;
        }
    
    TInt j;

    for ( j = i; j < KMms4; ++j )
        {
        // buffer indexes are safe because the buffer has been defined long enough.
        buffer[j] |= KMms0x80; // set Continue bit, but never to last
        }

    // buffer indexes are safe because the buffer has been defined long enough.
    iEncodeBuffer->Write( iPosition, &buffer[i], KMms5 - i );
    iPosition += KMms5 - i;

    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
TInt CMmsEncode::GetUintvarLength( TUint aInteger )
    {

    // The value is split into 7 bit chunks, and continue bit set
    // when needed.

    // No more than 5 bytes will be produced
    TUint8 buffer[KMms5];
    TUint temp = aInteger;
    TInt i;

    for (i = 0; i < KMms5; ++i )            
        {
        buffer[KMms4 - i] = TUint8( temp & KMms0x7F );
        temp >>= KMms7;
        }

    i = 0;
    // buffer indexes are safe because the buffer has been defined long enough.
    while ( i < KMms4 && buffer[i] == 0 )
        {
        i++;
        }
    
    return KMms5 - i;

    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CMmsEncode::EncodeRecipientL( const CDesCArray& aRecipientList,
    TMmsRecipients aType )
    {

    TInt i;
    TInt size = aRecipientList.Count();
    if ( size == 0 )
        {
        return;
        }

    TUint8 recipientType = KMmsAssignedTo;

    switch ( aType )
        {
        case EMmsTo:
            recipientType = KMmsAssignedTo;
            break;
        case EMmsCc:
            recipientType = KMmsAssignedCc;
            break;
        case EMmsBcc:
            recipientType = KMmsAssignedBcc;
            break;
        default:
            break;
        }

    for ( i = 0; i < size; ++i )
        {
        // check for fakes
        if ( aRecipientList[i].Length() > 0 )
            {
            iEncodeBuffer->Write( iPosition, &recipientType, 1 );
            iPosition++;

            EncodeAddressL( aRecipientList[i] );
            }

        }

    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CMmsEncode::EncodeOptionalStringL( TUint8 aHeader,
    const TPtrC16& aString )
    {

    if ( aString.Length() == 0 )
        {
        return;
        }

    iEncodeBuffer->Write( iPosition, &aHeader, 1 );
    iPosition++;

    EncodeTextStringL( aString );

    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CMmsEncode::EncodeOptionalString( TUint8 aHeader, const TPtrC8& aString )
    {

    if ( aString.Length() == 0 )
        {
        return;
        }

    EncodeHeaderAndTextString( aHeader, aString );

    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CMmsEncode::EncodeIntervalOrDate( TInt aInterval, const TInt64& aDate )
    {

    TInt64 temp = 0;
    TUint8 token = 0;

    if ( aDate != 0 )
        {
        temp = aDate;
        token = KMmsAbsoluteToken;
        }
    else
        {
        temp = aInterval;
        token = KMmsRelativeToken;
        }

    // calculate value length

    TUint length = KMms8;
    TUint mask = 0xff000000;

    while ( ( I64HIGH( temp ) & mask ) == 0 && length > KMms4)
        {
        mask >>= KMms8;
        length--;
        }

    mask = 0xff000000;

    // Test if the whole high half was zero
    if ( I64HIGH( temp ) == 0 )
        {
        while ( ( I64LOW( temp ) & mask ) == 0 && length > 1)
            {
            mask >>= KMms8;
            length--;
            }   
        }

    // now add short length and absolute/relative token

    length += KMms2;

    EncodeValueLength( length );

    iEncodeBuffer->Write( iPosition, &token, 1 );
    iPosition++;

    EncodeLongInteger( temp );

    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CMmsEncode::EncodeReplyChargingSize( TInt aReplyChargingSize )
    {
    if ( aReplyChargingSize == 0 )
        {
        return;
        }
    TInt64 size = aReplyChargingSize;
    iEncodeBuffer->Write( iPosition, &KMmsAssignedReplyChargingSize, 1 );
    iPosition++;

    EncodeLongInteger( size );
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CMmsEncode::EncodeOptionalByte( TUint8 aHeader, TInt aValue )
    {
    if ( aValue == 0 )
        {
        return; // not set, nothing to encode, header is optional
        }
    EncodeMandatoryByte( aHeader, aValue );

    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CMmsEncode::EncodeMandatoryByte( TUint8 aHeader, TInt aValue )
    {
    // When ecoding mandatory byte even value 0 is allowed.
    // It will become 0x80, and be correctly decoded back to 0.
    TUint8 value = TUint8 ( aValue ) | KMms0x80 ;

    iEncodeBuffer->Write( iPosition, &aHeader, 1 );
    iPosition++;

    iEncodeBuffer->Write( iPosition, &value, 1 );
    iPosition++;
    }


// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CMmsEncode::EncodeOptionalIntervalOrDate( TUint8 aHeader,
            TInt aInterval,
            const TInt64& aDate )
    {
    if ( aInterval == 0 && aDate == 0 )
        {
        return; // not set, nothing to encode, header is optional
        }

    iEncodeBuffer->Write( iPosition, &aHeader, 1 );
    iPosition++;

    EncodeIntervalOrDate( aInterval, aDate );
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CMmsEncode::EncodeHeaderAndTextString( TUint8 aHeader,
    const TDesC8& aString )
    {

    iEncodeBuffer->Write( iPosition, &aHeader, 1 );
    iPosition++;

    EncodeTextString( aString );

    }


// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
TBool CMmsEncode::IsStringSafe( const TDesC& aString )
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
//
TBool CMmsEncode::IsStringSafe( const TDesC8& aString, TInt& aNumNonSafe  )
    {

    // Very simple check to see if string is "safe" ASCII
    // Used for headers, which are short strings
    // spaces count as non-safe because their number must be taken into
    // account when calculating the length

    const TInt KIntQuestion = 0x3F;
    const TInt KIntEquals = 0x3D;
    
    TInt i;
    aNumNonSafe = 0;
    TBool safe = ETrue;

    for ( i = 0; i < aString.Length()/* && safe*/; ++i )
        {
        if ( aString[i] < KMmsLowestAscii || aString[i] >= KMmsHighestAscii )
            {
            safe = EFalse;
            aNumNonSafe++;
            }
        if ( aString[i] == KIntQuestion || aString[i] == KIntEquals )
            {
            // These are safe but must be encoded if quoted printable
            // encoding is used. The number is needed to check the header length
            aNumNonSafe++;
            }
        if ( aString[i] == KMmsIntUnderscore )
            {
            // This must always be encoded as base64 because Symbian
            // does not encode underscores when quoted printable is used.
            aNumNonSafe = KMaxEncodingLength;
            }
        
        }
    return safe;

    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CMmsEncode::EncodeMultipartRelatedHeaderL( const TMsvAttachmentId aRootId )
    {

    iEncodeBuffer->Write( iPosition, &KMmsAssignedContentType, 1 );
    iPosition++;

    // We have parameters (start), so we must use Content-general-form

    // We need the content-id for the root part. If it is not defined already,
    // we must generate one.

    TUint headerLength = 1; // one byte for well known media
    TInt cleanupCount = 0; // we must keep track what we have on store

    User::LeaveIfError( iEntryWrapper->SetCurrentEntry( iCurrentMessageId ) );
        
    CMsvStore* store = iEntryWrapper->EditStoreL();
    CleanupStack::PushL( store ); cleanupCount++;
    MMsvAttachmentManager& attachMan = store->AttachmentManagerL();
    CMsvAttachment* attachmentInfo = NULL;
    attachmentInfo = attachMan.GetAttachmentInfoL( (TMsvAttachmentId) aRootId );
    CleanupStack::PushL( attachmentInfo ); cleanupCount++;
    
    iMimeHeaders->RestoreL( *attachmentInfo );

    if ( iError != KErrNone )
        {
        CleanupStack::PopAndDestroy( cleanupCount ); 
        return;
        }

    TPtrC8 cid = iMimeHeaders->ContentId();
    TBufC8<KMmsMaxCidLength> target;

    // If cid has not been defined, we must generate one
    if ( cid.Length() == 0 )
        {
        TTime now;
        now.UniversalTime();
        TInt random;
        TInt64 seed = now.Int64();
        random = Math::Rand( seed );
        // type conversions irrelevant - just creating a magic number for content id
        target.Des().Num( random );
        target.Des().Insert(0, KMmsLeftAngle );
        target.Des().Append( KMmsRightAngle );
        cid.Set( target.Des() );
        // exclude the angle brackets
        iMimeHeaders->SetContentIdL( cid.Mid( 1, cid.Length() - KMms2 ) );
        // save the cid to be present when we create the attachment headers
        // If this does not succeed, the message will have no root.
        MMsvAttachmentManagerSync& attachManSync = 
            store->AttachmentManagerExtensionsL();
        iMimeHeaders->StoreL( *attachmentInfo );
        attachManSync.ModifyAttachmentInfoL( attachmentInfo );
        // attachment manager now own attachemnt info
        CleanupStack::Pop( attachmentInfo ); cleanupCount--;
        store->CommitL();
        attachmentInfo = attachMan.GetAttachmentInfoL( (TMsvAttachmentId) aRootId );
        CleanupStack::PushL( attachmentInfo ); cleanupCount++;
        }

    // add start header length &
    // assigned number for start parameter, and terminating zero for cid
    headerLength += cid.Length() + KMms2;
    if ( cid.Find( KMmsLeftAngle ) != 0 )
        {
        headerLength += KMms2; // we must add angle bracket
        }

    // Then the length of the content-type header...
    TPtrC8 contentTypeString = iMimeHeaders->ContentType();
    
     HBufC8* tempContentType = HBufC8::NewL( iMimeHeaders->ContentType().Length() +
         iMimeHeaders->ContentSubType().Length() + 1 );
     CleanupStack::PushL( tempContentType );
     tempContentType->Des().Copy( iMimeHeaders->ContentType() );
     if ( ( iMimeHeaders->ContentType().Find( KMmsSlash8 ) !=
        ( iMimeHeaders->ContentType().Length() - 1 ) ) &&
        ( iMimeHeaders->ContentSubType().Find( KMmsSlash8 ) != 0 ) &&
        ( iMimeHeaders->ContentSubType().Length() != 0 ) )
        {
        tempContentType->Des().Append( KMmsSlash8 );
        }
    tempContentType->Des().Append( iMimeHeaders->ContentSubType() );
    attachmentInfo->SetMimeTypeL( tempContentType->Des() );
    CleanupStack::PopAndDestroy( tempContentType );
    contentTypeString.Set( attachmentInfo->MimeType() );

    if ( contentTypeString.Length() == 0 )
        {
        // We need a content type...
        // If we don't know, we say "Any"
        contentTypeString.Set( KMmsAny );
        }
    // Check if we have well-known media.
    TInt8 rootContentType = -1;

    TInt8 i = 0;
    for ( i = 0; i < KNumberContentTypes && rootContentType < 0; ++i )
        {
        if ( contentTypeString.CompareF( TPtrC8( KContentTypeTable[i] ) ) == 0 )
            {
            rootContentType = i;
            }
        }
        
    // start parameter assignment
    headerLength += 1;
    if ( rootContentType != -1 )
        {
        // well known content type
        headerLength += 1;
        }
    else
        {
        // string + terminating zero
        headerLength += contentTypeString.Length() + 1;
        }
        
    TPtrC8 appIdPtr;
    appIdPtr.Set( KMmsJavaApplicationId ); 
    // Java application id parameters added to content-type        
    if ( iMmsHeaders->ApplicId().Length() > 0 )
        {
        headerLength += appIdPtr.Length() + 1 ;
        headerLength += iMmsHeaders->ApplicId().Length() + 1;
        }
    appIdPtr.Set( KMmsJavaReplyApplicationId );    
    if ( iMmsHeaders->ReplyApplicId().Length() > 0 )
        {
        headerLength += appIdPtr.Length() + 1 ;
        headerLength += iMmsHeaders->ReplyApplicId().Length() + 1;
        }

    // write general encoding length
    EncodeValueLength( headerLength );

    // Then the Media Type with parameters
        
    // We are multipart/related, which is a well-known media
    // encode as short integer
    TUint8 byte = KMmsAssignedApplicationVndWapMultipartRelated | KMms0x80;
    iEncodeBuffer->Write( iPosition, &byte, 1 );
    iPosition++;

    // Then the start parameter and cid text string
    byte = KWspStart | KMms0x80;
    
    if ( cid.Length() > 0 )
        {
        HBufC8* tempContentId = HBufC8::NewL( iMimeHeaders->ContentId().Length() + KMms2 );
        CleanupStack::PushL( tempContentId );
        if ( cid.Find( KMmsLeftAngle ) != 0 )
            {
            tempContentId->Des().Copy( KMmsLeftAngle );
            tempContentId->Des().Append( cid );
            tempContentId->Des().Append( KMmsRightAngle );
            }
        else
            {
            tempContentId->Des().Copy( cid );
            }
        EncodeHeaderAndTextString( byte, tempContentId->Des() );
        CleanupStack::PopAndDestroy( tempContentId );
        }


    // next the content type of the root part
    byte = KWspRelatedType | KMms0x80;
    // and the actual type either as a well-known media type
    // or a text string.

    if ( rootContentType != -1 )
        {
        // Well known content type.
        // EncodeMandatoryByte will set the high bit
        EncodeMandatoryByte( byte, rootContentType );
        }
    else
        {
        // string + terminating zero
        EncodeHeaderAndTextString( byte, contentTypeString );
        }
       
    if ( iMmsHeaders->ApplicId().Length() > 0 ||
        iMmsHeaders->ReplyApplicId().Length() > 0 )
        {
        EncodeApplicationIdParametersL();
        }
            
    // encode number of parts
    EncodeUintvar( iNumberOfAttachments );
    
    // get rid of stuff we put on stack
    CleanupStack::PopAndDestroy( cleanupCount ); 
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CMmsEncode::EncodeMultipartMixedHeaderL()
    {
    
    // If Java has added application id or reply-to application id, 
    // even multipart/mxed type needs parameters.
    
    TUint headerLength = 0; // assume no parameters
    
    if ( iMmsHeaders->ApplicId().Length() > 0 ||
        iMmsHeaders->ReplyApplicId().Length() > 0 )
        {
        headerLength = 1; // one byte for well known media
        TPtrC8 appIdPtr;
        appIdPtr.Set( KMmsJavaApplicationId ); 
        if ( iMmsHeaders->ApplicId().Length() > 0 )
            {
            headerLength += appIdPtr.Length() + 1 ;
            headerLength += iMmsHeaders->ApplicId().Length() + 1;
            }
        appIdPtr.Set( KMmsJavaReplyApplicationId );    
        if ( iMmsHeaders->ReplyApplicId().Length() > 0 )
            {
            headerLength += appIdPtr.Length() + 1 ;
            headerLength += iMmsHeaders->ReplyApplicId().Length() + 1;
            }
        }

    iEncodeBuffer->Write( iPosition, &KMmsAssignedContentType, 1 );
    iPosition++;
    
    if ( headerLength > 0 )
        {
        // write general encoding length
        EncodeValueLength( headerLength );
        }

    // encode as short integer
    TUint8 contentType = KMmsAssignedApplicationVndWapMultipartMixed | KMms0x80;
    iEncodeBuffer->Write( iPosition, &contentType, 1 );
    iPosition++;

    if ( iMmsHeaders->ApplicId().Length() > 0 ||
        iMmsHeaders->ReplyApplicId().Length() > 0 )
        {
        EncodeApplicationIdParametersL();
        }

    // No more parameters for multipart/mixed, actual parts follow

    // encode number of parts
    EncodeUintvar( iNumberOfAttachments );
    
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CMmsEncode::EncodeKeywordArrayL()
    {
    TInt i = 0;
    TInt length = 0;

    // caller must check that iMmsHeaders->ReadOnlyMMBoxMessageHeaders() is not NULL
    CMmsMMBoxMessageHeaders& temp = iMmsHeaders->MMBoxMessageHeadersL();
    for ( i = 0; i < temp.KeywordArray().Count(); ++i )
        {
        length = temp.KeywordArray()[i]->Keyword().Length();
        if ( length > 0 )
            {
            iEncodeBuffer->Write( iPosition, &KMmsAssignedMMFlags, 1 );
            iPosition++;
            // do some fake encoding to get the text length
            TUint oldPosition = iPosition; // we will return here
            EncodeTextStringL( temp.KeywordArray()[i]->Keyword() );
            length = iPosition - oldPosition;
            iPosition = oldPosition; // return to original place
            length += 1; // token must come before string
            EncodeValueLength( length );
            TUint8 token = (TUint8) temp.KeywordArray()[i]->Token();
            if ( iMmsHeaders->MessageType() == KMmsMessageTypeMboxViewReq ||
                 iMmsHeaders->MessageType() == KMmsMessageTypeMboxViewConf ||
                 iMmsHeaders->MessageType() == KMmsMessageTypeMBoxDescr )
                {
                // the token must always be filter for MMbox PDUs
                token = KMmsFilterToken;
                }
            iEncodeBuffer->Write( iPosition, &token, 1 );
            iPosition++;
            EncodeTextStringL( temp.KeywordArray()[i]->Keyword() );
            }
        }
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CMmsEncode::EncodeOptionalInteger( TUint8 aHeader, TUint aValue )
    {
    if ( aValue == 0 )
        {
        return; // not set, nothing to encode, header is optional
        }

    iEncodeBuffer->Write( iPosition, &aHeader, 1 );
    iPosition++;

    EncodeInteger( aValue );

    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CMmsEncode::EncodeAttributes( RArray<TUint>& aAttributeArray )
    {
    TInt i;

    for ( i = 0; i < aAttributeArray.Count(); ++i )
        {
        EncodeMandatoryByte( KMmsAssignedAttributes, aAttributeArray[i] );
        }
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CMmsEncode::EncodeMMBoxStates( RArray<TInt>& aStateArray )
    {
    TInt i;

    for ( i = 0; i < aStateArray.Count(); ++i )
        {
        EncodeMandatoryByte( KMmsAssignedMMState, aStateArray[i] );
        }
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CMmsEncode::EncodeContentLocationArray()
    {
    TBool mustCheck = EFalse;
    if ( iMmsHeaders->ContentLocation().Length() > 0 )
        {
        mustCheck = ETrue;
        EncodeHeaderAndTextString( KMmsAssignedContentLocation,
            iMmsHeaders->ContentLocation() );
        }
    // If there is content location array, encode it, too.
    // A content-location should never occur in both places, but we still check

    TInt i = 0;
    if ( iMmsHeaders->ReadOnlyMMBoxMessageHeaders() )
        {
        for ( i = 0;
            i < iMmsHeaders->ReadOnlyMMBoxMessageHeaders()->ContentLocationList().Count();
            i++ )
            {
            if ( !( mustCheck && iMmsHeaders->ContentLocation().Compare(
                iMmsHeaders->ReadOnlyMMBoxMessageHeaders()->ContentLocationList()[i] ) == 0 ) )
                {
                // the content location in the list does not appear as
                // separate header, we must add it
                EncodeHeaderAndTextString( KMmsAssignedContentLocation,
                    iMmsHeaders->ReadOnlyMMBoxMessageHeaders()->ContentLocationList()[i] );
                }
            }
        }
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CMmsEncode::EncodeStartingHeaders( TInt aMessageType,
    const TPtrC8& aTID, TInt aVersion )
    {
    // EncodeMandatoryByte will always set the high bit.
    
    // Message type
    EncodeMandatoryByte( KMmsAssignedMessageType, aMessageType );
    // TID if present
    EncodeOptionalString( KMmsAssignedTID, aTID );
    // MMS encapsulation version
    EncodeMandatoryByte( KMmsAssignedMmsVersion, aVersion );
    }
    
// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CMmsEncode::EncodeApplicationIdParametersL()
    {
    if ( iMmsHeaders->ApplicId().Length() > 0 )
        {
        EncodeTextString( KMmsJavaApplicationId );
        EncodeTextStringL( iMmsHeaders->ApplicId() );
        }
    if ( iMmsHeaders->ReplyApplicId().Length() > 0 )
        {
        EncodeTextString( KMmsJavaReplyApplicationId );
        EncodeTextStringL( iMmsHeaders->ReplyApplicId() );
        }
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
void CMmsEncode::Dump()
    {
    // no dump if not logging
#ifndef _NO_MMSS_LOGGING_
    TInt error = KErrNone;
    // if no can do, sorry.
    TRAP( error,
        {
        if ( iEntryWrapper )
            {
            if ( ( iEntryWrapper->GetDumpFlag() ) &&
                iEncodeBuffer &&
                iEncodeBuffer->Size() > 0 )
                {
                iFileName = KMmsDefaultLogDirectory;
                TUint att;
                if ( iFs.Att( iFileName, att ) == KErrNone )
                    {
                    _LIT( KRelated, "Sent.mms");
                    iParse.Set( iFileName, &KRelated, NULL );
                    iFileName = iParse.FullName();
                    User::LeaveIfError( CApaApplication::GenerateFileName(
                        iFs, iFileName ) );
                    RFile file;
                    User::LeaveIfError( file.Create( iFs, iFileName,
                        EFileWrite | EFileShareExclusive ) );
                    // for message id generation
                    iParse.Set( iFileName, NULL, NULL );

                    // the data is supposed to be in the encode buffer
                    TPtr8 ptr = iEncodeBuffer->BackPtr( iPosition );
                    file.Write( ptr );
                    file.Flush();

                    // done - close files
                    file.Close();
                    }
                }
            }
        }
    );
    if ( error != KErrNone )
        {
        TMmsLogger::Log( _L("Dump left with error %d"), error );
        }
#endif
    }
    
// ---------------------------------------------------------
//
// ---------------------------------------------------------
void CMmsEncode::DumpAppend()
    {
    
    // no dump if not logging
#ifndef _NO_MMSS_LOGGING_
    TInt error = KErrNone;
    // if no can do, sorry.
    TRAP( error,
        {
        if ( iEntryWrapper )
            {
            if ( ( iEntryWrapper->GetDumpFlag() ) &&
                iEncodeBuffer &&
                iEncodeBuffer->Size() > 0 )
                {
                iFileName = KMmsDefaultLogDirectory;
                TUint att;
                if ( iFs.Att( iFileName, att ) == KErrNone )
                    {
                    RFile file;
                    iFileName = iParse.FullName();
                    User::LeaveIfError( file.Open( iFs, iFileName,
                       EFileWrite | EFileShareExclusive ) );
                    TInt position = 0; // seek to end
                    file.Seek( ESeekEnd, position );
                    // the data is supposed to be in the encode buffer
                    TPtr8 ptr = iEncodeBuffer->BackPtr( iPosition );
                    file.Write( ptr );
                    file.Flush();
                    // done - close files
                    file.Close();
                    }
                }
            }
        }
    );
    if ( error != KErrNone )
        {
        TMmsLogger::Log( _L("DumpAppend left with error %d"), error );
        }
#endif
    }
    
// ---------------------------------------------------------
//
// ---------------------------------------------------------
void CMmsEncode::EncodeApplicationHeadersL()
    {
// Application headers are only supported in WINS for testing purposes
// Complete support for routing messages to arbitrary applications
// requires support for selecting which applications are allowed
// to send messages etc.    
#ifdef __WINS__
    // The optional string functions check the length
    // and return without doing anything if the length of the string is 0
    if ( IsStringSafe( iMmsHeaders->ApplicId() ) )
        {
        // We only send this header if it is us-ascii only
        // There is no encoding defined, so if this is converted
        // from unicode to some other character set there is 
        // no guarantee that the recipient can decode it.
        EncodeOptionalStringL( KMmsAssignedApplicId, iMmsHeaders->ApplicId() );
        }
        
    if ( IsStringSafe( iMmsHeaders->ReplyApplicId() ) )
        {
        EncodeOptionalStringL( KMmsAssignedReplyApplicId, iMmsHeaders->ReplyApplicId() );
        }

    EncodeOptionalString( KMmsAssignedAuxApplicInfo, iMmsHeaders->AuxApplicInfo() );
#endif    
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
void CMmsEncode::EncodeCancelRequest()
    {
// implemented for test purposes
#ifdef __WINS__
    // Insert message type, TID and version number
    EncodeStartingHeaders( KMmsMessageTypeCancelReq,
        iMmsHeaders->Tid(), iMmsHeaders->MmsVersion() );
        
    EncodeOptionalString( KMmsAssignedCancelId, iMmsHeaders->ReplaceCancelId() );
#endif    
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
void CMmsEncode::EncodeCancelResponse()
    {
    // Insert message type, TID and version number
    EncodeStartingHeaders( KMmsMessageTypeCancelConf,
        iMmsHeaders->Tid(), iMmsHeaders->MmsVersion() );
        
    EncodeOptionalByte( KMmsAssignedCancelStatus, iMmsHeaders->CancelStatus() );   
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
HBufC8* CMmsEncode::EncodeQuotedPrintableWordL( const TPtrC8& aSource )
    {
    // We have calculated that it fits
    
    TInt bufferLength = KMaxNameBufferLength;
    
    HBufC8* buffer = HBufC8::NewL( bufferLength );
    CleanupStack::PushL( buffer );
    TPtr8 ptr = buffer->Des();
    buffer->Des().Copy( KMmsQuotedPreamble );
    ptr.SetLength( bufferLength );
    TPtr8 encodeBuffer = ptr.MidTPtr( KMmsPreambleLength );
    encodeBuffer.SetLength( 0 ); // empty the buffer
    
    TImCodecQP encoder;

    // The function would return the number of characters written to the buffer.
    // We don't do anything with the result, so we ignore the return value.
    encoder.Encode( aSource, encodeBuffer );
    
    ptr.SetLength( encodeBuffer.Length() + KMmsPreambleLength );
    
    buffer->Des().Append( KMmsEncodingTrailer );
    CleanupStack::Pop( buffer );
    return buffer;
    }
    
// ---------------------------------------------------------
//
// ---------------------------------------------------------
HBufC8* CMmsEncode::EncodeBase64WordL( const TPtrC8& aSource )
    {
    // ((length + 2)/3)*4  3 bytes alwaus produces 4 encoded bytes. Allow filler
    TInt bufferLength = KMmsEncodingExtraLength + ( aSource.Length() + KMms2 ) / KMms3 * KMms4;
    
    HBufC8* buffer = HBufC8::NewL( bufferLength );
    CleanupStack::PushL( buffer );
    TPtr8 ptr = buffer->Des();
    buffer->Des().Copy( KMmsBase64Preamble );
    ptr.SetLength( bufferLength );

    TPtr8 encodeBuffer = ptr.MidTPtr( KMmsPreambleLength );
    encodeBuffer.SetLength( 0 ); // empty the buffer

    TImCodecB64 encoder;

    // It is rather unclear what this function actually returns (no documentation found). 
    // Therefore we just ignore the result.
    // Our buffer is long enough for the result to always fit.
    encoder.Encode( aSource, encodeBuffer );
    
    ptr.SetLength( encodeBuffer.Length() + KMmsPreambleLength );
    
    buffer->Des().Append( KMmsEncodingTrailer );
    CleanupStack::Pop( buffer );
    return buffer;
    }
    

// ---------------------------------------------------------
// CMmsEncode::PreProcessAttachmentDataL
// Open the message store(Edit mode) and process attachments for further encoding in required encoding type.
// Update the message store accordingly with the new encoding type and data in the corresponding attachments.
// ---------------------------------------------------------
void CMmsEncode::PreProcessAttachmentDataL()
    {
    TInt error = KErrNone;
    RFile attachFile;
    TBool retVal = EFalse;
    TInt currAttachI;
    
#ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L("CMmsEncode::PreProcessAttachmentDataL- start ") );
#endif /* _NO_MMSS_LOGGING_ */
    CMsvStore* editStore = iEntryWrapper->EditStoreL();
    CleanupStack::PushL( editStore );

    MMsvAttachmentManager& attachMan = editStore->AttachmentManagerL();
    MMsvAttachmentManagerSync& attachManagerSync = editStore->AttachmentManagerExtensionsL();
    TInt numberOfAttachments = attachMan.AttachmentCount();
    CMsvAttachment* attachmentInfo = NULL;
    iTextUtils = CMsgTextUtils::NewL( iFs );
    
    for ( currAttachI = 0; ( currAttachI < numberOfAttachments ) && ( error == KErrNone ); currAttachI++ )
        {
        //gets the ownership from attachment manager
        attachmentInfo = attachMan.GetAttachmentInfoL( currAttachI );
        CleanupStack::PushL( attachmentInfo );       
        iMimeHeaders->RestoreL( *attachmentInfo );
#ifndef _NO_MMSS_LOGGING_
        TMmsLogger::Log( _L("CMmsEncode::PreProcessAttachmentDataL- Attchment:%d "), currAttachI );
#endif /* _NO_MMSS_LOGGING_ */
        
        if ( iFileOpen )
            {
            // close any file in case we have been reset while the file is still open
            iAttachFile.Close();
            iFileOpen = EFalse;
            }

        retVal = CheckAndUpdateAttachmentL(*attachmentInfo, *iMimeHeaders);

        if( retVal )
            {
            TRAPD( 
                    err,
                    attachManagerSync.ModifyAttachmentInfoL(attachmentInfo);
                    editStore->CommitL();
                  );
#ifndef _NO_MMSS_LOGGING_
            if(err != KErrNone)
                {
                TMmsLogger::Log( _L("CMmsEncode::PreProcessAttachmentData:: store commit error: %d"), err );
                }
            else
                {
                TMmsLogger::Log( _L("CMmsEncode::PreProcessAttachmentData:: store commit success") );                
                }
#endif /* _NO_MMSS_LOGGING_ */
            /*  attachmentInfo ownership is transferred to attachment manager
             *  Hence, JUST pop attachmentInfo, DO NOT Destroy.
             */
            CleanupStack::Pop( attachmentInfo );
            }
        else
            {
            CleanupStack::PopAndDestroy( attachmentInfo );
            attachmentInfo = NULL;
#ifndef _NO_MMSS_LOGGING_
            TMmsLogger::Log( _L("CMmsEncode::PreProcessAttachmentData:: Tgt encoding NOT Reqd.") );                
#endif /* _NO_MMSS_LOGGING_ */
            }
        }
    CleanupStack::PopAndDestroy( editStore );
    
    delete iTextUtils;
    iTextUtils = NULL;
#ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L("CMmsEncode::PreProcessAttachmentDataL- end ") );
#endif /* _NO_MMSS_LOGGING_ */
    }

// ---------------------------------------------------------
// CMmsEncode::CheckAndUpdateAttachmentL
// Check and proceed if given attachment can be encoded using target encoding type based on its content type.
// Returns False if target encoding is not supported for this content type.
// ---------------------------------------------------------
TBool CMmsEncode::CheckAndUpdateAttachmentL( CMsvAttachment& aAttachmentInfo,
                                             CMsvMimeHeaders& aMimeHeaders )
    {
    //get the content type string... and set to attachment if required
    TInt contentType = -1; // indicate not found
    TPtrC8 contentTypeString;
    TBool retVal = EFalse;
#ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L("CMmsEncode::CheckAndUpdateAttachmentL- start ") );
#endif /* _NO_MMSS_LOGGING_ */

    HBufC8* tempContentType = NULL;
    contentTypeString.Set( aAttachmentInfo.MimeType() );
    if ( contentTypeString.Length() == 0 )
        {
        // take type from mime headers
        TInt cotentLength = aMimeHeaders.ContentType().Length();
        TInt subCotentLength = aMimeHeaders.ContentSubType().Length();
        tempContentType = HBufC8::NewL( cotentLength + subCotentLength + 1 );
        CleanupStack::PushL( tempContentType );
        tempContentType->Des().Copy( aMimeHeaders.ContentType() );
        if ( ( aMimeHeaders.ContentType().Find( KMmsSlash8 ) != ( cotentLength - 1 ) ) &&
             ( aMimeHeaders.ContentSubType().Find( KMmsSlash8 ) != 0 ) && ( subCotentLength != 0 ) )
            {
            tempContentType->Des().Append( KMmsSlash8 );
            }
        tempContentType->Des().Append( aMimeHeaders.ContentSubType() );
        aAttachmentInfo.SetMimeTypeL( tempContentType->Des() );
        contentTypeString.Set( aAttachmentInfo.MimeType() );
        }
        
    //map the content type string to content type
    TInt8 i;
    for ( i = 0; i < KNumberContentTypes && contentType < 0; i++ )
        {
        if ( contentTypeString.CompareF( TPtrC8( KContentTypeTable[i] ) ) == 0 )
            {
            contentType = i;
            }
        }
#ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L("CMmsEncode::CheckAndUpdateAttachmentL- content type:%d" ), contentType );
#endif /* _NO_MMSS_LOGGING_ */
    //check if this content type need to be encoded using korean specific
    if( IsConversionSupportedContentType( contentType ) )
        {
        /* Do the attachment data conversion from "src type" to "target type" for the given "attached file". 
         * Target type encodng MIB enum is obtained from cenrep.
         */
        retVal = ProcessAndConvertAttachmentDataL( aMimeHeaders.MimeCharset(),
                                                   iTargetEncodingType,
                                                   aAttachmentInfo);
        if( retVal )
            {
#ifndef _NO_MMSS_LOGGING_
            TMmsLogger::Log( _L("CMmsEncode::CheckAndUpdateAttachmentL- conv success: Tgt Enc: %d"), iTargetEncodingType );
#endif /* _NO_MMSS_LOGGING_ */
            //set new charset type in headers and update attachment info
            aMimeHeaders.SetMimeCharset( iTargetEncodingType );
            aMimeHeaders.StoreL(aAttachmentInfo); // save the new charset to attachment as well
            /* mimetype might get reset to original mimeheader content type.
             * ensure the full content type(if created from mime content and subcontent) is set to attachment
             */
            if( tempContentType != NULL )
                {
                aAttachmentInfo.SetMimeTypeL( tempContentType->Des() );
                CleanupStack::PopAndDestroy( tempContentType );                
                }
            }
        }
#ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L("CMmsEncode::CheckAndUpdateAttachmentL- End , retVal: %d"), retVal );
#endif /* _NO_MMSS_LOGGING_ */
    return retVal;
    }
    
// ---------------------------------------------------------
// CMmsEncode::ProcessAndConvertAttachmentDataL
// converts of attachment data from source to target encoding type.
// |src charset buffer| --->converted to ---> |unicode buffer| ---> converted to ---> |target charset|
// Returns false if data is already int target format, or plugins are missing, or file operation issues.
// ---------------------------------------------------------
TBool CMmsEncode::ProcessAndConvertAttachmentDataL( TUint aSrcCharSetMIBEnum,
                                                    TUint aTargetCharSetMIBEnum, 
                                                    CMsvAttachment& aAttachmentInfo)
    {
#ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L("CMmsEncode::ProcessAndConvertAttachmentDataL- start ") );
#endif /* _NO_MMSS_LOGGING_ */
    if( aSrcCharSetMIBEnum == aTargetCharSetMIBEnum )
        {
#ifndef _NO_MMSS_LOGGING_
        TMmsLogger::Log( _L("CMmsEncode::ProcessAndConvertAttachmentDataL- src and dest are same ") );
#endif /* _NO_MMSS_LOGGING_ */
        //if source and target charset MIB enums are same.  
        return EFalse; // no conversion 
        }
        
    //get source and target charset mapping here. if any error, return
    TUint srcCharSetId = iTextUtils->MibIdToCharconvIdL( aSrcCharSetMIBEnum );
    TUint targetCharSetId = iTextUtils->MibIdToCharconvIdL( aTargetCharSetMIBEnum );
    if( srcCharSetId == targetCharSetId || KDefaultCharConvCharset == targetCharSetId)
        {
        /* If target charset plugin is missing, then default is returned. 
           Do not encode to any target encoding type, send data as is .
         */ 
#ifndef _NO_MMSS_LOGGING_
        TMmsLogger::Log( _L("CMmsEncode::ProcessAndConvertAttachmentDataL- missing plugin: Tgt Charset %x"), targetCharSetId );
#endif /* _NO_MMSS_LOGGING_ */
        return EFalse;
        }
    
    RFile attachFile;
    TInt error;

    const TDesC& fileName = aAttachmentInfo.FilePath();
    error = attachFile.Open(iFs, fileName, EFileWrite);
    if(error != KErrNone)
        {
#ifndef _NO_MMSS_LOGGING_
        TMmsLogger::Log( _L("CMmsEncode::ProcessAndConvertAttachmentData:: file: %s, open error: %d"), fileName, error );
#endif /* _NO_MMSS_LOGGING_ */
        attachFile.Close();
        return EFalse;
        }

    TInt maxLength=0;
    error = attachFile.Size(maxLength);
    if( error != KErrNone || maxLength == 0 )
        {
#ifndef _NO_MMSS_LOGGING_
        TMmsLogger::Log( _L("CMmsEncode::ProcessAndConvertAttachmentData:: file size: %d, error: %d"), maxLength, error );
#endif /* _NO_MMSS_LOGGING_ */
        attachFile.Close();
        return EFalse;
        }

    //read the original file data into srcBuffer
    TInt cleanupCount = 0;    
    HBufC8* srcBuffer = HBufC8::NewLC( maxLength );
    cleanupCount++;
    
    TPtr8 srcPtr = srcBuffer->Des();
    attachFile.Read( srcPtr, maxLength );
        
    //intermediate buffer in the form of unicode. 
    HBufC16* unicodeBuffer(NULL);

    //Convert, scrBuffer to unicode format if not already in unicode format
    if(srcCharSetId != 0)
        {
        //Convert from respective foreign charset to unicode buffer.
        //returns buf16 pointer descriptor
        TRAPD(err, unicodeBuffer = iTextUtils->ConvertToUnicodeL(srcPtr, srcCharSetId) );
        if( err != KErrNone )
            {
#ifndef _NO_MMSS_LOGGING_
            TMmsLogger::Log( _L("CMmsEncode::ProcessAndConvertAttachmentData:: ConvertToUnicodeL error: %d"), err );
#endif /* _NO_MMSS_LOGGING_ */
            delete unicodeBuffer;
            attachFile.Close();
            CleanupStack::PopAndDestroy( cleanupCount, srcBuffer ); // srcBuffer
            return EFalse;
            }
        CleanupStack::PushL( unicodeBuffer );
        cleanupCount++;
        }
    else
        {
        TInt unicodeStdHeaderWordLE = 0xFEFF;
        TInt unicodeStdHeaderWordBE = 0xFFFE;
        //data is already in unicode format. need to extract the data to 16-bit buffer
        unicodeBuffer = HBufC16::NewLC( maxLength/2 );//maxlength value is in terms of bytes
        cleanupCount++;
        
        TPtr16 ptr = unicodeBuffer->Des();
        iTextUtils->ConvertPtrToDesC16(srcPtr, ptr);
        /* In case if attachment is UTF-16 format, it will have the UTF-16 representation word(2 butes)
         * at the start. Find and delete it before passing the data for conversion
         */ 
        if( ptr[0] == unicodeStdHeaderWordLE || ptr[0] == unicodeStdHeaderWordBE )
            {
            ptr.Delete(0, 1);
            }
        }

    //Now convert unicode buffer to respective target charset type.
    if( targetCharSetId != 0 )
        {
        // reset this file and write new encoded data to it directly
        error = attachFile.SetSize(0);
#ifndef _NO_MMSS_LOGGING_
        if( error != KErrNone )
            {
            TMmsLogger::Log( _L("CMmsEncode::ProcessAndConvertAttachmentData:: file size: %d, error: %d"), maxLength, error );
            //attachFile.Close();
            //return EFalse;
            }
#endif /* _NO_MMSS_LOGGING_ */
        
        /* convert to target charset id.
         * Ideally, this should not leave with plugin-NOT-found, since it it already verified
         */
        iTextUtils->ConvertToFileL(*unicodeBuffer, attachFile, targetCharSetId );
        }
    else
        {
        TPtr16 ptr = unicodeBuffer->Des();
        //Reset/erase old file content.
        TInt err = attachFile.SetSize(0);

        //write new encoded data to file using write stream
        RFileWriteStream writer( attachFile );
        writer.PushL();
        writer.WriteL( ptr );
        //writer.CommitL();
        writer.Pop();
        writer.Close();
        }
    //close file
    attachFile.Close();
    CleanupStack::PopAndDestroy( cleanupCount, srcBuffer ); //unicodeBuffer, srcBuffer
#ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L("CMmsEncode::ProcessAndConvertAttachmentDataL- End, retVal: TRUE") );
#endif /* _NO_MMSS_LOGGING_ */
    return ETrue;
    }


// ---------------------------------------------------------
// CMmsEncode::IsConversionSupportedContentType
// checks if input content type is supported for target encoding
// ---------------------------------------------------------
TBool CMmsEncode::IsConversionSupportedContentType( TInt aContentType )
    {
    /* Currently only "text/plain" content types are supported for korean encoding.
     * Add to the switch case, if any new content type can be supported as well.
     * 
     * IMPORTANT: 
     * Ensure the values added map to corresponding content type entry in the table KContentTypeTable[],
     * defined in ../inc/mmscodec.h
     */
#ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L("CMmsEncode::IsConversionSupportedContentType- start: %d "), aContentType );
#endif /* _NO_MMSS_LOGGING_ */
    TBool retVal = EFalse;
    switch( aContentType )
        {
        case 0x03 :           // "text/plain" 
            {
            retVal = ETrue;
            break;
            }
        default:
            //do nothing
            break;// to avoid compile errors/warning
        }
#ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L("CMmsEncode::IsConversionSupportedContentType- end ") );
#endif /* _NO_MMSS_LOGGING_ */
    return retVal;
    }

// ================= OTHER EXPORTED FUNCTIONS ==============

//  End of File  
