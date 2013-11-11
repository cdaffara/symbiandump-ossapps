// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// MIUTMSG.H
//

#ifndef __MIUTMSG_H__
#define __MIUTMSG_H__

#include <msvstd.h>
#include <msvapi.h>			// TMsvEntry, CMsvOperation etc
#include <mentact.h>		// CMsgActive
#include <miuthdr.h>		// CImHeader, CImMimeHeader
#include <txtrich.h>		// CRichText etc
#include <miutdef.h>		// CImHeader, CImMimeHeader
#include <mtmdef.h>
#include <miutatch.h>
#include <smtpset.h>
#include <miutpars.h>		// TImMessageField

#include <barsc.h>		// RResourceFile
#include <conarc.h>
#include <conlist.h>

#include <txtmrtsr.h>

//#include <mmsvattachmentmanager.h>
#include <s32mem.h>

//Content-Transfer-Encoding: types
_LIT8(KMimeQuotedPrintable, " quoted-printable");
_LIT8(KMimeApplOctet, " application/octet-stream");
_LIT8(KMime7Bit, " 7-bit");
_LIT8(KMimeBase64, " base64");
_LIT8(KMimeMime, "MIME*");
// Content-Disposition: types
_LIT8(KMimeInline, "inline");
_LIT8(KMimeAttachment, "attachment");
// Content-Type: types
_LIT8(KMimeMultipart, "multipart");
_LIT8(KMimeText, "text");
_LIT8(KMimeImage, "image");
_LIT8(KMimeAudio, "audio");
_LIT8(KMimeVideo, "video");
_LIT8(KMimeApplication, "application");
_LIT8(KMimeMessage, "message");
// Content-Type: Multipart types
_LIT8(KMimeMixed, "mixed");
_LIT8(KMimeRelated, "related");
_LIT8(KMimeAlternative, "alternative");
_LIT8(KMimeEncrypted, "encrypted");
_LIT8(KMimeParallel, "parallel");
_LIT8(KMimeDigest, "digest");
_LIT8(KMimeSigned, "signed");
// Content-Type: Text types
_LIT8(KMimeHtml, "html");
_LIT8(KMimePlain, "plain");
_LIT8(KMimeDirectory, "directory");
// Content-Type: Message types
_LIT8(KMimePartial, "partial");
_LIT8(KMimeExternal, "external-body");
_LIT8(KMimeRfc822, "rfc822");
// Content-Type: Directory types
_LIT8(KMimeProfile, "profile");
_LIT(KMimeVCardExtension, ".vcf");
_LIT8(KMimeVCalender, "vcalendar");
// Content-Type: Image, Audio, Video, Application types
_LIT8(KMimeBmp, "bmp");
_LIT8(KMimeGif, "gif");
_LIT8(KMimeJpeg, "jpeg");
_LIT8(KMimeTiff, "tiff");
_LIT8(KMimeWav, "wav");
_LIT8(KMimeZip, "x-gzip");
_LIT(KMimeHtmlExtension, ".html");
_LIT(KMiutMsgDefaultHtmlFileName, "attachment.html");
const TUid KUidMimeHeaders = {0x10204281};

#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS  
#include "cimmessagepart.h"
#endif

class CImHtmlConverter;

class CImAttachmentManager;
class CMsvMimeHeaders;
class CImPlainBodyText;

// Mixin provides interface to Browser control to resolve MHTML URIs.

class MImURITranslator
/** Translates a URI in an MHTML message into a message body part file name.

MHTML messages may contain multiple HTML body parts. Each of these body parts 
is able to reference another body part within the same multipart structure, 
or a body part within a parent multipart structure [RFC2110]. These references 
are called URI's.

When an MHTML message is parsed by the message engine, the HTML body of each 
part is stored as a binary file. This interface provides a means of finding 
the file name of the body part that corresponds to a given a URI.

The URI resolving process has the potential to become time consuming, therefore 
it is broken into two steps:

Resolve asynchronously the relative URI into an absolute URI. The message 
body parts are recursively searched in order to find an absolute base for 
the relative URI [RFC2110, RFC1808]. 

Once a relative URI has been resolved in to an absolute URI, then the message 
is searched for a body part that corresponds to it [RFC2110].

If no matching body part can be found, then the absolute URI is returned to 
the caller in place of the filename. This absolute URI should be used to search 
for the required data externally, for example by using HTTP.

@see CImEmailMessage 
@publishedAll
@released
*/
	{
public:
	/** Starts an asynchronous operation to resolve a specified URI.
	
	@param aMessageId The ID of the body part containing the URI which is to be 
	resolved. CImEmailMessage::FindFirstHTMLPageL() can be used to find the aMessageId 
	parameter for the first HTML page in a message.
	@param rBase The base parameter from the HTML part that contains the URI to 
	be resolved. If there is no base parameter in the HTML, then an empty descriptor 
	should be passed in.
	@param rURI The URI to be resolved.
	@param aStatus Asynchronous status word */
	virtual void FindUniversalResourceIdentifierL(TMsvId aMessageId, const TDesC& rBase, const TDesC& rURI, TRequestStatus &aStatus) = 0;

	/** Gets the results after a search.
	
	@param aLinkedEntryId On return, the message ID of the linked body part, providing 
	one is found. Note that you should keep a record of the returned message ID 
	as it is required for resolving any URI's that may be found in the linked 
	body part.
	@param aFileFound On return, true if a linked body part has been found. It 
	is set to false otherwise.
	@return If successful, the filename that corresponds to the URI, or the absolute 
	URI if no matching file can be found. If failed, NULL. */
	virtual HBufC* GetUniversalResourceIdentifierL(TMsvId& aLinkedEntryId, TBool& aFileFound) const = 0;

	/** Starts an asynchronous operation to resolve a specified URI.
	
	@param aMessageId The ID of the body part containing the URI which is to be 
	resolved. CImEmailMessage::FindFirstHTMLPageFileHandleL() can be used to find the aMessageId 
	parameter for the first HTML page in a message.
	@param aBase The base parameter from the HTML part that contains the URI to 
	be resolved. If there is no base parameter in the HTML, then an empty descriptor 
	should be passed in.
	@param aURI The URI to be resolved.
	@param aStatus Asynchronous status word */
	virtual void FindUniversalResourceIdentifierFileHandleL(TMsvId aMessageId, const TDesC& aBase, const TDesC& aURI, TRequestStatus &aStatus) = 0;
	
	/** Gets the results after a search.
	
	@param aLinkedEntryId On return, the message ID of the linked body part, providing 
	one is found. Note that you should keep a record of the returned message ID 
	as it is required for resolving any URI's that may be found in the linked 
	body part.
	
	@param aFile On return, the file handle corresponding to the URI if successful.
	If not successful, then this is not valid.
	
	@return KErrNone if successful and therefore aFile contains the filehandle 
	that corresponds to the URI. An error code if unsuccessful and therefore 
	aFile is not valid. */
	virtual TInt GetUniversalResourceIdentifierFileHandle(TMsvId& aLinkedEntryId, RFile& aFile) const = 0;
	};




class CImMhtmlUriResolver;
class CImRemoveMessagePart;
class CImStoreMessagePart;
class CImMimeHeader;

class CImEmailMessage : public CMsgActive , public MImURITranslator
/**
This class can be used to manipulate new emails that
have been created using CImEmailOperation when composing a message.

Use an instance of this class to add attachments, MHTML parts, body
text, and existing messages to a new message before it is to be sent.

This class can also be used to retrieve the body text, list of attachments,
and embedded messages from emails that have been downloaded and stored within
the message store.

If the store_8bit_body_text flag in imcm.rss is enabled, an instance of this class
can be used to retrieve the original body text character set for a particular message.
It can also be used to override the original body text character set so it will
be decoded in a new character set next time it is viewed. 

@see CImEmailOperation

@publishedAll
@released
*/
	{
public:
	/** Flags that specify to process on any messages embedded in this message. */
	enum TImEmailEntryType
		{
	/** Do not process embedded messages. */
		EThisMessageOnly,
	/** Process embedded messages. */
		EThisMessageAndEmbeddedMessages  // i.e messages with in a message
		};
	/** Flags to specify types of attachment. */
	enum TImAttachmentType
		{
	/** All attachments. */
		EAllAttachments=0,	// all attachment files contained within this message (but not inside messages within this message)
	/** VCards. */
		EVCards,
	/** VCalendars. */
		EVCalendars,
	/** VCards, VCalendars and ICalendars. */
		EVEntries,			// ie VCards, VCalendars or ICalendars (or all three)
	/** Encrypted. */
		EEncrypted,
	/** Signed. */
		ESigned,
	/** Secure. */
		ESecure,			// ie Encrypted or signed (or both)
	/** ICalendars. */
		EICalendars
		//.. add new items here
		};
public:
	IMPORT_C static CImEmailMessage* NewL(CMsvEntry& aEntry);
	IMPORT_C static CImEmailMessage* NewLC(CMsvEntry& aEntry);
	IMPORT_C ~CImEmailMessage();

	IMPORT_C void GetAttachmentsListL(TRequestStatus& aStatus, TMsvId aMessageId, TImAttachmentType aAttachmentType,TImEmailEntryType aEntryType);
	IMPORT_C void GetAttachmentsListL(TMsvId aMessageId, TImAttachmentType aAttachmentType,TImEmailEntryType aEntryType);
	void DoGetAttachmentsListL(TMsvId aMessageId, TImAttachmentType aAttachmentType,TImEmailEntryType aEntryType);

	IMPORT_C void FindFirstHTMLPageL(TMsvId aMessageId, TRequestStatus& aStatus);

	IMPORT_C const CMsvEntrySelection& Selection() const;	// return selection
	IMPORT_C const TDesC8& ProgressL() const;

	IMPORT_C void GetBodyTextL(TRequestStatus& aStatus, TMsvId aMessageId, TImEmailEntryType aEntryType, CRichText& aRichText,CParaFormatLayer& aParaLayer,CCharFormatLayer& aCharLayer);
	IMPORT_C void GetBodyTextL(TMsvId aMessageId, TImEmailEntryType aEntryType, CRichText& aRichText,CParaFormatLayer& aParaLayer,CCharFormatLayer& aCharLayer);
	void DoGetBodyTextInitL(TMsvId aMessageId, TImEmailEntryType aEntryType, CRichText& aRichText,CParaFormatLayer& aParaLayer, CCharFormatLayer& aCharLayer);

	IMPORT_C void GetBodyTextEntryIdL(TRequestStatus& aStatus, TMsvId aMessageId, TImEmailEntryType aEntryType);
	IMPORT_C void GetBodyTextEntryIdL(TMsvId aMessageId, TImEmailEntryType aEntryType);

	IMPORT_C CImPlainBodyText* OpenPlainBodyTextForWriteL();
	
	IMPORT_C CImPlainBodyText* OpenPlainBodyTextForReadL(TImEmailEntryType aEntryType, TInt aChunkLength);
	
	IMPORT_C virtual void FindUniversalResourceIdentifierL(TMsvId aMessageId, const TDesC& aBase, const TDesC& aURI, TRequestStatus &aStatus);
	IMPORT_C virtual HBufC* GetUniversalResourceIdentifierL(TMsvId& aLinkedEntryId, TBool& aFileFound) const;
	IMPORT_C void FindUniversalResourceIdentifierFileHandleL(TMsvId aMessageId, const TDesC& aBase, const TDesC& aURI, TRequestStatus &aStatus);
	IMPORT_C void FindFirstHTMLPageFileHandleL(TMsvId aMessageId, TRequestStatus& aStatus);
	IMPORT_C virtual TInt GetUniversalResourceIdentifierFileHandle(TMsvId& aLinkedEntryId, RFile& aFile) const;
	IMPORT_C MMsvAttachmentManager& AttachmentManager() const;
	void AddAttachmentL(const TDesC& aFilePath, CMsvAttachment* aAttachmentInfo, TRequestStatus& aStatus);
	void AddAttachmentL(RFile& aFileHandle, CMsvAttachment* aAttachmentInfo, TRequestStatus& aStatus);
	void AddLinkedAttachmentL(const TDesC& aFilePath, CMsvAttachment* aAttachmentInfo,TRequestStatus& aStatus);
	void AddEntryAsAttachmentL(TMsvId aAttachmentEntryId, CMsvAttachment* aAttachmentInfo,TRequestStatus& aStatus);
	const RPointerArray<CMsvAttachment>& AttachmentInfoSelection() const;
	void RemoveAttachmentL(TMsvAttachmentId aAttachmentId,TRequestStatus& aStatus) ;
	void RemoveAttachedMessageL(TMsvAttachmentId aAttachmentId,TRequestStatus& aStatus) ;
	void CreateAttachmentL(const TDesC& aFileName, RFile& aAttachmentFile, CMsvAttachment* aAttachmentInfo, TRequestStatus& aStatus);
	void CheckEntryAndResetStoreMessageL(TMsvId aMessageId);
	void CheckEntryAndResetRemoveMessageL(TMsvId aMessageId);
	void DoSetActive(TRequestStatus& aStatus);
	CImMimeHeader* ConvertToImMimeHeadersL(CMsvMimeHeaders* aMimeHeaders);
	CMsvMimeHeaders* ConvertToMsvMimeHeadersL(CImMimeHeader* aMimeHeader);
	void AppendEntryAttachmentInfoL();
	IMPORT_C void AddRelatedPartL(TMsvId aMessageId, const TDesC& aAttachmentFullName, TRequestStatus& aStatus, TMsvId aRelatedPartId, const TDesC8& aContentId);
	IMPORT_C void StoreBodyTextL(TMsvId aMessageId, CRichText& aRichText, TRequestStatus& aStatus);
	IMPORT_C void StoreBodyTextL(TMsvId aMessageId, CRichText& aRichText, TRequestStatus& aStatus, TBool aUsePlainTextStorage);
	IMPORT_C void GetMessageDigestEntriesL(TRequestStatus& aStatus, TMsvId aMessageId);	
	IMPORT_C void GetCharacterSetL(TMsvId aMessageId, TUint& aCharacterSetId, TBool& aOverride);
	IMPORT_C void SetCharacterSetL(TMsvId aMessageId, TUint aCharacterSetId);
	IMPORT_C void StoreBodyTextWithMimeHeaderL(TMsvId aMessageId, CRichText& aRichText, const CImMimeHeader& aMimeHeader, TRequestStatus& aStatus);

	TMsvId EmailEntryId();

private:
	void DoRunL();
	void DoCancel();
	CImEmailMessage(CMsvEntry& aEntry);
	void ConstructL();
	void DoComplete(TInt&);
	void DoStateL();

	void Start(TRequestStatus& aStatus);
	void StartL();

	void Reset();
	void ChangeMessageContextL();
	void CompareChildrenAndPopulateSelL();
	void AttachmentInfoL();
	void AssembleBodyTextL();
	void CheckAndInitialiseL(TMsvId aMessageId);
	
	void AppendAttachmentL();
	TBool MessageDigestEntry();
	TBool HandleDifferentFolderTypesL();
	void GetTextForAlternateFolderL();
	void GetTextForRelatedFolderL();
	void GetAttachmentsForRelatedFolderL();
	TBool GetBodyTextCharacterSetL();
	TUint GetOverrideCharacterSetL();
	TInt FindFilename(const CImMimeHeader& aMimeInfo, TPtrC8& aFilename);
	void FindFilenameDecodeL(
		const CImMimeHeader& aMimeInfo, TFileName& aFileName, TUint aCharset);	
	void AppendAttachmentNameL();
	void SetBodyTextCharacterSetL(TMsvId aEntryId, TUint8 aNewCharacterSetId);
	void FindFirstBodyTextPartL(TMsvId aEntryId, TBool& found);

	CImMimeHeader* FindAttachmentMimeHeaderL();
	TMsvEntry FindIdEntryL(TMsvId aMessageId);

	void DoGetBodyTextEntryIdL(TMsvId aMessageId, TImEmailEntryType aEntryType);


	enum TImEmailMessageStates
		{
		EIdle=0,
		ETextForThisMsg,
		ETextForMsgDigest,
		ETextEntryIdForThisMsg,
		ETextEntryIdMsgDigest,
		ETextForThisMsgGetCharacterSet,
		EAttachmentsForThisMsg,
		EAttachmentsForMsgDigest,
		EMsgDigestEntries,
		EResolveURI,
		EStoreMessagePart,
		ERemoveMessagePart,
		EFinished
		};
	CMsvEntry& iClientEntry;
	CMsvEntrySelection* iCompleteSel;
	CMsvEntrySelection* iResultSel;

	CRichText* iRichText;
	CParaFormatLayer* iParaLayer;
	CCharFormatLayer* iCharLayer;
	TMsvId iParentMsgId;			// the Id passed into Getxxx() function representing the 'parent' message ID
	TImEmailMessageStates iState;
	TImAttachmentType iAttachmentType;
	TMsvEmailEntry iEntry;
	CImMhtmlUriResolver* iUriResolver;
	TBool iUriFileFound;
	CImStoreMessagePart* iStoreMessagePart;
	CImRemoveMessagePart* iRemoveMessagePart;
	TPckgBuf<TMsvId>	iProgress;
	TImEmailEntryType iEntryType;
	TBool iIsAMHTMLmessage;
	TUint iCharacterSetId;
	RFs iFs;
	CDesCArray* iAttachmentNameList;
	CImAttachmentManager* 			iAttachmentManager;
	CMsvStore* 						iStore;
	
	// This is the entry Id using which the CImEmailMessage was created.
	// store this , so that when the  message id to which attachment is 
	// retrieved then the CIMEmailMessage is reset with this Id.
	TMsvId							iEmailEntryId;
	enum TImAttachmentStates
		{
		ENoAttachment=0,
		EAddAttachment,
		EDeleteAttachment,
		ECreateAttachment
		};	
	TInt		iAttachmentState;
	
	};

class CImEmailOperation : public CMsvOperation
/** Provides simple email messages creation functions, including creation of new 
messages, replies, forwarded messages, and receipts.

Both plain-text and HTML messages, with or without attachments, can be created. 
@publishedAll
@released
*/
	{
public:
	IMPORT_C static CImEmailOperation* CreateNewL(TRequestStatus& aObserverRequestStatus, CMsvSession& aMsvSession, TMsvId aDestination, TMsvPartList aPartList, const TMsvEmailTypeList& aMsvEmailTypeList, TUid aMsgType);
	IMPORT_C static CImEmailOperation* CreateNewL(TRequestStatus& aObserverRequestStatus, CMsvSession& aMsvSession, TMsvId aDestination, TMsvId aSmtpServiceId, TMsvPartList aPartList, const TMsvEmailTypeList& aMsvEmailTypeList, TUid aMsgType, TInt aPriority = EPriorityStandard);
	
	IMPORT_C static CImEmailOperation* CreateNewL(TRequestStatus& aObserverRequestStatus, CMsvSession& aMsvSession, TMsvId aDestination, TMsvId aSmtpServiceId, TMsvPartList aPartList, const TMsvEmailTypeList& aMsvEmailTypeList, TUid aMsgType, TInt aPriority , TBool aUsePlainTextStorage);
	
	IMPORT_C static CImEmailOperation* CreateReplyL(TRequestStatus& aObserverRequestStatus, CMsvSession& aMsvSession, TMsvId aMessageId, TMsvId aDestination, TMsvPartList aPartList, const TDesC& aFormatString, const TMsvEmailTypeList& aMsvEmailTypeList, TUid aMsgType);
	IMPORT_C static CImEmailOperation* CreateReplyL(TRequestStatus& aObserverRequestStatus, CMsvSession& aMsvSession, TMsvId aMessageId, TMsvId aDestination, TMsvPartList aPartList, const TMsvEmailTypeList& aMsvEmailTypeList, TUid aMsgType);
	IMPORT_C static CImEmailOperation* CreateReplyL(TRequestStatus& aObserverRequestStatus, CMsvSession& aMsvSession, TMsvId aMessageId, TMsvId aDestination, TMsvPartList aPartList, const TMsvEmailTypeList& aMsvEmailTypeList, TUid aMsgType, TInt aPriority);
	IMPORT_C static CImEmailOperation* CreateReplyL(TRequestStatus& aObserverRequestStatus, CMsvSession& aMsvSession, TMsvId aMessageId, TMsvId aDestination, TMsvPartList aPartList, const TMsvEmailTypeList& aMsvEmailTypeList, TUid aMsgType, TInt aPriority, TBool aUsePlainTextStorage);
	IMPORT_C static CImEmailOperation* CreateForwardL(TRequestStatus& aObserverRequestStatus, CMsvSession& aMsvSession, TMsvId aMessageId, TMsvId aDestination, TMsvPartList aPartList, const TDesC& aFormatString, const TMsvEmailTypeList& aMsvEmailTypeList, TUid aMsgType);
	IMPORT_C static CImEmailOperation* CreateForwardL(TRequestStatus& aObserverRequestStatus, CMsvSession& aMsvSession, TMsvId aMessageId, TMsvId aDestination, TMsvPartList aPartList, const TMsvEmailTypeList& aMsvEmailTypeList, TUid aMsgType);
	IMPORT_C static CImEmailOperation* CreateForwardL(TRequestStatus& aObserverRequestStatus, CMsvSession& aMsvSession, TMsvId aMessageId, TMsvId aDestination, TMsvId aSmtpServiceId, TMsvPartList aPartList, const TMsvEmailTypeList& aMsvEmailTypeList, TUid aMsgType, TInt aPriority = EPriorityStandard);
	IMPORT_C static CImEmailOperation* CreateForwardL(TRequestStatus& aObserverRequestStatus, CMsvSession& aMsvSession, TMsvId aMessageId, TMsvId aDestination, TMsvId aSmtpServiceId, TMsvPartList aPartList, const TMsvEmailTypeList& aMsvEmailTypeList, TUid aMsgType, TInt aPriority, TBool aUsePlainTextStorage);
	IMPORT_C static CImEmailOperation* CreateForwardAsAttachmentL(TRequestStatus& aObserverRequestStatus, CMsvSession& aMsvSession, TMsvId aMessageId, TMsvId aDestination, TMsvPartList aPartList, const TMsvEmailTypeList& aMsvEmailTypeList, TUid aMsgType);
	IMPORT_C static CImEmailOperation* CreateForwardAsAttachmentL(TRequestStatus& aObserverRequestStatus, CMsvSession& aMsvSession, TMsvId aMessageId, TMsvId aSmtpServiceId, TMsvId aDestination, TMsvPartList aPartList, const TMsvEmailTypeList& aMsvEmailTypeList, TUid aMsgType, TInt aPriority = EPriorityStandard);
	IMPORT_C static CImEmailOperation* CreateReceiptL(TRequestStatus& aObserverRequestStatus, CMsvSession& aMsvSession, TMsvId aMessageId, TMsvId aDestination, TMsvPartList aPartList, const TDesC& aFormatString, const TMsvEmailTypeList& aMsvEmailTypeList, TUid aMsgType);
	IMPORT_C static CImEmailOperation* CreateReceiptL(TRequestStatus& aObserverRequestStatus, CMsvSession& aMsvSession, TMsvId aMessageId, TMsvId aDestination, TMsvPartList aPartList, const TMsvEmailTypeList& aMsvEmailTypeList, TUid aMsgType);
	IMPORT_C static CImEmailOperation* CreateReceiptL(TRequestStatus& aObserverRequestStatus, CMsvSession& aMsvSession, TMsvId aMessageId, TMsvId aDestination, TMsvPartList aPartList, const TMsvEmailTypeList& aMsvEmailTypeList, TUid aMsgType, TInt aPriority);
	IMPORT_C static CImEmailOperation* CreateReceiptL(TRequestStatus& aObserverRequestStatus, CMsvSession& aMsvSession, TMsvId aMessageId, TMsvId aDestination, TMsvPartList aPartList, const TMsvEmailTypeList& aMsvEmailTypeList, TUid aMsgType, TInt aPriority, TBool aUsePlainTextStorage);
	IMPORT_C ~CImEmailOperation();
	IMPORT_C const TDesC8& FinalProgress();
	void DoCancel();
	void RunL();
	const TDesC8& ProgressL();

	static CImEmailOperation* CreateCopyL(TRequestStatus& aObserverRequestStatus, CMsvSession& aMsvSession, TMsvId aMessageId, TMsvId aDestination, TMsvPartList aPartList, const TMsvEmailTypeList& aMsvEmailTypeList, TUid aMsgType);

private:
	enum TImEmailOperation 
		{
		ENew					= 0,
		EReply					= 1,
		EForward				= 2,
		EReceipt				= 3,
		EDeletingBadEmail		= 4,
		ECopy					= 5,
		EAttachOriginal			= 0x80,
		EForwardAsAttachment    = EForward | EAttachOriginal,
		EReplyAsAttachment		= EReply | EAttachOriginal
		};

private:
	CImEmailOperation(TRequestStatus& aObserverRequestStatus, CMsvSession& aMsvSession, TMsvId aDestination, TMsvId aSmtpServiceId, TMsvPartList aPartList, const TMsvEmailTypeList& aMsvEmailTypeList, TUid aMsgType, TInt aPriority, TBool aUsePlainTextStorage);
	void ConstructL(TMsvId aMessageId, const TDesC& aFormatString, TImEmailOperation aOperation);
	void ConstructL(TMsvId aMessageId, TImEmailOperation aOperation);
	void ConstructL(TImEmailOperation aOperation);
	void OpenAndReadResourceFileL();
	void SetSmtpServiceFromDefaultsL();
	void SetSmtpServiceFromOriginalMessageL();
	void ErrorRecovery(TInt error);
	void SelectNextStateL();
	void ProcessStateL();
	void SelectAndProcessNextStateL();
	void RequestComplete(TInt aError);
	void CreateNewHeaderL();
	void GetBodyTextL();
	TInt RemoveIncompleteAttachments();
	void CreateEntryDetails(TMsvEmailEntry& aMsvEmailEntry);
	void CreateNewMessageL(TMsvId aDestinationId);
	void CreateMultipartMixedFolderEntryL();
	void CreateTextEntryL(TMsvId aFolderId);
	void StoreBodyL();
	void StorePlainBodyL();
	void CheckForSignatureOrVCardL();
	void CreateAttachmentEntryL();
	void StoreAttachmentL();
	void CreateMultipartAlternativeFolderEntryL();
	void AppendHtmlAttachmentL();
	void AttachOriginalMessageToNewMessageL();
	void CompleteEmailOperationL();
	void CreateHTMLEntryL();
	void PrepareToStoreHTMLEntryTextL();
	void StoreHTMLEntryTextL();
	void CreateVCardAttachmentL();
	void AddVCardAttachmentL();
	void AddMessageAsAttachmentL();
	TPtrC GetDefaultAttachmentName();
	void RestoreSmtpSettingsL();
	TMsvId ServiceId();
	TBool NeedMultipartMixedFolder() const;
	void ReadDefaultHtmlAttachmentNameL(TDes& aFileName);
	void CreateDefaultAttachmentL();
	void AddMessageAttachmentInfoL(TMsvId aAttachmentMessageId);
	void ResetStoreL();
	void CreateAddressListStringL(HBufC*& aListBuffer, const CDesCArray& aAddressArray);
	void RestoreOriginalHeaderL();
	void SetBodyHeaderFormatL();
	static void ClosePlainBodyTextArray(TAny* aPtr);
	void StorePlainTextL();
	TInt GetPlainBodyTextSizeL();
	
private:
	enum TImEmailOperationState
		{
		ECreateNewHeader,
		EGetBodyText,
		EGetAttachmentList,
		EGetMessageDigest,
		ECreateNewMessageEntry,
		ECheckMultipartMixedFolderRequired,
		ECreateMultipartMixedFolderEntry,
		ECheckMultipartAlternativeFolderRequired,
		ECreateMultipartAlternativeFolderEntry,
		ECheckTextEntryRequired,
		ECreateTextEntry,
		EStoreBody,
		ECheckVCardRequired,
		ECreateVCardAttachment,
		EAddVCardAttachment,
		ECheckHTMLEntryRequired,
		ECreateHTMLEntry,
		EPrepareToStoreHTMLEntryText,
		EStoreHTMLEntryText,
		ECheckAttachOriginalMessageRequired,
		EAttachOriginalMessage,
		ECheckHTMLPageRequired,
		EFindHTMLPage,
		ECopyHTMLPartToAttachment,
		ECheckAttachmentsRequired,
		ECreateAttachmentEntry,
		EStoreAttachment,
		ECheckEmbeddedMessagesRequired,
		EAddEmbeddedMessagesAsAttachments,
		ECompleteEmailOperation,
		EFinished
		,ECreateDefaultHtmlAttachment,
		EStoreHTMLTextInEntry,
		EAddMessageAttachmentInfo
		};

	// Flags specifying format of body header information to include
	// in reply and forwarded email addresses
	enum TImBodyHeaderFormat
		{
		ENoToCcInfo=0,
		EToOnly,
		ECcOnly,
		EToAndCc
		};
	
	CMsvEntry* iMsvEntry;
	CMsvOperation* iMsvOperation;
	
	CImHeader* iNewHeader;
	CParaFormatLayer* iParaLayer;
	CCharFormatLayer* iCharLayer;
	CRichText* iRichText;
	CImEmailMessage* iEmailMessage;
	HBufC* iFormatString;
	CFileMan* iFileMan;
	CImSmtpSettings* iSmtpSettings;
	TInt iState;
	TImEmailOperation iOperation;
	TMsvId iDestinationId;
	TMsvId iMultipartMixedId;
	TMsvId iMultipartAlternativeId;
	TMsvId iOrigMessageId;
	TMsvId iNewMessageId;
	TMsvId iSmtpServiceId;
	TMsvId iAttachedMessageId;
	TUid iMsgType;
	TMsvPartList iPartList;
	TMsvEmailTypeList iMsvEmailTypeList;
	TInt iAttachmentCount;
	TInt iTotalAttachments;
	TPckgBuf<TMsvId> iDataMember;
	HBufC* iBodyHeaderFormatString;
	HBufC* iBodyHeaderDateTimeFormatString;
	HBufC* iBodyHeaderToString;
	HBufC* iBodyHeaderCcString;
	HBufC* iBodyHeader;
	HBufC* iUserEmailAddress;
	TInt iRichTextSize;
	TInt iVCardAndHtmlSize;
	TBool iOrigMessageHtml;
	TBool iHtmlNoTextAlt;
	TInt iEmbeddedMessagesToProcess;
	TImMessageField iMessageField;

	// for MIME messages:
	TBool iCreateHtmlMessage;
	TBool iMultipartMixedFolderCreated;
	TBool iMultipartAlternativeFolderCreated;
	TMsvId iTextId;
	TMsvId iHtmlId;
	TMsvId iVcardId;

	// Used when converting rich text to html
	CImHtmlConverter* iHtmlConverter;
	TBool iFinishedConvertingHTML;

	// Relating to adding Signature and/or Vcard to an email
	TBool iNeedToAddVCardAttachment;
	HBufC* iDefaultVCardNameFormatString;
	TStreamId iVcardStoreId;
	CStreamStore* iVcardStore;
	CRichText* iSignatureText;
	TFileName iFileName;
	TImAttachmentFile iAttachmentFile;

	// PCMail
	TMsvId iPCMailServiceId;
	RPointerArray<CMsvAttachment>	iAttachmentInfoList;
	// set this anytime during the object life cycle attachment manager was used
	TBool iAttachmentMgrUsed;
	CMsvStore* iStore;
	RFile iFile;

	TImBodyHeaderFormat iBodyHeaderFormat;
	CImHeader* iOriginalHeader;
	/** Indicates if the body text entry is plain text */
	TBool  iUsePlainTextStorage;
	// The error value returned when CRichText is populated with message body text.
	TInt iRestoreErr;
	};

#endif // __MIUTMSG_H__
