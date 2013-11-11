/*
* Copyright (c) 2002 - 2009 Nokia Corporation and/or its subsidiary(-ies).
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
*     Binary encoding of a multimedia message
*
*/



#ifndef CMMSENCODE_H
#define CMMSENCODE_H

//  INCLUDES
#include <mentact.h>
#include <badesca.h>
#include <cmsvattachment.h>

#include "mmsconst.h" // needed for TMmsRecipients
#include "mmsservercommon.h" // needed for logging flag definition
#include "mmscodecdatasupplier.h"

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class RBufWriteStream;
class CMmsHeaders;
class CMsvMimeHeaders;
class MMmsEntryWrapper;
class MMmsEntry;
class CMsgTextUtils;

// CLASS DECLARATION

/**
* Binary encoding of a multimedia message.
*
* @lib mmscodec.lib
* @since v2.1
*/
class CMmsEncode :public CMsgActive, public MMmsCodecDataSupplier
    {
public:
    
    /**
     * @param aFs file system handle.
     */
    IMPORT_C static CMmsEncode* NewL( RFs& aFs );
    
    virtual ~CMmsEncode();

public: // New functions
    
    /**
     * Start the state machine for encoding a message with headers and data.
     *
     * @param aEntryWrapper mixin for handling all message entry access
     * @param aMmsHeaders reference to CMmsHeaders class.
     * @param aEncodeBuffer buffer to hold the encoded data,
     * @param aStatus status of calling active object
     */
    IMPORT_C void StartL(
        MMmsEntryWrapper& aEntryWrapper,
        CMmsHeaders& aMmsHeaders,
        CBufFlat& aEncodeBuffer,
        TRequestStatus& aStatus );

    /**
     * Encode headers only.
     *
     * This function is for the miscellaneous control messages.
     * This is not an active function, one-shot only.
     * @param aMmsHeaders reference to CMmsHeaders class.
     * @param aEncodeBuffer buffer to hold the encoded data,
     */
    IMPORT_C void EncodeHeadersL(
        CMmsHeaders& aMmsHeaders,
        CBufFlat& aEncodeBuffer );

    /**
     * Start chunked encoding.
     * This method can be called only when it is known that the component
     * that handles the encoded data can use the callback to get more 
     * data chunks.
     *
     * @since v3.1
     * @param aEntryWrapper mixin for handling all message entry access
     * @param aMmsHeaders reference to CMmsHeaders class.
     * @param aEncodeBuffer buffer to hold the encoded data,
     * @param aStatus status of calling active object
     */
    IMPORT_C void StartChunkedL(
        MMmsEntryWrapper& aEntryWrapper,
        CMmsHeaders& aMmsHeaders,
        CBufFlat& aEncodeBuffer,
        TRequestStatus& aStatus );

// from base class MMmsCodecDataSupplier

    /**
     * from MMmsCodecDataSupplier.
     * Give poiner to next encoded data chunk.
     *
     * @since v3.1
     * @param aDataPart pointer to the next encoded data part
     * @param aLastDataChunk 
     *     ETrue if this is the last data chunk
     *     EFalse if this is not the last data chunk
     * @return error code, KErrNone if all is well.
     *     If returns some other error, transaction must be cancelled.
     */
    virtual TInt GetNextDataPart(
        TPtrC8& aDataPart, TBool& aLastDataChunk );
    
    /**
     * from MMmsCodecDataSupplier.
     * Empty buffer and prepare next data part (synchronous encoding).
     * When needed, caller will tell HTTP stack that the next data part is
     * ready by calling RHTTPTransaction::NotifyNewRequestBodyPartL()
     *
     * @since v3.1
     * @return error code, KErrNone if all is well.
     *     If returns some other error, transaction must be cancelled.
     */
    virtual TInt ReleaseData();
    
    /**
     * from MMmsCodecDataSupplier.
     * Return the overall data size.
     *
     * @since v3.1
     * @return The overall data size if it is known, otherwise KErrNotFound
     */
    virtual TInt OverallDataSize();
    
    /**
     * from MMmsCodecDataSupplier.
     * Reset the data supplier.
     * Data supplier must start the data generation from the beginning again.
     *
     * @since v3.1
     * @return KErrNone if reset is successful, other error code if data
     *     supplier cannot be reset
     */
    TInt ResetSupplier();

protected:
    
private:

    CMmsEncode();

    /**
     * @param aFs file system handle.
     */
    void ConstructL( RFs& aFs );

    /**
     * from CMsgActive.
     *
     * Active object completion.
     */
    void DoRunL();

    /**
     * from CMsgActive.
     *    
     * Sweep the floor after everything has crashed.
     * @param aStatus completion code
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
     * Initialize members to allow one-shot functions.
     * (for short tasks no active object invocation is needed)
     */
    void Reset();

    /**
     * Encode headers.
     */
    void EncodeHeadersL();
    
    /**
     * Encode headers in chunked mode.
     */
    void EncodeHeadersChunkedL();

    /** 
     * Do one attachment at a time.
     */
    void EncodeAttachmentL();
    
    /**
     * Do the actual work, can be called from active loop or passive loop
     */
    void DoEncodeAttachmentL(); 
    
    /**
     * Get encoding length for one attachment
     * @return size of the encoded attachment headers and attachment data
     */
    TInt DoGetAttachmentEncodingLengthL();
    
    /**
     * Encode attachment headers into the buffer and open the attachment file
     * @param aAttachMan reference to attachment manager for accessing
     *        attachment information and opening the file
     * @return size of the attachment
     */
    TInt EncodeHeadersAndGetFileL( MMsvAttachmentManager& aAttachMan );
    
    /**
     * Get the encoded size of the mime headers and the attachment data
     * @param aAttachMan reference to attachment manager for accessing
     *        attachment information and opening the file
     * @return size of the encoded attachment headers and attachment data
     */
    TInt GetHeadersAndFileSizeL( MMsvAttachmentManager& aAttachMan );
    
    /**
     * Calculate the length of headers for the attachment
     * @param aAttachmentInfo attachmentInformation structure for current attacment
     * @param aHeaderLength total length of encoded headers
     * @param aFoundName ETrue the name is among content-type parameters
     *                   EFalse the name is in the recommended filename parameter
     * @param aContentTypeSize length of the content type header
     * @param aContentType assigned number for content type, -1 if not found
     * @param aContentTypeString content type in string format if aContentType == -1
     * @return pointer to attachment name in 8bit format if aFoundName == EFalse
     *     If aFoundName == ETrue, returns NULL, because name in included in content
     *     type parameters and is not added separately.
     *     Caller must delete buffer when no longer needed.
     */
    HBufC8* CalculateAttachmentHeaderLengthL(
        CMsvAttachment& aAttachmentInfo,
        TUint& aHeaderLength,
        TBool& aFoundName,
        TUint& aContentTypeSize,
        TInt8& aContentType,
        TPtrC8& aContentTypeString );
    
    /**
     * Encode headers for one attachment
     * @param aSize size of the attachment binary data
     * @param aHeaderSize size of the headers in encoded format
     * @param aFoundName ETrue if the name parameter is among the 
     *        Content type parameters in MIME headers
     * @param aContentTypeSize size of the content type header
     * @param aContentType assigned number for the content type
     * @param aContentTypeString content type in string format if
     *        no assigned number is available
     * @param aNameString name of the attachment if aFoundName == EFalse
     */
    void EncodeAttachmentHeadersL(
        TUint aSize,
        TUint aHeaderSize,
        TBool aFoundName,
        TUint aContentTypeSize,
        TInt8 aContentType,
        TPtrC8& aContentTypeString,
        TPtrC8& aNameString );

    /**
     * Write all attachment data to the buffer
     * @param aAttachFile open handle for attachment file
     *     After reading the file pointer will point past data read
     * @param aSize amount of data to be read
     */
    void EncodeAttachmentData( RFile& aAttachFile, TInt aSize );

    /**
     * Cleanup (close streams etc.).
     */
    void FinishL();
    
    /**
     * Encode MMS request headers depending on the PDU type
     */
    void EncodeRequestHeadersL();

    // As different transactions have widely different headers,
    // a separate routine is used for each to avoid lots of extra
    // work when certain headers are not needed anyway.

    // If new transactions (message types) are added, they need
    // special handling anyway.

    /**
     * Send Request.
     */
    void EncodeSendRequestHeadersL();

    /**
     * Notify response.
     */
    void EncodeNotifyResponse();

    /**
     * Delivery acknowledgement.
     */
    void EncodeAcknowledgeIndication();

    /**
     * Mms notification.
     */
    void EncodeMmsNotificationL();

    // PDUs reserved for testing purposes
    
    /**
     * Send confirmation
     */
    void EncodeSendConfirmationL();

    /**
     * Retrieve confirmation.
     */
    void EncodeRetrieveConfirmationL();

    /**
     * Delivery report.
     */
    void EncodeDeliveryReportL();

    // MMS encapsulation version 1.1 PDUs

    /**
     * Forward Request.
     */
    void EncodeForwardRequestL();

    /**
     * Forward confirmation.
     * for testing purposes
     */
    void EncodeForwardConfirmationL();

    /**
     * ReadReply.
     * @param aPDUType KMmsMessageTypeReadRecInd or
     *    KMmsMessageTypeReadOrigInd (supported for testing purposes)
     */
    void EncodeReadReplyL();

    // MMS encapsulation 1.2 PDUs

    /**
     * MMBox store request.
     * @since 2.6
     */
    void EncodeMMBoxStoreRequestL();

    /**
     * MMBox store confirmation.
     * for testing purposes.
     * @since 2.6
     */
    void EncodeMMBoxStoreConfirmationL();

    /**
     * MMBox view request.
     * @since 2.6
     */
    void EncodeMMBoxViewRequestL();

    /**
     * MMBox view confirmation.
     * for testing purposes.
     * @since 2.6
     */
    void EncodeMMBoxViewConfirmationL();

    /**
     * MMBox upload request
     * @since 2.6
     */
    void EncodeMMBoxUploadRequestL();

    /**
     * MMBox upload confirmation
     * for testing purposes.
     * @since 2.6
     */
    void EncodeMMBoxUploadConfirmationL();

    /**
     * MMBox delete request or MMSC delete request
     * @since 2.6
     */
    void EncodeDeleteRequestL();

    /**
     * MMBox delete confirmation or MMSC delete confirmation.
     * for testing purposes.
     * @since 2.6
     */
    void EncodeDeleteConfirmationL();

    /**
     * MMBox description
     * for testing purposes.
     * @since 2.6
     */
    void EncodeMMBoxDescriptionL();

    // end of MMS encapsulation PDUs

    /**
     * Encode Text String (No Character set).
     * @param aString byte string
     */
    void EncodeTextString( const TDesC8& aString );

    /**
     * Encode Quoted Text String (No Character set).
     * This function adds a quote to the beginning of the text string.
     * needed for content-id
     * @param aString byte string
     */
    void EncodeQuotedTextString( const TDesC8& aString );

    /**
     * Encode Text String.
     * Checks if string can be sent as plain ASCII,
     *     if not, encodes it into utf-8
     * @param aString unicode string
     */
    void EncodeTextStringL( const TDesC& aString );

    /**
     * Encode date.
     * @param aDate local datetime in seconds from 1.1.1970
     */
    void EncodeDate( const TInt64& aDate );
    
    /**
     * Encode long integer.
     * @param 8 byte integer to be encoded
     */
    void EncodeLongInteger( const TInt64& aLongInteger );

    /**
     * Encode integer.
     * Will be encoded as short integer or long integer depending on value.
     * @param 4 byte integer to be encoded
     */
    void EncodeInteger( TUint aInteger );

    /**
     * Encode Sender.
     * The result is either a legal sender address or an "insert-address"
     *     token.
     * @param aSender pointer to sender name
     */
    void EncodeSenderL( const TPtrC& aSender );

    /**
     * Encode Address.
     * @param aAddress pointer to address
     */
    void EncodeAddressL( const TPtrC& aAddress );

    /**
     * Encode value length (long or short).
     * @param aLength value length that may be short or long
     */
    void EncodeValueLength( TUint aLength );

    /**
     * Encode Uintvar.
     * @param aInteger the integer to be coded - usually a length
     */
    void EncodeUintvar( TUint aInteger );

    /**
     * Check how many bytes are needed to encode a Uintvar.
     * @param aInteger the integer to be coded - usually a length
     * @return number of bytes needed in the encoding.
     */
    TInt GetUintvarLength( TUint aInteger );

    /**
     * Encode recipient.
     * @param aRecipientList array of recipient descriptors
     * @param aType recipient type (To, Cc, Bcc)
     */
    void EncodeRecipientL( const CDesCArray& aRecipientList,
        TMmsRecipients aType );

    /**
     * Encode optional encoded string text.
     * @param aHeader assigned header byte
     * @param aString text
     */
    void EncodeOptionalStringL( TUint8 aHeader, const TPtrC16& aString );

    /**
     * Encode optional encoded string text.
     * @param aHeader assigned header byte
     * @param aString text
     */
    void EncodeOptionalString( TUint8 aHeader, const TPtrC8& aString );

    /**
     * Encode absolute time or a time interval.
     * @param aInterval interval.
     * @param aDate absolute date. If date is 0, interval is encoded.
     */
    void EncodeIntervalOrDate( TInt aInterval, const TInt64& aDate );

    /**
     * Encode ReplyCharging size if defined.
     * @param aReplyChargingSize maximum number in octets for the reply-MM
     */
    void EncodeReplyChargingSize( TInt aReplyChargingSize );

    /**
     * Encode specified header if value is different from zero.
     * Encoding must be assigned byte value.
     * @param aHeader assigned header byte
     * @param aValue assigned value for the header
     */
    void EncodeOptionalByte( TUint8 aHeader, TInt aValue );

    /**
     * Encode specified header (mandatory).
     * Encoding must be assigned byte value
     * @param aHeader assigned header byte
     * @param aValue assigned value for the header
     */
    void EncodeMandatoryByte( TUint8 aHeader, TInt aValue );

    /**
     * Encode optional header that may be date or interval
     * @param aHeader assigned header byte
     * @param aInterval interval.
     * @param aDate absolute date. If date is 0, interval is encoded.
     * if both date and interval are 0, header is not added
     */
    void EncodeOptionalIntervalOrDate( TUint8 aHeader,
        TInt aInterval,
        const TInt64& aDate );

    /**
     * Encode header byte and 8-bit Text String (No Character set).
     * @param aHeader assigned header byte
     * @param aString byte string
     */
    void EncodeHeaderAndTextString( TUint8 aHeader,
        const TDesC8& aString );

    /**
     * Check if string contains only US-ASCII characters.
     * @param aString the string to be checked
     * @return ETrue = ASCII, EFalse = needs encoding
     */
    TBool IsStringSafe( const TDesC& aString );
    
    /**
     * Check if string contains only US-ASCII characters.
     * @param aString the string to be checked
     * @param aNumNonSafe number of characters >= 0x7F
     * @return ETrue = ASCII, EFalse = needs encoding
     */
    TBool IsStringSafe( const TDesC8& aString, TInt& aNumNonSafe );

    /**
     * Encode Content type header with start parameter.
     * The content-id of the attachment is searched. If the
     * attachment has no content-id, an id is generated by a random
     * number generator (not globally unique).
     * @param aRootId Internal Id of the attachment that is the root
     *     part of the message.
     */
    void EncodeMultipartRelatedHeaderL( const TMsvAttachmentId aRootId );

    /**
     * Encode Content type as multipart/mixed (no start specified).
     */
    void EncodeMultipartMixedHeaderL();

    /**
     * Encode keyword array.
     */
    void EncodeKeywordArrayL();

    /**
     * Encode optional integer. If value is 0, it is not added
     * @param aHeader assigned header byte
     * @aValue value to be encoded as an integer
     */
    void EncodeOptionalInteger( TUint8 aHeader, TUint aValue );

    /**
     * Encode attribute headers from array.
     * This is the list of information elements required for each message
     * @param aAttributeArray list of attributes as assigned numbers
     */
    void EncodeAttributes( RArray<TUint>& aAttributeArray );

    /**
     * Encode X-Mms-MM-State headers from an array.
     * In MMBox view PDUs this header may appear multiple times
     * @param aStateArray list of states used for filtering messages
     * (items have values draft/sent/new/retrieved/forwarded)
     */
    void EncodeMMBoxStates( RArray<TInt>& aStateArray );

    /**
     * Encode content location array.
     * Encodes content location headers in cases where more than one are
     *     allowed
     */
    void EncodeContentLocationArray();

    /**
     * Encode the three headers appearing at the beginning of most PDUs.
     * TID is not always required. If length of TID is 0, the header is
     *     not added.
     * @param aMessageType value for X-Mms-Message-Type header
     * @param aTID transaction ID
     * @param aVersion MMS encapsulation version number
     */
    void EncodeStartingHeaders( TInt aMessageType,
        const TPtrC8& aTID, TInt aVersion );
        
    /**
     * Encode Application Id and Reply to Application id as content type parameters
     */    
    void EncodeApplicationIdParametersL();    

    /**
     * Dump binary data into file.
     */
    void Dump();
    
    /**
     * Append a chunk of binary data into file - for chunked encoding support
     */
    void DumpAppend();
    
    /**
     * Encode that application id, reply application id and application info
     */
    void EncodeApplicationHeadersL();
    
    /**
     *  Encode cancel request PDU - for testing only
     */    
    void EncodeCancelRequest();
    
    /**
     * Encode cancel response PDU
     */
    void EncodeCancelResponse();
    
    /**
     * Encode utf8 string into MIME quoted printable format.
     * @param aSource string in utf8 character set.
     * @return data in encoded format. Caller must delete buffer
     */
    HBufC8* EncodeQuotedPrintableWordL( const TPtrC8& aSource );
    
    /**
     * Encode utf8 string into MIME base64 format.
     * @param aSource string in utf8 character set
     * @return data in encoded format. Caller must delete buffer
     */
    HBufC8* EncodeBase64WordL( const TPtrC8& aSource );

    /**
     * Open the message store(Edit mode) and process attachments 
     * for further encoding
     * @param None
     * @return void
     */
    void PreProcessAttachmentDataL();

    /**
     * Check and proceed if given attachment can be encoded using 
     * target encoding type based on its content type.
     * @param aAttachmentInfo attachment of the msv entry
     *        aMimeHeaders corresponding attachment mime headers
     * @return true/false based on attachment content type
     */
    TBool CheckAndUpdateAttachmentL( CMsvAttachment& aAttachmentInfo,
                                     CMsvMimeHeaders& aMimeHeaders );
    

    /**
     * checks if input content type is supported for target encoding 
     * @param aSrcCharSetMIBEnum MIB enum reprsenting current data format 
     *        aTargetCharSetMIBEnum target encoding MIB enum
     *        aAttachmentInfo attachment data to be converted 
     * @return true/false based on encoding success/failure
     */
    TBool ProcessAndConvertAttachmentDataL( TUint aSrcCharSetMIBEnum,
                                            TUint aTargetCharSetMIBEnum, 
                                            CMsvAttachment& aAttachmentInfo);

    /**
     * checks if input content type is supported for target encoding 
     * @param aContentType to check if the attachment data encoding is supported
     * @return true/false based on attachment content type
     */
    TBool IsConversionSupportedContentType( TInt aContentType );


    
private:    // Data
    RFs                 iFs; // file system
    TInt                iError;
    TInt                iState;
    MMmsEntryWrapper*   iEntryWrapper;
    CBufFlat*           iEncodeBuffer;
    CMmsHeaders*        iMmsHeaders;
    CMsvMimeHeaders*    iMimeHeaders;
    TInt                iPosition; // pointer to encode buffer (if needed)
    TInt                iNumberOfAttachments;
    TInt                iCurrentAttachment; // AttachmentManager index
    TFileName           iFileName;
#ifndef _NO_MMSS_LOGGING_
// only needed if output logging is enabled
// parse buffer as member to save stack space
    TParse              iParse;     
#endif
    TMsvId              iCurrentMessageId;
    
    TInt                iOverallDataSize;
    TBool               iLastChunk;
    TBool               iOnlyOneChunk;
    TInt                iEncodingStage; // stage for chunked encoding
    // Handle for the current attachment.
    // If the file is large it may be read to the buffer in chunks.
    // The file is kept open between reads. We want sole access.
    RFile               iAttachFile;
    // The attachment file is kept open if it is read in several parts.
    // In destructor the file must be closed in case the transaction
    // terminates with error
    TBool               iFileOpen;
    // Size of the current attachment.
    // This will allow us to keep track of the amount of data read from the file
    TInt                iCurrentFileSize;
    // The stage variable that will keep track of the part of message that
    // is going to be encoded next in case of chunked encoding
    TInt                iDataSupplierStage;
    // The max size of the ecode buffer for chunked encoding.
    TInt                iBufferSize; 
    // object to provide chaconv functionalities
    CMsgTextUtils*      iTextUtils;
    // target encoding type
    TUint               iTargetEncodingType;
    };

#endif      // CMMSENCODE_H
            
// End of File
