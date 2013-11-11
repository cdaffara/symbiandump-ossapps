// Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// MIUTHDR.H
//

/**
 * @file 
 * @publishedAll
 * @released
 */

#ifndef __MIUTHDR_H__
#define __MIUTHDR_H__

#include <s32strm.h>
#include <badesca.h>
#include <pop3cmds.h>
#include <msvstd.h>
#include <msvids.h>			// KMsvNullIndexEntryId
#include <miutstd.hrh>			// KUidMsvEmailTextEntryValue
#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS  
#include "timrfc822datefield.h"
#endif
/** 
 * IMail-specific stream type within an IMail file store for CImHeader
 * @publishedAll
 * @released
 */
const TUid KUidMsgFileMimeHeader	= {0x1000160F};		// 268441103 decimal
/** 
 * IMail-specific stream type within an IMail file store for CImMimeHeader
 * @publishedAll
 * @released
 */
const TUid KUidMsgFileIMailHeader	= {0x100001D8};		// 268435928 decimal

/** 
 * @publishedAll
 * @released
 */
class CMsvStore;
class TMsvEntry;
class RMsvReadStream;
class RMsvWriteStream;
/** 
 * @internalTechnology
 * @released
 */
class CImEncodedHeader;

/** Defines how emails will be encoded when they are sent.

MIME standards are defined in RFC 2045, RFC 2046 and RFC 2047. 
@publishedAll
@released
*/
enum TMsgOutboxBodyEncoding
	{
	/** Emails are sent with the default encoding, which is as for EMsgOutboxMIME. */
	EMsgOutboxDefault,
	/** Emails are sent as plain text RFC822 messages. 
	
	Attachments are encoded using UUEncoding algorithm. This method would not 
	normally be used unless the sender was sure that the receiving mail clients 
	don't support MIME. */
	EMsgOutboxNoAlgorithm,
	/** Text is encoded using quoted-printable encoding, and all attachments are encoded 
	using BASE64 encoding.
	
	This is the default setting for this option. */
	EMsgOutboxMIME,
	/** Text parts of the message are sent as MIME TEXT/HTML parts, and are encoded 
	using UTF-8 encoding system. 
	
	All attachments are encoded using BASE64 encoding. Receiving mail clients 
	that do no support MHTML mail will display the HTML message text to the user 
	including all HTML tags, etc. */
	EMsgOutboxMHTMLAsMIME,
	/** Each text part of the message is sent as two alternative MIME parts.
	
	The first part is sent as plain text using quoted-printable encoding, the 
	second part is sent as a TEXT/HTML part which is encoded using UTF-8 encoding 
	system. All attachments are encoded using BASE64 encoding. This is the standard 
	encoding method if the user wishes to send MHTML mail. */
	EMsgOutboxMHTMLAlternativeAsMIME
	};

/** Messaging encoding types. 
@publishedAll
@released
*/
enum TImEncodingType 
	{
	/** Unknown. */
	EEncodingTypeUnknown,
	/** None. */
	EEncodingTypeNone,
	/** 7-bit. */
	EEncodingType7Bit,
	/** 8-bit. */
	EEncodingType8Bit,		// ie lines still <1000 chars and terminated by CRLF
	/** Binary. */
	EEncodingTypeBinary,	// ie any length of data, not terminated by CRLF
	/** QP. */
	EEncodingTypeQP,
	/** Base64. */
	EEncodingTypeBASE64,
	/** UU. */
	EEncodingTypeUU
	};


// Class used for storing extracting information (to/from header)..
// regarding the encoding in an incoming email message header.
//----------------------------------------------------------------------------------------
class TImHeaderEncodingInfo
//----------------------------------------------------------------------------------------
/** Stores encoding data found in message headers. 
@publishedAll
@released
*/
	{
public:
	/** Message field types. */
	enum TFieldList 
		{ 
		EFrom, 		/**< "From" field. */
		EReplyTo, 	/**< "Reply To" field. */
		ETo, 		/**< "To" field. */
		ECc, 		/**< "Cc" field. */
		EBcc, 		/**< "Bcc" field. */
		ESubject	/**< Subject field. */
		};
	/** Encoding types. */
	enum TEncodingType 
		{ 
		EBase64, 		/**< Base64. */
		EQP, 			/**< QP. */
		ENoEncoding, 	/**< No encoding */
		EUU				/**< UU. */
		};

public:
	IMPORT_C TImHeaderEncodingInfo();
	IMPORT_C TImHeaderEncodingInfo(const TImHeaderEncodingInfo& aFrom);

	IMPORT_C void ExternalizeL( RWriteStream& aWriteStream ) const;
	IMPORT_C void InternalizeL( RReadStream& aReadStream );

	inline void SetField(const TFieldList aField);
	inline TFieldList Field() const;

	inline void SetOffset(const TInt aOffset);
	inline TInt Offset() const;

	inline void SetLength(const TInt aLength);
	inline TInt Length() const;

	inline void SetCharsetUid(const TUint aUid);
	inline TUint CharsetUid() const;

	inline void SetEncodingType(const TDesC8& aChar);
	inline void SetEncodingType(const TEncodingType aType);
	inline  TEncodingType EncodingType() const;

	inline void SetArrayValue(const TInt aValue);
	inline  TInt ArrayValue() const;

	inline void SetAddSpace(const TBool atrueFalse);
	inline  TBool AddSpace() const;

	inline void SetEncodedLength(const TInt aLength);
	inline TInt EncodedLength() const;

private:
	TFieldList		iField; 
	TUint16			iOffset;
	TUint16			iLength;
	TEncodingType	iType;
	TUint16			iArrayValue;
	TBool	iAddSpace; // A space char needs adding between two adjoining 'encoded-words'
	TUint8	iEncodedLength; // This can be used to determine space needed for buffer.
	TUint	iCharsetUid; 

	};


//------------------------------------------------------------------------------


class CImHeader : public CBase
/** Encapsulates an Internet Mail (RFC822) header.

Note that the class contains some obsolete functions that take a narrow descriptor: 
always use the equivalent wide descriptor function instead. 
@publishedAll
@released
*/
	{
public:
/** Flags used to determine where to obtain the ReplyTo address for the header 
field of that name. */
	enum TReplyTo 
		{ 
	/** Get address from originator. */
		EOriginator, 
	/** Get address from sender. */
		ESender, 
	/** Get address from all sources. */
		EAll,
	/** Get address from recipients. */
		ERecipients
		};

	IMPORT_C static CImHeader* NewLC();
	IMPORT_C ~CImHeader();
	IMPORT_C void Reset();

	// streaming operations
	IMPORT_C void InternalizeL(RMsvReadStream& aReadStream);
	IMPORT_C void ExternalizeL(RMsvWriteStream& aWriteStream) const;
	IMPORT_C void RestoreL(CMsvStore& aMessageStore);
	IMPORT_C void StoreL  (CMsvStore& aMessageStore) const;
	IMPORT_C void StoreWithoutCommitL(CMsvStore& aMessageStore) const;


	// the bulk of the member functions are accessors & mutators so we can use
	// objects of the class as data repositories during the data-marshalling stage
	// of getting imail headers to/from the internet
	IMPORT_C const TPtrC Subject() const;
	IMPORT_C void SetSubjectL(const TDesC8& aSubject);

	// Internet message ID
	IMPORT_C const TPtrC8 ImMsgId() const;
	/** Sets the "MessageId" header field.
	
	@param aImMsgIdL Field value */
	IMPORT_C void SetImMsgIdL(const TDesC8& aImMsgIdL);

	IMPORT_C const TPtrC From() const;
	IMPORT_C void SetFromL(const TDesC8& aFrom);

	IMPORT_C const TPtrC ReplyTo() const;
	IMPORT_C void SetReplyToL(const TDesC8& aReplyTo);

	IMPORT_C const TPtrC ReceiptAddress() const;
	/** Sets the "Receipt" header field. 
	
	@param aReceiptAddress Field value */
	IMPORT_C void SetReceiptAddressL(const TDesC8& aReceiptAddress);

	IMPORT_C TUint Charset() const;
	IMPORT_C void SetCharset(const TUint aCharset);

#if defined (_UNICODE)
	IMPORT_C void SetFromL(const TDesC16& aFrom);
	IMPORT_C void SetReplyToL(const TDesC16& aReplyTo);
	/** Sets the "Receipt" header field. 
	
	@param aReceiptAddress Field value */
	IMPORT_C void SetReceiptAddressL(const TDesC16& aReceiptAddress);
	IMPORT_C void SetSubjectL(const TDesC16& aSubject);
#endif

	IMPORT_C TInt DataSize();

	inline const TUint RemoteSize() const;
	inline void SetRemoteSize(TUint aSize);

	inline const TUint16 Version() const;
	inline void SetVersion(TUint16 aVersion);

	// give const access to the recipient lists
	inline const CDesCArray& ToRecipients () const;
	inline const CDesCArray& CcRecipients () const;
	inline const CDesCArray& BccRecipients() const;
	
	// give non-const access to the recipient lists
	inline CDesCArray& ToRecipients () ;
	inline CDesCArray& CcRecipients () ;
	inline CDesCArray& BccRecipients() ;

	inline CArrayFix<TImHeaderEncodingInfo>& EncodingInfo() ;
	inline const CArrayFix<TImHeaderEncodingInfo>& EncodingInfo() const;

	IMPORT_C void AddEncodingInfoL(TImHeaderEncodingInfo& aInfo);

//-------------------------------------------------------------------------------------
//----------------------Used for forwarding/replying to an email ----------------------

	IMPORT_C const TPtrC8 ResentMsgId() const;
	IMPORT_C void SetResentMsgIdL( const TDesC8& );

	IMPORT_C const TPtrC ResentFrom() const;
	IMPORT_C void SetResentFromL( const TDesC& );

	// give const access to the recipient lists
	inline const CDesCArray& ResentToRecipients () const;
	inline const CDesCArray& ResentCcRecipients () const;
	inline const CDesCArray& ResentBccRecipients() const;

	// give non-const access to the recipient lists
	inline CDesCArray& ResentToRecipients ();
	inline CDesCArray& ResentCcRecipients ();
	inline CDesCArray& ResentBccRecipients();

	IMPORT_C const TPtrC8 InReplyTo() const;
	IMPORT_C void SetInReplyToL(const TDesC8&);

	IMPORT_C TInt CreateForwardL(CImHeader&, TDesC&);
	IMPORT_C TInt CreateReplyL(CImHeader&, TReplyTo, TDesC&);
	IMPORT_C void CreateReceiptL(CImHeader&, TDesC&);

//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------

	// Body Encoding (charset) stored on a per message basis
	IMPORT_C TMsgOutboxBodyEncoding BodyEncoding() const;
	IMPORT_C void SetBodyEncoding(TMsgOutboxBodyEncoding aMessageBodyEncoding);

	IMPORT_C void ReDecodeL(RFs& aFS);
	IMPORT_C TUint OverrideCharset() const;
	IMPORT_C void SetOverrideCharset(TUint aCharset);
	IMPORT_C void SaveEncodedHeadersL();

private:
	CImHeader();
	void ConstructL();
	
	// these four functions keep the line count down as there are several instances
	// of streaming in/out HBufC objects.
	void ReadRecipientsL ( RReadStream&, CDesCArray* );
	void WriteRecipientsL( RWriteStream&, const CDesCArray* ) const;
	void FormatSubjectL(CImHeader&, TDesC&);

	//finds the duplicate recipients in To and Cc list
	TBool IsRecipientPresent(CImHeader& aCImHeader, TPtrC16 newRecipient);

#if (defined SYMBIAN_MESSAGESTORE_HEADER_BODY_USING_SQLDB)
	void ReStoreDBL(CMsvStore& aMessageStore);
	void StoreDBL  (CMsvStore& aMessageStore) const;
	void CreateImHeaderArrayListL(TDesC16& aStr, CDesCArray& aRecipients);
	void CreateEncodingInfoL(TDesC16& aEncodingStr);
	HBufC16* Convert8to16L(const TDesC8& aStr) const;
	void CreateBufferL( RBuf16& aBuf, CDesCArray& aRecipients)const;
#endif 
private:
	TUint16 iVersion;

	HBufC16* iFrom;
	HBufC* iSubject;
	HBufC8* iImMsgId;
	HBufC16* iReplyTo;
	HBufC16* iReceipt;

	CDesCArray* iTo;
	CDesCArray* iCc;
	CDesCArray* iBcc;
	TUint iRemoteSize;

//-------------------------------------------------------------------------------------
//----------------------Used for forwarding/replying to an email ----------------------

	HBufC* iResentFrom;
	HBufC8* iResentMsgId;
 	CDesCArray* iResentTo;
	CDesCArray* iResentCc;
	CDesCArray* iResentBcc;
	HBufC8* iInReplyTo;

//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------
	CArrayFix<TImHeaderEncodingInfo>* iEncodingInfo;
	TMsgOutboxBodyEncoding			iBodyEncoding;
	// The character set to be when sending message.
	// Overrides the charset passed in from the settings.
	TUint							i822HeaderCharset; 

	CImEncodedHeader* iEncodedHeader;
#if (defined SYMBIAN_MESSAGESTORE_HEADER_BODY_USING_SQLDB)
	friend class CTestEmailHeaderStoreReStore;
#endif 
	};

/** Folder type flags.

The flags correspond to the MIME multipart subtype. 
@publishedAll
@released
*/
enum TImEmailFolderType
	{
	/** Unknown. */
	EFolderTypeUnknown,
	/** Related. */
	EFolderTypeRelated,
	/** Mixed. */
	EFolderTypeMixed,
	/** Parallel. */
	EFolderTypeParallel,
	/** Alternative. */
	EFolderTypeAlternative,
	/** Digest. */
	EFolderTypeDigest,
	/** RFC822. */
	EFolderTypeRFC822,
	/** Partial. */
	EFolderTypePartial,
	/** Directory. */
	EFolderTypeDirectory,
	/** External. */
	EFolderTypeExternal
	};

/** Describes the types of off-line email operations than can occur.

@see TMsvEmailEntry::DisconnectedOperation() 
@publishedAll
@released
*/
enum TImDisconnectedOperationType
	{
	/** No operation. */
	ENoDisconnectedOperations,
	/** Create operation. */
	EDisconnectedCreateOperation,
	/** Delete operation. */
	EDisconnectedDeleteOperation,
	/** Change operation. */
	EDisconnectedChangeOperation,
	/** "Copy to" operation. */
	EDisconnectedCopyToOperation,
	/** "Copy from" operation. */
	EDisconnectedCopyFromOperation,
	/** "Copy within service" operation. */
	EDisconnectedCopyWithinServiceOperation,
	/** "Move to" operation. */
	EDisconnectedMoveToOperation,
	/** "Move from" operation. */
	EDisconnectedMoveFromOperation,
	/** "Move within service" operation. */
	EDisconnectedMoveWithinServiceOperation,
	/** Special operation. */
	EDisconnectedSpecialOperation,
	/** Unknown operation. */
	EDisconnectedUnknownOperation,
	/** Multiple operations. */
	EDisconnectedMultipleOperation
	};

// Remote folder message count limited to TUint16 by using lower 16 bits of "iMtmData3".
// Note: The upper 16 bits of "iMtmData3" are not currently used by "TMsvEmailEntry".
const TUint KMsvRemoteFolderEntriesMask =		0x0000FFFF;

class TMsvEmailEntry : public TMsvEntry
/** Provides storage for and access to extra email specific information for a message.

To get a TMsvEmailEntry, get the entry's TMsvEntry, e.g. using
@c CMsvEntry::Entry(), and use the constructor that takes a TMsvEntry parameter.

Many of the flags are only used by IMAP4 entries. These can be grouped as follows:

- IMAP-specific flags, as defined by the IMAP protocol (see RFC3501, 2.3.2. Flags Message Attribute)
  - GetIMAP4Flags() and SetIMAP4Flags() handle all the flags as a group
  - \\Seen flag: SetSeenIMAP4Flag(), SeenIMAP4Flag(). 
  - \\Answered flag: SetAnsweredIMAP4Flag(), AnsweredIMAP4Flag()
  - \\Flagged flag: SetFlaggedIMAP4Flag(), FlaggedIMAP4Flag()
  - \\Deleted flag: SetDeletedIMAP4Flag(), DeletedIMAP4Flag() 
  - \\Draft flag: SetDraftIMAP4Flag(), DraftIMAP4Flag() 
  - \\Recent flag: SetRecentIMAP4Flag(), RecentIMAP4Flag()
- Folder subscriptions and other folder-related flags: 
  - SetMailbox(), Mailbox(), 
  - SetSubscribed(), Subscribed(), 
  - SetLocalSubscription(), LocalSubscription()
  - SetRemoteFolderEntries(), RemoteFolderEntries()  
- Disconnected operation information:
  - SetDisconnectedOperation(), DisconnectedOperation().
- Partial download flags: these flags allow a client to test if a message or its body has
  been partially downloaded (see TImImap4GetPartialMailInfo)   
  - SetPartialDownloaded(), PartialDownloaded() 
  - SetBodyTextComplete(), BodyTextComplete()
- UID information: IMAP messages have a unique identifier number (see 
  RFC3501, 2.3.1.1. Unique Identifier (UID) Message Attribute)
  - SetValidUID(), ValidUID()
  - SetUID(), UID().
  
The following flags are set only by the IMAP MTM. They indicate if the 
message contains a vCard, vCalendar (http://www.imc.org), or iCalendar (RFC 2445) attachment.

- VCard(), SetVCard()
- VCalendar(), SetVCalendar() 
- ICalendar(), SetICalendar()

The following flags apply to all email protocols:

- Receipt requested: Receipt(), SetReceipt()
- MHTML content: MHTMLEmail(), SetMHTMLEmail()

Note that the email specific data is stored in the reserved data members @c iMtmData1, 
@c iMtmData2 and @c iMtmData3 of the TMsvEntry base class. These public data members 
must not be directly altered by clients.

@publishedAll
@released
@see CImap4ClientMtm
*/
	{
public: // Public member functions
	IMPORT_C TMsvEmailEntry();	// default constructor
	IMPORT_C TMsvEmailEntry(const TMsvEntry& aGenericEntry); 

	IMPORT_C TBool operator==(const TMsvEntry& aEntry) const;
	IMPORT_C TBool operator==(const TMsvEmailEntry& aEntry) const;

	IMPORT_C void GetIMAP4Flags(TBool& rUnread,TBool& rSeen,TBool& rAnswered,TBool& rFlagged,TBool& rDeleted,TBool& rDraft,TBool& rRecent);
	IMPORT_C void SetIMAP4Flags(TBool aUnread,TBool aSeen,TBool aAnswered,TBool aFlagged,TBool aDeleted,TBool aDraft,TBool aRecent);

	IMPORT_C void SetMessageFolderType(TImEmailFolderType aFolderType);
	IMPORT_C TImEmailFolderType MessageFolderType() const;

	IMPORT_C void SetDisconnectedOperation(TImDisconnectedOperationType aDisconnectedOperationType);
	IMPORT_C TImDisconnectedOperationType DisconnectedOperation() const;

	IMPORT_C void SetPartialDownloaded(TBool aPartialDownloaded);
	IMPORT_C TBool PartialDownloaded() const;

	//
	inline void SetEncrypted(TBool aEncrypt);
	inline TBool Encrypted() const;
	//
	inline void SetSigned(TBool aSignature);
	inline TBool Signed() const;
	//
	inline TBool VCard() const;
	inline void SetVCard(TBool aVCard);
	//
	inline TBool VCalendar() const;
	inline void SetVCalendar(TBool aVCal);
	//
	inline TBool ICalendar() const;
	inline void SetICalendar(TBool aICal);
	//
	inline TBool Receipt() const;
	inline void SetReceipt(TBool aReceipt);

	inline TBool MHTMLEmail() const;
	inline void SetMHTMLEmail(TBool aMhtml);

	inline TBool BodyTextComplete() const;
	inline void SetBodyTextComplete(TBool aFlag);

	//
	// IMAP4-specific flags
	//
	inline void SetUnreadIMAP4Flag(TBool aUnread);
	inline TBool UnreadIMAP4Flag() const;
	//
	inline void SetSeenIMAP4Flag(TBool aSeen);
	inline TBool SeenIMAP4Flag() const;
	//
	inline void SetAnsweredIMAP4Flag(TBool aAnswered);
	inline TBool AnsweredIMAP4Flag() const;
	//
	inline void SetFlaggedIMAP4Flag(TBool aFlagged);
	inline TBool FlaggedIMAP4Flag() const;
	//
	inline void SetDeletedIMAP4Flag(TBool aDeleted);
	inline TBool DeletedIMAP4Flag() const;
	//
	inline void SetDraftIMAP4Flag(TBool aDraft);
	inline TBool DraftIMAP4Flag() const;
	//
	inline void SetRecentIMAP4Flag(TBool aRecent);
	inline TBool RecentIMAP4Flag() const;
	//
	inline void SetMailbox(TBool aMailbox);
	inline TBool Mailbox() const;
	//
	inline TBool Orphan() const;

	inline void SetOrphan(TBool aOrphan);
	//
	inline void SetValidUID(TBool aValid);
	inline TBool ValidUID() const;
	//
	inline void SetSubscribed(TBool aSubscribed);
	inline TBool Subscribed() const;
	//
	inline void SetLocalSubscription(TBool aLocal);		// i.e. is subscription local, or remote
	inline TBool LocalSubscription() const;
	//
	inline void SetUID(TUint32 aUid);
	inline TUint32 UID() const;
	//
	inline TInt RemoteFolderEntries() const;
	inline void SetRemoteFolderEntries(TInt aEntries);

private:
	enum TMsvEmailEntryFlags
		{	
		KMsvEmailEntryClearFlag				=0x00,
		// Flags set on Message entries...
		KMsvEmailEntryEncryptedFlag			=0x00000001,
		KMsvEmailEntrySignedFlag			=0x00000002,
		KMsvEmailEntryReceiptFlag			=0x00000004,
		KMsvEmailEntryOrphanFlag			=0x00000008,

		// VCard and VCalendar entries may be set on folders or attachments
		KMsvEmailEntryVCardFlag				=0x00000010,
		KMsvEmailEntryVCalendarFlag			=0x00000020,

		// Message contains HTML data
		// (Note: HTML and not only MHTML)
		KMsvEmailEntryMHTMLFlag				= 0x00000040,
		KMsvEmailEntryBodyTextCompleteFlag	= 0x00000080,

		KMsvEmailEntryNoDisconnectedOperations					= 0x00000000,
		KMsvEmailEntryDisconnectedCreateOperation				= 0x00000100,
		KMsvEmailEntryDisconnectedDeleteOperation				= 0x00000200,
		KMsvEmailEntryDisconnectedChangeOperation				= 0x00000300,
		KMsvEmailEntryDisconnectedCopyToOperation				= 0x00000400,
		KMsvEmailEntryDisconnectedCopyFromOperation				= 0x00000500,
		KMsvEmailEntryDisconnectedCopyWithinServiceOperation	= 0x00000600,
		KMsvEmailEntryDisconnectedMoveToOperation				= 0x00000700,
		KMsvEmailEntryDisconnectedMoveFromOperation				= 0x00000800,
		KMsvEmailEntryDisconnectedMoveWithinServiceOperation	= 0x00000900,
		KMsvEmailEntryDisconnectedSpecialOperation				= 0x00000A00,
		KMsvEmailEntryDisconnectedUnknownOperation				= 0x00000B00,
		KMsvEmailEntryDisconnectedMultipleOperation				= 0x00000C00,
		KMsvEmailEntryDisconnectedOperation						= 0x00000F00,

		// IMAP4-specific masks. Can be re-used by non-IMAP4 entries (eg POP3, SMTP)
		KMsvEmailEntryIMAP4SubscribedFlag	=0x00001000,
		KMsvEmailEntryIMAP4UnreadFlag		=0x00002000,
		KMsvEmailEntryIMAP4SeenFlag			=0x00004000,
		KMsvEmailEntryIMAP4AnsweredFlag		=0x00008000,	
		KMsvEmailEntryIMAP4FlaggedFlag		=0x00010000,	
		KMsvEmailEntryIMAP4DeletedFlag		=0x00020000,	
		KMsvEmailEntryIMAP4DraftFlag		=0x00040000,	
		KMsvEmailEntryIMAP4RecentFlag		=0x00080000,	
		KMsvEmailEntryIMAP4ValidUIDFlag		=0x00100000,
		KMsvEmailEntryIMAP4MailboxFlag		=0x00200000,
		KMsvEmailEntryIMAP4LocalSubFlag		=0x00400000,

		// MIME "multipart" types assigned to folders. 
		KMsvMimeFolderUnknownMask			=0x00000000,
		KMsvMimeFolderRelatedMask			=0x01000000,
		KMsvMimeFolderMixedMask				=0x02000000,
		KMsvMimeFolderAlternativeMask		=0x03000000,
		KMsvMimeFolderParallelMask			=0x04000000,
		KMsvMimeFolderDigestMask			=0x05000000,

		// MIME message types
		KMsvMimeFolderRFC822Mask			=0x06000000,
		KMsvMimeFolderPartialMask			=0x07000000,
		KMsvMimeFolderDirectoryMask			=0x08000000,	// for VCard, VCalendar types
		KMsvMimeFolderExternalMask			=0x09000000,
		KMsvMimeFolderMask					=0x0F000000,
		
		// Can be used by POP3 and IMAP4 MTMs.
		KMsvEmailEntryPartialDownloadFlag	=0x10000000,
		
		KMsvEmailEntryICalendarFlag			=0x20000000
		};

	};

const TUint16 KImMimeHeaderVersion = 2; // identify which version of this class has been stored

class CImMimeHeader : public CBase
/** Stores a MIME email header. 
@publishedAll
@released
*/
	{
public:
	IMPORT_C static CImMimeHeader* NewLC();
	IMPORT_C static CImMimeHeader* NewL();
	IMPORT_C ~CImMimeHeader();
	IMPORT_C void Reset();
	IMPORT_C TInt Size();	// number of header fields stored in object

	// streaming operations
	IMPORT_C void InternalizeL( RMsvReadStream& aReadStream );
	IMPORT_C void ExternalizeL( RMsvWriteStream& aWriteStream ) const;
	IMPORT_C void StoreWithoutCommitL(CMsvStore& aMessageStore) const;
	IMPORT_C void RestoreL( CMsvStore& aMessageStore );
	IMPORT_C void StoreL  ( CMsvStore& aMessageStore ) const;


	/** Sets the Content-Type field value (e.g. "text" in Content-Type:text/plain). 
	
	The value is MIME defined and should always be 8-bit. 
	
	@param aPath Field value */
	IMPORT_C void SetContentTypeL(const TDesC8& aPath);
	IMPORT_C const TPtrC8 ContentType() const;

	/** Sets the Content-Type subtype field value (e.g. "plain" in Content-Type:text/plain). 
	
	
	The value is MIME defined and should always be 8-bit. 
	
	@param aPath Field value */
	IMPORT_C void SetContentSubTypeL(const TDesC8& aPath);
	IMPORT_C const TPtrC8 ContentSubType() const;

	/** Sets the Content-Disposition field value (either inline or attachment). 
	
	The value is MIME defined and should always be 8-bit. 
	
	@param aPath Field value */
	IMPORT_C void SetContentDispositionL(const TDesC8& aPath);
	IMPORT_C const TPtrC8 ContentDisposition() const;

	/** Sets the Content-Description field value.
	
	The value is MIME defined and should always be 8-bit. 
	
	@param aPath Field value */
	IMPORT_C void SetContentDescriptionL(const TDesC8& aPath);
	IMPORT_C const TPtrC8 ContentDescription() const;

	/** Sets the Content-Base field value.
	
	@param aPath Field value */
	IMPORT_C void SetContentBaseL(const TDesC8& aPath);
	IMPORT_C const TPtrC8 ContentBase() const;

	/** Sets the Content-ID field value.
	
	@param aPath Field value */
	IMPORT_C void SetContentIDL(const TDesC8& aPath);
	IMPORT_C const TPtrC8 ContentID() const;

	/** Sets the Content-Location field value.
	
	@param aPath Field value */
	IMPORT_C void SetContentLocationL(const TDesC16& aPath);
	IMPORT_C const TPtrC16 ContentLocation() const;

	// IMAP-specific info describing relative location of entry w.r.t. parent Email message
	/** Sets IMAP-specific information for the relative location of the entry to the 
	parent email message.
	
	@param aPath Relative location */
	IMPORT_C void SetRelativePathL(const TDesC8& aPath);
	IMPORT_C const TPtrC8 RelativePath() const;

	/** Sets the Content-Transfer-Encoding field value.
	
	@param aType Field value */
	IMPORT_C void SetContentTransferEncodingL(const TDesC8& aType);
	IMPORT_C TImEncodingType ContentTransferEncoding() const;
	IMPORT_C TImEncodingType ContentTransferEncoding(TDes8& rType) const;
	IMPORT_C const TPtrC8 GetContentTypeValue(const TDesC8& aContentTypeParameter) const;

	// inlines to access arrays
	inline const CDesC8Array& ContentTypeParams() const;
	inline const CDesC8Array& ContentDispositionParams() const;
	inline const CDesC8Array& XTypeParams() const;

	inline CDesC8Array& ContentTypeParams();
	inline CDesC8Array& ContentDispositionParams();
	inline CDesC8Array& XTypeParams();		// anything else that is in the header, but not recognised

	inline const TUint16 Version() const;
	inline void SetVersion( TUint16 aVersion );

	IMPORT_C void SetMimeCharset(const TUint aCharset);
	IMPORT_C TUint MimeCharset() const;

private:
	CImMimeHeader();
	void ConstructL();
	TBool ArrayEntryExists(CDesCArray& aArray, TInt aField);

#if (defined SYMBIAN_MESSAGESTORE_HEADER_BODY_USING_SQLDB)
	void ReStoreMimeDBL(CMsvStore& aMessageStore);
	void StoreMimeDBL  (CMsvStore& aMessageStore) const;
	void CreateMimeArrayListL(TPtrC16 astr1 ,TInt ai);
	TInt BufSize() const;
#endif 
private:
	TUint16 iVersion;
	HBufC8* iRelativePath;
	HBufC8* iContentDescription;

	TImEncodingType iContentTransferEncoding;

	// MHTML parameters
	HBufC8* iContentBase;
	HBufC16* iContentLocation;
	HBufC8* iContentID;
	//
	HBufC8* iContentType;
	HBufC8* iContentSubType;
	HBufC8* iContentDisposition;
	//
	CDesC8Array* iContentTypeParams;			// zero or more "parameter"
	CDesC8Array* iContentDispositionParams;		// zero or more "parameters"
	CDesC8Array* iXTypeParams;					// zero or more X-Type "parameters"
	//
	TBuf<20> iContentTransferEncodingBuffer;	// must be big enough to store "quoted-printable" string

	// The uid value is always used in preference to the stored string.
	// Only when the uid is not set is the string used when sending.
	TUint iMimeCharset;	
	};



/**
 * Convert16to8L()
 *
 * Convert the 16 bit descripter to 8 bit.
 * @param astr A descripter to be convert into 8 bit discripter.
 * @return HBufC16* A 8 bit discripter.
 */
static inline HBufC8* Convert16to8L(TDesC16& aStr)//const
	{
	HBufC8* newFrom1 = HBufC8::NewL(aStr.Length());
	newFrom1->Des().Copy(aStr);
	return newFrom1;
	}


/**
 * ConvertToTInt()
 * 
 * Converts a string to an integer.
 * @param aStr A string to make Integer.
 * @return TInt A integer value 
 */
static inline TInt ConvertToTInt(TDesC16& aStr)
	{
	TLex string(aStr);
	TInt32 stringIntValue;
	string.Val(stringIntValue);
	return stringIntValue;
	}


#include <miuthdr.inl>

#endif // #defined __MIUTHDR_H__
