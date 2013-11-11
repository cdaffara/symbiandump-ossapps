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



#ifndef CMMSDECODE_H
#define CMMSDECODE_H

//  INCLUDES
#include    <mentact.h>
#include    <cmsvattachment.h>

#include    "mmsservercommon.h"
#include    "mmscodecdatasink.h"

// CONSTANTS
// We cannot save unlimited number of attachments.
// We use a "sanity" limit which is actually insanely high, but should be safe.
const TInt KMmsMaxAttachments = 500;

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class MMmsEntryWrapper;
class RBufReadStream;
class CMmsHeaders;
class CMsvMimeHeaders;
class CMmsSettings;

// CLASS DECLARATION

/**
* Decoding of binary multimedia message.
*
* @lib mmscodec.lib
* @since V2.1
*/
class CMmsDecode :public CMsgActive, public MMmsCodecDataSink
    {
    
public:
        
    /** Lookup for file extension */
    typedef struct
        {
        const TText8* tag;
        const TText16* extension;
        }TMmsExtensionLookup;

    /** Lookup for extension header assigned values */
    typedef struct MmsExtensionHeaderLookup
        {
        const TText8* extensionHeader;
        const TUint8* assignedValue;
        }TMmsExtensionHeaderLookup;

    /**
     * @param aFs file system handle.
     */
    IMPORT_C static CMmsDecode* NewL( RFs& aFs );
    
    virtual ~CMmsDecode();

public:
    
    /**
     * Start the state machine.
     * @param aEntryWrapper entry class for message access
     * @param aMmsHeadders reference to CMmsHeaders class.
     * @param aDecodeBuffer buffer holding the data to be decoded.
     * @param aStatus status of calling active object
     * @param aStartPosition the starting point in the buffer.
     *     Default = 0, start from beginning.
     * @param aLength length of the data to be decoded.
     *     Default = 0, decode whole buffer
     * If memory runs out before state machine is started, leaves with error code.
     */
    IMPORT_C void StartL(
        MMmsEntryWrapper& aEntryWrapper,
        CMmsHeaders& aMmsHeaders,
        CBufFlat& aDecodeBuffer,
        TRequestStatus& aStatus,
        TInt aStartPosition = 0,
        TInt aLength = 0 );

    /**
     * Synchronous decoding function.
     * To be used for messages that have only headers and no body.
     * For example MMS notifications and delivery reports.
     * @param aEntryWrapper entry class for message access
     * @param aMmsHeaders reference to CMmsHeaders class.
     * @param aDecodeBuffer buffer holding the data to be decoded.
     * @param aStartPosition the starting point in the buffer.
     *     Default = 0, start from beginning.
     * @param aLength length of the data to be decoded.
     *     Default = 0, decode whole buffer
     * @param aNumberofAttachments pointer to a variable that will contain
     *     the number of attachments on return.
     *     Default = 0, no attachment number requested
     * @param aDataStart pointer to a variable that will contain the
     *     starting position of data in this MMS message.
     *     Will point to start of first multipart entry or start of
     *     monoblock data. If data type is WSP multipart, the assigned
     *     number of the multipart has been stored in aMmsHeaders.
     *     Default = 0, data part start not requested.
     * leaves with error code.
     */
    IMPORT_C void DecodeHeadersL(
        MMmsEntryWrapper& aEntryWrapper,
        CMmsHeaders& aMmsHeaders,
        CBufFlat& aDecodeBuffer,
        TInt aStartPosition = 0,
        TInt aLength = 0,
        TInt* aNumberOfAttachments = 0,
        TInt* aDataStart = 0 );

    /**
     * Decode attachments separately
     * @since 2.6
     * @param aEntryWrapper entry class for message access
     * @param aMmsHeaders reference to CMmsHeaders class.
     * @param aDecodeBuffer buffer holding the data to be decoded.
     * @param aNumberOfAttachments number of attachments to be decoded.
     * @param aStartPosition start position of the data to be decoded.
     *     On return this is updated to point to next data part.
     *     If this was the last data part, aStartPosition will point
     *     past the end of the buffer.
     * @param aStatus status of calling active object.
     * @param aDoNotUpdateParentEntry should the parent entry be updated with 
     *     attachment info.
     *     The parent entry should be updated only when the last attachment
     *     has been handled.
     *     Default EFalse = update parent entry.
     * @param aLength length of the data to be decoded.
     *     Default = 0, decode whole buffer.
     * If aNumberOfAttachments is the actual number of attachments, and
     * aDoNotUpdateParentEntry and aLength have their default values, the whole
     * attachment buffer is decoded and the result is the same as it were
     * if the headers and attachments had been decoded together.
     * If the attachments are handled one by one, aDoNotUpdateParentEntry should
     * be false only when the last attachment has been reached.
     */
    IMPORT_C void DecodeAttachmentsL(
        MMmsEntryWrapper& aEntryWrapper,
        CMmsHeaders& aMmsHeaders,
        CBufFlat& aDecodeBuffer,
        TInt aNumberOfAttachments,
        TInt& aStartPosition,
        TRequestStatus& aStatus,
        TBool aDoNotUpdateParentEntry = EFalse,
        TInt aLength = 0 );

    /**
     * Extract starting position and length of data content from WSP multipart
     * @since 2.6
     * @param aDecodeBuffer buffer holding the data to be decoded.
     * @param aStartPosition start position of the data to be decoded.
     *     On return this is updated to point to next data part.
     *     If this was the last data part, aStartPosition will point
     *     past the end of the buffer.
     * @param aStartOfData On return start of the actual data content in multipart entry
     * @param aLengthOfData On return the length of the data content.
     * @return Mime headers containing content type and content type parameters
     *     of the data part. The structure will be valid only until next call to 
     *     some function of CMmsDecode.
     */
    IMPORT_C CMsvMimeHeaders& ExtractNextDataPartL(
        CBufFlat& aDecodeBuffer,
        TInt& aStartPosition,
        TInt& aStartOfData,
        TInt& aLengthOfData);
        
    /*
     * Commit the store.
     * This function must be called to commit the store if attachments are decoded
     *     and stored individually without allowing the parent entry to be updated
     *     after last attachment.
     * This function can also be called to commit the store after each attachment.
     * This function does not delete the store. The store is deleted when headers
     *     are reseted to start decoding a new entry.
     */            
    IMPORT_C void CommitStoreL();
    
    
    /**
     * Initialize the decoder for work in chunked decoding mode.
     *
     * @since v3.1
     * Only chunked decoding of headers or incoming messages is supported.
     * Chunked decoding of MMBox descriptions is not supported.
     * Needed variables are initialized.
     * Data will be provided later through the MmsCodecDataSink API
     * @param aEntryWrapper entry class for message access.
     *     Must be initialized to point to created message entry to receive the data.
     * @param aMmsHeaders reference to CMmsHeaders class.
     * @param aDecodeBuffer buffer. Given only to ensure that the buffer is always defined.
     *     During initialization buffer contains no data and it can be changed by NextDataPart
     *     function. Normally the buffer given here is the same as given in NextDataPart,
     *     only content will change.
     *
     */
    IMPORT_C void InitializeChunkedMode(
        MMmsEntryWrapper& aEntryWrapper,
        CMmsHeaders& aMmsHeaders,
        CBufFlat& aDecodeBuffer
        );

// from base class MMmsCodecDataSink

    /**
     * From MMmsCodecDataSink.
     * Gives the next data part to be decoded.
     * Decoding is synchronous. When the code returns, data can be released.
     *
     * @since v3.1
     * @param aBuffer flat buffer containing the data to be decoded.
     * @param aPosition start of data to be decoded.
     *     Decoding will start from aPosition.
     *     At return aPosition will point to the data that has not been
     *     decoded yet (in case data ends with an incomplete header) or
     *     past the end of data if everything has been decoded.
     *     The caller can discard the beginning of data up to aPosition.
     *     If there is data left in the buffer it must be shifted to the
     *     beginning of buffer and new data appended to the end to allow
     *     an incomplete header to be decoded at next round.
     * @param aLastChunk ETrue if the data chunk in the buffer is the last one.
     * @return error code, KErrNone if decoding succeeds
     *     If returns some other error, transaction must be cancelled.
     */
    TInt NextDataPart(
        CBufFlat& aBuffer,
        TInt& aPosition,
        TBool aLastDataChunk );
        
    /**
     * From MMmsCodecDataSink.
     * Release data sink and free any resources
     * This funktion must be called after last data chunk has been processed.
     * This funktion must also be called if the process is terminated by an error.
     *
     * @since v3.1
     */
    void RelaseDataSink();
    
    /**
     * Reset data sink.
     * Delete all attachments from current message.
     * Reset MMS headers
     * Start message decoding from the beginning
     * 
     * This function must be called if transaction is interrupted and restarted
     *
     * @since v3.1
     */
    void ResetDataSink();
    
          
private:

    CMmsDecode();

    /**
     * @param aFs file system handle.
     */
    void ConstructL( RFs& aFs );

    /**
     * Active object completion.
     */
    void DoRunL();

    /**
     * Cleanup
     * @param aStatus the completion code
     */
    void DoComplete( TInt& aStatus );

    /**
     * Select next state for state machine.
     */
    void SelectNextState();

    /**
     * Switch states.
     */
    void ChangeStateL();

    /**
     * Initialize members to allow several entry points.
     * We have at least one service that does not preceed
     * in stages, but completes in one shot.
     */
    void Reset();
    
    /**
     * Header decode stage for chunked decoding
     * @param aLastDataChunk is the data chunk the last one
     * @return
     * - ETrue if more data is needed to finish decoding the headers
     * - EFalse if no more data is needed
     */
    TBool SinkHeaders( TBool aLastDataChunk );

    /**
     * Attachment header decode stage for chunked decoding
     * @param aLastDataChunk is the data chunk the last one
     * @return
     * - ETrue if more data is needed to finish decoding the headers
     * - EFalse if no more data is needed
     */
    TBool SinkAttachmentHeaders( TBool aLastDataChunk );
    
    /**
     * Attachment data start stage for chunked decoding
     */
    void SinkAttachmentDataStart();
    
    /**
     * Attachment data stage for chunked decoding
     */
    void SinkAttachmentData();
    
    /**
     * Attachment data finished for chunked decoding
     * @param aLastDataChunk is the data chunk the last one
     */
    void FinishSinkingAttachment( TBool aLastDataChunk );
    
    /**
     * Last chunk postamble for chunked decoding
     */
    void FinalizeSinkingLastChunk();
    
    /**
     * Do headers
     */
    void DecodeHeadersL();

    /**
     * Save the MMS headers
     */
    void SaveMMSHeadersL(); 

    /**
     * Decode one header
     */
    void DecodeOneHeaderL();
    
    /**
     * Get one header.
     * iPosition is advanced to point to the start of value. <br>
     *     (value coding depends on header.) <br>
     * If header name is unknown, pointer is advanced to point
     *     beyond parameters.
     * MMS headers only, assigned numbers only
     * @return -1, if unknown header assignment
     */
    TInt GetHeaderName();

    /**
     * Skip an unknown header name.
     */
// no longer used     
/*     
    void SkipTextString();
*/    

    /**
     * Skip field value (needed for unknown fields).
     */ 
    void SkipFieldValue();

    /**
     * Skip parameter name (needed for unsupported parameters).
     */ 
    void SkipParameterName();

    /**
     * Get uintvar that tells how many bytes will follow.
     * @return decoded number according to WSP uintvar specs
     */
    TUint32 GetUintvar();

    /**
     * Decode From field.
     * Caller must delete returned TDes16*
     * @return descriptor holding the address in unicode
     * (type has been removed)
     * from is different from rest of addresses, as there 
     * will be the "address present" token
     */
    HBufC16* DecodeFromL();
    
    /**
     * Decode address.
     * Caller must delete returned TDes16*
     * @return descriptor holding the address in unicode
     * (type has been removed)
     */
    HBufC16* DecodeAddressL();

    /**
     * Get simple text string.
     * ASCII into unicode.
     * @param aKeepQuote retain quote in the beginning of string.
     *      Needed only when caled from GetEncodedTextStringL.
     * @return descriptor holding unicode version of ASCII string
     */
    HBufC16* GetSimpleTextStringL( TBool aKeepQuote = EFalse );

    /**
     * Get encoded text string.
     * Convert to unicode from character set specified.
     * @return descriptor holding unicode version of string
     */
    HBufC16* GetEncodedTextStringL();

    /**
     * Get a text string as bytes (no conversions).
     * Data is not copied! The result is valid only as long as the
     *     source stays alive. The pointer will point to our
     *     decoding buffer, so it is valid as long as we stay alive.
     * @return descriptor pointing to byte string
     */
    TPtrC8 GetByteString();

    /**
     * Get a text string as bytes (no conversions).
     * Data is not copied! The result is valid only as long as the
     *     source stays alive. The pointer will point to our
     *     decoding buffer, so it is valid as long as we stay alive.
     * Leading 0x7F is removed, but characters from 0x80 to 0x8f are valid
     * @return descriptor pointing to byte string
     */
    TPtrC8 GetUtf8String();

    /**
     * Get short integer or a long integer that is no longer than 32 bits.
     * @return 32 bit integer
     * If the length of the integer is more than 4 bytes, the field is skipped
     */
    TUint32 GetLongOrShortInteger();

    /**
     * Get an integer that may be up to 64 bits long (for example date).
     * @return 64 bit integer
     */
    TInt64 GetVeryLongInteger();

    /**
     * Get a well known header assignment, skip field value if unknown coding.
     * @return token value, 0 if unknown coding (text string etc.)
     */
    TUint8 GetWellKnownFieldValueOrSkip();

    /**
     * Get token and date or interval value for time fields that have alternate coding.
     * @param aTime reference to 64 bit integer that receives
     *     the time.
     * @return absolute or reltive token, KMmsAbsoluteToken or
     *     KMmsRelativeToken. Returns 0, if cannot interpret field.
     */
    TUint8 GetRelativeOrAbsoluteTime( TInt64& aTime );

    /**
     * Get field length and skip the bytes that indicate the length.
     * If field is one-byte token, it is skipped and 0 is returned.
     * If field is text string, the length of the string (including 
     * terminating zero) is returned.
     * The field can be skipped by adding the indicated number of bytes to
     * current postition
     * @return the length of the value. 0 if not enough data
     */
    TUint GetValueLength();
    
    /**
     * Check if there is enough data in the buffer to decode a whole header
     *
     * @since v3.1
     * iPosition is always restored to its original value.
     *
     * @return ETrue if a whole header and its value is in the buffer.
     *     EFalse if not enough data and more should be obtained from the data source
     */
    TBool CheckHeaderLength();
    
    /**
     * Check if there is enough data in the buffer to decode a whole value
     *
     * @since v3.1
     *
     * @param aCheckUintvarData ETrue if routine must check that enough data bytes
     *     follow uintvar.
     *     This is needed when uintvar specifies the length of a header
     *     EFalse if only the integrity of the uintvar itself must be checked
     * @return ETrue if a whole value is in the buffer.
     *     EFalse if not enough data and more should be obtained from the data source
     */
    TBool CheckValueLength( TBool aCheckUintvarData );
    
    /**
     * See if there are enough bytes in the buffer to define a legal uintvar
     *
     * @since v3.1
     *
     * @return ETrue if a whole value is in the buffer.
     *     EFalse if not enough data and more should be obtained from the data source
     *     After check iPosition still points to the beginning of the Uintvar checked
     */
    TBool CheckUintvarLength();

    /**
     * Get attachment mime type.
     * @return pointer (with length) to the mime type string
     */
    TPtrC8 GetContentTypeL();

    /**
     * Get multipart content type.
     * Also gets parameters for the multipart type (start).
     * Root content type parameter is discarded (not used).
     * Start-info parameter is discarded (unless we know how to use it!).
     * @return assigned number for multipart content type,
     *   0 if not multipart (monoblock body)   
     */
    TUint8 GetMultipartContentTypeL();

    /**
     * Get content type and content type parameters for one attachment
     */
    void GetAttachmentContentTypeL();

    /**
     * Decode one content header from multipart headers.
     */
    void DecodeOneContentHeaderL();

    /**
     * Get one content header.
     * iPosition is advanced to point to the start of value.
     *     (value coding depends on header.)
     * @param atextHeader pointer set to header in text form
     * @return assigned number of header
     *      -1, if unknown header assignment
     *      0x7f if textual header (to handle X-type headers)
     */
    TInt GetContentHeaderName( TPtrC8& aTextHeader );

    /**
     * Decode one attachment.
     */
    void DecodeOneAttachmentL();
    
    /**
     * Decode attachment headers
     */
    void DecodeAttachmentHeadersL();
     
    /**
     * Extract parameters that are separated by semicolons.
     * Extracted parameters are stored to MIME headers.
     * @param aBuffer content parameter string ( actual content type string removed already)
     *     this should be of the form:
     *     parameter1=value1;parameter2=value2;parameter3=value3
     */
    void ExtractContentTypeParametersL( const TPtrC8& aBuffer );

    /**
     * Try to form filename from possibly MIME-encoded string
     * @param aSource suggestion for filename, may be MIME encoded
     * @param aDestination full path and filename, if aSource is valid name.
     * @param aPath path for the file.
     * @return ETrue if aSource forms a valid filename, EFalse if not.
     */
    TBool MakeFilenameL( TPtrC8& aSource, TParse& aDestination, const TFileName& aPath );

    /**
     * Cleanup (close streams etc.).
     */
    void FinishL();
    /**
    * check DRM contents.
    */
    TBool CheckDRMContent();
    /**
     * Dump binary data into file.
     */
    void DumpL();
    
    /**
     * Dump smil part into separate file if needed
     @param aDataLength length of smil part
     */
    void DumpSmil(TInt aDataLength);

    /**
     * Log value yes/no
     * @param aTitle title text
     * @param aValue value that maps to KMmsYes or KMmsNo
     */
    void LogYesNo( TRefByValue<const TDesC> aTitle, TInt aValue );

    /**
     * Decode X-Mms-MM-Flags header and store value
     */
    void GetKeywordL();
    
    /** 
     * Get active and complete own status with error code.
     * To be used when state machine loop must be terminated with error
     */
    void CompleteSelf( TInt aError );
    
    /**
     * Check if string contains only US-ASCII characters.
     * @param aString the string to be checked
     * @return ETrue = ASCII, EFalse = contains characters >= 0x80
     */
    TBool IsStringSafe( const TDesC8& aString );
    
    /**
    * decode FROM header
    */
    void DecodeFromHeaderL();
    
    /**
    * decode TO header
    */
    void DecodeToL();
    
    /**
    * decode CC header
    */
    void DecodeCcL();
    
    /**
    * decode BCC header
    */
    void DecodeBccL();
    
    /**
    * decode content location header
    */
    void DecodeContentLocationHeaderL();
    
    /**
    * Decode message date header
    */
    void DecodeDateHeaderL();
    
    /**
    * Decode delivery report wanted header
    */
    void DecodeDeliveryReportHeader();
    
    /**
    * Decode message delivery time header
    */
    void DecodeDeliveryTimeL();
    
    /**
    * Decode message expiration header
    */
    void DecodeExpiryL();
    
    /**
    * Decode Message Class header
    */
    void DecodeMessageClass();
    
    /**
    * Decode message Id header
    */
    void DecodeMessageIdL();
    
    /**
    * Decode Message Type Header
    */
    void DecodeMessageType();
    
    /**
    * Decode MMS encapsulation version header
    */
    void DecodeMmsVersion();
    
    /**
    * Decode message size header
    */
    void DecodeMessageSize();
    
    /**
    * Decode priority header
    */
    void DecodePriority();
    
    /**
    * Decode Read Reply wanted header
    */
    void DecodeReadReply();
    
    /**
    * Decode delivery report sending allowed header
    */
    void DecodeReportAllowed();
    
    /**
    * decode response status or retrieve status header
    * @param aHeader tells if this is a response status or retrieve status
    */
    void DecodeResponseStatusL( TInt aHeader );
    
    /**
    * decode response text or retrieve text header
    */
    void DecodeResponseTextL( TInt aHeader );
    
    /**
    * Decode sender visibility header
    */
    void DecodeSenderVisibility();
    
    /**
    * decode message status header
    */
    void DecodeStatus();
    
    /**
    * decode subject header
    */
    void DecodeSubjectL();
    
    /**
    * decode Transaction ID
    */
    void DecodeTidL();
    
    /**
    * decode content type header
    */
    void DecodeContentTypeL();
    
    /**
    * Decode read status header
    */
    void DecodeReadStatus();
    
    /**
    * Decode Reply charging header
    */
    void DecodeReplyCharging();
    
    /**
    * Decode reply charging deadline header
    */
    void DecodeReplyChargingDeadlineL();
    
    /**
    * Decode Reply charging id header
    */
    void DecodeReplyChargingIdL();
    
    /**
    * Decode Reply charging size header
    */
    void DecodeReplyChargingSize();
    
    /**
    * Decode Previously-sent-by header
    */
    void DecodePreviousSenderL();
    
    /**
    * Decode Previously sent date header
    */
    void DecodePreviouslySentDateL();
    
    /**
    * Decode store-to-MMBox header
    */
    void DecodeStoreHeaderL();
    
    /**
    * Decode State in MMBox
    */
    void DecodeMMBoxStateL();
    
    /**
    * Decode MMBox Store status header
    */
    void DecodeMMBoxStoreStatusL();
    
    /**
    * Decode MMBox Store status text header
    */
    void DecodeMMBoxStoreStatusTextL();
    
    /**
    * Decode stored in MMBox header
    */
    void DecodeStoredInMMBoxHeaderL();
    
    /**
    * Decode attributes header in MMBox View
    */
    void DecodeAttributesHeaderL();
    
    /**
    * Decode Totals header in MMBox view
    */
    void DecodeTotalsL();
    
    /**
    * Decode totals header in MMBox view
    */
    void DecodeMboxTotalsL();
    
    /**
    * Decode MMBox View Quota header
    */
    void DecodeQuotaHeaderL();
    
    /**
    * Decode MMBox Quota header
    */
    void DecodeMBoxQuotasL();
    
    /**
    * Decode message count header in MMBox View
    */
    void DecodeMessageCountL();
    
    /**
    * Decode start header in MMBox View
    */
    void DecodeStartInMMBoxViewL();
    
    /**
    * Decode Distribution Indicator header
    */
    void DecodeDistributionIndicator();
    
    /**
    * Decode Element descriptor header
    */
    void DecodeElementDescriptorL();
    
    /**
    * Decode the limit header in MMBox view (max number of messages)
    */
    void DecodeMessageLimitL();
    
    /**
    * Decode the extended notification header
    */
    void DecodeExtNotifTextL();

    /**
    * Decode the extended notification end of string indicator
    */
    void DecodeExtNotifEolL();
    
    /**
    * Decode Content Class header
    */
    void DecodeContentClass();
    
    /**
    * Decode DRM Content header
    */
    void DecodeDrmContentHeader();
    
    /**
    * Decode Adaptation allowed header
    */
    void DecodeAdaptationAllowed();
    
    /**
    * Decode application id header
    */
    void DecodeApplicationIdL();
    
    /**
    * Decode reply to application id header 
    */
    void DecodeReplyApplicationIdL();
    
    /**
    * Decode application info header
    */
    void DecodeApplicationInfoL();
    
    /**
    * Decode recommended retrieval mode
    */
    void DecodeRecommendedRetrievalMode();

    /**
    * Decode explanation for the recommended retrieval mode
    */
    void DecodeRecommendedRetrievalModeTextL();
    
    /**
    * Decode id of a message to be cancelled or replaced
    */
    void DecodeCancelReplaceIdL( TInt aHeader );
    
    /**
    * Decode status of cancel operation
    */
    void DecodeCancelStatus();

#ifndef _NO_MMSS_LOGGING_
    /**
     * log date
     * @param aDate date to be logged
     */
    void LogDateL( const TInt64& aDate );
#endif

private:    // Data
    RFs                 iFs; // file system
    TInt                iError;
    TInt                iState;
    MMmsEntryWrapper*   iEntryWrapper;
    CMmsHeaders*        iMmsHeaders;
    CMsvMimeHeaders*    iMimeHeaders;

    CBufFlat*           iDecodeBuffer;
    TUint               iPosition;  // pointer to decode buffer
    TUint               iLength;    // total length of the message to be encoded
    TUint               iDataStart; // starting point of multipart data
    TUint               iNextStart; // starting point of next multipart
    TUint               iNumberOfAttachments;
    TUint               iTotalSize;
    // pointer to start of first text/plain part to be used to generate
    // a description in case actual subject is missing
    TUint               iFakeSubject;
    TUint               iTextPlainLength; // length of the data
    // IANA MIBEnum value of the first text/plain part
    TUint               iCharacterSet;
    // Assigned number for multipart body type.
    // If not multipart, iMultipartType will be 0.
    TUint8              iMultipartType;
    TPtrC8              iRootContentId;
    HBufC8*             iRootContentIdBuffer;
    TMsvAttachmentId    iRootAttachmentId;
    TMsvAttachmentId    iFirstAttachmentId; // if multipart/related,
                        // and root not defined, first attachment is root
    // Counter to generate filenames atta1, atta2, etc.
    // Also keeps track of total number of attachments to support audio messaging
    TInt                iAttaNumber;
    TInt                iSmilCount; // counts the number of SMIL parts
    TInt                iAudioCount; // counts the number of audio(amr) parts
    
    TInt32              iDRMFlags;

    // iLogAllDecoded is obsolete - everything is logged in debug version
    TBool               iLogAllDecoded;
    TBool               iDumpIncoming;

    TPtrC8              iMultipartRootType; // for logging only
    TParse              iParse;     // parse buffer stores current atta filename
    TBufC8<18>          iTempBuffer;
    TInt                iPlainTexts; // number of plain text attachments
    // if caller wants to know where position is, this pointer is non-zero
    TInt*               iUpdatedPosition;
    TBool               iDoNotUpdateParentEntry;
    CMsvStore*          iStore; // store kept open to save several attachments
    
    TInt                iDecodingStage; // stage for chunked encoding
    TInt                iCurrentAttaLength; // data length for current attachment
    TInt                iAttaDataWritten; // data that has already been written to atta file
    TInt                iOldData; // the amount of data left undecoded
    TMsvAttachmentId    iCurrentAttachment;
    TMsvAttachmentId    iFirstTextPlain;
    TBool               iUseForSubject;
    TBool               iRemoveDrm; // remove DRM atta (not really an error)
    // Emergency brake to detect when last header has been decoded
    // Needed when receiving a message that has no attachments from a server
    // that always sends an empty last chunk
    TBool               iLastHeader; 
    
        
#ifndef _NO_MMSS_LOGGING_
// iFilename is only needed when logging and dumping
    TFileName           iFileName ; // filename buffer as member to save stack space
#endif

    };

#endif      // CMMSDECODE_H
            
// End of File
