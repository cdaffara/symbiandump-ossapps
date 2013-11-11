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
// MIUTMSG.CPP
//

#include <e32def.h>
#include "MIUTMSG.H"
#include "MIUT_ERR.H"
#include "IMCMMAIN.H"
#include "SMTPSET.H"	// CImSmtpSettings
#include "MIUTRSLV.H"
#include "MIUTCONV.H"
#include "POP3SET.H"
#include "IMAPSET.H"
#include <msventry.h>
#include <msvuids.h>
#include <bautils.h>	// BaflUtils
#include <barsread.h>	// TResourceReader
#include <imcm.rsg>
#include <cntdb.h>
#include <vcard.h>
#include <s32mem.h>
#include <f32file.h>
#include <apgcli.h> // for recognizeData
#include <eikenv.h>
#include <txtrich.h>
#include <cmsvbodytext.h>
#include <imcvcodc.h>
#include <tz.h> //Converting from UTC to local time
#include <tzconverter.h>
#include <cmsvplainbodytext.h>
#include <cmsvattachment.h>
#include <cmsvmimeheaders.h>
#include "cimattachmentmanager.h"
#include <mmsvattachmentmanagersync.h>
#include <cemailaccounts.h>
#include "CIMPLAINBODYTEXT.H"
#include <numberconversion.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS  
#include "cimmessagepart.h"
#include "miut_errconsts.h"
#include "cimconvertcharconv.h"
#include "cimconvertheader.h"
#include <mtmuidsdef.hrh>
#include "msvconsts.h"
#endif

const TInt KMaxChunkLength = 1000;
const TInt KMiutAttachmentListGranularity=8;
const TInt KVCardFilenameLength=25;
const TUid KUidMsgTypePCMail = {0x1000412A};
const TUid KMessageCharSetStreamUid = {0x101FD0E4};
const TUint8 KMessageCharSetStreamVersion = 1;
const TInt KRFC2231Encoded = 2;
_LIT(KEllipsesString,",...");
_LIT(KSeparatorString,", ");
_LIT(KMhtmlUrlAmpersand, "&amp;");


//
//CImEmailMessage
//

/**
Allocates and creates a new CImEmailMessage object.

@param aEntry
A CMsvEntry object that relates to a message server entry.
This object is used by CImEmailMessage to perform operations on particular
message server entries.  It does not matter what context aEntry happens to
be focused on.

@return
A CImEmailMessage message object.

@see CMsvEntry
*/
EXPORT_C CImEmailMessage* CImEmailMessage::NewL(CMsvEntry& aEntry)
	{
	CImEmailMessage* self = CImEmailMessage::NewLC(aEntry);
	CleanupStack::Pop();
	return self;
	}


/**
Allocates and creates a new CImEmailMessage object, leaving the object on the 
cleanup stack.

@param aEntry
A CMsvEntry object that relates to a message server entry.
This object is used by CImEmailMessage to perform operations on particular
message server entries.  It does not matter what context aEntry happens to
be focused on.

@return
A CImEmailMessage object.

@see CMsvEntry
*/
EXPORT_C CImEmailMessage* CImEmailMessage::NewLC(CMsvEntry& aEntry)
	{
	CImEmailMessage* self = new (ELeave) CImEmailMessage(aEntry);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}


/** 
Destructor.
*/
EXPORT_C CImEmailMessage::~CImEmailMessage()
	{
	Cancel();
	delete iAttachmentNameList;
	delete iCompleteSel;
	delete iResultSel;
	delete iUriResolver;
	delete iStoreMessagePart;
	delete iRemoveMessagePart;
	iFs.Close();
	delete iAttachmentManager;
	delete iStore;
	}


CImEmailMessage::CImEmailMessage(CMsvEntry& aEntry)
	: CMsgActive(EPriorityStandard), iClientEntry(aEntry)
	{
	}


void CImEmailMessage::ConstructL()
	{
	iCompleteSel=new (ELeave) CMsvEntrySelection();
	iResultSel=new (ELeave) CMsvEntrySelection();
	iUriResolver = CImMhtmlUriResolver::NewL(iClientEntry);
	iProgress = KMsvNullIndexEntryId;
	User::LeaveIfError(iFs.Connect());
	iAttachmentNameList = new (ELeave) CDesCArrayFlat(KMiutAttachmentListGranularity);
	iEmailEntryId = iClientEntry.EntryId();
	iAttachmentManager = CImAttachmentManager::NewL(*this,iClientEntry);
	iAttachmentManager->LoadL();
	iAttachmentState=ENoAttachment; 
	CActiveScheduler::Add(this);	
	}

/** 
Asynchronously retrieves the body text for a specified message. If the email's character
set has been overridden by a prior call to SetCharacterSetL(), the body text is decoded
with the new character set before it is inserted into the rich text object. A list containing
the entry Ids for each body text part within the specified message is created during this call.
The list can be retrieved after this call has completed by calling Selection().

@param aStatus
Asynchronous status object (TRequestStatus) that is signalled when the operation
completes.  aStatus should be checked by the caller to ensure that the operation
was successful.

@param aMessageId
The entry Id of the email message that the body text is to be retrieved from.
The entry must be of type KUidMsvMessageEntry.

@param aEntryType
Specifies whether to search just the email message (EThisMessageOnly), 
or to search within any embedded messages it may contain (EThisMessageAndEmbeddedMessages).

@param aRichText
Upon completion, returns the aRichText object that contains the message body text of all text parts found
for the message entry specified by aMessageId.  If the body text was not found,
then aRichText will be unchanged.

@param aParaLayer
Paragraph format layer for the rich text object specified by aRichText.

@param aCharLayer
Character format layer for the rich text object specified by aRichText.

@leave KErrNotFound
aMessageId entry could not be located.

@panic imcm 10
aMessageId is not of type KUidMsvMessageEntry.
*/
EXPORT_C void CImEmailMessage::GetBodyTextL(TRequestStatus& aStatus, TMsvId aMessageId, TImEmailEntryType aEntryType, CRichText& aRichText,CParaFormatLayer& aParaLayer, CCharFormatLayer& aCharLayer)
	{
	DoGetBodyTextInitL(aMessageId, aEntryType, aRichText, aParaLayer, aCharLayer);
	Start(aStatus);
	}


/** 
Retrieves the body text for a specified message. If the email's character
set has been overridden by a prior call to SetCharacterSetL(), the body text is decoded
with the new character set before it is inserted into the rich text object.
A list containing the entry Ids for each body text part within the specified message
is created during this call. The list can be retrieved by calling Selection().

@param aMessageId
The entry Id of the email message that the body text is to be retrieved from.
The entry must be of type KUidMsvMessageEntry.

@param aEntryType
Specifies whether to search just the email message (EThisMessageOnly), 
or to search within any embedded messages it may contain (EThisMessageAndEmbeddedMessages).

@param aRichText
Returns the aRichText object that contains the message body text of all text parts found
for the message entry specified by aMessageId.  If the body text was not found,
then aRichText will be unchanged.

@param aParaLayer
Paragraph format layer for the rich text object specified by aRichText.

@param aCharLayer
Character format layer for the rich text object specified by aRichText.

@leave KErrNotFound
aMessageId entry could not be located.

@panic imcm 10
aMessageId is not of type KUidMsvMessageEntry.
*/
EXPORT_C void CImEmailMessage::GetBodyTextL(TMsvId aMessageId, TImEmailEntryType aEntryType, CRichText& aRichText,CParaFormatLayer& aParaLayer, CCharFormatLayer& aCharLayer)
	{
	DoGetBodyTextInitL(aMessageId, aEntryType, aRichText, aParaLayer, aCharLayer);
	StartL();
	}

void CImEmailMessage::DoGetBodyTextInitL(TMsvId aMessageId, TImEmailEntryType aEntryType, CRichText& aRichText,CParaFormatLayer& aParaLayer, CCharFormatLayer& aCharLayer)
	{
	CheckAndInitialiseL(aMessageId);

	if (aEntryType==EThisMessageOnly)
		iState=ETextForThisMsg;
	else if (aEntryType==EThisMessageAndEmbeddedMessages)
		iState=ETextForMsgDigest;
	else
		User::LeaveIfError(KErrNotSupported);
	iEntryType=aEntryType;
	iRichText=&aRichText;
	iParaLayer=&aParaLayer;
	iCharLayer=&aCharLayer;
	iCompleteSel->AppendL(iParentMsgId);
	// Get the new character set if it has been specified.
	iCharacterSetId = GetOverrideCharacterSetL();
	}


EXPORT_C void CImEmailMessage::GetBodyTextEntryIdL(TRequestStatus& aStatus, TMsvId aMessageId, TImEmailEntryType aEntryType)
	{
	DoGetBodyTextEntryIdL(aMessageId, aEntryType);
	Start(aStatus);
	}
	
EXPORT_C void CImEmailMessage::GetBodyTextEntryIdL(TMsvId aMessageId, TImEmailEntryType aEntryType)
	{
	DoGetBodyTextEntryIdL(aMessageId, aEntryType);
	StartL();
	}
	
void CImEmailMessage::DoGetBodyTextEntryIdL(TMsvId aMessageId, TImEmailEntryType aEntryType)
	{
	CheckAndInitialiseL(aMessageId);

	if (aEntryType==EThisMessageOnly)
		iState=ETextEntryIdForThisMsg;
	else if (aEntryType==EThisMessageAndEmbeddedMessages)
		iState=ETextEntryIdMsgDigest;
	else
		User::LeaveIfError(KErrNotSupported);
	
	iEntryType=aEntryType;
	iCompleteSel->AppendL(iParentMsgId);
	}


/**
Asynchronously populates a list containing all the attachment entry Ids found that are of a specified
type belonging to the specified message. After this function has completed, call AttachmentSelection()
to get the list of found attachments. 

If the email's character set has been overridden by a prior call
to SetCharacterSetL(), the attachment names are decoded with the new character set.

@param aStatus
Asynchronous status object (TRequestStatus) that is signalled when the operation
completes.  aStatus should be checked by the caller to ensure that the operation
was successful.

@param aMessageId
The entry Id of the email message that the body text is to be retrieved from.
The entry must be of type KUidMsvMessageEntry.

@param aAttachmentType
The type of attachment to find. Supported attachment 
types are EAllAttachments, which would get all attachments for the message, 
EVCards that would get all attachments that are VCards, EVCalendars that would 
get VCalenders, EICalendar that would get ICalendars, and EVEntries which would get 
attachments that are either VCards, VCalendars or ICalendars. EEncrypted, ESigned 
and ESecure are currently not supported.

@param aEntryType
Specifies whether to search just the email message (EThisMessageOnly), 
or to search within any embedded messages it may contain (EThisMessageAndEmbeddedMessages).

@leave KErrNotFound
aMessageId entry could not be located.

@panic imcm 10
aMessageId is not of type KUidMsvMessageEntry.

@leave KErrNotSupported
aEntryType is not EThisMessageOnly or EThisMessageAndEmbeddedMessages. 
*/
EXPORT_C void CImEmailMessage::GetAttachmentsListL(TRequestStatus& aStatus, TMsvId aMessageId, TImAttachmentType aAttachmentType,TImEmailEntryType aEntryType)
	{
	DoGetAttachmentsListL(aMessageId, aAttachmentType, aEntryType);
	Start(aStatus);
	}


/**
Populates a list containing all the attachment entry Ids found that are of a specified
type belonging to the specified message. After this function has completed, call AttachmentSelection()
to get the list of found attachments. 

If the email's character set has been overridden by a prior call
to SetCharacterSetL(), the attachment names are decoded with the new character set.

@param aMessageId
The entry Id of the email message that the attachment list is to be populated.
The entry must be of type KUidMsvMessageEntry.

@param aAttachmentType
The type of attachment to find. Supported attachment 
types are EAllAttachments, which would get all attachments for the message, 
EVCards that would get all attachments that are VCards, EVCalendars that would 
get VCalenders, EICalendar that would get ICalendars, and EVEntries which would get 
attachments that are either VCards, VCalendars or ICalendars. EEncrypted, ESigned 
and ESecure are currently not supported.

@param aEntryType
Specifies whether to search just the email message (EThisMessageOnly), 
or to search within any embedded messages it may contain (EThisMessageAndEmbeddedMessages).

@leave KErrNotFound
aMessageId entry could not be located.

@panic imcm 10
aMessageId is not of type KUidMsvMessageEntry.

@leave KErrNotSupported
aEntryType is not EThisMessageOnly or EThisMessageAndEmbeddedMessages. 
*/
EXPORT_C void CImEmailMessage::GetAttachmentsListL(TMsvId aMessageId, TImAttachmentType aAttachmentType,TImEmailEntryType aEntryType)
	{
	DoGetAttachmentsListL(aMessageId, aAttachmentType, aEntryType);
	StartL();
	}

	
void CImEmailMessage::DoGetAttachmentsListL(TMsvId aMessageId, TImAttachmentType aAttachmentType,TImEmailEntryType aEntryType)
	{
	CheckAndInitialiseL(aMessageId);


	iAttachmentManager->AttachmentInfoSelection().ResetAndDestroy();
	
	iAttachmentType=aAttachmentType;
	if (aEntryType==EThisMessageOnly)
		iState=EAttachmentsForThisMsg;
	else if (aEntryType==EThisMessageAndEmbeddedMessages)
		iState=EAttachmentsForMsgDigest;
	else
		User::LeaveIfError(KErrNotSupported);
	iEntryType=aEntryType;
	iCompleteSel->AppendL(iParentMsgId);
	}


/**
Asynchronously populates a list of embedded message entries contained within the specified message.
Note that any embedded messages within embedded messages are not included in the list. 
Call Selection() to get the results of the search after it has completed. 

@param aStatus
Asynchronous status object (TRequestStatus) that is signalled when the operation
completes.  aStatus should be checked by the caller to ensure that the operation
was successful.

@param aMessageId
The entry Id of the email message that will be searched for embedded messages.
The entry must be of type KUidMsvMessageEntry.

@leave KErrNotFound
aMessageId entry could not be located.

@panic imcm 10
aMessageId is not of type KUidMsvMessageEntry.
*/
EXPORT_C void CImEmailMessage::GetMessageDigestEntriesL(TRequestStatus& aStatus, TMsvId aMessageId)
	{
	CheckAndInitialiseL(aMessageId);
	iState=EMsgDigestEntries;
	iCompleteSel->AppendL(iParentMsgId);
	Queue(aStatus);
	TRequestStatus* status=&iStatus;
	iStatus=KRequestPending;
	User::RequestComplete(status,KErrNone);
	SetActive();
	}


/**
Asynchronously searches for the starting MHTML part embedded in a multipart/related email message.

Used when retrieving specific parts of an MHTML message, such as inline images
that are referenced as MHTML anchors within the HTML part of a message.  
Refer to RFC 2557 - "MIME Encapsulation of Aggregate Documents, such as HTML (MHTML)" for information
about MHTML email messages.

If the starting MHTML part cannot be located, then the first HTML part
that occurs within the multipart/related message is located.

The result of the search is retrieved by calling GetUniversalResourceIdentifierL() after this methods
has completed.

@param aMessageId
The entry Id of the email message that will be searched.

@param aStatus
Asynchronous status object (TRequestStatus) that is signalled when the operation
completes.  aStatus should be checked by the caller to ensure that the operation
was successful.

@leave KErrNotFound
aMessageId entry could not be located.

@panic imcm 10
aMessageId is not of type KUidMsvMessageEntry.
*/
EXPORT_C void CImEmailMessage::FindFirstHTMLPageL(TMsvId aMessageId, TRequestStatus& aStatus)
	{
	TMsvEntry entry = FindIdEntryL(aMessageId);
	__ASSERT_ALWAYS(entry.iType==KUidMsvMessageEntry, gPanic(EEntryIsNotMessage));
	iUriResolver->FindFirstL(aMessageId,ETrue,iStatus);
	Queue(aStatus);
	iState = EResolveURI;
	SetActive();
	}

/**
Asynchronously searches a message entry for an MHTML URI, and resolves it to the message entry that
contains the message part specified by the URI.

Used when retrieving specific parts of an MHTML message, such as inline images
that are referenced as MHTML anchors within the HTML part of a message.  
Refer to RFC 2557 - "MIME Encapsulation of Aggregate Documents, such as HTML (MHTML)" for information
about MHTML email messages.

The result of the search is retrieved by calling GetUniversalResourceIdentifierL() after this method
has completed.

@param aMessageId
The message Id of the body part that is to be searched for the URI.
In the first instance, this value should be the starting MHTML entry Id that was located
by calling FindFirstHTMLPageL().

@param aBase
The base URI to use when searching for aURI if aURI is relative.
If aBase is empty, and aURI is relative, then the Content-location
MIME header contained within the HTML part specified by aMessageId is used as a base.
If the Content-location header does not exist or is not absolute, then
just the relative aURI is searched for. 


@param aURI
The absolute or relative URI to resolve.  If aURI is absolute, then aBase is ignored
and this method completes faster.  If aURI is relative, then aBase is used as a base
to resolve the relative URI.  If aBase is empty and aURI is relative, then the 
Content-location MIME header contained within the HTML part specified by aMessageId is
used as the base.  If the Content-location header does not exist or is not absolute, then
just the relative aURI is searched for.

@param aStatus
Asynchronous status object (TRequestStatus) that is signalled when the operation
completes.  aStatus should be checked by the caller to ensure that the operation
was successful.

@leave KErrNotFound
aMessageId entry could not be located.

@panic imcm 10
aMessageId is not of type KUidMsvMessageEntry.
*/
EXPORT_C void CImEmailMessage::FindUniversalResourceIdentifierL(TMsvId aMessageId, const TDesC& aBase, const TDesC& aURI, TRequestStatus &aStatus)
	{
	TMsvEntry entry = FindIdEntryL(aMessageId);
	__ASSERT_ALWAYS(entry.iType==KUidMsvEmailHtmlEntry, gPanic(EEntryIsNotMessage));
	iUriFileFound = EFalse;
	iUriResolver->ResolveL(aURI, aBase, aMessageId,ETrue,iStatus);
	Queue(aStatus);
	iState = EResolveURI;
	SetActive();
	}

/**
Gets the results from FindUniversalResourceIdentifierL() or FindFirstHTMLPageL() 
method calls when they have completed. 

Used when retrieving specific parts of an MHTML message, such as inline images
that are referenced as MHTML anchors within the HTML part of a message.  
Refer to RFC 2557 - "MIME Encapsulation of Aggregate Documents, such as HTML (MHTML)" for information
about MHTML email messages.

@pre
FindUniversalResourceIdentifierL() or FindFirstHTMLPageL() must have been called
prior and completed successfully.

@param aLinkedEntryId
Returns the message Id of the message entry that contains the resolved body part, providing 
one is found. Note that you should keep a record of the returned message Id 
as it is required for resolving any URI's that may be found in the resolved body part.

@param aFileFound
Returns ETrue if the URI has been resolved and the message entry containing the MHTML part has been
located. Returns EFalse otherwise.

@return
If the URI is resolved, returns the full path specification and file name of the file containing the HTML,
image, or other such content stored in the message store.  If no file can be located, the absolute URI is returned
instead.
*/
EXPORT_C HBufC* CImEmailMessage::GetUniversalResourceIdentifierL(TMsvId& aLinkedEntryId, TBool& aFileFound) const
	{
	aLinkedEntryId = iUriResolver->LinkedEntryId();
	aFileFound = iUriFileFound;
	return iUriResolver->FileNameL();
	}

/**
Asynchronously searches for the starting MHTML part embedded in a multipart/related email message.

Used when retrieving specific parts of an MHTML message, such as inline images
that are referenced as MHTML anchors within the HTML part of a message.  
Refer to RFC 2557 - "MIME Encapsulation of Aggregate Documents, such as HTML (MHTML)" for information
about MHTML email messages.

If the starting MHTML part cannot be located, then the first HTML part
that occurs within the multipart/related message is located.

The result of the search is retrieved by calling GetUniversalResourceIdentifierFileHandle() after this methods
has completed.

@param aMessageId
The entry Id of the email message that will be searched.

@param aStatus
Asynchronous status object (TRequestStatus) that is signalled when the operation
completes.  aStatus should be checked by the caller to ensure that the operation
was successful.

@leave KErrNotFound
aMessageId entry could not be located.

@panic imcm 10
aMessageId is not of type KUidMsvMessageEntry.
*/
EXPORT_C void CImEmailMessage::FindFirstHTMLPageFileHandleL(TMsvId aMessageId, TRequestStatus& aStatus)
	{
	TMsvEntry entry = FindIdEntryL(aMessageId);
	__ASSERT_ALWAYS(entry.iType==KUidMsvMessageEntry, gPanic(EEntryIsNotMessage));
	
	iUriResolver->FindFirstL(aMessageId,EFalse,iStatus);
	Queue(aStatus);
	iState = EResolveURI;
	SetActive();
	}

/**
Asynchronously searches a message entry for an MHTML URI, and resolves it to the message entry that
contains the message part specified by the URI.

Used when retrieving specific parts of an MHTML message, such as inline images
that are referenced as MHTML anchors within the HTML part of a message.  
Refer to RFC 2557 - "MIME Encapsulation of Aggregate Documents, such as HTML (MHTML)" for information
about MHTML email messages.

The result of the search is retrieved by calling GetUniversalResourceIdentifierFileHandle() after this method
has completed.

@param aMessageId
The message Id of the body part that is to be searched for the URI.
In the first instance, this value should be the starting MHTML entry Id that was located
by calling FindFirstHTMLPageFileHandleL().

@param aBase
The base URI to use when searching for aURI if aURI is relative.
If aBase is empty, and aURI is relative, then the Content-location
MIME header contained within the HTML part specified by aMessageId is used as a base.
If the Content-location header does not exist or is not absolute, then
just the relative aURI is searched for. 


@param aURI
The absolute or relative URI to resolve.  If aURI is absolute, then aBase is ignored
and this method completes faster.  If aURI is relative, then aBase is used as a base
to resolve the relative URI.  If aBase is empty and aURI is relative, then the 
Content-location MIME header contained within the HTML part specified by aMessageId is
used as the base.  If the Content-location header does not exist or is not absolute, then
just the relative aURI is searched for.

@param aStatus
Asynchronous status object (TRequestStatus) that is signalled when the operation
completes.  aStatus should be checked by the caller to ensure that the operation
was successful.

@leave KErrNotFound
aMessageId entry could not be located.

@panic imcm 10
aMessageId is not of type KUidMsvMessageEntry.
*/
EXPORT_C void CImEmailMessage::FindUniversalResourceIdentifierFileHandleL(TMsvId aMessageId, const TDesC& aBase, const TDesC& aURI, TRequestStatus &aStatus)
	{
	TMsvEntry entry = FindIdEntryL(aMessageId);
	__ASSERT_ALWAYS(entry.iType==KUidMsvEmailHtmlEntry, gPanic(EEntryIsNotMessage));
	iUriFileFound = EFalse;
	//Browsers always append "amp;", when "&" is found while writing URLs in HTML
	//replacing "&amp;" with "&", before resolving the URLs
	// eg : if the content location of the image is as below
	// https://www.ttdm.symbian.com/tmtrack.dll?AttachmentPage&AttachmentID=552810/Morning.jpg
	// then the browsers writes this to hmtl file by appending "amp;" after "&" because browsers
	// treat "&" as special character. After apending, the content-location now looks as below
	// https://www.ttdm.symbian.com/tmtrack.dll?AttachmentPage&amp;AttachmentID=552810/Morning.jpg
	const TDesC& ampersand = KMhtmlUrlAmpersand;
	TInt embedUrlIndex;
	if((embedUrlIndex = aBase.Find(ampersand))!=KErrNotFound)
		{
		HBufC* embedUrl = aBase.AllocLC();
		TPtr embedUrlPtr = embedUrl->Des();
		embedUrlPtr.Delete(embedUrlIndex+1, 4);
		iUriResolver->ResolveL(embedUrlPtr, embedUrlPtr, aMessageId,EFalse, iStatus);
		CleanupStack::PopAndDestroy(); //embedUrl
		}
	else 
		{
		iUriResolver->ResolveL(aURI, aBase, aMessageId,EFalse, iStatus);
		} 

	Queue(aStatus);
	iState = EResolveURI;
	SetActive();
	}
	
/**
Gets the results from FindUniversalResourceIdentifierFileHandleL() or FindFirstHTMLPageFileHandleL() 
method calls when they have completed. 

Used when retrieving specific parts of an MHTML message, such as inline images
that are referenced as MHTML anchors within the HTML part of a message.  
Refer to RFC 2557 - "MIME Encapsulation of Aggregate Documents, such as HTML (MHTML)" for information
about MHTML email messages.

@pre
FindUniversalResourceIdentifierFileHandleL() or FindFirstHTMLPageFileHandleL() must have been called
prior and completed successfully.

@param aLinkedEntryId
Returns the message Id of the message entry that contains the resolved body part, providing 
one is found. Note that you should keep a record of the returned message Id 
as it is required for resolving any URI's that may be found in the resolved body part.

@param aFile
Returns the file handle of the file containing the HTML,image, or other such 
content stored in the message store if the URI has been resolved. If not then
this argument is not valid. Ownership is transferred. The caller must close the file handle.

@return
If the URI is resolved, returns KErrNone. The output argument aFile is the file
handle to the content. If no file is located the KErrNotFound is returned and 
aFile is not valid.
*/
EXPORT_C TInt CImEmailMessage::GetUniversalResourceIdentifierFileHandle(TMsvId& aLinkedEntryId, RFile& aFile) const
	{
	aLinkedEntryId = iUriResolver->LinkedEntryId();
	return iUriResolver->FileHandle(aFile);
	}

/**
Returns a selection of entries. The entries are either the results of GetMessageDigestEntriesL(), 
or of GetBodyTextL().

@pre
GetMessageDigestEntriesL() or GetBodyTextL() must be called prior and successfully completed.

@return
Selection of entries populated by GetMessageDigestEntriesL() or GetBodyTextL().
*/
EXPORT_C const CMsvEntrySelection& CImEmailMessage::Selection() const
	{
	return *iResultSel;
	}
	

/**
Returns a message entry Id as a type-safe package buffer (TPckg<TMsvId>).
The meaning of the Id depends on the request as follows:

AddAttachmentL(), AddMessageAsAttachmentL(), and AddRelatedPartL():
While the attachment, message, or related part is being added, a null Id 
(KMsvNullIndexEntryId); Once the attachment, message, or related part has been added, the entry Id
of the newly created attachment, message, or related part.

DeleteAttachmentL() and DeleteAttachedMessageL(): While the attachment or attached 
message is being removed, a null Id (KMsvNullIndexEntryId); Once the attachment
or attached message has been removed, the Id of the message from which the
attachment or attached message was removed.

StoreBodyTextL(): While the body text is being stored, a null Id (KMsvNullIndexEntryId);
Once the text has been stored, the Id of the message for which the body text was 
stored.

@pre
Call this method after the following asynchronous calls have completed to check progress.
AddAttachmentL()
AddMessageAsAttachmentL()
AddRelatedPartL()
DeleteAttachmentL()
DeleteAttachedMessageL()
StoreBodyTextL()

@code
// Example code demonstrating how to retrieve the progress.

// Create and initialise a temporary TPckg object that can hold a message Id.
TMsvId msgId;
TPckg<TMsvId> param(msgId);

// Copy the message Id returned from the ProgressL() call into the
// temporary TPckg object.
param.Copy(emailMsg->ProgressL());  // where emailMsg is of type CImEmailMessage.

// Check the value of the returned message Id in the TPckg object.
if (msgId != KMsvNullIndexEntryId)
	{
	// More code...
	} 
@endcode

@return
The entry Id as a type-safe packaged buffer (TPckg<TMsvId>).
*/
EXPORT_C const TDesC8& CImEmailMessage::ProgressL() const
	{
	return iProgress;
	}
	

/**
Adds a file to a specified message as a MIME multipart/related part. After the
call has completed, calling ProgressL() will return the Id of the newly created
related part entry.

Used when adding specific parts of an MHTML message, such as inline images
that are referenced as MHTML anchors within the HTML part of a message.  
Refer to RFC 2557 - "MIME Encapsulation of Aggregate Documents, such as HTML (MHTML)" for information
about MHTML email messages.

@pre
This function should only be used on messages created using CImEmailOperation.

@param aMessageId
The entry Id of the email message that the related part will be added to.
The entry must be of type KUidMsvMessageEntry.

@param aAttachmentFullName
The full path and file name of the related part file to be copied to the message store.
If the file cannot be located, this call completes with KErrNotFound.

@param aStatus
Asynchronous status object (TRequestStatus) that is signalled when the operation
completes. aStatus should be checked by the caller to ensure that the operation
was successful.

@param aRelatedPartId
The entry Id of the MHTML message part that the file being added is referenced from.

@param aContentId
The Content-Id of the related part.

@leave KErrNotFound
aMessageId entry could not be located.

@panic imcm 10
aMessageId is not of type KUidMsvMessageEntry.
*/
EXPORT_C void CImEmailMessage::AddRelatedPartL(TMsvId aMessageId, const TDesC& aAttachmentFullName, TRequestStatus& aStatus, TMsvId aRelatedPartId, const TDesC8& aContentId)
	{
	TMsvEntry entry = FindIdEntryL(aMessageId);
	__ASSERT_ALWAYS(entry.iType==KUidMsvMessageEntry, gPanic(EEntryIsNotMessage));

	// adds 'object' with name aAttachmentFullName related to aRelatedPartId in Multipart/Related Folder
	CImStoreMessagePart* temp = CImStoreMessagePart::AddRelatedPartL(iStatus, iClientEntry, aMessageId, aAttachmentFullName, aRelatedPartId, aContentId);
	delete iStoreMessagePart;
	iStoreMessagePart = temp;
	Queue(aStatus);
	iState = EStoreMessagePart;
	SetActive();
	}


/** 
Asynchronously stores a body text part for the specified message. After the call
has completed, calling ProgressL() will return the Id of the message entry for
which the body text was stored.
 
@pre
This function should only be used on messages created using CImEmailOperation.

@param aMessageId
The entry Id of the email message that the body text will be stored in.
The entry must be of type KUidMsvMessageEntry.

@param aRichText
A rich text object (CRichText) that contains the message body text.

@param aStatus
Asynchronous status object (TRequestStatus) that is signalled when the operation
completes. aStatus should be checked by the caller to ensure that the operation
was successful.

@leave KErrNotFound
aMessageId entry could not be located.

@panic imcm 10
aMessageId is not of type KUidMsvMessageEntry.
*/
EXPORT_C void CImEmailMessage::StoreBodyTextL(TMsvId aMessageId, CRichText& aRichText, TRequestStatus& aStatus)
	{
	TMsvEntry entry = FindIdEntryL(aMessageId);
	__ASSERT_ALWAYS(entry.iType==KUidMsvMessageEntry, gPanic(EEntryIsNotMessage));
	delete iStoreMessagePart;
	iStoreMessagePart = NULL;
	iStoreMessagePart = CImStoreMessagePart::StoreBodyTextL(iStatus, iClientEntry, aMessageId, aRichText);
	Queue(aStatus);
	iState = EStoreMessagePart;
	SetActive();
	}

/** 
Asynchronously stores a body text part for the specified message. After the call
has completed, calling ProgressL() will return the Id of the message entry for
which the body text was stored.
 
@pre
This function should only be used on messages created using CImEmailOperation.

@param aMessageId
The entry Id of the email message that the body text will be stored in.
The entry must be of type KUidMsvMessageEntry.

@param aRichText
A rich text object (CRichText) that contains the message body text.

@param aStatus
Asynchronous status object (TRequestStatus) that is signalled when the operation
completes. aStatus should be checked by the caller to ensure that the operation
was successful.

@param aUsePlainTextStorage		
if set to ETrue inidcates that the new message entry needs to be created as plain text
if set to EFalse indicates that message will be created as richtext entry.

@leave KErrNotFound
aMessageId entry could not be located.

@panic imcm 10
aMessageId is not of type KUidMsvMessageEntry.
*/	
EXPORT_C void CImEmailMessage::StoreBodyTextL(TMsvId aMessageId, CRichText& aRichText, TRequestStatus& aStatus, TBool aUsePlainTextStorage)
	{
	TMsvEntry entry = FindIdEntryL(aMessageId);
	__ASSERT_ALWAYS(entry.iType==KUidMsvMessageEntry, gPanic(EEntryIsNotMessage));
	delete iStoreMessagePart;
	iStoreMessagePart = NULL;
	iStoreMessagePart = CImStoreMessagePart::StoreBodyTextL(iStatus, iClientEntry, aMessageId, aRichText, aUsePlainTextStorage);
	Queue(aStatus);
	iState = EStoreMessagePart;
	SetActive();
	}	
	
/** 
Asynchronously stores a body text part for the specified message. A Mime header
is created with the passed in the CImMimeHeader object passed.
After the call has completed, calling ProgressL() will return the Id of 
the message entry for which the body text was stored.
 
@pre
This function should only be used on messages created using CImEmailOperation.

@param aMessageId
The entry Id of the email message that the body text will be stored in.
The entry must be of type KUidMsvMessageEntry.

@param aRichText
A rich text object (CRichText) that contains the message body text.

@param aMimeHeader
A MIME header object (CImMimeHeader) that contains the content-type etc.

@param aStatus
Asynchronous status object (TRequestStatus) that is signalled when the operation
completes. aStatus should be checked by the caller to ensure that the operation
was successful.

@leave KErrNotFound
aMessageId entry could not be located.

@panic imcm 10
aMessageId is not of type KUidMsvMessageEntry.
*/
EXPORT_C void CImEmailMessage::StoreBodyTextWithMimeHeaderL(TMsvId aMessageId, CRichText& aRichText, const CImMimeHeader& aMimeHeader, TRequestStatus& aStatus)
	{
	TMsvEntry entry = FindIdEntryL(aMessageId);
	__ASSERT_ALWAYS(entry.iType==KUidMsvMessageEntry, gPanic(EEntryIsNotMessage));
	delete iStoreMessagePart;
	iStoreMessagePart = NULL;
	iStoreMessagePart = CImStoreMessagePart::StoreBodyTextWithMimeHeaderL(iStatus, iClientEntry, aMessageId, aRichText, aMimeHeader);
	Queue(aStatus);
	iState = EStoreMessagePart;
	SetActive();
	}

/**
Returns the character set Id that the body text will be decoded in when it is 
returned in the call to GetBodyTextL(). It also indicates whether the original
character set has been overridden by calling SetCharacterSetL().

@pre
The store_8bit_body_text flag in imcm.rss should be enabled. This is done at build
time by the manufacturer.

@param aMessageId
The entry Id of the email message that the character set Id is to be retrieved from.
The entry must be of type KUidMsvMessageEntry.

@param aCharacterSetId
Returns the character set Id that the message will be displayed when it is viewed.
If aCharacterSetId is zero and aOverride is EFalse, the store_8bit_body_text flag
in imcm.rss has not been enabled.  

@param aOverride
Returns ETrue if the message's original character set has been overridden (changed)
via the call to SetCharacterSetL(). If aCharacterSetId is zero and aOverride is EFalse,
the store_8bit_body_text flag in imcm.rss has not been enabled.

@leave KErrNotFound
aMessageId entry could not be located.

@panic imcm 10
aMessageId is not of type KUidMsvMessageEntry.
*/
EXPORT_C void CImEmailMessage::GetCharacterSetL(TMsvId aMessageId, TUint& aCharacterSetId, TBool& aOverride)
	{
	CheckAndInitialiseL(aMessageId);
	aCharacterSetId = GetOverrideCharacterSetL();
	if (aCharacterSetId == 0)
		{
		// The user has not changed the character set.  Return the body text character set.
		aOverride = EFalse;
		iState = ETextForThisMsgGetCharacterSet;
		iEntryType = EThisMessageOnly;
		iCompleteSel->AppendL(aMessageId);
		StartL();
		aCharacterSetId = iCharacterSetId;
		}
	else
		{
		aOverride = ETrue;
		}
	}


/**
Stores the new character set that the message will be displayed as next time
it is viewed.

@pre
The store_8bit_body_text flag in imcm.rss should be enabled. This is done at build
time by the manufacturer.

@param aMessageId
The entry Id of the email message that the character set will be returned from.
The entry must be of type KUidMsvMessageEntry.

@param aCharacterSetId
The character set Id to be stored in the message store.  Setting this value to
zero will cause the message to be displayed in its original character set when
it was viewed for the first time after it was downloaded.

@leave KErrNotFound
aMessageId entry can not be located in the message store, or the character set
information can not be located. If the character set information cannot be
located, it could be because it was not stored when downloaded. To enable this
functionality set the store_8bit_body_text flag in imcm.rss.
It is also possible that a call to StoreBodyTextL() has overwritten
the original character set stored when the message was downloaded.

@panic imcm 10
aMessageId is not of type KUidMsvMessageEntry.
*/
EXPORT_C void CImEmailMessage::SetCharacterSetL(TMsvId aMessageId, TUint aCharacterSetId)
	{
	CheckAndInitialiseL(aMessageId);
	CMsvStore* store = iClientEntry.EditStoreL();
	CleanupStack::PushL(store);
	
	// If the character set is zero, remove the new character set stream so that the original
	// character set from the body text will be returned when GetCharacterSetL is called.
	if (aCharacterSetId == 0)
		{
		if (store->IsPresentL(KMessageCharSetStreamUid))
			{
			store->RemoveL(KMessageCharSetStreamUid);
			store->CommitL();
			}
		CleanupStack::PopAndDestroy(store);
		return;
		}
		
	// Write the new character set to the store.
	RMsvWriteStream out;
	out.AssignLC(*store, KMessageCharSetStreamUid);
	out.WriteUint8L(KMessageCharSetStreamVersion);
	out.WriteUint32L(aCharacterSetId);
	out.CommitL();
	store->CommitL();
	CleanupStack::PopAndDestroy(2, store); //out, store.
	}


void CImEmailMessage::CheckAndInitialiseL(TMsvId aMessageId)
	{
	iClientEntry.SetEntryNoCheckL(aMessageId);
	const TMsvEmailEntry& entry=iClientEntry.Entry();
	__ASSERT_ALWAYS(entry.iType==KUidMsvMessageEntry, gPanic(EEntryIsNotMessage));
	iParentMsgId=aMessageId;
	iIsAMHTMLmessage=entry.MHTMLEmail();
	iCharacterSetId = 0;
	Reset();
	}


void CImEmailMessage::Reset()
	{
	iState=EIdle;
	iCompleteSel->Reset();
	iResultSel->Reset();
	iAttachmentManager->AttachmentInfoSelection().ResetAndDestroy();
	iAttachmentNameList->Reset();
	}


void CImEmailMessage::DoCancel()
	{
	Reset();
	iUriResolver->Cancel();
	if (iStoreMessagePart)
		iStoreMessagePart->Cancel();
	if (iRemoveMessagePart)
		iRemoveMessagePart->Cancel();
	CMsgActive::DoCancel();
	}


void CImEmailMessage::DoComplete(TInt& aStatus)
	{//set back to the context the user passed in
	TRAPD(error, iClientEntry.SetEntryL(iParentMsgId));
    if(aStatus == KErrNone)
	{
	    aStatus = error;   	
	}
	}


void CImEmailMessage::DoStateL()
	{
	switch (iState)
		{
	case EResolveURI:
		iUriFileFound = (iStatus == KErrNone) ? ETrue : EFalse;
		iState = EIdle;
		return;
	case EStoreMessagePart:
		iProgress.Copy(iStoreMessagePart->ProgressL());
		if(iAttachmentState == EAddAttachment || iAttachmentState == ECreateAttachment)
			{
			TMsvId attachmentId =  iProgress();
			iAttachmentManager->AppendAttachmentArrayL(attachmentId);
			// Once the file is attached to email, reseting the iAttachmentState
			iAttachmentState = ENoAttachment;
			}
		iState = EIdle;
		return;
	case ERemoveMessagePart:
		iProgress.Copy(iRemoveMessagePart->ProgressL());
		if(iAttachmentState ==  EDeleteAttachment)
			{
			TMsvId deletedAttachmentId = iRemoveMessagePart->RemovedAttachmentId();
			// check whether we currently have a cache of the attachments before
			// trying to delete the one we have removed.
			if(iAttachmentManager->AttachmentCount()!=0)
				{			
				iAttachmentManager->DeleteAttachmentInArrayL(deletedAttachmentId);
				}
			}
		
		iState = EIdle;
		return;
	case EFinished:
		return;
	case ETextForThisMsg:
	case ETextForThisMsgGetCharacterSet:
	case ETextForMsgDigest:
	case ETextEntryIdForThisMsg:
	case ETextEntryIdMsgDigest:
	case EAttachmentsForThisMsg:
	case EAttachmentsForMsgDigest:
	case EMsgDigestEntries:
		ChangeMessageContextL();
		break;
	default:
		break;
		};
	}


void CImEmailMessage::Start(TRequestStatus& aStatus)
	{
	Queue(aStatus);
	TRequestStatus* status=&iStatus;
	iStatus=KRequestPending;
	User::RequestComplete(status,KErrNone);
	SetActive();
	}


void CImEmailMessage::StartL()
	{
	TRAPD(error, do DoStateL(); while(iState != EIdle && iState != EFinished););
	DoComplete(error);
	User::LeaveIfError(error);
	}


void CImEmailMessage::DoRunL()
	{
	DoStateL();
	if (iState != EIdle && iState != EFinished)
		{
		TRequestStatus* status=&iStatus;
		iStatus=KRequestPending;
		User::RequestComplete(status,KErrNone);
		SetActive();
		}
	}


/**
This function takes the first id in iCompleteSel and sets the context to 
that and appends its children to the same list and deletes the id(current) after it has finished with it.
*/
void CImEmailMessage::ChangeMessageContextL()
	{
	iClientEntry.SetEntryL((*iCompleteSel)[0]);
	iEntry=iClientEntry.Entry();
	TUid type=iEntry.iType;
	TBool searchChildren=ETrue;
	
	switch (type.iUid)
		{
		case KUidMsvFolderEntryValue:
			{
			searchChildren=HandleDifferentFolderTypesL();
			break;
			}
		case KUidMsvAttachmentEntryValue:
			{
			if (iState==EAttachmentsForThisMsg || iState==EAttachmentsForMsgDigest)
				AttachmentInfoL();
			break;
			}
		case KUidMsvEmailTextEntryValue:
			{
			switch (iState)
				{
				case ETextForThisMsg:
				case ETextForMsgDigest:
					{
					iResultSel->AppendL((*iCompleteSel)[0]);
					AssembleBodyTextL();
					break;
					}

				case ETextEntryIdForThisMsg:
				case ETextEntryIdMsgDigest:
					{
					iResultSel->AppendL((*iCompleteSel)[0]);
					break;
					}

				case ETextForThisMsgGetCharacterSet:
					{
					if (GetBodyTextCharacterSetL())
						{
						// Found an 8 bit body text part.  The character set is the
						// same for every 8 bit body text part so we can return immediately.
						iCompleteSel->Reset();
						iState=EFinished;
						return;
						}
					break;
					}
				default:
					{
					break;
					}
				}
			break;
			}
		case KUidMsvMessageEntryValue:
			{
			//check if user requested for this entry or digest entry and terminate accordingly.   
			if ((*iCompleteSel)[0]!=iParentMsgId)
				{
				// we have a entry attachment, append this to the list of attachments
				AppendEntryAttachmentInfoL();
				if (iEntryType==EThisMessageOnly || iState==EMsgDigestEntries)
					{
					searchChildren=EFalse;
					if (iState==EMsgDigestEntries)
						iResultSel->AppendL((*iCompleteSel)[0]);
					}
				}
			break;
			}
		default:
			{
			break;
			}
		};
	
	TInt index=iCompleteSel->Find(iEntry.Id());
	iCompleteSel->Delete(index);
	if (searchChildren)
		CompareChildrenAndPopulateSelL();
	if (!iCompleteSel->Count()) 
		{
		iState=EFinished;
		return;
		}
	}


void CImEmailMessage::CompareChildrenAndPopulateSelL()
	{  
	TKeyArrayFix uidKey(0,ECmpTUint32);
	iClientEntry.SetSortTypeL(TMsvSelectionOrdering(KMsvNoGrouping,EMsvSortByNone,ETrue));
	TInt count = iClientEntry.Count();

	TMsvId id = KMsvNullIndexEntryId;
	for (TInt i=0; i<count; ++i)
 		{
		id = iClientEntry[i].Id();
		if (iCompleteSel->Find(id) == KErrNotFound)
			{			
			iCompleteSel->InsertIsqL(id ,uidKey);
			}
 		}
	}


void CImEmailMessage::AssembleBodyTextL()
	{
   	if (!iClientEntry.HasStoreL())
   		return;
   		
	CMsvStore* store=iClientEntry.ReadStoreL();
   	CleanupStack::PushL(store);
	
	if (store->HasBodyTextL())
   		{
		CRichText* bodyText=CRichText::NewL(iParaLayer,iCharLayer);	
		CleanupStack::PushL(bodyText);
		store->RestoreBodyTextL(*bodyText, iCharacterSetId);
		iRichText->AppendTakingSolePictureOwnershipL(*bodyText);
		CleanupStack::PopAndDestroy(bodyText);	
		}
	
   	CleanupStack::PopAndDestroy(store);
	}

void CImEmailMessage::AttachmentInfoL()
	{
	switch (iAttachmentType)
		{
	case EAllAttachments: break;
	case EVCards:
		if (!iEntry.VCard()) return; break;
	case EVCalendars:
		if (!iEntry.VCalendar()) return; break;
	case EICalendars:
		if (!iEntry.ICalendar()) return; break;
	case EVEntries: 
		if (iEntry.VCard() || iEntry.VCalendar() || iEntry.ICalendar()) break; return;
	case EEncrypted:
		if (!iEntry.Encrypted()) return; break;
	case ESigned:
		if (!iEntry.Signed()) return; break;
	case ESecure: 
		if (iEntry.Encrypted() || iEntry.Signed()) break; return;
	default: return;
		};

	AppendAttachmentL();
	}

/**
Walking the tree attachments added as found.
*/
void CImEmailMessage::AppendAttachmentL()
	{
	if(iClientEntry.HasStoreL())
		{
		CMsvStore* store = iClientEntry.ReadStoreL();
		CleanupStack::PushL(store);
		MMsvAttachmentManager& attachmentMgr = store->AttachmentManagerL();
		// Modified to handle multiple attachments belonging to a single attachment entry
		for(TInt i=0;i<attachmentMgr.AttachmentCount();i++)
			{
			CMsvAttachment* attachment = attachmentMgr.GetAttachmentInfoL(i);
			CleanupStack::PushL(attachment);
			attachment->SetSize(iEntry.iSize);
			attachment->SetComplete(iEntry.Complete());
			// For compatibility keep entry [0] as the TMsvId of the owning attachment entry
			if(i == 0)
				{
				attachment->SetId(iClientEntry.EntryId());
				}
			else
				{				
				// Store the index and the owning attachment entry's TMsvId
				attachment->SetIntAttributeL(KUidMsvEmailAttachmentEntryIndex,i);
				attachment->SetIntAttributeL(KUidMsvEmailAttachmentEntryId,iClientEntry.EntryId());
				attachment->SetId(iAttachmentManager->UniqueAttachmentId());
				}
			// Add to the per-message list of CMsvAttachments
			User::LeaveIfError(iAttachmentManager->AttachmentInfoSelection().Append(attachment));
			// Add the attachment id to the results
			iResultSel->AppendL(iClientEntry.EntryId());
			CleanupStack::Pop(attachment);				
			}
		CleanupStack::PopAndDestroy(store);	
		}
	AppendAttachmentNameL();
	}


/** 
Retrieves the encoded mime header containing the encoded data for the attachment
name and redecodes with the overriding character set if one has been given. If
no overriding character set has been specified, a zero length entry is added.
This indicates whether redcoding is required at retrieval time.
*/
void CImEmailMessage::AppendAttachmentNameL()
	{
	TMsvId originalId = iClientEntry.EntryId();
	TUint  overrideCharset = 0;

	iClientEntry.SetEntryL(iParentMsgId);
	overrideCharset = GetOverrideCharacterSetL();

	// Set iClientEntry back to original entry.
	iClientEntry.SetEntryL(originalId);
	
	TFileName filename;
	TPtrC filenamePtr(KNullDesC);
	// If our override charset has been set.
	if (overrideCharset != 0)
		{
		CImMimeHeader* mimeHeader = FindAttachmentMimeHeaderL();	
		if (mimeHeader != NULL)
			{
			CleanupStack::PushL(mimeHeader);
			// Decode mime header
			FindFilenameDecodeL(*mimeHeader, filename, overrideCharset);

			filenamePtr.Set(filename);
			CleanupStack::PopAndDestroy(mimeHeader);
			}
		}
	iAttachmentNameList->AppendL(filenamePtr);
	}

CImMimeHeader* CImEmailMessage::FindAttachmentMimeHeaderL()
	{
	TMsvId     originalId = iClientEntry.EntryId();
	CMsvStore* store      = NULL;

	// If the current entry in the message hierarchy has a store, look for the
	// mime header there.
	if (iClientEntry.HasStoreL())
		{
		store = iClientEntry.ReadStoreL();
		CleanupStack::PushL(store);
		if (!store->IsPresentL(KUidMsgFileMimeHeader))
			{
			CleanupStack::PopAndDestroy(store);
			store = NULL;
			}
		}

	if (store == NULL)
		{
		// Check the parent entry in the message hierarchy for the mime header
		// but only if it is the message.
		iClientEntry.SetEntryL(iClientEntry.Entry().Parent());
		if (iClientEntry.Entry().iType == KUidMsvMessageEntry && 
			iClientEntry.HasStoreL())
			{
			store = iClientEntry.ReadStoreL();
			CleanupStack::PushL(store);
			if(!store->IsPresentL(KUidMsgFileMimeHeader))
				{
				CleanupStack::PopAndDestroy(store);
				store = NULL;
				}
			}
		}

	CImMimeHeader* mimeHeader = NULL;
	if (store)
		{
		mimeHeader = CImMimeHeader::NewLC();
		mimeHeader->RestoreL(*store);
		// found the mime headers,
		CMsvMimeHeaders* mimeHeaders = ConvertToMsvMimeHeadersL(mimeHeader);
		CleanupStack::PushL(mimeHeaders);
		// store the mimeheaders into the previously stored attachment  
		TInt count = iAttachmentManager->AttachmentInfoSelection().Count();
		if(count)
			mimeHeaders->StoreL(*(iAttachmentManager->AttachmentInfoSelection()[count-1]));
		CleanupStack::PopAndDestroy(mimeHeaders);
		CleanupStack::Pop(mimeHeader);
		CleanupStack::PopAndDestroy(store);
		CleanupStack::PushL(mimeHeader);
		}
	if (iClientEntry.EntryId() != originalId)
		iClientEntry.SetEntryL(originalId);

	if (mimeHeader)
		CleanupStack::Pop(mimeHeader);
	return mimeHeader;
	}



	
TInt CImEmailMessage::FindFilename(const CImMimeHeader& aMimeInfo, TPtrC8& aFilename)
	{
	_LIT8(KMimeName,"NAME");
	_LIT8(KMimeNameRFC2231, "NAME*");
	_LIT8(KMimeFilename, "FILENAME");
	_LIT8(KMimeFilenameRFC2231, "FILENAME*");

	// Look in content-type list
	const CDesC8Array& ctype = aMimeInfo.ContentTypeParams();
	TInt tuple = 0;

	while (tuple < ctype.Count())
		{
		// Look for "name xxx"
		if (ctype[tuple].CompareF(KMimeName) == 0)
			{
			// Got it: report that we found it
			aFilename.Set(ctype[tuple + 1]);
			return (KErrNone);
			}
		else if (ctype[tuple].CompareF(KMimeNameRFC2231) == 0)
			{
			// Got it: report that we found it
			aFilename.Set(ctype[tuple + 1]);
			return(KRFC2231Encoded);
			}
		tuple += 2;
		}

	// Not found in the content type, try content disposition
	tuple = 0;
	const CDesC8Array& cdisp = aMimeInfo.ContentDispositionParams();
	while (tuple < cdisp.Count())
		{
		// Look for "filename xxx"
		if (cdisp[tuple].CompareF(KMimeFilename) == 0)
			{
			// Got it: report that we found it
			aFilename.Set(cdisp[tuple + 1]);
			return(KErrNone);
			}
		else if (cdisp[tuple].CompareF(KMimeFilenameRFC2231) == 0)
			{
			// Got it: report that we found it
			aFilename.Set(cdisp[tuple + 1]);
			return (KRFC2231Encoded);
			}

		tuple += 2;
		}

	// Didn't find it
	return(KErrNotFound);
	}

void CImEmailMessage::FindFilenameDecodeL(
	const CImMimeHeader& aMimeInfo, TFileName& aFileName, TUint aCharset)
	{
	// Create everything needed for a CImConvertHeader.
	RFs& fileSvrSession = iClientEntry.Session().FileSession();
	CCnvCharacterSetConverter* characterConverter = CCnvCharacterSetConverter::NewL();
	CleanupStack::PushL(characterConverter);
	CImConvertCharconv* charConv = CImConvertCharconv::NewL(*characterConverter, fileSvrSession);
	CleanupStack::PushL(charConv);
	CImConvertHeader* headerConverter = CImConvertHeader::NewL(*charConv); 
	CleanupStack::PushL(headerConverter);

	// Make an attachment name
	aFileName.Zero();

	TPtrC8 origFileName;

	// Look for filename in Content-Type list
	TInt returnValue = FindFilename(aMimeInfo, origFileName);
	
	if (KErrNotFound != returnValue && KRFC2231Encoded != returnValue)
		{
		// Run it through the QP decoder
		HBufC* decoded = HBufC::NewLC(origFileName.Length());
		TPtr decoded_ptr(decoded->Des());
		// Set the overriding charset.
		headerConverter->SetOverrideCharset(aCharset);

		/* if it starts =? then assume its RFC2047 encoding -
		 * otherwise assume its plain text encoded in the system
		 * charset and decode directly */
		if (origFileName.Length() >= 2 && origFileName[0] == KImcvEquals && origFileName[1] == KImcvQuestionMark)
			headerConverter->DecodeHeaderFieldL(origFileName, decoded_ptr);
		else
			{
			if (charConv->PrepareToConvertToFromOurCharsetL(charConv->SystemDefaultCharset()))
				{
				// Character set conversion
				TInt unconvertedChars;
				TInt pos;
				charConv->ConvertToOurCharsetL(origFileName, decoded_ptr, unconvertedChars, pos);
				}
			else
				{
				// Charset not available, don't decode.
				decoded_ptr.Copy(origFileName);
				}
			}
		aFileName.Copy(decoded_ptr);
		CleanupStack::PopAndDestroy(); // decoded
		}
	CleanupStack::PopAndDestroy(3, characterConverter);
	}

TBool CImEmailMessage::HandleDifferentFolderTypesL()
	{
	/* This bit of code looks at the folder type and finds out how many children there are
	 and depending on the type of the reqest (attachments/bodytext etc..) it selects only 
	those type of children and ignores the others.  This avoids going through the whole tree. */
	TImEmailFolderType folderType=iEntry.MessageFolderType();
	
	if(iIsAMHTMLmessage == EFalse && folderType==EFolderTypeRelated)
		folderType=EFolderTypeMixed;

	switch (folderType)
		{
	case EFolderTypeUnknown:
	case EFolderTypeMixed:
	case EFolderTypeParallel:
	case EFolderTypeDigest:
	case EFolderTypeRFC822:
	case EFolderTypePartial:
	case EFolderTypeDirectory:
	case EFolderTypeExternal:
		return ETrue;
	case EFolderTypeRelated:  //doesn't apply for get- txt,attch,msgdigest
		if (iState == EAttachmentsForThisMsg)
			{
			GetAttachmentsForRelatedFolderL();
			return EFalse;	
			}
		else if (iState==EAttachmentsForMsgDigest || iState==EMsgDigestEntries)
			{
			return EFalse;
			}

		if (iState==ETextForThisMsg || iState==ETextForThisMsgGetCharacterSet || iState==ETextForMsgDigest
			|| iState == ETextEntryIdForThisMsg || iState == ETextEntryIdMsgDigest)
			{
			GetTextForRelatedFolderL();
			return EFalse;
			}
		return ETrue;
	case EFolderTypeAlternative: // only applies for get text, get body text character set.
		if (iState==EAttachmentsForThisMsg || iState==EAttachmentsForMsgDigest)
			{
			return ETrue;
			}

		if (iState==ETextForThisMsg || iState==ETextForThisMsgGetCharacterSet || iState==ETextForMsgDigest
			|| iState == ETextEntryIdForThisMsg || iState == ETextEntryIdMsgDigest)
			{
			GetTextForAlternateFolderL();
			}
		return EFalse;
		}
	return ETrue;
	}


void CImEmailMessage::GetTextForAlternateFolderL()		
	{
	iClientEntry.SetSortTypeL(TMsvSelectionOrdering(KMsvNoGrouping,EMsvSortByNone,ETrue));
	TInt count = iClientEntry.Count();
	TInt typeindex=-1;  //for alternate we need the last id.
	while (count--)
		{
		if (iClientEntry[count].iType.iUid==KUidMsvEmailTextEntryValue)  //ignore the rest of the types.  The last text entry is the best one.
			{
			typeindex=count;
			break;
			}
		}
	if (typeindex >-1) //means it found a txt entry
		{
		TKeyArrayFix uidKey(0,ECmpTUint32);
		iCompleteSel->InsertIsqL(iClientEntry[typeindex].Id(),uidKey);
		}
	}


void CImEmailMessage::GetTextForRelatedFolderL()		
	{
	iClientEntry.SetSortTypeL(TMsvSelectionOrdering(KMsvNoGrouping,EMsvSortByNone,ETrue));
	CMsvEntrySelection* sel=iClientEntry.ChildrenL();
	CleanupStack::PushL(sel);		
	TInt count=sel->Count();
	TMsvEmailEntry entry;
	TKeyArrayFix uidKey(0,ECmpTUint32);
	for (TInt i=0; i<count; i++)
		{
		entry=iClientEntry.ChildDataL((*sel)[i]);

		if ((entry.iType.iUid==KUidMsvFolderEntryValue) || ((iState==ETextForMsgDigest || iState == ETextEntryIdMsgDigest)
		&& (entry.iType.iUid==KUidMsvMessageEntryValue))) 
			{
			iCompleteSel->InsertIsqL((*sel)[i],uidKey);
			};
		}
	CleanupStack::PopAndDestroy(); //sel
	}


void CImEmailMessage::GetAttachmentsForRelatedFolderL()
	{
	// This fixes a defect where forwarding an inline attachment failed. Before this
	// fix, no attachments under multipart related were forwarded. 
	// This routine will only forward attachments taht are directly under the multipart
	// related folder.
	TKeyArrayFix uidKey(0,ECmpTUint32);
	iClientEntry.SetSortTypeL(TMsvSelectionOrdering(KMsvNoGrouping,EMsvSortByNone,ETrue));
	for (TInt entry(0); entry < iClientEntry.Count(); entry++)
		{
		if (iClientEntry[entry].iType.iUid == KUidMsvAttachmentEntryValue)
			{
			iCompleteSel->InsertIsqL(iClientEntry[entry].Id(), uidKey);
			}
		}
	}

TBool CImEmailMessage::GetBodyTextCharacterSetL()
	{
	// Open body text entry store for reading.
	CMsvStore* store = iClientEntry.ReadStoreL();
	CleanupStack::PushL(store);

 	// check the body text entry store available if not return EFalse
 	if(!store->HasBodyTextL())
 		{
 		CleanupStack::PopAndDestroy(store); // store.	
		return EFalse;
 		}

	// Test if the old rich text body OR 16 bit plain body text is present. Return immediately if so, 
	// We are looking for the new 8 bit body text format which has character set
	// information stored with it. 
	if (store->IsPresentL(KMsvEntryRichTextBody) || store->IsPresentL(KMsvPlainBodyText16))
		{
		CleanupStack::PopAndDestroy(store);
		return EFalse;
		}
	// Check if body text is stored as 8 bit body text format
	if(store->IsPresentL(KMsv8BitEncodedBodyData))
		{
		// The body text entry must be the new 8 bit format.
		CMsvBodyText* bodyText = CMsvBodyText::NewLC();

		// If there is no body text for this message RestoreL will leave
		// In this case we will change that into returning EFalse so any
		// attachments will display properly
		TRAPD(err, bodyText->RestoreL(*store));
		if (err == KErrNotFound)
			{
			CleanupStack::PopAndDestroy(2, store); // bodyText, store.	
			return EFalse;
			}
	
		iCharacterSetId = bodyText->CharacterSet();
		
		if (!iCharacterSetId)
			{
			iCharacterSetId = bodyText->DefaultCharacterSet();
			}
		CleanupStack::PopAndDestroy(bodyText);
		}
	else // Body text is stored as 8 bit plain text.
		{
		CMsvPlainBodyText* plainBodyText = store->InitialisePlainBodyTextForReadL(KMaxChunkLength);
		CleanupStack::PushL(plainBodyText);
		iCharacterSetId = plainBodyText->CharacterSet();
		if(!iCharacterSetId)
			{
			iCharacterSetId = plainBodyText->DefaultCharacterSet();
			}
		CleanupStack::PopAndDestroy(plainBodyText);	
		}
	
	CleanupStack::PopAndDestroy(store);
	return ETrue;
	}


/**
Returns the character set id of the email message entry if it exists.  This method assumes that the
context is set to the top level message entry.
*/	
TUint CImEmailMessage::GetOverrideCharacterSetL()
	{
	TUint characterSetId = 0;
	if (iClientEntry.HasStoreL())
		{
		CMsvStore* store  = iClientEntry.ReadStoreL();
		CleanupStack::PushL(store);
		if (store->IsPresentL(KMessageCharSetStreamUid))
			{
			RMsvReadStream in;
			in.OpenLC(*store, KMessageCharSetStreamUid);
			in.ReadUint8L(); // Version. 
			characterSetId = in.ReadUint32L();
			in.Close();
			CleanupStack::PopAndDestroy(&in);
			}
		CleanupStack::PopAndDestroy(store);
		}
	return characterSetId;
	}

TMsvEntry CImEmailMessage::FindIdEntryL(TMsvId aMessageId)
	{
	TMsvId service;
	TMsvEntry returnEntry;
	iClientEntry.Session().GetEntry(aMessageId, service, returnEntry);
	return returnEntry;
	}
	
EXPORT_C MMsvAttachmentManager& CImEmailMessage::AttachmentManager() const
	{
	return *iAttachmentManager;
	}
	
void CImEmailMessage::CheckEntryAndResetStoreMessageL(TMsvId aMessageId)
	{
	TMsvEntry entry = FindIdEntryL(aMessageId);
	__ASSERT_DEBUG(entry.iType==KUidMsvMessageEntry, gPanic(EEntryIsNotMessage));
	
	// reset the entry with which CImEmailMessage was created
	iClientEntry.SetEntryL(iEmailEntryId);
	
	delete iStoreMessagePart;
	iStoreMessagePart = NULL;
	}
	
void CImEmailMessage::CheckEntryAndResetRemoveMessageL(TMsvId aMessageId)
	{
	TMsvEntry entry = FindIdEntryL(aMessageId);
	__ASSERT_DEBUG(entry.iType==KUidMsvMessageEntry, gPanic(EEntryIsNotMessage));
	
	// reset the entry with which CImEmailMessage was created
	iClientEntry.SetEntryL(iEmailEntryId);
	
	delete iRemoveMessagePart;
	iRemoveMessagePart = NULL;
	}	
	
void CImEmailMessage::DoSetActive(TRequestStatus& aStatus)
	{
	Queue(aStatus);
	SetActive();	
	}	

void CImEmailMessage::AddAttachmentL(const TDesC& aFilePath, CMsvAttachment* aAttachmentInfo, TRequestStatus& aStatus)
	{
	// get the message id
	TMsvId messageId = iClientEntry.EntryId();
	CheckEntryAndResetStoreMessageL(messageId);

	iStoreMessagePart = CImStoreMessagePart::AddAttachmentL(messageId,aFilePath,iClientEntry, aAttachmentInfo,iStatus );
	iState = EStoreMessagePart;
	iAttachmentState = EAddAttachment;

	DoSetActive(aStatus);
	}

void CImEmailMessage::AddAttachmentL(RFile& aFile, CMsvAttachment* aAttachmentInfo, TRequestStatus& aStatus)
	{
	// get the message id
	TMsvId messageId = iClientEntry.EntryId();
	CheckEntryAndResetStoreMessageL(messageId);
	
	iStoreMessagePart = CImStoreMessagePart::AddAttachmentL(messageId,aFile,iClientEntry, aAttachmentInfo, iStatus);
	iState = EStoreMessagePart;
	iAttachmentState = EAddAttachment;
	
	DoSetActive(aStatus);
	}
	
void CImEmailMessage::AddLinkedAttachmentL(const TDesC& aFilePath, CMsvAttachment* aAttachmentInfo,TRequestStatus& aStatus)
	{
	// get the message id
	TMsvId messageId = iClientEntry.EntryId();
	CheckEntryAndResetStoreMessageL(messageId);
	
	iStoreMessagePart = CImStoreMessagePart::AddLinkedAttachmentL(messageId,aFilePath,iClientEntry, aAttachmentInfo,iStatus);
	iState = EStoreMessagePart;
	iAttachmentState = EAddAttachment;
	
	DoSetActive(aStatus);
	}
	
void CImEmailMessage::AddEntryAsAttachmentL(TMsvId aAttachmentEntryId, CMsvAttachment* aAttachmentInfo,TRequestStatus& aStatus)
	{
	// get the message id
	TMsvId messageId = iClientEntry.EntryId();
	CheckEntryAndResetStoreMessageL(messageId);
		
	iStoreMessagePart = CImStoreMessagePart::AddEntryAsAttachmentL(messageId,aAttachmentEntryId,iClientEntry,aAttachmentInfo,iStatus);
	iState = EStoreMessagePart;
	iAttachmentState = EAddAttachment;
	
	DoSetActive(aStatus);
	}

void CImEmailMessage::CreateAttachmentL(const TDesC& aFileName, RFile& aAttachmentFile, CMsvAttachment* aAttachmentInfo, TRequestStatus& aStatus)
	{
	// get the message id
	TMsvId messageId = iClientEntry.EntryId();
	CheckEntryAndResetStoreMessageL(messageId);
	
	iStoreMessagePart = CImStoreMessagePart::CreateAttachmentL(messageId,aFileName,aAttachmentFile,iClientEntry, aAttachmentInfo,iStatus);
	iState = EStoreMessagePart;
	iAttachmentState = ECreateAttachment;
	
	DoSetActive(aStatus);
	}
	
/**
Returns a list of attachment information pointers (CMsvAttachment)
populated by a call to GetAttachmentsListL().

@pre
The attachment information list must first be populated by calling GetAttachmentsListL().

@return
An array of attachment information objects (CMsvAttachment) representing the attachments
contained within a particular message specified in the call to GetAttachmentsListL().
*/
const RPointerArray<CMsvAttachment>& CImEmailMessage::AttachmentInfoSelection() const
	{
	return iAttachmentManager->AttachmentInfoSelection();
	}

void CImEmailMessage::RemoveAttachmentL(TMsvAttachmentId aAttachmentId,TRequestStatus& aStatus) 
	{
	// get the message id
	TMsvId messageId = iClientEntry.EntryId();
	CheckEntryAndResetRemoveMessageL(messageId);
	
	iRemoveMessagePart = CImRemoveMessagePart::DeleteAttachmentL(iStatus, iClientEntry, messageId, aAttachmentId);
	iState = ERemoveMessagePart;
	iAttachmentState = EDeleteAttachment;
	
	DoSetActive(aStatus);
	}
	
void CImEmailMessage::RemoveAttachedMessageL(TMsvAttachmentId aAttachmentId,TRequestStatus& aStatus) 
	{
	// get the message id
	TMsvId messageId = iClientEntry.EntryId();
	CheckEntryAndResetRemoveMessageL(messageId);
	
	iRemoveMessagePart = CImRemoveMessagePart::DeleteAttachmentL(iStatus, iClientEntry, messageId, aAttachmentId);
	iState = ERemoveMessagePart;
	iAttachmentState = EDeleteAttachment;
	
	DoSetActive(aStatus);
	}
	
CMsvMimeHeaders* CImEmailMessage::ConvertToMsvMimeHeadersL(CImMimeHeader* aMimeHeader)
	{
	CMsvMimeHeaders* mimeHeaders = CMsvMimeHeaders::NewL();
	CleanupStack::PushL(mimeHeaders);
	
	mimeHeaders->SetContentDescriptionL(aMimeHeader->ContentDescription());
	mimeHeaders->SetContentBaseL(aMimeHeader->ContentBase());
	mimeHeaders->SetContentLocationL(aMimeHeader->ContentLocation());
	mimeHeaders->SetContentIdL(aMimeHeader->ContentID());
	mimeHeaders->SetContentTypeL(aMimeHeader->ContentType());
	mimeHeaders->SetContentDispositionL(aMimeHeader->ContentDisposition());
	mimeHeaders->SetMimeCharset(aMimeHeader->MimeCharset());
	mimeHeaders->SetRelativePathL(aMimeHeader->RelativePath());
	mimeHeaders->SetContentSubTypeL(aMimeHeader->ContentSubType());
	mimeHeaders->SetRelativePathL(aMimeHeader->RelativePath());
	
	CleanupStack::Pop(mimeHeaders);	
	return mimeHeaders;
	}
	
CImMimeHeader* CImEmailMessage::ConvertToImMimeHeadersL(CMsvMimeHeaders* aMimeHeaders)
	{
	CImMimeHeader* mimeHeader = CImMimeHeader::NewL();
	CleanupStack::PushL(mimeHeader);
	
	mimeHeader->SetContentDescriptionL(aMimeHeaders->ContentDescription());
	mimeHeader->SetContentBaseL(aMimeHeaders->ContentBase());
	mimeHeader->SetContentLocationL(aMimeHeaders->ContentLocation());
	mimeHeader->SetContentIDL(aMimeHeaders->ContentId());
	mimeHeader->SetContentTypeL(aMimeHeaders->ContentType());
	mimeHeader->SetContentDispositionL(aMimeHeaders->ContentDisposition());
	mimeHeader->SetMimeCharset(aMimeHeaders->MimeCharset());
	mimeHeader->SetRelativePathL(aMimeHeaders->RelativePath());
	mimeHeader->SetContentSubTypeL(aMimeHeaders->ContentSubType());
	mimeHeader->SetRelativePathL(aMimeHeaders->RelativePath());
	CleanupStack::Pop(mimeHeader);	
	return mimeHeader;
	}	
	
void CImEmailMessage::AppendEntryAttachmentInfoL()
	{
	CMsvStore* store = iClientEntry.ReadStoreL();
	CleanupStack::PushL(store);
	MMsvAttachmentManager& attachmentMgr = store->AttachmentManagerL();
	// Modified to handle multiple attachments belonging to a single attachment entry
	for(TInt i=0;i<attachmentMgr.AttachmentCount();i++)
		{
		CMsvAttachment* attachment = attachmentMgr.GetAttachmentInfoL(i);
		CleanupStack::PushL(attachment);
		attachment->SetSize(iEntry.iSize);
		attachment->SetComplete(iEntry.Complete());
		// For compatibility keep entry [0] as the TMsvId of the owning attachment entry
		if(i == 0)
			{
			attachment->SetId(iClientEntry.EntryId());
			}
		else
			{				
			// Store the index and the owning attachment entry's TMsvId
			attachment->SetIntAttributeL(KUidMsvEmailAttachmentEntryIndex,i);
			attachment->SetIntAttributeL(KUidMsvEmailAttachmentEntryId,iClientEntry.EntryId());
			attachment->SetId(iAttachmentManager->UniqueAttachmentId());
			}
		// Add to the per-message list of CMsvAttachments
		User::LeaveIfError(iAttachmentManager->AttachmentInfoSelection().Append(attachment));
		CleanupStack::Pop(attachment);				
		}
	CleanupStack::PopAndDestroy(store);		
	}
	
TMsvId CImEmailMessage::EmailEntryId()
	{
	return iClientEntry.EntryId();
	}

/**
This returns a object of CImPlainBodyText.This needs to be called when bodytext 
needs to be restored in chunks for Read operation.
@param aMessageId	The Message Id for which to get plain body text part.
@param aEntryType   The TImEmailEntryType for this message.
@return CMsvPlainBodyText
*/
EXPORT_C CImPlainBodyText* CImEmailMessage::OpenPlainBodyTextForReadL(TImEmailEntryType aEntryType, TInt aChunkLength)
	{
	CImPlainBodyText* plainBodyText = CImPlainBodyText::NewL(*this, iClientEntry, aEntryType, aChunkLength, EFalse);
	return plainBodyText;
	}

/**
This returns a object of CImPlainBodyText.This needs to be called when bodytext 
needs to be created in chunks.
@param aMessageId	  The id of the message whose bodytext needs to be edited.
@return CImPlainBodyText
*/	
EXPORT_C CImPlainBodyText* CImEmailMessage::OpenPlainBodyTextForWriteL()
	{
	CImPlainBodyText* plainBodyText = CImPlainBodyText::NewL(*this, iClientEntry, CImEmailMessage::EThisMessageOnly, KMaxChunkLength, ETrue);
	return plainBodyText;
	}

//
// CImRemoveMessagePart
//

CImRemoveMessagePart* CImRemoveMessagePart::DeleteAttachmentL(TRequestStatus &aStatus, CMsvEntry& aMsvEntry, TMsvId aMessageId, TMsvId aAttachmentId)
	{
	CImRemoveMessagePart* self = new(ELeave) CImRemoveMessagePart(aStatus, aMsvEntry, aMessageId);
	CleanupStack::PushL(self);
	self->ConstructL(aAttachmentId);
	CleanupStack::Pop(); //self
	return self;
	}

CImRemoveMessagePart* CImRemoveMessagePart::DeleteAttachedMessageL(TRequestStatus &aStatus, CMsvEntry& aMsvEntry, TMsvId aMessageId, TMsvId aAttachedMessageId)
	{
	// this does exactly the same as for DeleteAttachmentL() but this function is added to seperate deleting attachments/attached messages
	CImRemoveMessagePart* self = new(ELeave) CImRemoveMessagePart(aStatus, aMsvEntry, aMessageId);
	CleanupStack::PushL(self);
	self->ConstructL(aAttachedMessageId);
	CleanupStack::Pop();
	return self;
	}

CImRemoveMessagePart::~CImRemoveMessagePart()
	{
	Cancel();
	delete iMsvOperation;
	delete iMessageEntrySelection;
	delete iEmailMessage;
	}

const TDesC8& CImRemoveMessagePart::FinalProgress()
	{
	__ASSERT_ALWAYS(!IsActive(), gPanic(EMiutActiveInFinalProgress));
 	const TDesC8* progress = &KNullDesC8();
 	TRAPD(leave, progress = &ProgressL());
	__ASSERT_ALWAYS(leave == KErrNone, gPanic(EImcmFinalProgressFailed));
	return *progress;	
	}

const TDesC8& CImRemoveMessagePart::ProgressL()
	{
	if (iState==EFinished)
		iDataMember() = iMessageId;
	return iDataMember;
	}

void CImRemoveMessagePart::DoCancel()
	{
	if (iMsvOperation)
		iMsvOperation->Cancel();
	if (iEmailMessage)
		iEmailMessage->Cancel();
	Recover();

	TRequestStatus* st = &iObserverRequestStatus;
	User::RequestComplete(st, KErrCancel);
	}

void CImRemoveMessagePart::SelectAndChangeToNextStateL()
	{
	SelectNextStateL();
	ChangeStateL();
	}

void CImRemoveMessagePart::RunL()
	{
	if (iStatus.Int() != KErrNone)
		{
		ErrorRecovery(iStatus.Int());
		return;
		}
	if ((iState == EMoveOtherEntryToParentOfFolder) || 
		(iState == EDeleteAttachmentEntry) ||
		(iState == EDeleteFolderEntry) ||
		(iState == ECompleteRemoveMessagePart))
		{
		TInt sysProgressError = McliUtils::GetProgressErrorL(*iMsvOperation);
		if (sysProgressError != KErrNone)
			{
			ErrorRecovery(sysProgressError);
			return;
			}
		}
	if (iState != EFinished)
		{
		TRAPD(error, SelectAndChangeToNextStateL());
		if (error)
			ErrorRecovery(error);
		else if (iState != EFinished)
			SetActive();
		}
	}

CImRemoveMessagePart::CImRemoveMessagePart(TRequestStatus& aStatus, CMsvEntry& aMsvEntry, TMsvId aMessageId)
	: CMsvOperation(aMsvEntry.Session(), EPriorityStandard, aStatus),
	  iMsvEntry(aMsvEntry),
	  iMessageId(aMessageId)
	{
	}

void CImRemoveMessagePart::ConstructL(TMsvId aAttachmentId)
	{
	iDataMember = KMsvNullIndexEntryId;
	iAttachmentId = aAttachmentId;
	iFolderToDeleteId = KMsvNullIndexEntryId;
	iEntryToMoveId = KMsvNullIndexEntryId;
	iEmailMessage = CImEmailMessage::NewL(iMsvEntry);
	
	CActiveScheduler::Add(this);

	TMsvId serviceId = KMsvNullIndexEntryId;
	TMsvEmailEntry attachmentEntry;
	iMsvEntry.Session().GetEntry(iAttachmentId, serviceId, attachmentEntry);
	iAttachmentSize = attachmentEntry.iSize;
	iState = ECheckAttachmentParentType;
	ChangeStateL();

	iObserverRequestStatus = KRequestPending;
	SetActive();
	}

void CImRemoveMessagePart::ErrorRecovery(TInt error)
	{
	// if there is an error then leave message in a 'good' state
	Recover();
	
	// complete the observer with error
	TRequestStatus* status=&iObserverRequestStatus;
	User::RequestComplete(status,error);
	}

void CImRemoveMessagePart::SelectNextStateL()
	{
	switch (iState)
		{
	// do move and then the delete because if the move fails, the message
	// will still be in a good state
	case ECheckAttachmentParentType:
		{
		// set entry to parent just in case GetAttachmentsListL changes context
		TMsvId serviceId; // temp
		TMsvEmailEntry attachmentEntry;
		iMsvEntry.Session().GetEntry(iAttachmentId, serviceId, attachmentEntry);
		iMsvEntry.SetEntryL(attachmentEntry.Parent());

		iState = EDeleteAttachmentEntry;
		}
		break;
	case EMoveOtherEntryToParentOfFolder:
		iState = EDeleteFolderEntry;
		break;
	case EDeleteAttachmentEntry:
		if (iFolderToDeleteId)
			iState = EMoveOtherEntryToParentOfFolder;
		else
			iState = ECompleteRemoveMessagePart;
		break;
	case EDeleteFolderEntry:
		iState = ECompleteRemoveMessagePart;
		break;
	case ECompleteRemoveMessagePart:
		iState = EFinished;
		break;
	default:
		User::LeaveIfError(KErrNotSupported);
		}
	}

void CImRemoveMessagePart::ChangeStateL()
	{
	switch (iState)
		{
	case ECheckAttachmentParentType:
		CheckAttachmentParentTypeL();
		break;
	case EMoveOtherEntryToParentOfFolder:
		MoveOtherEntryToParentOfFolderL();
		break;
	case EDeleteAttachmentEntry:
		DeleteAttachmentEntryL();
		break;
	case EDeleteFolderEntry:
		DeleteFolderEntryL();
		break;
	case ECompleteRemoveMessagePart:
		CompleteRemoveMessagePartL();
		break;
	case EFinished:
		{
		TRequestStatus* status=&iObserverRequestStatus;
		User::RequestComplete(status,KErrNone);
		}
		}
	}

void CImRemoveMessagePart::RequestComplete(TInt aError)
	{
	iStatus = KRequestPending;
	TRequestStatus* status=&iStatus;
	iStatus=KRequestPending;
	User::RequestComplete(status,aError);
	}

void CImRemoveMessagePart::Recover()
	{
	switch (iState)
		{
	case EDeleteFolderEntry:
	case EDeleteAttachmentEntry:
		// delete folder if one exists else delete attachment
		if (iFolderToDeleteId)
			iMsvEntry.Session().RemoveEntry(iFolderToDeleteId);
		else
			iMsvEntry.Session().RemoveEntry(iAttachmentId);
		break;
	case ECompleteRemoveMessagePart: // not fatal - details might be incorrect but delete already done
	// for other states - original message not changed:
	case EMoveOtherEntryToParentOfFolder:
	case ECheckAttachmentParentType: 
	case EFinished:	// operation completed anyway
		break;
		}
	}

void CImRemoveMessagePart::CheckAttachmentParentTypeL()
	{
	TMsvId serviceId; // temp
	TMsvEmailEntry attachmentEntry;
	iMsvEntry.Session().GetEntry(iAttachmentId, serviceId, attachmentEntry);
	iMsvEntry.SetEntryL(attachmentEntry.Parent());

	// get children of attachments parent
	iMessageEntrySelection = iMsvEntry.ChildrenL();

	TMsvEmailEntry parentEntry = iMsvEntry.Entry();
	if (parentEntry.iType == KUidMsvFolderEntry)
		{
		// if there are 2 or less children, the other children need to be moved and then the folder deleted
		if (iMessageEntrySelection->Count() <= 2)
			iFolderToDeleteId = iMsvEntry.EntryId();
		}
	// if there are no other attachments then unset attachment flag in message
	iEmailMessage->GetAttachmentsListL(iStatus, iMessageId, CImEmailMessage::EAllAttachments,CImEmailMessage::EThisMessageOnly);
	}

void CImRemoveMessagePart::MoveOtherEntryToParentOfFolderL()
	{
	// context on parent of attachment
	// move entry to folder's parent
	TInt i = iMessageEntrySelection->Count();
	while (i--)
		{
		if ((*iMessageEntrySelection)[i] != iAttachmentId)
			iEntryToMoveId = (*iMessageEntrySelection)[i];
		}
	delete iMsvOperation;
	iMsvOperation = NULL;
	iMsvOperation = iMsvEntry.MoveL(iEntryToMoveId, iMsvEntry.Entry().Parent(), iStatus);
	}

void CImRemoveMessagePart::DeleteAttachmentEntryL()
	{
	// context on parent of attachment
	delete iMsvOperation;
	iMsvOperation = NULL;
	iMsvOperation = iMsvEntry.DeleteL(iAttachmentId, iStatus);
	}

void CImRemoveMessagePart::DeleteFolderEntryL()
	{
	// context on parent of attachment - i.e. folder
	// move context to parent of folder before deleting folder
	iMsvEntry.SetEntryL(iMsvEntry.Entry().Parent());

	delete iMsvOperation;
	iMsvOperation = NULL;
	iMsvOperation = iMsvEntry.DeleteL(iFolderToDeleteId, iStatus);
	}

void CImRemoveMessagePart::CompleteRemoveMessagePartL()
	{
	iMsvEntry.SetEntryL(iMessageId);
	TMsvEntry entry(iMsvEntry.Entry());

	TInt count = iEmailMessage->AttachmentManager().AttachmentCount();
	if (count)
		{
		entry.SetAttachment((count-1)); // minus one since we have removed attachment
		}

	// update the size
	entry.iSize -= iAttachmentSize;

	CMsvEntrySelection* selection = iMsvEntry.ChildrenWithTypeL(KUidMsvFolderEntry);
	CleanupStack::PushL(selection);
	CMsvStore* store = iMsvEntry.EditStoreL();
	CleanupStack::PushL(store);

	if (selection->Count()) // folder exist
		{
		TMsvEmailEntry childEntry = iMsvEntry.ChildDataL((*selection)[0]);
	
		CImMimeHeader* mimeHeader = CImMimeHeader::NewLC();
		mimeHeader->RestoreL(*store);
		switch (childEntry.MessageFolderType())
			{
		case EFolderTypeAlternative:
			mimeHeader->SetContentSubTypeL(KMimeAlternative);
			break;
		case EFolderTypeMixed:
			mimeHeader->SetContentSubTypeL(KMimeMixed);
			break;
		case EFolderTypeRelated:
			mimeHeader->SetContentSubTypeL(KMimeRelated);
			break;
		default:
			User::Leave(KErrNotSupported);
			}
		mimeHeader->StoreL(*store);
		store->CommitL();
		CleanupStack::PopAndDestroy(); // mimeHeader
		}
	else if (store->IsPresentL(KUidMsgFileMimeHeader))
		{
		// if the child of the message is not a folder then the MimeHeader must be deleted
		store->RemoveL(KUidMsgFileMimeHeader);
		store->CommitL();
		}
	CleanupStack::PopAndDestroy(2); // selection, store
	delete iMsvOperation;
	iMsvOperation=NULL;
	iMsvOperation=iMsvEntry.ChangeL(entry, iStatus);
	}
	
TMsvId CImRemoveMessagePart::RemovedAttachmentId()
	{
	return iAttachmentId;
	}

//
// CImStoreMessagePart
//

CImStoreMessagePart* CImStoreMessagePart::AddAttachmentL(TMsvId aMessageId,const TDesC& aFilePath, CMsvEntry& aMsvEntry,CMsvAttachment* aAttachmentInfo,TRequestStatus& aStatus )
	{
	CImStoreMessagePart* self = new(ELeave) CImStoreMessagePart(aStatus, aMsvEntry,aMessageId);
	CleanupStack::PushL(self);
	self->ConstructL(aFilePath);
	self->ConstructAttachmentInfo(aAttachmentInfo,EAddAttachment);
	CleanupStack::Pop(self);
	return self;
	}
	
CImStoreMessagePart* CImStoreMessagePart::AddEntryAsAttachmentL(TMsvId aMessageId,TMsvId aAttachmentMessageId, CMsvEntry& aMsvEntry, CMsvAttachment* aAttachmentInfo, TRequestStatus& aStatus)
	{
	CImStoreMessagePart* self = new(ELeave) CImStoreMessagePart(aStatus, aMsvEntry, aMessageId);
	CleanupStack::PushL(self);
	self->ConstructL(aAttachmentMessageId);
	self->ConstructAttachmentInfo(aAttachmentInfo,EAddEntryAsAttachment);
	CleanupStack::Pop(self);
	return self;
	}

CImStoreMessagePart* CImStoreMessagePart::AddAttachmentL(TMsvId aMessageId, RFile& aFile, CMsvEntry& aMsvEntry, CMsvAttachment* aAttachmentInfo, TRequestStatus& aStatus) 	
	{
	CImStoreMessagePart* self = new(ELeave) CImStoreMessagePart(aStatus, aMsvEntry,aMessageId);
	CleanupStack::PushL(self);
	self->ConstructL(aFile);
	self->ConstructAttachmentInfo(aAttachmentInfo,EAddAttachmentUsingFileHandle);
	CleanupStack::Pop(self);
	return self;
	}
	
CImStoreMessagePart* CImStoreMessagePart::CreateAttachmentL(TMsvId aMessageId,const TDesC& aFileName, RFile& aAttachmentFile,CMsvEntry& aMsvEntry, CMsvAttachment* aAttachmentInfo, TRequestStatus& aStatus)	
	{
	CImStoreMessagePart* self = new(ELeave) CImStoreMessagePart(aStatus, aMsvEntry,aMessageId);
	CleanupStack::PushL(self);
	self->ConstructL(aAttachmentFile,aFileName);
	self->ConstructAttachmentInfo(aAttachmentInfo,ECreateAttachment);
	CleanupStack::Pop(self);
	return self;
	}
	
CImStoreMessagePart* CImStoreMessagePart::AddLinkedAttachmentL(TMsvId aMessageId,const TDesC& aFilePath, CMsvEntry& aMsvEntry,CMsvAttachment* aAttachmentInfo,TRequestStatus& aStatus )	
	{
	CImStoreMessagePart* self = new(ELeave) CImStoreMessagePart(aStatus, aMsvEntry,aMessageId);
	CleanupStack::PushL(self);
	self->ConstructL(aFilePath);
	self->ConstructAttachmentInfo(aAttachmentInfo,EAddLinkedAttachment);
	CleanupStack::Pop(self);
	return self;
	}


CImStoreMessagePart* CImStoreMessagePart::AddRelatedPartL(TRequestStatus &aStatus, CMsvEntry& aMsvEntry, TMsvId aMessageId, const TDesC& aAttachmentFullName, TMsvId aRelatedPartId, const TDesC8& aContentId)
	{
	CImStoreMessagePart* self = new(ELeave) CImStoreMessagePart(aStatus, aMsvEntry, aMessageId);
	CleanupStack::PushL(self);
	self->ConstructL(aAttachmentFullName, aRelatedPartId, aContentId);
	CleanupStack::Pop(self);
	return self;
	}

CImStoreMessagePart* CImStoreMessagePart::StoreBodyTextL(TRequestStatus& aStatus, CMsvEntry& aMsvEntry, TMsvId aMessageId, CRichText& aRichText, TBool aUsePlainTextStorage)
	{
	CImStoreMessagePart* self = new(ELeave) CImStoreMessagePart(aStatus, aMsvEntry, aMessageId, aUsePlainTextStorage);
	CleanupStack::PushL(self);
	self->ConstructL(aRichText);
	CleanupStack::Pop(self);
	return self;
	}

CImStoreMessagePart* CImStoreMessagePart::StoreBodyTextWithMimeHeaderL(TRequestStatus& aStatus, CMsvEntry& aMsvEntry, TMsvId aMessageId, CRichText& aRichText, const CImMimeHeader& aMimeHeader, TBool aUsePlainTextStorage)
	{
	CImStoreMessagePart* self = new(ELeave) CImStoreMessagePart(aStatus, aMsvEntry, aMessageId, aUsePlainTextStorage);
	CleanupStack::PushL(self);
	self->ConstructL(aRichText, aMimeHeader);
	CleanupStack::Pop(self);
	return self;
	}


CImStoreMessagePart* CImStoreMessagePart::StorePlainBodyTextL(TRequestStatus& aStatus, CMsvEntry& aMsvEntry, TMsvId aMessageId, TBool aUsePlainTextStorage)
	{
	CImStoreMessagePart* self = new(ELeave) CImStoreMessagePart(aStatus, aMsvEntry, aMessageId, aUsePlainTextStorage);
	CleanupStack::PushL(self);
	self->ConstructL(EMessagePartBody);
	CleanupStack::Pop(self);
	return self;
	}
	

	
CImStoreMessagePart* CImStoreMessagePart::StorePlainBodyTextL(TRequestStatus& aStatus, CMsvEntry& aMsvEntry, TMsvId aMessageId,CImMimeHeader& aMimeHeader, TBool aUsePlainTextStorage)
	{
	CImStoreMessagePart* self = new(ELeave) CImStoreMessagePart(aStatus, aMsvEntry, aMessageId, aUsePlainTextStorage);
	CleanupStack::PushL(self);
	self->ConstructL(aMimeHeader);
	CleanupStack::Pop(self);
	return self;
	}
		
CImStoreMessagePart::~CImStoreMessagePart()
	{
	Cancel();
	delete iMsvOperation;
	delete iFileMan;
	delete iMessageEntrySelection;
	delete iAttachmentFullName;
	delete iContentId;
	delete iHtmlConverter;
	delete iParaLayer;
	delete iCharLayer;
	delete iEmailMessage;
	delete iRichText;
	delete iStore;
	delete iAttachmentInfo;
	}

const TDesC8& CImStoreMessagePart::FinalProgress()
	{
	__ASSERT_ALWAYS(!IsActive(), gPanic(EMiutActiveInFinalProgress));
 	const TDesC8* progress = &KNullDesC8();
 	TRAPD(leave, progress = &ProgressL());
	__ASSERT_ALWAYS(leave == KErrNone, gPanic(EImcmFinalProgressFailed));
	return *progress;	
	}

const TDesC8& CImStoreMessagePart::ProgressL()
	{
	if (iState==EFinished)
		{
		switch(iMessagePart)
			{
		case EMessagePartAttachment:
		case EMessagePartMessageAttachment:
			iDataMember() = iAttachmentId;
			break;
		case EMessagePartBody:
			iDataMember() = iMessageId;
			break;
		default:
			iDataMember() = KMsvNullIndexEntryId;
			User::Leave(KErrNotSupported);
			}
		}
	return iDataMember;
	}

void CImStoreMessagePart::DoCancel()
	{
	if (iMsvOperation)
		iMsvOperation->Cancel();
	if (iEmailMessage)
		iEmailMessage->Cancel();

	// If we are in a state where we are waiting for the attachment manager to
	// complete us, then we need to cancel the attachment manager request
	if ((iState == EStoreAttachment) || (iState == EAddEntryAttachment))
		{
		if (iStore != NULL)
			{
			MMsvAttachmentManager* attachmentMgr(NULL);

			TRAPD(err, attachmentMgr = &(iStore->AttachmentManagerL()));

			if (err == KErrNone)
				{
				attachmentMgr->CancelRequest();
				}
			}
		}
	else
		{
		// we are not waiting for the attachment manager to complete us,
		// so the file handle needs to be closed by us.
		if((iAttachmentInfo != NULL) && (iAttachmentCreateState == EAddAttachmentUsingFileHandle))
			{
			iFile.Close();
			}
		}

	// Use ErrorRecovery routine to cleanup and complete the caller with KErrCancel
	ErrorRecovery(KErrCancel);

	}

void CImStoreMessagePart::SelectAndChangeToNextStateL()
	{
	SelectNextStateL();
	ChangeStateL();
	}

void CImStoreMessagePart::RunL()
	{
	if (iStatus.Int() != KErrNone)
		{
		ErrorRecovery(iStatus.Int());
		return;
		}
	if ((iState == ECreateAttachmentEntry) || 
		(iState == ECopyOrigMessageToMessage) || 
		(iState == ECreateMultipartMixedFolderEntry) ||
		(iState == ECreateMultipartRelatedFolderEntry) ||
		(iState == ECreateMultipartAlternativeFolderEntry) ||
		(iState == EMoveOriginalMessageEntryChildrenToNewFolder) ||
		(iState == ECreateTextEntry) ||
		(iState == EStoreRichText) ||
		(iState == ECreateHTMLEntry) ||
		(iState == ERemoveHTMLEntry && iHtmlId !=KMsvNullIndexEntryId) || 
		(iState == EMoveTextToAlternativeFolder) ||
		(iState == ECompleteStoreMessage))
		{
		TInt sysProgressError = McliUtils::GetProgressErrorL(*iMsvOperation);
		if (sysProgressError != KErrNone)
			{
			ErrorRecovery(sysProgressError);
			return;
			}
		}
	if (iState != EFinished)
		{
		TRAPD(error, SelectAndChangeToNextStateL());
		if (error)
			ErrorRecovery(error);
		else if (iState != EFinished)
			SetActive();
		}
	}

CImStoreMessagePart::CImStoreMessagePart(TRequestStatus& aStatus, CMsvEntry& aMsvEntry, TMsvId aMessageId, TBool aUsePlainTextStorage)
	: CMsvOperation(aMsvEntry.Session(), EPriorityStandard, aStatus),
	  iMsvEntry(aMsvEntry),
	  iMessageId(aMessageId),
	  iUsePlainTextStorage(aUsePlainTextStorage)
	{
	}

void CImStoreMessagePart::OpenAndReadResourceFileL()
	{
	RResourceFile resourceFile;
	OpenResourceFileL(resourceFile, iMsvSession.FileSession());
	CleanupClosePushL(resourceFile);

	if (iHtmlConverter)
		iHtmlConverter->ReadDefaultAttachmentNameL(resourceFile);
			
	CleanupStack::PopAndDestroy(); // resourceFile (Close resourceFile)
	}

void CImStoreMessagePart::ConstructL()
	{
	iFlags = KStoreMessagePartClearFlag;
	iDataMember = KMsvNullIndexEntryId;
	iMixFolderId = KMsvNullIndexEntryId;
	iAltFolderId = KMsvNullIndexEntryId;
	iAttachmentId = KMsvNullIndexEntryId;
	iTextId = KMsvNullIndexEntryId;
	iHtmlId = KMsvNullIndexEntryId;

	iFileMan = CFileMan::NewL(iMsvEntry.Session().FileSession());

	iParaLayer = CParaFormatLayer::NewL();
	iCharLayer = CCharFormatLayer::NewL();
	iRichText = CRichText::NewL(iParaLayer, iCharLayer);
	iEmailMessage = CImEmailMessage::NewL(iMsvEntry);
	
	CActiveScheduler::Add(this);

	if (iRelatedPartId != KMsvNullIndexEntryId)
		iState = EFindMultipartRelatedFolder;
	else
		iState = ECheckForSubfolder;
	ChangeStateL();
	

	iObserverRequestStatus = KRequestPending;
	SetActive();
	}

void CImStoreMessagePart::ConstructL(TImMessagePart aMessagePart)
	{
	iMessagePart = aMessagePart;
	ConstructL();	
	}
	
void CImStoreMessagePart::ConstructL(const TDesC& aAttachmentFullName)
	{
	iAttachmentFullName = aAttachmentFullName.AllocL();
	iMessagePart = EMessagePartAttachment;
	ConstructL();
	}

void CImStoreMessagePart::ConstructL(TMsvId aAttachmentMessageId)
	{
	iAttachmentMessageId = aAttachmentMessageId;
	iMessagePart = EMessagePartMessageAttachment;
	ConstructL();
	}

void CImStoreMessagePart::ConstructL(const TDesC& aAttachmentFullName, TMsvId aRelatedPartId, const TDesC8& aContentId)
	{
	iAttachmentFullName = aAttachmentFullName.AllocL();
	iRelatedPartId = aRelatedPartId;
	iMessagePart = EMessagePartAttachment;
	iContentId = aContentId.AllocL();
	ConstructL();
	}

void CImStoreMessagePart::ConstructL(CRichText& aRichText)
	{
	iRichTextToStore=&aRichText;
	iMessagePart = EMessagePartBody;
	ConstructL();
	}

void CImStoreMessagePart::ConstructL(CRichText& aRichText, const CImMimeHeader& aMimeHeader)
	{
	iRichTextToStore=&aRichText;
	// We don't take ownership of the Mime header, we just 'flag' 
	// its presence:
	iMimeHeaderOfBodyText=const_cast<CImMimeHeader*>(&aMimeHeader);
	iMessagePart = EMessagePartBody;
	ConstructL();
	}

void CImStoreMessagePart::ConstructL(CImMimeHeader& aMimeHeader)
	{
	// We don't take ownership of the Mime header, we just 'flag' 
	// its presence:
	iMimeHeaderOfBodyText = &aMimeHeader;
	iMessagePart = EMessagePartBody;
	ConstructL();
	}
void CImStoreMessagePart::ConstructL(const TDesC& aAttachmentFullName, const TDesC8& aContentId)
	{
	iAttachmentFullName = aAttachmentFullName.AllocL();
	iMessagePart = EMessagePartAttachment;
	iContentId = aContentId.AllocL();
	iStore=NULL;
	ConstructL();
	}
	
void CImStoreMessagePart::ConstructL(RFile& aFile)
	{
	iFile = aFile;
	iFileHandle=&aFile;
	iMessagePart = EMessagePartAttachment;
	iIsAddByFileHandle = ETrue;
	ConstructL();
	}

void CImStoreMessagePart::ConstructL(RFile& aFile,const TDesC& aAttachmentName)
	{
	iAttachmentFullName = aAttachmentName.AllocL();
	iFileHandle=&aFile;
	iMessagePart = EMessagePartAttachment;
	iIsAddByFileHandle=ETrue;
	ConstructL();
	}

void CImStoreMessagePart::ConstructAttachmentInfo(CMsvAttachment* aAttachmentInfo,TInt aAttachmentCreationState)
	{
	iAttachmentInfo = aAttachmentInfo;
	iAttachmentCreateState = aAttachmentCreationState;
	}
	
void CImStoreMessagePart::ErrorRecovery(TInt error)
	{
	iHtmlConverter->ResetStoreWithoutCommit();
	// Close store if it is currently open for read or edit
	if (iStore != NULL)
		{
		delete iStore;
		iStore = NULL;
		}
	
	// if there is an error then need to undo any changes
	Recover();

	// complete the observer with error
	TRequestStatus* status=&iObserverRequestStatus;
	User::RequestComplete(status,error);
	}

void CImStoreMessagePart::SelectNextStateL()
	{
	switch (iState)
		{
	case EFindMultipartRelatedFolder:
		iState = ECheckForSubfolder;
		break;
	case ECheckForSubfolder:
		// context on Message
		if (iEmailMessage->Selection().Count())
			iTextId = iEmailMessage->Selection().At(0);
		SetTextPartExists(iEmailMessage->Selection().Count());
		if (iMessagePart == EMessagePartAttachment)
			{
			if (iMessageEntrySelection->Count() && (iRelatedPartId == KMsvNullIndexEntryId) && (!MultipartMixedExists()))
				// one item under message entry as we should have more than one entry under message entry
				iState = ECreateMultipartMixedFolderEntry;
			else if (iMessageEntrySelection->Count() && (iRelatedPartId != KMsvNullIndexEntryId) && (!MultipartRelatedExists())) 
				{
				// we want to add an attachment related to iRelatedPartId
				if (MultipartMixedExists())
					iMsvEntry.SetEntryL(iMixFolderId);
				iState = ECreateMultipartRelatedFolderEntry;
				}
			else
				{
				// if multipart/related exists then set context to related
				if (MultipartRelatedExists())
					iMsvEntry.SetEntryL(iRelFolderId);
				if (MultipartMixedExists())
					iMsvEntry.SetEntryL(iMixFolderId);
				iState = ECreateAttachmentEntry;
				}
			}
		else if (iMessagePart == EMessagePartMessageAttachment)
			{
			if (iMessageEntrySelection->Count() && (iRelatedPartId == KMsvNullIndexEntryId) && (!MultipartMixedExists()))
				// one item under message entry as we should have more than one entry under message entry
				iState = ECreateMultipartMixedFolderEntry;
			else 
				iState = ECopyOrigMessageToMessage;
			}
		else if (iMessagePart == EMessagePartBody)
			{
	        if (HTMLMessage() && iHtmlConverter)
	            {
	            if (iTextId != KMsvNullIndexEntryId)
		            {
		            if (MultipartAlternativeExists())
		                {
		                // Alternative folder exists - continue as normal...
		                iState = EStoreRichText;
		                }
	                else
						{	
						// This is an HTML message and the text entry already
						// exists. Need to check that the text entry's parent
						// is an alternative folder.
						TMsvId serviceId = KMsvNullIndexEntryId;
						TMsvEmailEntry entry;
						// Get text entry...
						iMsvEntry.Session().GetEntry(iTextId, serviceId, entry);
						// ...and then its parent
						iMsvEntry.Session().GetEntry(entry.Parent(), serviceId, entry);
						if (entry.MessageFolderType() != EFolderTypeAlternative)
							{
						   // The situation is that a text entry exist, but there
	                       // is no alternative folder for. This is probably cos
                           // the message was originally plain, but has been changed
                           // to be an HTML message. Create the alternative folder
                           // under the text entry's parent.
	                       if (iMsvEntry.EntryId() != entry.Id())
		                       {
		                       // Ensure that entry text entry's parent
		                       iMsvEntry.SetEntryL(entry.Id());
							   }
			                  iState = ECreateMultipartAlternativeFolderEntry;
		                   }
						else
						   {
			              // Alternative folder exists - continue as normal...
	                      iState = EStoreRichText;
						   }
						}
					}
				else
					{
					// No text entry exists - need to create an alternative folder
					iState = ECreateMultipartAlternativeFolderEntry;
					}
				}
			else if (iTextId != KMsvNullIndexEntryId)
				iState = EStoreRichText;
			else if (iMessageEntrySelection->Count() && (!MultipartMixedExists())) // one item under message entry
				iState = ECreateMultipartMixedFolderEntry;
			else
				{
				if (MultipartMixedExists())
					iMsvEntry.SetEntryL(iMixFolderId);
				iState = ECreateTextEntry;
				}
			}
		break;
	case ECreateMultipartMixedFolderEntry:
		{
		SetMultipartMixedCreated(ETrue);
		iMixFolderId = McliUtils::GetProgressIdL(*iMsvOperation);
		iMsvEntry.SetEntryL(iMixFolderId);
		switch (iMessagePart)
			{
		case EMessagePartAttachment:
		case EMessagePartMessageAttachment:
			if ((iRelatedPartId != KMsvNullIndexEntryId) && (!MultipartRelatedExists()))
				iState = ECreateMultipartRelatedFolderEntry;
			else if (iMessagePart == EMessagePartAttachment)
				iState = ECreateAttachmentEntry;
			else if (iMessagePart == EMessagePartMessageAttachment)
				iState = ECopyOrigMessageToMessage;
			break;
		case EMessagePartBody:
			if (HTMLMessage())
				iState = ECreateMultipartAlternativeFolderEntry;
			else
				iState = ECreateTextEntry;
			break;
		default:
			User::Leave(KErrNotSupported);
			}
		}
		break;
	case ECreateMultipartRelatedFolderEntry:
		{
		SetMultipartRelatedCreated(ETrue);
		iRelFolderId = McliUtils::GetProgressIdL(*iMsvOperation);
		iMsvEntry.SetEntryL(iRelFolderId);
		iState = ECreateAttachmentEntry;
		}
		break;
	case ECreateAttachmentEntry:
		{
		SetAttachmentEntryCreated(ETrue);
		iAttachmentId = McliUtils::GetProgressIdL(*iMsvOperation);
		iMsvEntry.SetEntryL(iAttachmentId);
		// we only need to store the attachment if the path is provided - otherwise, only 
		// the entry needs to be created so that the actual attachment can be added later.
		if(iIsAddByFileHandle || iAttachmentFullName->Length())	
			iState = EStoreAttachment;
		else if (MultipartMixedCreated() || MultipartRelatedCreated())
			iState = EMoveOriginalMessageEntryChildrenToNewFolder;
		else
			iState = ECompleteStoreMessage;
		}
		break;
	case EStoreAttachment:
		iStore->CommitL();
	
		delete iStore;
		iStore = NULL;	
		
		if (MultipartMixedCreated() || MultipartRelatedCreated())
			iState = EMoveOriginalMessageEntryChildrenToNewFolder;
		else
			iState = ECompleteStoreMessage;
		break;
	case ECopyOrigMessageToMessage:
		{
		iAttachmentId = McliUtils::GetProgressIdL(*iMsvOperation);
		iState = EAddEntryAttachment;
		}
		break;
	case ECreateMultipartAlternativeFolderEntry:
		{
		iAltFolderId = McliUtils::GetProgressIdL(*iMsvOperation);
		iMsvEntry.SetEntryL(iAltFolderId);
		SetMultipartAlternativeCreated(ETrue);
		SetMultipartAlternativeExists(ETrue);
		
		// Text Entry was already created without alternative folder . 
		// This is because of change in email settings from Plain (when creating the message) 
		// to Formatted (when sending the message)
		if(iTextId != KMsvNullIndexEntryId)
			iState = EMoveTextToAlternativeFolder;
		else 
			iState = ECreateTextEntry;
		}
		break;
	case EMoveTextToAlternativeFolder:
		{
		iState = EStoreRichText;
		}
		break;
	case ECreateTextEntry:
		if (iTextId == KMsvNullIndexEntryId)
			{
			iTextId = McliUtils::GetProgressIdL(*iMsvOperation);
			}
		iState = EStoreRichText;
		break;
	case EStoreRichText:
		// a text part has been created ONLY if one did not exist previously
		SetTextPartCreated(!TextPartExists());
		if (TextPartExists() && MultipartAlternativeCreated())
			{
			// The text entry already existed, but an alternative folder had to
			// created. This is probably due to the message being originally
			// plain and then being changed to be an HTML message. Need to create
			// the HTML entry.
			iState = ECreateHTMLEntry;
			}
		else if (TextPartExists() && iHtmlConverter)
			{
			// create HTML alternative if HTML message and text part just created
			iState = EPrepareToStoreHTMLEntryText;
			}
		else if (!HTMLMessage() && !TextPartCreated())
			{
			// The message is a non-HTML message, and this is not the first time
			// that the message body has been stored. Need to remove an HTML 
			// entry if it exists.
			iState = ERemoveHTMLEntry;
			}
		else if (HTMLMessage() && (TextPartCreated() || MultipartAlternativeCreated()))
			iState = ECreateHTMLEntry;
		else if (MultipartMixedCreated())
			iState = EMoveOriginalMessageEntryChildrenToNewFolder;
		else
			iState = ECompleteStoreMessage;
		break;
	case ECreateHTMLEntry:
		{
		iHtmlId = McliUtils::GetProgressIdL(*iMsvOperation);
		// Store character set info in a MimeHeader.
		iMsvEntry.SetEntryL(iHtmlId);
		CMsvStore* store = iMsvEntry.EditStoreL();
		CleanupStack::PushL(store);
		CImMimeHeader* mimeHeader = CImMimeHeader::NewLC();
		mimeHeader->SetMimeCharset(KCharacterSetIdentifierUtf8);
		mimeHeader->StoreL(*store);
		store->CommitL();
		CleanupStack::PopAndDestroy(2,store); //mimeHeader, store
		iState = EPrepareToStoreHTMLEntryText;
		}
		break;
	case EPrepareToStoreHTMLEntryText:
		iFinishedConvertingHTML=EFalse;
		if(iUsePlainTextStorage && iRestoreErr == KErrNoMemory)
			{
			// If iRestoreErr is KErrNoMemory then complete the store message operation.
			iState = ECompleteStoreMessage;	
			}
		else
			{
			iState = iHtmlId ? EStoreHTMLEntryText : ECompleteStoreMessage;	
			}
		break;
	case EStoreHTMLEntryText:
		if (!iFinishedConvertingHTML)
			break;
		iState = EStoreHTMLTextInEntry;
		iHtmlConverter->ResetStoreL();	
		break;
	case EStoreHTMLTextInEntry:
		if (MultipartMixedCreated())
			iState = EMoveOriginalMessageEntryChildrenToNewFolder;
		else
			iState = ECompleteStoreMessage;
		break;
	case EAddEntryAttachment:
		{
		iStore->CommitL();
		delete iStore;
		iStore = NULL;
					
		if (MultipartMixedCreated() || MultipartRelatedCreated())
			iState = EMoveOriginalMessageEntryChildrenToNewFolder;
		else
			{
			// AddEntryAsAttachmentL() was called in this state and resets iMsvEntry.
			// Next state requires that it's set to the messageId
			// DEF 050410
			iMsvEntry.SetEntryL(iMessageId);
			iState = ECompleteStoreMessage;
			}
		} break;
	case ERemoveHTMLEntry:
		{
		iState = ECompleteStoreMessage;
		} break;
	case EMoveOriginalMessageEntryChildrenToNewFolder:
		iState = ECompleteStoreMessage;
		break;
	case ECompleteStoreMessage:
		iState = EFinished;
		break;
	default:
		gPanic(EImcmUnknownState);
		}
	}

void CImStoreMessagePart::ChangeStateL()
	{
	CMsvOperation* op;
	switch (iState)
		{
	case EFindMultipartRelatedFolder:
		FindMultipartRelatedFolderL();
		break;
	case ECheckForSubfolder:
		CheckForSubfolderL();
		break;
	case ECreateMultipartMixedFolderEntry:
		CreateMultipartMixedFolderEntryL();
		break;
	case ECreateMultipartRelatedFolderEntry:
		CreateMultipartRelatedFolderEntryL();
		break;
	case ECreateAttachmentEntry:
		CreateAttachmentEntryL();
		break;
	case EStoreAttachment:
		{
		// store the attachment
		DoAttachmentStoreL();
		}
		break;
	case ECopyOrigMessageToMessage:
		CopyOrigMessageToMessageL();
		break;
	case ECreateMultipartAlternativeFolderEntry:
		CreateMultipartAlternativeFolderEntryL();
		break;
	case EMoveTextToAlternativeFolder:
		// This case arrives when the email settings was plain while the message was created and 
		// the setting is changed to formatted (html) while sending.
		MoveTextEntryToAltFolderL();
		break;
	case ECreateTextEntry:
		CreateTextEntryL();
		break;
	case EStoreRichText:
		StoreRichTextL();
		break;
	case ECreateHTMLEntry:
		CreateHTMLEntryL();
		break;
	case EPrepareToStoreHTMLEntryText:
		TRAP(iRestoreErr ,iHtmlConverter->PrepareToStoreHTMLEntryTextL(iHtmlId, iTextId));
		// Do not leave if it is KErrNoMemory while creating HTML part for a message, since the 
		// plainbody text part of message is already created and that be used to send the message.
		if(iUsePlainTextStorage)
			{
			if(iRestoreErr != KErrNoMemory)
				{
				User::LeaveIfError(iRestoreErr);
				}
			}
		else
			{
			User::LeaveIfError(iRestoreErr);
			}
		
		RequestComplete(KErrNone);
		break;
	case EStoreHTMLEntryText:
		iFinishedConvertingHTML = iHtmlConverter->StoreHTMLEntryTextAL(iStatus);
		if (!iFinishedConvertingHTML)
			RequestComplete(KErrNone);
		break;
	case EStoreHTMLTextInEntry:
		op = iHtmlConverter->ChangeHTMLTextInEnrtyL(iStatus);
		if (!op)
			{
			RequestComplete(KErrNone);
			}
		else
			{
			delete iMsvOperation;
			iMsvOperation = op;
			}
		break;
	case EAddEntryAttachment:
		AddEntryAsAttachmentL();		
		break;
	case ERemoveHTMLEntry:
		{
		RemoveHTMLEntryL();
		} break;
	case EMoveOriginalMessageEntryChildrenToNewFolder:
		MoveOriginalMessageEntryChildrenToNewFolderL();
		break;
	case ECompleteStoreMessage:
		CompleteStoreMessagePartL();
		break;
	case EFinished:
		{
		TRequestStatus* status=&iObserverRequestStatus;
		User::RequestComplete(status,KErrNone);
		}
		break;
	default:
		gPanic(EImcmUnknownState);
		}
	}

void CImStoreMessagePart::RequestComplete(TInt aError)
	{
	iStatus = KRequestPending;
	TRequestStatus* status=&iStatus;
	iStatus=KRequestPending;
	User::RequestComplete(status,aError);
	}

void CImStoreMessagePart::Recover()
	{
	switch (iState)
		{
	case ECreateHTMLEntry:
	case EStoreRichText:
	case ERemoveHTMLEntry:
	case ECreateTextEntry:
	case ECreateMultipartAlternativeFolderEntry:
	case ECreateMultipartRelatedFolderEntry:
	case EPrepareToStoreHTMLEntryText:
	case EStoreHTMLEntryText:
		if (MultipartMixedCreated()) // iMixFolderId set when folder created
			iMsvEntry.Session().RemoveEntry(iMixFolderId);
		else if (MultipartRelatedCreated())
			iMsvEntry.Session().RemoveEntry(iRelFolderId);
		else if (MultipartAlternativeCreated())
			iMsvEntry.Session().RemoveEntry(iAltFolderId);
		else if (TextPartCreated())
			iMsvEntry.Session().RemoveEntry(iTextId);
		break;
	case EMoveOriginalMessageEntryChildrenToNewFolder:
	case ECopyOrigMessageToMessage:
	case EStoreAttachment:
	case EMoveTextToAlternativeFolder:
	case ECreateAttachmentEntry:
		{
		if (MultipartMixedCreated())
			iMsvEntry.Session().RemoveEntry(iMixFolderId);
		else if (MultipartRelatedCreated())
			iMsvEntry.Session().RemoveEntry(iRelFolderId);
		else if (MultipartAlternativeCreated())
			iMsvEntry.Session().RemoveEntry(iAltFolderId);
		else if (iAttachmentId != KMsvNullIndexEntryId)
			iMsvEntry.Session().RemoveEntry(iAttachmentId);
		}
		break;
	// for other states - original message not changed:
	case ECompleteStoreMessage: // message details may be incorrect but structure is ok to send
	case ECreateMultipartMixedFolderEntry:
	case ECheckForSubfolder:
	case EFindMultipartRelatedFolder:
	case EFinished:	// operation completed anyway
		break;
	default:
		// shouldn't get to this state
		__ASSERT_DEBUG(EFalse, gPanic(EImcmUnknownState));
		}
	}

TBool CImStoreMessagePart::HTMLMessage() const
	{
	return iFlags & KStoreMessagePartHTMLMessage;
	}

void CImStoreMessagePart::SetHTMLMessage(TBool aFlag)
	{
	iFlags = (iFlags & ~KStoreMessagePartHTMLMessage) | (aFlag ? KStoreMessagePartHTMLMessage : KStoreMessagePartClearFlag);
	}

TBool CImStoreMessagePart::MultipartMixedExists() const
	{
	return iFlags & KStoreMessagePartMultipartMixedExists;
	}

void CImStoreMessagePart::SetMultipartMixedExists(TBool aFlag)
	{
	iFlags = (iFlags & ~KStoreMessagePartMultipartMixedExists) | (aFlag ? KStoreMessagePartMultipartMixedExists : KStoreMessagePartClearFlag);
	}

TBool CImStoreMessagePart::MultipartMixedCreated() const
	{
	return iFlags & KStoreMessagePartMultipartMixedCreated;
	}

void CImStoreMessagePart::SetMultipartMixedCreated(TBool aFlag)
	{
	iFlags = (iFlags & ~KStoreMessagePartMultipartMixedCreated) | (aFlag ? KStoreMessagePartMultipartMixedCreated : KStoreMessagePartClearFlag);
	}

TBool CImStoreMessagePart::MultipartAlternativeExists() const
	{
	return iFlags & KStoreMessagePartMultipartAlternativeExists;
	}

void CImStoreMessagePart::SetMultipartAlternativeExists(TBool aFlag)
	{
	iFlags = (iFlags & ~KStoreMessagePartMultipartAlternativeExists) | (aFlag ? KStoreMessagePartMultipartAlternativeExists : KStoreMessagePartClearFlag);
	}

TBool CImStoreMessagePart::MultipartAlternativeCreated() const
	{
	return iFlags & KStoreMessagePartMultipartAlternativeCreated;
	}

void CImStoreMessagePart::SetMultipartAlternativeCreated(TBool aFlag)
	{
	iFlags = (iFlags & ~KStoreMessagePartMultipartAlternativeCreated) | (aFlag ? KStoreMessagePartMultipartAlternativeCreated : KStoreMessagePartClearFlag);
	}

TBool CImStoreMessagePart::MultipartRelatedExists() const
	{
	return iFlags & KStoreMessagePartMultipartRelatedExists;
	}

void CImStoreMessagePart::SetMultipartRelatedExists(TBool aFlag)
	{
	iFlags = (iFlags & ~KStoreMessagePartMultipartRelatedExists) | (aFlag ? KStoreMessagePartMultipartRelatedExists : KStoreMessagePartClearFlag);
	}

TBool CImStoreMessagePart::MultipartRelatedCreated() const
	{
	return iFlags & KStoreMessagePartMultipartRelatedCreated;
	}

void CImStoreMessagePart::SetMultipartRelatedCreated(TBool aFlag)
	{
	iFlags = (iFlags & ~KStoreMessagePartMultipartRelatedCreated) | (aFlag ? KStoreMessagePartMultipartRelatedCreated : KStoreMessagePartClearFlag);
	}

TBool CImStoreMessagePart::TextPartExists() const
	{
	return iFlags & KStoreMessagePartTextPartExists;
	}

void CImStoreMessagePart::SetTextPartExists(TBool aFlag)
	{
	iFlags = (iFlags & ~KStoreMessagePartTextPartExists) | (aFlag ? KStoreMessagePartTextPartExists : KStoreMessagePartClearFlag);
	}

TBool CImStoreMessagePart::TextPartCreated() const
	{
	return iFlags & KStoreMessagePartTextPartCreated;
	}

void CImStoreMessagePart::SetTextPartCreated(TBool aFlag)
	{
	iFlags = (iFlags & ~KStoreMessagePartTextPartCreated) | (aFlag ? KStoreMessagePartTextPartCreated : KStoreMessagePartClearFlag);
	}

TBool CImStoreMessagePart::AttachmentEntryCreated() const
	{
	return iFlags & KStoreMessagePartAttachmentEntryCreated;
	}

void CImStoreMessagePart::SetAttachmentEntryCreated(TBool aFlag)
	{
	iFlags = (iFlags & ~KStoreMessagePartAttachmentEntryCreated) | (aFlag ? KStoreMessagePartAttachmentEntryCreated : KStoreMessagePartClearFlag);
	}

void CImStoreMessagePart::FindMultipartRelatedFolderL()
	{
	TMsvId serviceId = KMsvNullIndexEntryId;
	TMsvEmailEntry entry;
	iMsvEntry.Session().GetEntry(iRelatedPartId, serviceId, entry);
	iMsvEntry.SetEntryL(entry.Parent());
	entry = iMsvEntry.Entry();
	switch (entry.MessageFolderType())
		{
	case EFolderTypeRelated:
		iRelFolderId = entry.Id();
		SetMultipartRelatedExists(ETrue);
		break;
	case EFolderTypeAlternative:
		iAltFolderId = entry.Id();
		iMsvEntry.Session().GetEntry(iAltFolderId, serviceId, entry);
		iMsvEntry.SetEntryL(entry.Parent());
		entry = iMsvEntry.Entry();
		if (entry.MessageFolderType() == EFolderTypeRelated)
			{
			iRelFolderId = entry.Id();
			SetMultipartRelatedExists(ETrue);
			}
		break;
	default:
		User::Leave(KErrNotSupported);
		}
	RequestComplete(KErrNone);
	}

void CImStoreMessagePart::CheckForSubfolderL()
	{
	/* 
	//	Check whether the message entry has a multipart mixed folder under it.
	*/
	iMsvEntry.SetEntryL(iMessageId);
	TMsvEmailEntry emailEntry = iMsvEntry.Entry();
	SetHTMLMessage(emailEntry.MHTMLEmail());
	if (HTMLMessage())
		{
		TRAPD(err,iHtmlConverter=CImHtmlConverter::NewL(iMsvEntry, *iParaLayer, *iCharLayer));
 		if (err)
			{
			if (err == KErrNoMemory)
				User::Leave(KErrNoMemory);
			else
				gPanic(EImcmHtmlConverterNotFound);
			}

		// Need to check if the Html converter has been created as we only open the resource file
		// to get the default html attachment name.
		if (iHtmlConverter)
			OpenAndReadResourceFileL();
		}

	iMessageEntrySelection = iMsvEntry.ChildrenL();
	CMsvEntrySelection* entrySelection = iMsvEntry.ChildrenWithTypeL(KUidMsvFolderEntry);
	CleanupStack::PushL(entrySelection);

	if (entrySelection->Count())
		{
		__ASSERT_DEBUG(entrySelection->Count() == 1, gPanic(EImcmMessageEntryHasMoreThanOneFolder));
		emailEntry = iMsvEntry.ChildDataL((*entrySelection)[0]);
		if (emailEntry.MessageFolderType() == EFolderTypeMixed)
			{
			iMixFolderId = (*entrySelection)[0];
			SetMultipartMixedExists(ETrue);
			}
		else if (emailEntry.MessageFolderType() == EFolderTypeRelated)
			{
			iRelFolderId = (*entrySelection)[0];
			SetMultipartRelatedExists(ETrue);
			}
		else if (emailEntry.MessageFolderType() == EFolderTypeAlternative)
			{
			iAltFolderId = (*entrySelection)[0];
			SetMultipartAlternativeExists(ETrue);
			}
		}

	CleanupStack::PopAndDestroy(); // entrySelection
	
	iEmailMessage->GetBodyTextEntryIdL(iMessageId, CImEmailMessage::EThisMessageOnly);	
	TBool isPlainText = EFalse;
	if(iEmailMessage->Selection().Count())
		{
		TMsvId textId = iEmailMessage->Selection().At(0);	
		iMsvEntry.SetEntryL(textId);// pointing to the text entry
			
		if(iMsvEntry.HasStoreL())
			{
			CMsvStore* bodyTextStore = iMsvEntry.ReadStoreL();
			CleanupStack::PushL(bodyTextStore);
			
			// Check if the original message was stored as plain text.
			if(iUsePlainTextStorage)
				{
				isPlainText = (bodyTextStore->IsPresentL(KMsvPlainBodyText16) || bodyTextStore->IsPresentL(KMsvPlainBodyText8));
				}			
			
			// This is not a richtext message.	
			if(isPlainText)
				{
				CMsvPlainBodyText* textOrig = bodyTextStore->InitialisePlainBodyTextForReadL(KMaxChunkLength);	
				iSizeFwdReplyBody = textOrig->Size();
				
				delete textOrig;

				// Need to release body text store before changing to a new entry
				CleanupStack::PopAndDestroy(bodyTextStore);

				iMsvEntry.SetEntryL(iMessageId);

				TRequestStatus* myStatus=&iStatus;
				iStatus=KRequestPending;
				User::RequestComplete(myStatus,KErrNone);	
				}
			else
				{
				CleanupStack::PopAndDestroy(bodyTextStore);
				iMsvEntry.SetEntryL(iMessageId);
				iEmailMessage->GetBodyTextL(iStatus, iMessageId, CImEmailMessage::EThisMessageOnly, *iRichText, *iParaLayer, *iCharLayer);	
				}
			}
		else
			{
			iMsvEntry.SetEntryL(iMessageId);
			iEmailMessage->GetBodyTextL(iStatus, iMessageId, CImEmailMessage::EThisMessageOnly, *iRichText, *iParaLayer, *iCharLayer);	
			}
		}
	else
		{
		iMsvEntry.SetEntryL(iMessageId);
		iEmailMessage->GetBodyTextL(iStatus, iMessageId, CImEmailMessage::EThisMessageOnly, *iRichText, *iParaLayer, *iCharLayer);	
		}	
	}

void CImStoreMessagePart::CreateMultipartMixedFolderEntryL()
	{
	iMsvEntry.SetEntryL(iMessageId);

	TMsvEmailEntry entry;
	entry.iType = KUidMsvFolderEntry;
	entry.iMtm = iMsvEntry.Entry().iMtm;
	entry.iServiceId = iMsvEntry.Entry().iServiceId;
	entry.SetMessageFolderType(EFolderTypeMixed);
	entry.iSize = 0;

	delete iMsvOperation;
	iMsvOperation=NULL;
	iMsvOperation=iMsvEntry.CreateL(entry, iStatus);
	}

void CImStoreMessagePart::CreateMultipartRelatedFolderEntryL()
	{
	TMsvEmailEntry entry;
	entry.iType = KUidMsvFolderEntry;
	entry.iMtm = iMsvEntry.Entry().iMtm;
	entry.iServiceId = iMsvEntry.Entry().iServiceId;
	entry.SetMessageFolderType(EFolderTypeRelated);
	entry.iSize = 0;

	delete iMsvOperation;
	iMsvOperation=NULL;
	iMsvOperation=iMsvEntry.CreateL(entry, iStatus);
	}

void CImStoreMessagePart::CreateAttachmentEntryL()
	{
	// create the index entry
	TMsvEntry entry;
	entry.iMtm = iMsvEntry.Entry().iMtm;
	entry.iServiceId = iMsvEntry.Entry().iServiceId;
	entry.iType = KUidMsvAttachmentEntry;
	entry.iDate.UniversalTime();
	entry.SetAttachment(ETrue);
	entry.SetInPreparation(ETrue);
	TParse parse;
	if(!iIsAddByFileHandle)
		{
	//if the attachment path is set then set details, otherwise just create the entry.
	if (iAttachmentFullName->Length())
		{
		parse.Set(iAttachmentFullName->Des(), NULL, NULL);
		entry.iDetails.Set(parse.NameAndExt());

		// Set the size of the attachment
		TEntry attEntry;
		iMsvEntry.Session().FileSession().Entry(iAttachmentFullName->Des(), attEntry);
		iAttachmentSize = attEntry.iSize;
		entry.iSize = iAttachmentSize;
		}
		}
	else if(iIsAddByFileHandle)
		{
		SetEntryDetailsL(entry);
		}
	delete iMsvOperation;
	iMsvOperation = NULL;
	iMsvOperation = iMsvEntry.CreateL(entry, iStatus);
	}
	
void CImStoreMessagePart::DoAttachmentStoreL()
	{
	if(iRelatedPartId)
		{
		delete iAttachmentInfo;
		iAttachmentInfo=NULL;
		iAttachmentInfo = CMsvAttachment::NewL(CMsvAttachment::EMsvFile);
		iAttachmentCreateState = EAddAttachment;	
		}
	switch(iAttachmentCreateState)	
		{
	case ECreateAttachment:
		{
		CreateAttachmentL();
		}
		break;
	case EAddAttachment:
		{
		AddAttachmentL();	
		}
	break;	
	case EAddAttachmentUsingFileHandle:
		{
		AddAttachmentUsingFileHandleL();
		}
	break;	
	case  EAddLinkedAttachment:
		{
		AddLinkedAttachmentL();	
		}
	break;
	case EAddEntryAsAttachment:
		{
		AddEntryAsAttachmentL();
		}
	break;	
	default:
	__ASSERT_DEBUG(EFalse, User::Invariant());
	break;
		}
	}
	
void CImStoreMessagePart::CreateAttachmentL()
	{
	
	iStore = iMsvEntry.EditStoreL();
	
	MMsvAttachmentManager& attachmentMgr = iStore->AttachmentManagerL();
	attachmentMgr.CreateAttachmentL(iAttachmentFullName->Des(),(*iFileHandle),iAttachmentInfo,iStatus);
	iAttachmentInfo=NULL; 
	}
	
void CImStoreMessagePart::AddAttachmentL()
	{
	iStore = iMsvEntry.EditStoreL();
	
	MMsvAttachmentManager& attachmentMgr = iStore->AttachmentManagerL();
	attachmentMgr.AddAttachmentL(iAttachmentFullName->Des(),iAttachmentInfo,iStatus); 
	iAttachmentInfo=NULL; 
	}
void CImStoreMessagePart::AddAttachmentUsingFileHandleL()	
	{

	iStore = iMsvEntry.EditStoreL();
	
	MMsvAttachmentManager& attachmentMgr = iStore->AttachmentManagerL();
	attachmentMgr.AddAttachmentL((*iFileHandle),iAttachmentInfo,iStatus);
	iAttachmentInfo=NULL; 
	}
	
void CImStoreMessagePart::AddLinkedAttachmentL()	
	{

	iStore = iMsvEntry.EditStoreL();
	
	MMsvAttachmentManager& attachmentMgr = iStore->AttachmentManagerL();
	attachmentMgr.AddLinkedAttachmentL(iAttachmentFullName->Des(),iAttachmentInfo,iStatus);
	iAttachmentInfo=NULL; 
	}
	
void CImStoreMessagePart::AddEntryAsAttachmentL()	
	{

	iMsvEntry.SetEntryL(iAttachmentId);
	iStore = iMsvEntry.EditStoreL();
	
	MMsvAttachmentManager& attachmentMgr = iStore->AttachmentManagerL();
	attachmentMgr.AddEntryAsAttachmentL(iAttachmentMessageId,iAttachmentInfo,iStatus);
	iAttachmentInfo=NULL; 
	}

TInt CImStoreMessagePart::CheckMimeInAttachmentInfoL(CMsvMimeHeaders& aMsvMimeHeaders)
	{
	if(!iMsvEntry.HasStoreL())
		return KErrNotFound;
	
	CMsvStore* store  = iMsvEntry.ReadStoreL();
	CleanupStack::PushL(store);
	// get the attachment info
	MMsvAttachmentManager& attachmentMgr = 	store->AttachmentManagerL();
	TInt mimefound = KErrNotFound;
	if(attachmentMgr.AttachmentCount())
		{
		// one attachment per attachment entry in email
		CMsvAttachment* attachmentInfo = attachmentMgr.GetAttachmentInfoL(0);
		CleanupStack::PushL(attachmentInfo);
		TPtrC8 bufferPtr;
		// check if attachment info had mimeheaders
			mimefound =  attachmentInfo->GetDesC8Attribute(KUidMimeHeaders, bufferPtr);
		
		// mimeheaders found
		if(mimefound == KErrNone)
			aMsvMimeHeaders.RestoreL(*attachmentInfo);
	
		CleanupStack::PopAndDestroy(attachmentInfo);	
		}
	CleanupStack::PopAndDestroy(store); 
	return mimefound;		
	}
	
CImMimeHeader* CImStoreMessagePart::ProcessAttachmentMimeHeadersL()
	{
	CImMimeHeader* mimeHeader = NULL;
	CMsvMimeHeaders* msvMimeHeaders = CMsvMimeHeaders::NewLC();
	TInt err = CheckMimeInAttachmentInfoL(*msvMimeHeaders);
	if(err == KErrNone)
		{
		// convert the CMsvMimeHeaders in attactment info to CImMimeHeader
		mimeHeader = iEmailMessage->ConvertToImMimeHeadersL(msvMimeHeaders);
		CleanupStack::PushL(mimeHeader);

		CMsvStore* store = iMsvEntry.EditStoreL();
		CleanupStack::PushL(store);
		mimeHeader->StoreL(*store);
		store->CommitL();				
		CleanupStack::PopAndDestroy(store);
		
		CleanupStack::Pop(mimeHeader);//mimeHeader
		}
	CleanupStack::PopAndDestroy(msvMimeHeaders);
	return mimeHeader;	
	}	

void CImStoreMessagePart::SetEntryDetailsL(TMsvEntry& aEntry)	
	{
	if(iAttachmentFullName)
		{
		// for create attachment name is set by user
		aEntry.iDetails.Set(iAttachmentFullName->Des());
		}
	else
		{
		// adding the attachment using file handle. , get the name from handle
		TFileName fileName;
		User::LeaveIfError(iFileHandle->Name(fileName));
		aEntry.iDetails.Set(fileName);
		}
	// get the size of the file from the handle	if add by file handle
	if(iAttachmentCreateState != ECreateAttachment)
		{
		iFileHandle->Size(iAttachmentSize);
		aEntry.iSize = iAttachmentSize;		
		}
	}		


void CImStoreMessagePart::CopyOrigMessageToMessageL()
	{
	TMsvId serviceId;
	TMsvEmailEntry entry;
	iMsvEntry.Session().GetEntry(iAttachmentMessageId, serviceId, entry);
	iMsvEntry.SetEntryL(entry.Parent());

	delete iMsvOperation;
	iMsvOperation = NULL;
	
	if (MultipartMixedCreated() || MultipartMixedExists())
		iMsvOperation = iMsvEntry.CopyL(iAttachmentMessageId, iMixFolderId, iStatus);
	else 
		iMsvOperation = iMsvEntry.CopyL(iAttachmentMessageId, iMessageId, iStatus);
	}

void CImStoreMessagePart::CreateMultipartAlternativeFolderEntryL()
	{
	TMsvEmailEntry entry;
	entry.iType = KUidMsvFolderEntry;
	entry.iMtm = iMsvEntry.Entry().iMtm;
	entry.iServiceId = iMsvEntry.Entry().iServiceId;
	entry.SetMessageFolderType(EFolderTypeAlternative);
	entry.iSize = 0;

	delete iMsvOperation;
	iMsvOperation=NULL;
	iMsvOperation=iMsvEntry.CreateL(entry, iStatus);
	}

void CImStoreMessagePart::CreateTextEntryL()
	{
	// create the index entry
	TMsvEntry entry;
	entry.iMtm = iMsvEntry.Entry().iMtm;
	entry.iServiceId = iMsvEntry.Entry().iServiceId;
	entry.iType = KUidMsvEmailTextEntry;
	entry.iDate.UniversalTime();
	entry.iSize = 0;

	delete iMsvOperation;
	iMsvOperation = NULL;
	iMsvOperation = iMsvEntry.CreateL(entry, iStatus);
	}

void CImStoreMessagePart::StoreRichTextL()
	{
	if(iUsePlainTextStorage)
		{
		StorePlainTextL();
		return;
		}
	
	iMsvEntry.SetEntryL(iTextId);	//pointing to the text entry
	CMsvStore* store = iMsvEntry.EditStoreL();
	CleanupStack::PushL(store);
	
	store->StoreBodyTextL(*iRichTextToStore);
	store->CommitL();
	
	// Store Mime header if we've got one
	if ( iMimeHeaderOfBodyText )
		{
		iMimeHeaderOfBodyText->StoreL(*store);
		store->CommitL();
		iMimeHeaderOfBodyText = NULL;	// We never really took ownership of the object
		}
	
	CleanupStack::PopAndDestroy(store);

	// update size of text entry
	TMsvEmailEntry entry(iMsvEntry.Entry());
	// DocumentLength() gives no. of chars and since unicode we multiply by 2
	entry.iSize = (iRichTextToStore->DocumentLength() * 2); 
	
	entry.iDate.UniversalTime();
	
	delete iMsvOperation;
	iMsvOperation = NULL;
	iMsvOperation = iMsvEntry.ChangeL(entry, iStatus);	
	}


void CImStoreMessagePart::StorePlainTextL()
	{
	TUint charset = 0;
	TBool override = EFalse;

	iEmailMessage->GetCharacterSetL(iMessageId,charset,override);
	iMsvEntry.SetEntryL(iTextId);	//pointing to the text entry
	CMsvStore* store = iMsvEntry.EditStoreL();
	CleanupStack::PushL(store);
	
	iSizeOfBody = 0;
	// If this method was called as a result of a call to CImEmailMessage::StoreBodyTextL
	if(iRichTextToStore)
		{
		RFs& fileSvrSession = iMsvSession.FileSession();
		
		CCnvCharacterSetConverter* characterConverter = CCnvCharacterSetConverter::NewL();
		CleanupStack::PushL(characterConverter);
		
		CImConvertCharconv* charConv = CImConvertCharconv::NewL(*characterConverter, fileSvrSession);
		CleanupStack::PushL(charConv);
		
		TUint defaultCharset =	charConv->SystemDefaultCharset();
	
		CleanupStack::PopAndDestroy(2,characterConverter);
		characterConverter = NULL;
		charConv = NULL;
		
		// Body text is stored in MailStore as 16 bit so set iIs8Bit to EFalse.		
		CMsvPlainBodyText* text = store->InitialisePlainBodyTextForWriteL(EFalse,charset,defaultCharset);	
		CleanupStack::PushL(text);
		
		text->StoreRichTextAsPlainTextL(*iRichTextToStore);
		text->CommitL();
		
		CleanupStack::PopAndDestroy(text);	
		}
	else // If this method was called as a result to a call to CImPlainBodyText::CommitL
		{
		// Delete the store since for Read operation we need to open in Read mode.
		CleanupStack::PopAndDestroy(store);
		store = NULL;
		
		store = iMsvEntry.ReadStoreL();
		CleanupStack::PushL(store);	
		// The plain bodytext storeoperation is completed when CImPlainBodyText::CommitL is called
		// Find out the length of the body text here.
		CMsvPlainBodyText* bodyText = store->InitialisePlainBodyTextForReadL(KMaxChunkLength);	
		iSizeOfBody = bodyText->Size();
		delete bodyText;
			
		CleanupStack::PopAndDestroy(store);
		store = NULL;
		// Open it in Edit mode so that store CommitL do not fail.
		store = iMsvEntry.EditStoreL();
		CleanupStack::PushL(store);	
		}
	
	// Store Mime header if we've got one
	if ( iMimeHeaderOfBodyText )
		{
		iMimeHeaderOfBodyText->StoreL(*store);
		store->CommitL();
		iMimeHeaderOfBodyText = NULL;	// We never really took ownership of the object
		}
	
	CleanupStack::PopAndDestroy(store);

	// update size of text entry
	TMsvEmailEntry entry(iMsvEntry.Entry());
	
	// DocumentLength() gives no. of chars and since unicode we multiply by 2
	if(iRichTextToStore)
		{
		entry.iSize = (iRichTextToStore->DocumentLength() * 2); 
		iSizeOfBody = entry.iSize;
		}
	else
		{
		entry.iSize = iSizeOfBody;	
		}
		
	entry.iDate.UniversalTime();
	
	delete iMsvOperation;
	iMsvOperation = NULL;
	iMsvOperation = iMsvEntry.ChangeL(entry, iStatus);	
	}
	
	
void CImStoreMessagePart::CreateHTMLEntryL()
	{
	iMsvEntry.SetEntryL(iAltFolderId);

	TMsvEntry entry;
	entry.iServiceId = iMsvEntry.Entry().iServiceId;
	entry.iType = KUidMsvEmailHtmlEntry;
	entry.iMtm = iMsvEntry.Entry().iMtm;
	entry.iDate.UniversalTime();
	entry.iSize = 0;
	
	delete iMsvOperation;
	iMsvOperation = NULL;
	iMsvOperation = iMsvEntry.CreateL(entry, iStatus);
	}

void CImStoreMessagePart::RemoveHTMLEntryL()
	{
	// Find the id for the HTML Entry - current entry is the text entry. Set 
	// entry to the parent and verify that this is an EFolderTypeAlternative.
	// Then search its children for an HTML entry.
	iHtmlId = KMsvNullIndexEntryId;
	iMsvEntry.SetEntryL(iMsvEntry.Entry().Parent());

	if( ((TMsvEmailEntry) iMsvEntry.Entry()).MessageFolderType() != EFolderTypeAlternative )
		{
		// Self-complete to continue with state machine.
		RequestComplete(KErrNone);
		return;
		}

	TInt ii = iMsvEntry.Count();
	while( ii-- )
		{
		if( iMsvEntry[ii].iType == KUidMsvEmailHtmlEntry )
			{
			iHtmlId = iMsvEntry[ii].Id();
			break;
			}
		}

	if( iHtmlId == KMsvNullIndexEntryId )
		{
		// There is no HTML entry - self-complete to continue with state machine.
		RequestComplete(KErrNone);
		return;
		}

	// Delete the HTML entry asynchronously.
	delete iMsvOperation;
	iMsvOperation = NULL;
	iMsvOperation = iMsvEntry.DeleteL(iHtmlId, iStatus);
	}

void CImStoreMessagePart::MoveTextEntryToAltFolderL()
	{
	// moves the text entry to alternativelder entry
	TMsvId serviceId = KMsvNullIndexEntryId;
	TMsvEmailEntry entry;
	iMsvEntry.Session().GetEntry(iTextId, serviceId, entry);
	iMsvEntry.SetEntryL(entry.Parent());
	
	delete iMsvOperation;
	iMsvOperation = NULL;
	iMsvOperation = iMsvEntry.MoveL(iTextId, iAltFolderId, iStatus);
	}

void CImStoreMessagePart::MoveOriginalMessageEntryChildrenToNewFolderL()
	{
	if (MultipartRelatedCreated())
		{
		// set entry to the multipart/related folders parent and copy the multipart alternative
		// into the multipart/related.
		TMsvId serviceId = KMsvNullIndexEntryId;
		TMsvEmailEntry entry;
		iMsvEntry.Session().GetEntry(iRelFolderId, serviceId, entry);
		iMsvEntry.SetEntryL(entry.Parent());
		delete iMsvOperation;
		iMsvOperation = NULL;
		iMsvOperation = iMsvEntry.MoveL(iAltFolderId, iRelFolderId, iStatus);
		}
	else //if MultipartMixedCreated()
		{
		iMsvEntry.SetEntryL(iMessageId);
		delete iMsvOperation;
		iMsvOperation = NULL;
		iMsvOperation = iMsvEntry.MoveL(*iMessageEntrySelection, iMixFolderId, iStatus);
		}
	}

void CImStoreMessagePart::CompleteStoreMessagePartL()
	{
	if (iMsvEntry.Entry().iType != KUidMsvMessageEntry)
		iMsvEntry.SetEntryL(iMessageId);	//pointing to the message
	
	TMsvEmailEntry entry(iMsvEntry.Entry());

	if (iMessagePart == EMessagePartAttachment)
		{
		entry.iSize += iAttachmentSize;
		entry.SetAttachment(ETrue);
		}
	else if (iMessagePart == EMessagePartMessageAttachment)
		{
		TMsvId serviceId;
		TMsvEmailEntry attachmentMessageEntry;
		iMsvEntry.Session().GetEntry(iAttachmentMessageId, serviceId, attachmentMessageEntry);
		entry.SetAttachment(ETrue);
		entry.iSize += attachmentMessageEntry.iSize;
		}
	else if (iMessagePart == EMessagePartBody)
		{
		if(iUsePlainTextStorage)
			{
			entry.iSize -= iSizeFwdReplyBody; // first subtract length of original plain text.
			entry.iSize += iSizeOfBody;
			}
		else
			{
			// DocumentLength() gives no. of chars and since unicode we multiply by 2
			entry.iSize -= (iRichText->DocumentLength() * 2); // first subtract length of original RichText
			entry.iSize += (iRichTextToStore->DocumentLength() * 2);
			}
		
		
		}
	if (iHtmlConverter)
		entry.iSize+=iHtmlConverter->Size();

	entry.iDate.UniversalTime();

	StoreMimeHeaderL();

	delete iMsvOperation;
	iMsvOperation = NULL;
	entry.SetInPreparation(EFalse);
	iMsvOperation = iMsvEntry.ChangeL(entry, iStatus);
	}

void CImStoreMessagePart::StoreMimeHeaderL()
	{
	CImMimeHeader* mimeHeader = NULL;
	
	if (MultipartMixedCreated() || MultipartAlternativeCreated() || MultipartRelatedCreated())
		{
		CMsvStore* store = iMsvEntry.EditStoreL();
		CleanupStack::PushL(store);

		mimeHeader = CImMimeHeader::NewLC();
		CreateFolderMimeHeaderL(*mimeHeader);
		mimeHeader->StoreL(*store);
		store->CommitL();		
		CleanupStack::PopAndDestroy(2); //mimeHeader, store
		}
	
	if (iMessagePart==EMessagePartAttachment && iAttachmentId)
		{
		TMsvId oldId = iMsvEntry.Entry().Id();
		iMsvEntry.SetEntryL(iAttachmentId);
		mimeHeader = NULL;
		mimeHeader = ProcessAttachmentMimeHeadersL();
		// mimeheader not found in attachment info, hence create mime header
		if (mimeHeader == NULL)
			{
			mimeHeader = CImMimeHeader::NewLC();
			if (CreateAttachmentMimeHeaderL(*mimeHeader, iMsvEntry.Entry().iDetails))
				{
				CMsvStore* store = iMsvEntry.EditStoreL();
				CleanupStack::PushL(store);
				mimeHeader->StoreL(*store);
				store->CommitL();			
				CleanupStack::PopAndDestroy(store);
				}
			CleanupStack::PopAndDestroy(mimeHeader);
			}
		else
			{
			delete mimeHeader;
			}
		iMsvEntry.SetEntryL(oldId); // Reset
		}
	else if (iMessagePart==EMessagePartMessageAttachment && iAttachmentMessageId)
		{
		CMsvStore* store = iMsvEntry.EditStoreL();
		CleanupStack::PushL(store);
		mimeHeader = CImMimeHeader::NewLC();
	
		CreateMessageMimeHeaderL(*mimeHeader);
		mimeHeader->StoreL(*store);
		store->CommitL();
		CleanupStack::PopAndDestroy(2); //mimeHeader, store
		}
	}

void CImStoreMessagePart::CreateFolderMimeHeaderL(CImMimeHeader& aMimeHeader)
	{
	aMimeHeader.SetContentTypeL(KMimeMultipart);
	if (MultipartMixedCreated())
		aMimeHeader.SetContentSubTypeL(KMimeMixed);
	else if (MultipartRelatedCreated())
		aMimeHeader.SetContentSubTypeL(KMimeRelated);
	else
		aMimeHeader.SetContentSubTypeL(KMimeAlternative);
	aMimeHeader.SetContentTransferEncodingL(KMimeQuotedPrintable);
	}
	
void CImStoreMessagePart::CreateMessageMimeHeaderL(CImMimeHeader& aMimeHeader)
	{
	aMimeHeader.SetContentTypeL(KMimeMessage);
	aMimeHeader.SetContentSubTypeL(KMimeRfc822);
	}

TBool CImStoreMessagePart::CreateAttachmentMimeHeaderL(CImMimeHeader& aMimeHeader, const TDesC& aDetails)
/**
Opens attachment file & extract initial text, pass into recognizer.
If the file type is recognized, use the MIME string produced in 
the MIMEheader object stored in the attachment entry.
*/
	{
	if (!aDetails.Length())
		return EFalse; // No file to check 
	
	RApaLsSession lsSession;
	if (lsSession.Connect() != KErrNone)
		{
		// Unable to connect to Recognizer server.
		return EFalse;
		}
	
	CleanupClosePushL(lsSession);
	TDataRecognitionResult result;
	User::LeaveIfError(lsSession.RecognizeData(aDetails, TPtrC8(), result));

	TPtrC8 mimeBuf8=result.iDataType.Des8();
	if (!mimeBuf8.Length())
		{
		// No point passing the file handle to the recogniser for Create Attachment 
		// as it will be zero length
		if(iAttachmentCreateState != ECreateAttachment)
			{
			CMsvStore* store = iMsvEntry.ReadStoreL();
			CleanupStack::PushL(store);
			MMsvAttachmentManager& attachmentMgr = store->AttachmentManagerL();
			// as this is email attachment entry , it has one attachment per entry
			RFile file = attachmentMgr.GetAttachmentFileL(0);
			CleanupStack::PopAndDestroy(store);
			CleanupClosePushL(file);

			// pass the handle to the recogniser instead of filepath
			User::LeaveIfError(lsSession.RecognizeData(file, result));
			CleanupStack::PopAndDestroy(&file);

	   		mimeBuf8.Set(result.iDataType.Des8());
			}
 				
		if (!mimeBuf8.Length())
			{
			CleanupStack::PopAndDestroy(); // lssession
			return EFalse;
			}
		}

	TInt locatePos = mimeBuf8.Locate(TChar('/'));
	
	if (locatePos != KErrNotFound)
		{
		aMimeHeader.SetContentTypeL(mimeBuf8.Left(locatePos));
		aMimeHeader.SetContentSubTypeL(mimeBuf8.Mid(locatePos + 1));
		}
		
	if (iRelatedPartId)
		aMimeHeader.SetContentIDL(*iContentId);
	
	CleanupStack::PopAndDestroy(); // lssession
	return ETrue; 
	}

//	---------------------------------------------------------------------
//	Class to create receipts, replies, forwarded emails, and new emails
//	---------------------------------------------------------------------

/** Creates a new email message.

@param aObserverRequestStatus Asynchronous status word to complete when the 
operation completes
@param aMsvSession Message server session to use
@param aDestination The Id of the folder where the new message is to be created
@param aPartList The body parts that are required in the new message. If a 
message with body text and attachments is required, then the KMsvMessagePartBody 
and KMsvMessagePartAttachments parts have to be set.
@param aMsvEmailTypeList Creation flags. This can be 0, or a bitmask of KMsvEmailTypeListMHTMLMessage, 
KMsvEmailTypeListInvisibleMessage, and KMsvEmailTypeListMessageInPreparation. 
If KMsvEmailTypeListMHTMLMessage is not set, a plain-text message is created.
@param aMsgType The type of message to create e.g. KUidMsgTypeSMTP.
@return Operation object by which to control the operation */
EXPORT_C CImEmailOperation* CImEmailOperation::CreateNewL(TRequestStatus& aObserverRequestStatus, CMsvSession& aMsvSession, TMsvId aDestination, TMsvPartList aPartList, const TMsvEmailTypeList& aMsvEmailTypeList, TUid aMsgType)
	{
	CImEmailOperation* self = new(ELeave) CImEmailOperation(aObserverRequestStatus, aMsvSession, aDestination, KMsvUnknownServiceIndexEntryId, aPartList, aMsvEmailTypeList, aMsgType, EPriorityStandard, EFalse);
	CleanupStack::PushL(self);
	self->ConstructL(ENew);
	CleanupStack::Pop(self);
	return self;
	}

/** Creates a new email message with a specified priority and SMTP service.

@param aObserverRequestStatus Asynchronous status word to complete when the 
operation completes
@param aMsvSession Message server session to use
@param aDestination The Id of the folder where the new message is to be created
@param aSmtpServiceId The Id of the SMTP service entry to handle the email
@param aPartList The body parts that are required in the new message. If a 
message with body text and attachments is required, then the KMsvMessagePartBody 
and KMsvMessagePartAttachments parts have to be set.
@param aMsvEmailTypeList Creation flags. This can be 0, or a bitmask of KMsvEmailTypeListMHTMLMessage, 
KMsvEmailTypeListInvisibleMessage, and KMsvEmailTypeListMessageInPreparation. 
If KMsvEmailTypeListMHTMLMessage is not set, a plain-text message is created.
@param aMsgType The type of message to create e.g. KUidMsgTypeSMTP.
@param aPriority The priority setting for the email
@return Operation object by which to control the operation */
EXPORT_C CImEmailOperation* CImEmailOperation::CreateNewL(TRequestStatus& aObserverRequestStatus, CMsvSession& aMsvSession, TMsvId aDestination, TMsvId aSmtpServiceId, TMsvPartList aPartList, const TMsvEmailTypeList& aMsvEmailTypeList, TUid aMsgType, TInt aPriority)
	{
	CImEmailOperation* self = new(ELeave) CImEmailOperation(aObserverRequestStatus, aMsvSession, aDestination, aSmtpServiceId, aPartList, aMsvEmailTypeList, aMsgType, aPriority, EFalse);
	CleanupStack::PushL(self);
	self->ConstructL(ENew);
	CleanupStack::Pop(self);
	return self;
	}


/** 
Creates a new plain text email message with a specified priority and SMTP service.

@param aObserverRequestStatus  	Asynchronous status word to complete when the 
								operation completes
@param aMsvSession 				Message server session to use
@param aDestination 			The Id of the folder where the new message is to be created
@param aSmtpServiceId 			The Id of the SMTP service entry to handle the email
@param aPartList 				The body parts that are required in the new message. If a 
								message with body text and attachments is required, then the KMsvMessagePartBody 
								and KMsvMessagePartAttachments parts have to be set.
@param aMsvEmailTypeList 		Creation flags. This can be 0, or a bitmask of KMsvEmailTypeListMHTMLMessage, 
								KMsvEmailTypeListInvisibleMessage, and KMsvEmailTypeListMessageInPreparation. 
								If KMsvEmailTypeListMHTMLMessage is not set, a plain-text message is created.
@param aMsgType 				The type of message to create e.g. KUidMsgTypeSMTP.
@param aPriority 				The priority setting for the email- by default this must be EPriorityStandard.
@param aUsePlainTextStorage		TBool, if set to ETrue inidcates that the new message entry needs to be created as plain text
								if set to EFalse indicates that message will be created as richtext entry.
@return CImEmailOperation		Operation object by which to control the operation 
*/
EXPORT_C CImEmailOperation* CImEmailOperation::CreateNewL(TRequestStatus& aObserverRequestStatus, CMsvSession& aMsvSession, TMsvId aDestination, TMsvId aSmtpServiceId, TMsvPartList aPartList, const TMsvEmailTypeList& aMsvEmailTypeList, TUid aMsgType, TInt aPriority, TBool aUsePlainTextStorage)
	{
	CImEmailOperation* self = new(ELeave) CImEmailOperation(aObserverRequestStatus, aMsvSession, aDestination, aSmtpServiceId, aPartList, aMsvEmailTypeList, aMsgType, aPriority, aUsePlainTextStorage);
	CleanupStack::PushL(self);
	self->ConstructL(ENew);
	CleanupStack::Pop(self);
	return self;
	}


/** Creates a reply email message, overriding the default subject format string.

The aFormatString parameter allows you to override the default string used 
in the subject field of the new message (the default is the localised string 
STRING_reply_formatting_string1 defined in the source file imcm.rls).

Note that if you reply to an HTML message that does not contain a text/plain 
alternative to the HTML, then the HTML part is copied as an attachment (still 
an HTML entry) into the new message even if aPartList does not specify KMsvMessagePartAttachments. 
This occurs because there is no other way of reading the original message 
unless the user switches between the editor/viewer and the application. 

@param aObserverRequestStatus Asynchronous status word to complete when the 
operation completes
@param aMsvSession Message server session to use
@param aMessageId The Id of the message to reply to
@param aDestination The Id of the folder where the new message is to be created
@param aPartList The body parts that are required in the new message. If a 
message with body text is required, then set KMsvMessagePartBody; if attachments 
are required too, also set KMsvMessagePartAttachments. To reply to the originator 
only, set KMsvMessagePartOriginator, otherwise a reply will be sent to all 
recipients of the original message. If the subject field is not required, 
then do not set KMsvMessagePartDescription.
@param aFormatString A string to be inserted into the subject field in the 
header before the subject, e.g. "Re: %S", sets the field to be "Re: " followed 
by the original subject text
@param aMsvEmailTypeList Creation flags. This can be 0, or a bitmask of KMsvEmailTypeListMHTMLMessage, 
KMsvEmailTypeListInvisibleMessage, and KMsvEmailTypeListMessageInPreparation. 
If KMsvEmailTypeListMHTMLMessage is not set, a plain-text message is created.
@param aMsgType The type of message to create e.g. KUidMsgTypeSMTP.
@return Operation object by which to control the operation */
EXPORT_C CImEmailOperation* CImEmailOperation::CreateReplyL(TRequestStatus& aObserverRequestStatus, CMsvSession& aMsvSession, TMsvId aMessageId, TMsvId aDestination, TMsvPartList aPartList, const TDesC& aFormatString, const TMsvEmailTypeList& aMsvEmailTypeList, TUid aMsgType)
	{
	CImEmailOperation* self = new(ELeave) CImEmailOperation(aObserverRequestStatus, aMsvSession, aDestination, KMsvUnknownServiceIndexEntryId, aPartList, aMsvEmailTypeList, aMsgType, EPriorityStandard, EFalse);
	CleanupStack::PushL(self);
	self->ConstructL(aMessageId, aFormatString, EReply);
	CleanupStack::Pop(self);
	return self;
	}

/** Creates a reply email message.

For details of how replies to HTML messages are handled, see the description 
above for the first overload of this function.

@param aObserverRequestStatus Asynchronous status word to complete when the 
operation completes
@param aMsvSession Message server session to use
@param aMessageId The Id of the message to reply to
@param aDestination The Id of the folder where the new message is to be created
@param aPartList The body parts that are required in the new message. If a 
message with body text and attachments is required, then the KMsvMessagePartBody 
and KMsvMessagePartAttachments parts have to be set.
@param aMsvEmailTypeList Creation flags. This can be 0, or a bitmask of KMsvEmailTypeListMHTMLMessage, 
KMsvEmailTypeListInvisibleMessage, and KMsvEmailTypeListMessageInPreparation. 
If KMsvEmailTypeListMHTMLMessage is not set, a plain-text message is created.
@param aMsgType The type of message to create e.g. KUidMsgTypeSMTP.
@return Operation object by which to control the operation */
EXPORT_C CImEmailOperation* CImEmailOperation::CreateReplyL(TRequestStatus& aObserverRequestStatus, CMsvSession& aMsvSession, TMsvId aMessageId, TMsvId aDestination, TMsvPartList aPartList, const TMsvEmailTypeList& aMsvEmailTypeList, TUid aMsgType)
	{
	CImEmailOperation* self = new(ELeave) CImEmailOperation(aObserverRequestStatus, aMsvSession, aDestination, KMsvUnknownServiceIndexEntryId, aPartList, aMsvEmailTypeList, aMsgType, EPriorityStandard, EFalse);
	CleanupStack::PushL(self);
	self->ConstructL(aMessageId, EReply);
	CleanupStack::Pop(self);
	return self;
	}

/** Creates a reply email message, specifying a message priority.

For details of how replies to HTML messages are handled, see the description 
above for the first overload of this function.

@param aObserverRequestStatus Asynchronous status word to complete when the 
operation completes
@param aMsvSession Message server session to use
@param aMessageId The Id of the message to reply to
@param aDestination The Id of the folder where the new message is to be created
@param aPartList The body parts that are required in the new message. If a 
message with body text and attachments is required, then the KMsvMessagePartBody 
and KMsvMessagePartAttachments parts have to be set.
@param aMsvEmailTypeList Creation flags. This can be 0, or a bitmask of KMsvEmailTypeListMHTMLMessage, 
KMsvEmailTypeListInvisibleMessage, and KMsvEmailTypeListMessageInPreparation. 
If KMsvEmailTypeListMHTMLMessage is not set, a plain-text message is created.
@param aMsgType The type of message to create e.g. KUidMsgTypeSMTP.
@param aPriority The priority setting for the email
@return Operation object by which to control the operation */
EXPORT_C CImEmailOperation* CImEmailOperation::CreateReplyL(TRequestStatus& aObserverRequestStatus, CMsvSession& aMsvSession, TMsvId aMessageId, TMsvId aDestination, TMsvPartList aPartList, const TMsvEmailTypeList& aMsvEmailTypeList, TUid aMsgType, TInt aPriority)
	{
	CImEmailOperation* self = new(ELeave) CImEmailOperation(aObserverRequestStatus, aMsvSession, aDestination, KMsvUnknownServiceIndexEntryId, aPartList, aMsvEmailTypeList, aMsgType, aPriority, EFalse);
	CleanupStack::PushL(self);
	self->ConstructL(aMessageId, EReply);
	CleanupStack::Pop(self);
	return self;
	}

/** Creates a reply email message, specifying a message priority.

For details of how replies to HTML messages are handled, see the description 
above for the first overload of this function.

@param aObserverRequestStatus Asynchronous status word to complete when the 
operation completes
@param aMsvSession Message server session to use
@param aMessageId The Id of the message to reply to
@param aDestination The Id of the folder where the new message is to be created
@param aPartList The body parts that are required in the new message. If a 
message with body text and attachments is required, then the KMsvMessagePartBody 
and KMsvMessagePartAttachments parts have to be set.
@param aMsvEmailTypeList Creation flags. This can be 0, or a bitmask of KMsvEmailTypeListMHTMLMessage, 
KMsvEmailTypeListInvisibleMessage, and KMsvEmailTypeListMessageInPreparation. 
If KMsvEmailTypeListMHTMLMessage is not set, a plain-text message is created.
@param aMsgType The type of message to create e.g. KUidMsgTypeSMTP.
@param aPriority The priority setting for the email
@param aUsePlainTextStorage		TBool, if set to ETrue inidcates that the new message entry needs to be created as plain text
								if set to EFalse indicates that message will be created as richtext entry.
@return Operation object by which to control the operation */
EXPORT_C CImEmailOperation* CImEmailOperation::CreateReplyL(TRequestStatus& aObserverRequestStatus, CMsvSession& aMsvSession, TMsvId aMessageId, TMsvId aDestination, TMsvPartList aPartList, const TMsvEmailTypeList& aMsvEmailTypeList, TUid aMsgType, TInt aPriority, TBool aUsePlainTextStorage)
	{
	CImEmailOperation* self = new(ELeave) CImEmailOperation(aObserverRequestStatus, aMsvSession, aDestination, KMsvUnknownServiceIndexEntryId, aPartList, aMsvEmailTypeList, aMsgType, aPriority, aUsePlainTextStorage);
	CleanupStack::PushL(self);
	self->ConstructL(aMessageId, EReply);
	CleanupStack::Pop(self);
	return self;
	}
	
/** Creates a forwarded email message, overriding the default subject format string.

The aFormatString parameter allows you to override the default string used 
in the subject field of the new message (the default is the localised string 
STRING_forward_formatting_string1 defined in the source file imcm.rls).

Note that if you forward an HTML message that does not contain a text/plain 
alternative to the HTML, then the HTML part is copied as an attachment (still 
an HTML entry) into the new message even if aPartList does not specify KMsvMessagePartAttachments. 
This occurs because there is no other way of reading the original message 
unless the user switches between the editor/viewer and the application. 

@param aObserverRequestStatus Asynchronous status word to complete when the 
operation completes
@param aMsvSession Message server session to use
@param aMessageId The Id of the message to forward
@param aDestination The Id of the folder where the new message is to be created
@param aPartList The body parts that are required in the new message. If a 
message with body text and attachments is required, then the KMsvMessagePartBody 
and KMsvMessagePartAttachments parts have to be set.
@param aFormatString A string to be inserted into the subject field in the 
header before the subject, e.g. "Fwd: %S", sets the field to be "Fwd: " followed 
by the original subject text
@param aMsvEmailTypeList Creation flags. This can be 0, or a bitmask of KMsvEmailTypeListMHTMLMessage, 
KMsvEmailTypeListInvisibleMessage, and KMsvEmailTypeListMessageInPreparation. 
If KMsvEmailTypeListMHTMLMessage is not set, a plain-text message is created.
@param aMsgType The type of message to create e.g. KUidMsgTypeSMTP.
@return Operation object by which to control the operation */
EXPORT_C CImEmailOperation* CImEmailOperation::CreateForwardL(TRequestStatus& aObserverRequestStatus, CMsvSession& aMsvSession, TMsvId aMessageId, TMsvId aDestination, TMsvPartList aPartList, const TDesC& aFormatString, const TMsvEmailTypeList& aMsvEmailTypeList, TUid aMsgType)
	{
	CImEmailOperation* self = new(ELeave) CImEmailOperation(aObserverRequestStatus, aMsvSession, aDestination, KMsvUnknownServiceIndexEntryId,  aPartList, aMsvEmailTypeList, aMsgType, EPriorityStandard, EFalse);
	CleanupStack::PushL(self);
	self->ConstructL(aMessageId, aFormatString, EForward);
	CleanupStack::Pop(self);
	return self;
	}

/** Creates a forwarded email message.

For details of how forwarding HTML messages is handled, see the description 
above for the first overload of this function.

@param aObserverRequestStatus Asynchronous status word to complete when the 
operation completes
@param aMsvSession Message server session to use
@param aMessageId The Id of the message to forward
@param aDestination The Id of the folder where the new message is to be created
@param aPartList The body parts that are required in the new message. If a 
message with body text and attachments is required, then the KMsvMessagePartBody 
and KMsvMessagePartAttachments parts have to be set.
@param aMsvEmailTypeList Creation flags. This can be 0, or a bitmask of KMsvEmailTypeListMHTMLMessage, 
KMsvEmailTypeListInvisibleMessage, and KMsvEmailTypeListMessageInPreparation. 
If KMsvEmailTypeListMHTMLMessage is not set, a plain-text message is created.
@param aMsgType The type of message to create e.g. KUidMsgTypeSMTP.
@return Operation object by which to control the operation */
EXPORT_C CImEmailOperation* CImEmailOperation::CreateForwardL(TRequestStatus& aObserverRequestStatus, CMsvSession& aMsvSession, TMsvId aMessageId, TMsvId aDestination, TMsvPartList aPartList, const TMsvEmailTypeList& aMsvEmailTypeList, TUid aMsgType)
	{
	CImEmailOperation* self = new(ELeave) CImEmailOperation(aObserverRequestStatus, aMsvSession, aDestination, KMsvUnknownServiceIndexEntryId,  aPartList, aMsvEmailTypeList, aMsgType, EPriorityStandard, EFalse);
	CleanupStack::PushL(self);
	self->ConstructL(aMessageId, EForward);
	CleanupStack::Pop(self);
	return self;
	}

/** Creates a forwarded email message, specifying the message priority and SMTP 
service with which to send the message.

For details of how forwarding HTML messages is handled, see the description 
above for the first overload of this function.

@param aObserverRequestStatus Asynchronous status word to complete when the 
operation completes
@param aMsvSession Message server session to use
@param aMessageId The Id of the message to forward
@param aDestination The Id of the folder where the new message is to be created
@param aSmtpServiceId The Id of the SMTP service with which to send the new 
message
@param aPartList The body parts that are required in the new message. If a 
message with body text and attachments is required, then the KMsvMessagePartBody 
and KMsvMessagePartAttachments parts have to be set.
@param aMsvEmailTypeList Creation flags. This can be 0, or a bitmask of KMsvEmailTypeListMHTMLMessage, 
KMsvEmailTypeListInvisibleMessage, and KMsvEmailTypeListMessageInPreparation. 
If KMsvEmailTypeListMHTMLMessage is not set, a plain-text message is created.
@param aMsgType The type of message to create e.g. KUidMsgTypeSMTP.
@param aPriority The priority setting for the email
@return Operation object by which to control the operation */
EXPORT_C CImEmailOperation* CImEmailOperation::CreateForwardL(TRequestStatus& aObserverRequestStatus, CMsvSession& aMsvSession, TMsvId aMessageId, TMsvId aDestination, TMsvId aSmtpServiceId, TMsvPartList aPartList, const TMsvEmailTypeList& aMsvEmailTypeList, TUid aMsgType, TInt aPriority)
	{
	CImEmailOperation* self = new(ELeave) CImEmailOperation(aObserverRequestStatus, aMsvSession, aDestination, aSmtpServiceId, aPartList, aMsvEmailTypeList, aMsgType, aPriority, EFalse);
	CleanupStack::PushL(self);
	self->ConstructL(aMessageId, EForward);
	CleanupStack::Pop(self);
	return self;
	}
	
/** Creates a forwarded email message, specifying the message priority and SMTP 
service with which to send the message.

For details of how forwarding HTML messages is handled, see the description 
above for the first overload of this function.

@param aObserverRequestStatus Asynchronous status word to complete when the 
operation completes
@param aMsvSession Message server session to use
@param aMessageId The Id of the message to forward
@param aDestination The Id of the folder where the new message is to be created
@param aSmtpServiceId The Id of the SMTP service with which to send the new 
message
@param aPartList The body parts that are required in the new message. If a 
message with body text and attachments is required, then the KMsvMessagePartBody 
and KMsvMessagePartAttachments parts have to be set.
@param aMsvEmailTypeList Creation flags. This can be 0, or a bitmask of KMsvEmailTypeListMHTMLMessage, 
KMsvEmailTypeListInvisibleMessage, and KMsvEmailTypeListMessageInPreparation. 
If KMsvEmailTypeListMHTMLMessage is not set, a plain-text message is created.
@param aMsgType The type of message to create e.g. KUidMsgTypeSMTP.
@param aPriority The priority setting for the email
@param aUsePlainTextStorage		TBool, if set to ETrue inidcates that the new message entry needs to be created as plain text
								if set to EFalse indicates that message will be created as richtext entry.
@return Operation object by which to control the operation */
EXPORT_C CImEmailOperation* CImEmailOperation::CreateForwardL(TRequestStatus& aObserverRequestStatus, CMsvSession& aMsvSession, TMsvId aMessageId, TMsvId aDestination, TMsvId aSmtpServiceId, TMsvPartList aPartList, const TMsvEmailTypeList& aMsvEmailTypeList, TUid aMsgType, TInt aPriority, TBool aUsePlainTextStorage)
	{
	CImEmailOperation* self = new(ELeave) CImEmailOperation(aObserverRequestStatus, aMsvSession, aDestination, aSmtpServiceId, aPartList, aMsvEmailTypeList, aMsgType, aPriority, aUsePlainTextStorage);
	CleanupStack::PushL(self);
	self->ConstructL(aMessageId, EForward);
	CleanupStack::Pop(self);
	return self;
	}

/** Creates a forwarded email message.

@param aObserverRequestStatus Asynchronous status word to complete when the 
operation completes
@param aMsvSession Message server session to use
@param aMessageId The Id of the message to forward
@param aDestination The Id of the folder where the new message is to be created
@param aPartList The body parts that are required in the new message. If a 
message with body text and attachments is required, then the KMsvMessagePartBody 
and KMsvMessagePartAttachments parts have to be set.
@param aMsvEmailTypeList Creation flags. This can be 0, or a bitmask of KMsvEmailTypeListMHTMLMessage, 
KMsvEmailTypeListInvisibleMessage, and KMsvEmailTypeListMessageInPreparation. 
If KMsvEmailTypeListMHTMLMessage is not set, a plain-text message is created.
@param aMsgType The type of message to create e.g. KUidMsgTypeSMTP.
@return Operation object by which to control the operation */
EXPORT_C CImEmailOperation* CImEmailOperation::CreateForwardAsAttachmentL(TRequestStatus& aObserverRequestStatus, CMsvSession& aMsvSession, TMsvId aMessageId, TMsvId aDestination, TMsvPartList aPartList, const TMsvEmailTypeList& aMsvEmailTypeList, TUid aMsgType)
	{
	CImEmailOperation* self = new(ELeave) CImEmailOperation(aObserverRequestStatus, aMsvSession, aDestination, KMsvUnknownServiceIndexEntryId, aPartList, aMsvEmailTypeList, aMsgType, EPriorityStandard, EFalse);
	CleanupStack::PushL(self);
	self->ConstructL(aMessageId, EForwardAsAttachment);
	CleanupStack::Pop(self);
	return self;
	}

/** Creates a forwarded email message, specifying the message priority and SMTP 
service with which to send the message.

@param aObserverRequestStatus Asynchronous status word to complete when the 
operation completes
@param aMsvSession Message server session to use
@param aMessageId The Id of the message to forward
@param aSmtpServiceId The Id of the SMTP service with which to send the new 
message
@param aDestination The Id of the folder where the new message is to be created
@param aPartList The body parts that are required in the new message. If a 
message with body text and attachments is required, then the KMsvMessagePartBody 
and KMsvMessagePartAttachments parts have to be set.
@param aMsvEmailTypeList Creation flags. This can be 0, or a bitmask of KMsvEmailTypeListMHTMLMessage, 
KMsvEmailTypeListInvisibleMessage, and KMsvEmailTypeListMessageInPreparation. 
If KMsvEmailTypeListMHTMLMessage is not set, a plain-text message is created.
@param aMsgType The type of message to create e.g. KUidMsgTypeSMTP.
@param aPriority The priority setting for the email
@return Operation object by which to control the operation */
EXPORT_C CImEmailOperation* CImEmailOperation::CreateForwardAsAttachmentL(TRequestStatus& aObserverRequestStatus, CMsvSession& aMsvSession, TMsvId aMessageId, TMsvId aDestination, TMsvId aSmtpServiceId, TMsvPartList aPartList, const TMsvEmailTypeList& aMsvEmailTypeList, TUid aMsgType, TInt aPriority)
	{
	CImEmailOperation* self = new(ELeave) CImEmailOperation(aObserverRequestStatus, aMsvSession, aDestination, aSmtpServiceId, aPartList, aMsvEmailTypeList, aMsgType, aPriority, EFalse);
	CleanupStack::PushL(self);
	self->ConstructL(aMessageId, EForwardAsAttachment);
	CleanupStack::Pop(self);
	return self;
	}

/** Creates a email receipt message, overriding the default subject format string.

The aFormatString parameter allows you to override the default string used 
in the subject field of the new message (the default is the localised string 
STRING_receipt_formatting_string1 defined in the source file imcm.rls).

@param aObserverRequestStatus Asynchronous status word to complete when the 
operation completes
@param aMsvSession Message server session to use
@param aMessageId The Id of the original message for which the receipt is required
@param aDestination The Id of the folder where the new message is to be created
@param aPartList The body parts that are required in the new message. If a 
message with body text and attachments is required, then the KMsvMessagePartBody 
and KMsvMessagePartAttachments parts have to be set.
@param aFormatString A string to be inserted into the subject field in the 
header before the subject, e.g. "Receipt of message: %S", sets the field to 
be "Receipt of message: " followed by the original subject text
@param aMsvEmailTypeList Creation flags. This can be 0, or a bitmask of KMsvEmailTypeListMHTMLMessage, 
KMsvEmailTypeListInvisibleMessage, and KMsvEmailTypeListMessageInPreparation. 
If KMsvEmailTypeListMHTMLMessage is not set, a plain-text message is created.
@param aMsgType The type of message to create e.g. KUidMsgTypeSMTP.
@return Operation object by which to control the operation */
EXPORT_C CImEmailOperation* CImEmailOperation::CreateReceiptL(TRequestStatus& aObserverRequestStatus, CMsvSession& aMsvSession, TMsvId aMessageId, TMsvId aDestination, TMsvPartList aPartList, const TDesC& aFormatString, const TMsvEmailTypeList& aMsvEmailTypeList, TUid aMsgType)
	{
	CImEmailOperation* self = new(ELeave) CImEmailOperation(aObserverRequestStatus, aMsvSession, aDestination,KMsvUnknownServiceIndexEntryId, aPartList, aMsvEmailTypeList, aMsgType, EPriorityStandard, EFalse);
	CleanupStack::PushL(self);
	self->ConstructL(aMessageId, aFormatString, EReceipt);
	CleanupStack::Pop(self);
	return self;
	}

/** Creates a email receipt message.

@param aObserverRequestStatus Asynchronous status word to complete when the 
operation completes
@param aMsvSession Message server session to use
@param aMessageId The Id of the original message for which the receipt is required
@param aDestination The Id of the folder where the new message is to be created
@param aPartList The body parts that are required in the new message. If a 
message with body text and attachments is required, then the KMsvMessagePartBody 
and KMsvMessagePartAttachments parts have to be set.
@param aMsvEmailTypeList Creation flags. This can be 0, or a bitmask of KMsvEmailTypeListMHTMLMessage, 
KMsvEmailTypeListInvisibleMessage, and KMsvEmailTypeListMessageInPreparation. 
If KMsvEmailTypeListMHTMLMessage is not set, a plain-text message is created.
@param aMsgType The type of message to create e.g. KUidMsgTypeSMTP.
@return Operation object by which to control the operation */
EXPORT_C CImEmailOperation* CImEmailOperation::CreateReceiptL(TRequestStatus& aObserverRequestStatus, CMsvSession& aMsvSession, TMsvId aMessageId, TMsvId aDestination, TMsvPartList aPartList, const TMsvEmailTypeList& aMsvEmailTypeList, TUid aMsgType)
	{
	CImEmailOperation* self = new(ELeave) CImEmailOperation(aObserverRequestStatus, aMsvSession, aDestination, KMsvUnknownServiceIndexEntryId, aPartList, aMsvEmailTypeList, aMsgType, EPriorityStandard, EFalse);
	CleanupStack::PushL(self);
	self->ConstructL(aMessageId, EReceipt);
	CleanupStack::Pop(self);
	return self;
	}

/** Creates a email receipt message, specifying the message priority.

@param aObserverRequestStatus Asynchronous status word to complete when the 
operation completes
@param aMsvSession Message server session to use
@param aMessageId The Id of the original message for which the receipt is required
@param aDestination The Id of the folder where the new message is to be created
@param aPartList The body parts that are required in the new message. If a 
message with body text and attachments is required, then the KMsvMessagePartBody 
and KMsvMessagePartAttachments parts have to be set.
@param aMsvEmailTypeList Creation flags. This can be 0, or a bitmask of KMsvEmailTypeListMHTMLMessage, 
KMsvEmailTypeListInvisibleMessage, and KMsvEmailTypeListMessageInPreparation. 
If KMsvEmailTypeListMHTMLMessage is not set, a plain-text message is created.
@param aMsgType The type of message to create e.g. KUidMsgTypeSMTP.
@param aPriority The priority setting for the email
@return Operation object by which to control the operation */
EXPORT_C CImEmailOperation* CImEmailOperation::CreateReceiptL(TRequestStatus& aObserverRequestStatus, CMsvSession& aMsvSession, TMsvId aMessageId, TMsvId aDestination, TMsvPartList aPartList, const TMsvEmailTypeList& aMsvEmailTypeList, TUid aMsgType, TInt aPriority)
	{
	CImEmailOperation* self = new(ELeave) CImEmailOperation(aObserverRequestStatus, aMsvSession, aDestination, KMsvUnknownServiceIndexEntryId, aPartList, aMsvEmailTypeList, aMsgType, aPriority, EFalse);
	CleanupStack::PushL(self);
	self->ConstructL(aMessageId, EReceipt);
	CleanupStack::Pop(self);
	return self;
	}

/** Creates a email receipt message, specifying the message priority.

@param aObserverRequestStatus Asynchronous status word to complete when the 
operation completes
@param aMsvSession Message server session to use
@param aMessageId The Id of the original message for which the receipt is required
@param aDestination The Id of the folder where the new message is to be created
@param aPartList The body parts that are required in the new message. If a 
message with body text and attachments is required, then the KMsvMessagePartBody 
and KMsvMessagePartAttachments parts have to be set.
@param aMsvEmailTypeList Creation flags. This can be 0, or a bitmask of KMsvEmailTypeListMHTMLMessage, 
KMsvEmailTypeListInvisibleMessage, and KMsvEmailTypeListMessageInPreparation. 
If KMsvEmailTypeListMHTMLMessage is not set, a plain-text message is created.
@param aMsgType The type of message to create e.g. KUidMsgTypeSMTP.
@param aPriority The priority setting for the email
@param aUsePlainTextStorage		TBool, if set to ETrue inidcates that the new message entry needs to be created as plain text
								if set to EFalse indicates that message will be created as richtext entry.
@return Operation object by which to control the operation */
EXPORT_C CImEmailOperation* CImEmailOperation::CreateReceiptL(TRequestStatus& aObserverRequestStatus, CMsvSession& aMsvSession, TMsvId aMessageId, TMsvId aDestination, TMsvPartList aPartList, const TMsvEmailTypeList& aMsvEmailTypeList, TUid aMsgType, TInt aPriority, TBool aUsePlainTextStorage)
	{
	CImEmailOperation* self = new(ELeave) CImEmailOperation(aObserverRequestStatus, aMsvSession, aDestination, KMsvUnknownServiceIndexEntryId, aPartList, aMsvEmailTypeList, aMsgType, aPriority, aUsePlainTextStorage);
	CleanupStack::PushL(self);
	self->ConstructL(aMessageId, EReceipt);
	CleanupStack::Pop(self);
	return self;
	}
	
CImEmailOperation* CImEmailOperation::CreateCopyL(TRequestStatus& aObserverRequestStatus, CMsvSession& aMsvSession, TMsvId aMessageId, TMsvId aDestination, TMsvPartList aPartList, const TMsvEmailTypeList& aMsvEmailTypeList, TUid aMsgType)
	{
	CImEmailOperation* self = new(ELeave) CImEmailOperation(aObserverRequestStatus, aMsvSession, aDestination, KMsvUnknownServiceIndexEntryId,  aPartList, aMsvEmailTypeList, aMsgType, EPriorityStandard, EFalse);
	CleanupStack::PushL(self);
	self->ConstructL(aMessageId, ECopy);
	CleanupStack::Pop(self);
	return self;
	}


/** Destructor. */
EXPORT_C CImEmailOperation::~CImEmailOperation()
	{
	Cancel();
	delete iMsvOperation;
	delete iFormatString;
	delete iRichText;
	delete iParaLayer;
	delete iCharLayer;
	delete iEmailMessage;
	delete iNewHeader;
	delete iFileMan;
	delete iBodyHeaderFormatString;
	delete iBodyHeaderDateTimeFormatString;
	delete iBodyHeader;
	iAttachmentInfoList.ResetAndDestroy();
	iFile.Close();
	delete iStore;
	delete iMsvEntry;
	delete iSignatureText;
	delete iDefaultVCardNameFormatString;
	delete iHtmlConverter;
	delete iSmtpSettings;
	delete iVcardStore;
	delete iBodyHeaderToString;
	delete iBodyHeaderCcString;
	iOriginalHeader = NULL;
	delete iOriginalHeader;
	}

/** Gets progress information for a completed operation.

The function returns (in packaged form):

for a successfully completed operation, the Id of the new message 

if there was an error/problem while creating the message, a null Id (KMsvNullIndexEntryId). 
The new message will also be deleted.

@return A message Id as a TPckg<TMsvId> */
EXPORT_C const TDesC8& CImEmailOperation::FinalProgress()
	{
	__ASSERT_ALWAYS(!IsActive(), gPanic(EMiutActiveInFinalProgress));
 	const TDesC8* progress = &KNullDesC8();
 	TRAPD(leave, progress = &ProgressL());
	__ASSERT_ALWAYS(leave == KErrNone, gPanic(EImcmFinalProgressFailed));
	return *progress;	
	}

/** Gets progress information.

While the operation is in progress, a null Id (KMsvNullIndexEntryId) is returned. 
For a completed operation, values are as described for FinalProgress().

@return A message Id as a TPckg<TMsvId> */
const TDesC8& CImEmailOperation::ProgressL()
	{
	if (iState==EFinished)
		iDataMember() = iNewMessageId;
	return iDataMember;
	}

void CImEmailOperation::DoCancel()
	{
	if (iMsvOperation)
		iMsvOperation->Cancel();
	if (iEmailMessage)
		iEmailMessage->Cancel();
	if (iNewMessageId!=KMsvNullIndexEntryId)
		iMsvEntry->Session().RemoveEntry(iNewMessageId);

	TRequestStatus* st = &iObserverRequestStatus;
	User::RequestComplete(st, KErrCancel);
	}

void CImEmailOperation::SelectAndProcessNextStateL()
	{
	SelectNextStateL();
	ProcessStateL();
	}

void CImEmailOperation::RunL()
	{
	if (iStatus.Int() != KErrNone)
		{
		ErrorRecovery(iStatus.Int());
		return;
		}
	if ((iState == ECreateNewMessageEntry) || 
		(iState == ECreateMultipartMixedFolderEntry) ||
		(iState == ECreateTextEntry) ||
		(iState == EStoreBody) ||
		(iState == ECreateAttachmentEntry) || 
		(iState == ECreateMultipartAlternativeFolderEntry) ||
		(iState == ECreateHTMLEntry) ||
		(iState == EStoreAttachment) ||
		(iState == ECreateVCardAttachment) ||
		(iState == ECompleteEmailOperation)
		|| (iState == ECreateDefaultHtmlAttachment)	
		|| (iState == EStoreHTMLTextInEntry)
		) 
		{
		TInt progressError = McliUtils::GetProgressErrorL(*iMsvOperation);
		if (progressError != KErrNone)
			{
			ErrorRecovery(progressError);
			return;
			}			
		}
	if (iState != EFinished)
		{
		TRAPD(error, SelectAndProcessNextStateL());
		if (error)
			{
			ErrorRecovery(error);
			return;
			}
		else if (iState != EFinished)
			SetActive();
		}
	}

CImEmailOperation::CImEmailOperation(TRequestStatus& aObserverRequestStatus, CMsvSession& aMsvSession, TMsvId aDestination, TMsvId aSmtpServiceId, TMsvPartList aPartList, const TMsvEmailTypeList& aMsvEmailTypeList, TUid aMsgType, TInt aPriority, TBool aUsePlainTextStorage)
	: CMsvOperation(aMsvSession, aPriority, aObserverRequestStatus),
	  iDestinationId(aDestination),
	  iSmtpServiceId(aSmtpServiceId),
	  iMsgType(aMsgType),
	  iPartList(aPartList),
	  iMsvEmailTypeList(aMsvEmailTypeList),
	  iAttachmentFile(aMsvSession.FileSession()),
	  iUsePlainTextStorage(aUsePlainTextStorage)
	{
	iMtm = iMsgType;
	}

void CImEmailOperation::ConstructL(TMsvId aMessageId, const TDesC& aFormatString, TImEmailOperation aOperation)
	{
    iOrigMessageId = aMessageId;
	iFormatString = aFormatString.AllocL();
	ConstructL(aOperation);
	}

void CImEmailOperation::ConstructL(TMsvId aMessageId, TImEmailOperation aOperation)
	{
    iOrigMessageId = aMessageId;
	ConstructL(aOperation);
	}

void CImEmailOperation::ConstructL(TImEmailOperation aOperation)
	{
	iOperation = aOperation;
	iMsvEntry = CMsvEntry::NewL(iMsvSession, iDestinationId, TMsvSelectionOrdering());
	iSmtpSettings = new (ELeave) CImSmtpSettings;
		
	if( iMsgType == KUidMsgTypeSMTP )
		{
		// Restore the SMTP settings - check if the service has been supplied.
		if( iSmtpServiceId == KMsvNullIndexEntryId || iSmtpServiceId == KMsvUnknownServiceIndexEntryId )
			{
			// Nope - the operation dictates where the SMTP settings are restored
			// from.
			switch( iOperation )
				{
			case EReply:
			case EReceipt:
 			case EForward:
			case EForwardAsAttachment:
				{
				// Restore from the original message.
				TRAPD(error, SetSmtpServiceFromOriginalMessageL());

				if( error != KErrNone )
					{
					// Failed to restore the SMTP settings from the original 
					// message - use the SMTP settings from the default settings
					// specified in the root entry.
					SetSmtpServiceFromDefaultsL();
					}
				} break;
			case ENew:
			case ECopy:
				{
				// No SMTP service has been supplied - restore SMTP settings 
				// from the default service specified in the root entry.
				SetSmtpServiceFromDefaultsL();
				} break;
			default:
				__ASSERT_DEBUG( EFalse, User::Invariant() );
				break;
				}
			}
		else
			{
			// Restore SMTP settings from the service supplied.
			iMsvEntry->SetEntryL(iSmtpServiceId);
			RestoreSmtpSettingsL();
			}

		}
	else if( iMsgType == KUidMsgTypePCMail )
		{
		if( iOperation == EReply )
			{
			// reply via the same service as the original message
			iMsvEntry->SetEntryL(iOrigMessageId);
			iPCMailServiceId = iMsvEntry->Entry().iServiceId;
			}
		else if( iSmtpServiceId != KMsvNullIndexEntryId ) // iSmtpService may hold the PCMail Service Id!
			iPCMailServiceId = iSmtpServiceId; 
		else
			iPCMailServiceId = KMsvUnknownServiceIndexEntryId;
		}
	
	iNewHeader = CImHeader::NewLC();
	CleanupStack::Pop(iNewHeader);
	iEmailMessage = CImEmailMessage::NewL(*iMsvEntry);

	iParaLayer = CParaFormatLayer::NewL();
	iCharLayer = CCharFormatLayer::NewL();
	iRichText = CRichText::NewL(iParaLayer, iCharLayer);

	iAttachmentInfoList.ResetAndDestroy();
	if( iOrigMessageId != KMsvNullIndexEntryId )
		{
		iMsvEntry->SetEntryL(iOrigMessageId);
		iOrigMessageHtml = static_cast<TMsvEmailEntry>(iMsvEntry->Entry()).MHTMLEmail();
		}

	if( iMsvEmailTypeList & KMsvEmailTypeListMHTMLMessage )
		{
		TRAPD(err, iHtmlConverter = CImHtmlConverter::NewL(*iMsvEntry, *iParaLayer, *iCharLayer));
 		if( err != KErrNone )
			{
			if( err == KErrNoMemory )
				User::Leave(KErrNoMemory);
			else
				gPanic(EImcmHtmlConverterNotFound);
			}
		iCreateHtmlMessage = ETrue;
		}

	if( iMsgType == KUidMsgTypeSMTP && iOperation != ECopy )
		CheckForSignatureOrVCardL();

	if( iNeedToAddVCardAttachment || iCreateHtmlMessage || (iOperation!=ENew && iOperation!=ECopy) )
		OpenAndReadResourceFileL();

	CActiveScheduler::Add(this);

	// The signature needs to be added to the richtext regardless of whether the
	// body is included iSignatureText will only be set if the Signature flag is
	// set in the SMTP settings and if there is a richtext stream.
	if( iSignatureText )
		iRichText->AppendTakingSolePictureOwnershipL(*iSignatureText);
	

	if( iOperation == ENew )
		iState = ECreateNewMessageEntry;
	ProcessStateL();

	iObserverRequestStatus = KRequestPending;
	SetActive();
	}

void CImEmailOperation::OpenAndReadResourceFileL()
	{
	RResourceFile resourceFile;
	OpenResourceFileL(resourceFile, iMsvSession.FileSession());
	CleanupClosePushL(resourceFile);

	HBufC8* buf;
	TResourceReader reader;
	if( iFormatString == NULL )
		{
		if( (iOperation == EForward) || (iOperation == EForwardAsAttachment) )
			{
			buf = resourceFile.AllocReadLC(FORWARD_FORMATTING_STRING);
			reader.SetBuffer(buf);
			iFormatString = (reader.ReadTPtrC()).AllocL();
			CleanupStack::PopAndDestroy(buf);
			}
		else if( iOperation == EReply )
			{
			buf = resourceFile.AllocReadLC(REPLY_FORMATTING_STRING);
			reader.SetBuffer(buf);
			iFormatString = (reader.ReadTPtrC()).AllocL();
			CleanupStack::PopAndDestroy(buf);
			}
		else if( iOperation == EReceipt )
			{
			buf = resourceFile.AllocReadLC(RECEIPT_FORMATTING_STRING);
			reader.SetBuffer(buf);
			iFormatString = (reader.ReadTPtrC()).AllocL();
			CleanupStack::PopAndDestroy(buf);
			}
		}
	if( (iOperation == EForward) || (iOperation == EForwardAsAttachment) || (iOperation == EReply) )
		{
		RestoreOriginalHeaderL();
		TInt format;
		if( (iOperation == EForward) || (iOperation == EForwardAsAttachment) )
			{
			switch (iBodyHeaderFormat)
				{
				case EToAndCc:
					format = FORWARD_BODY_HEADER_TOCC;
					break;				
				case EToOnly:
					format = FORWARD_BODY_HEADER_TOONLY;
					break;
				case ECcOnly:
					format = FORWARD_BODY_HEADER_CCONLY;
					break;
				case ENoToCcInfo:
				default:
					format = FORWARD_BODY_HEADER;
					break;
				}

			}
		else  // iOperation == EReply
			{
			switch (iBodyHeaderFormat)
				{
				case EToAndCc:
					format = REPLY_BODY_HEADER_TOCC;
					break;				
				case EToOnly:
					format = REPLY_BODY_HEADER_TOONLY;
					break;
				case ECcOnly:
					format = REPLY_BODY_HEADER_CCONLY;
					break;
				case ENoToCcInfo:
				default:
					format = REPLY_BODY_HEADER;
					break;
				}
			}
		buf = resourceFile.AllocReadLC(format);
		reader.SetBuffer(buf);
		iBodyHeaderFormatString = (reader.ReadTPtrC()).AllocL();
		CleanupStack::PopAndDestroy(buf);
		
		buf = resourceFile.AllocReadLC(BODY_HEADER_DATETIME_FORMAT);
		reader.SetBuffer(buf);
		iBodyHeaderDateTimeFormatString = (reader.ReadTPtrC()).AllocL();
		CleanupStack::PopAndDestroy(buf);
		}

	if( iNeedToAddVCardAttachment )
		{
		buf = resourceFile.AllocReadLC(DEFAULT_VCARD_NAME);
		reader.SetBuffer(buf);
		iDefaultVCardNameFormatString = (reader.ReadTPtrC()).AllocL();
		CleanupStack::PopAndDestroy(buf);
		}
	if( iHtmlConverter != NULL )
		iHtmlConverter->ReadDefaultAttachmentNameL(resourceFile);

	CleanupStack::PopAndDestroy(&resourceFile);
	}

void CImEmailOperation::SetSmtpServiceFromDefaultsL()
	{
  	// Get the default SMTP service Id from CenRep
  	CEmailAccounts* account = CEmailAccounts::NewLC();		
  	TSmtpAccount id;
	TInt error = account->DefaultSmtpAccountL(id);
	CleanupStack::PopAndDestroy(account); 	   
  	
  	if (error == KErrNone)
  		{
		iSmtpServiceId = id.iSmtpService;		
  		iMsvEntry->SetEntryL(iSmtpServiceId);
  		RestoreSmtpSettingsL();		
  		}
  	else if (error == KErrNotFound)
  		{
  		iSmtpServiceId = KMsvUnknownServiceIndexEntryId;
  		}
	}

void CImEmailOperation::SetSmtpServiceFromOriginalMessageL()
	{
	__ASSERT_DEBUG( iOperation != ENew, User::Invariant() );

	iMsvEntry->SetEntryL(iOrigMessageId);

	__ASSERT_DEBUG(iMsvEntry->Entry().iServiceId != KMsvLocalServiceIndexEntryIdValue, gPanic(EMiutLocalServiceIdSet));

	// Get iServiceId from original message and then move context to the service
	// (e.g. POP).
	iSmtpServiceId = iMsvEntry->Entry().iServiceId;
	iMsvEntry->SetEntryL(iMsvEntry->Entry().iServiceId);

	// Now move context to SMTP to access the SMTP settings.
	iSmtpServiceId = iMsvEntry->Entry().iRelatedId;
	iMsvEntry->SetEntryL(iSmtpServiceId);

	RestoreSmtpSettingsL();
	}

void CImEmailOperation::RestoreSmtpSettingsL()
	{
  	CEmailAccounts* account = CEmailAccounts::NewLC();  	
  	TSmtpAccount id;
	account->GetSmtpAccountL(iMsvEntry->Entry().Id(), id);
	account->LoadSmtpSettingsL(id, *iSmtpSettings);  	
  	CleanupStack::PopAndDestroy(account);    
	}

void CImEmailOperation::ErrorRecovery(TInt error)
	{
	if (iNewMessageId!=KMsvNullIndexEntryId)
		iMsvEntry->Session().RemoveEntry(iNewMessageId);
	// complete the observer with error
	TRequestStatus* status=&iObserverRequestStatus;
	User::RequestComplete(status,error);
	}

void CImEmailOperation::SelectNextStateL()
	{
	switch( iState )
		{
	case ECreateNewHeader:
		{
		
		if( iOperation & EAttachOriginal )
			iState = ECreateNewMessageEntry;
		else if( iPartList & KMsvMessagePartBody )
			iState = EGetBodyText;
		else if( iPartList & KMsvMessagePartAttachments )
			iState = EGetAttachmentList;
		else
			iState = EGetMessageDigest;
		} break;
	case EGetBodyText:
		{
		// This may be an HTML message with no text alternative to the HTML if 
		// there is no body text.
		TInt sizeOfBody = 0;
		TBool isPlainText = EFalse;
		iEmailMessage->GetBodyTextEntryIdL(iOrigMessageId,CImEmailMessage::EThisMessageOnly);
		// Check if the original message was stored as plain text or rich text.
		if( iEmailMessage->Selection().Count() )
			{
			TMsvId textId = iEmailMessage->Selection().At(0);
			iMsvEntry->SetEntryL(textId);	
			CMsvStore* store = iMsvEntry->ReadStoreL();
			CleanupStack::PushL(store);
			
			isPlainText = (store->IsPresentL(KMsvPlainBodyText16) || store->IsPresentL(KMsvPlainBodyText8));
			CleanupStack::PopAndDestroy(store);
			store = NULL;
			}
		
		// The body text is not in richtext format, so don't populate it.	
		if(isPlainText)
			{
			// Find out the size of the body text.
			sizeOfBody = GetPlainBodyTextSizeL();
			}
		else
			{
			sizeOfBody = iRichText->DocumentLength();
			sizeOfBody = sizeOfBody * 2;
			}
			
		if( iOrigMessageHtml && (((sizeOfBody) - iRichTextSize) == 0) )
			iHtmlNoTextAlt = ETrue;

		if( iPartList & KMsvMessagePartAttachments )
			iState = EGetAttachmentList;
		else
			iState = EGetMessageDigest;
		} break;
	case EGetAttachmentList:
		{
		// Need a copy of attachment list as the CImEmailMessage object may be reset
		TInt count = iEmailMessage->AttachmentManager().AttachmentCount();
		for(TInt index = 0; index < count ; index++)
			{
			CMsvAttachment* attachment = CMsvAttachment::NewL(*iEmailMessage->AttachmentInfoSelection()[index]);
			CleanupStack::PushL(attachment);
			User::LeaveIfError(iAttachmentInfoList.Append(attachment));
			CleanupStack::Pop(attachment);
			}
		iState = EGetMessageDigest;
		} break;
	case EGetMessageDigest:
		{
		iEmbeddedMessagesToProcess = iEmailMessage->Selection().Count();
		iState = ECreateNewMessageEntry;
		} break;
	case ECreateNewMessageEntry:
		{
		iNewMessageId = McliUtils::GetProgressIdL(*iMsvOperation);
		iState = ECheckMultipartMixedFolderRequired;
		} break;
	case ECheckMultipartMixedFolderRequired:
		{
		if(	NeedMultipartMixedFolder() )
			iState = ECreateMultipartMixedFolderEntry;
		else
			iState = ECheckMultipartAlternativeFolderRequired;
		} break;
	case ECreateMultipartMixedFolderEntry:
		{
		iMultipartMixedFolderCreated = ETrue;
		iMultipartMixedId = McliUtils::GetProgressIdL(*iMsvOperation);
		iState = ECheckMultipartAlternativeFolderRequired;
		} break;
	case ECheckMultipartAlternativeFolderRequired:
		{
		if( iCreateHtmlMessage )
			iState = ECreateMultipartAlternativeFolderEntry;
		else
			iState = ECheckTextEntryRequired;
		} break;
	case ECreateMultipartAlternativeFolderEntry:
		{
		iMultipartAlternativeFolderCreated = ETrue;
		iMultipartAlternativeId = McliUtils::GetProgressIdL(*iMsvOperation);
		iState = ECheckTextEntryRequired;
		} break;
	case ECheckTextEntryRequired:
		{
		if( (iMultipartMixedFolderCreated && ((iPartList & KMsvMessagePartBody) || iNeedToAddVCardAttachment || iSignatureText )) ||
			(iMultipartAlternativeFolderCreated) ||
			(iPartList & KMsvMessagePartBody) ||
			((iPartList & KMsvMessagePartAttachments) && iOperation != ECopy && iTotalAttachments == 0) ||
			(!(iPartList & KMsvMessagePartAttachments) && !(iPartList & KMsvMessagePartBody) && !iNeedToAddVCardAttachment && !iCreateHtmlMessage) )
			{
			// Hmmm, quite a convaluted way of working out if a text entry is needed!!
			iState = ECreateTextEntry;
			}
		else
			iState = ECheckVCardRequired;
		} break;
	case ECreateTextEntry:
		{
		iTextId = McliUtils::GetProgressIdL(*iMsvOperation);
	
		if( (iPartList & KMsvMessagePartBody && !(iOperation & EAttachOriginal)) || iSignatureText )
			iState = EStoreBody;
		else
			iState = ECheckVCardRequired;
		} break;
	case EStoreBody:
		{
		iState = ECheckVCardRequired;
		} break;
	case ECheckVCardRequired:
		{
		if( iNeedToAddVCardAttachment && (iOperation != ECopy) )
			iState = ECreateVCardAttachment;
		else
			iState = ECheckHTMLEntryRequired;
		} break;
	case ECreateVCardAttachment:
		{
		iVcardId = McliUtils::GetProgressIdL(*iMsvOperation);
		iState = EAddVCardAttachment;
		} break;
	case EAddVCardAttachment:
		{
		iState = ECheckHTMLEntryRequired;
		} break;
	case ECheckHTMLEntryRequired:
		{
		if( iCreateHtmlMessage )
			iState = ECreateHTMLEntry;
		else
			iState = ECheckAttachOriginalMessageRequired;
		} break;
	case ECreateHTMLEntry:
		{
		__ASSERT_DEBUG( iMultipartAlternativeFolderCreated, User::Invariant() );

		iHtmlId = McliUtils::GetProgressIdL(*iMsvOperation);
		TMsvId serviceId;
		TMsvEmailEntry textEntry;
		iMsvEntry->Session().GetEntry(iTextId, serviceId, textEntry);

		// Store character set info in a MimeHeader.
		iMsvEntry->SetEntryL(iHtmlId);
		iState = ECreateDefaultHtmlAttachment;
		iStore = iMsvEntry->EditStoreL();
		} break;
	case ECreateDefaultHtmlAttachment:
		{
		CImMimeHeader* mimeHeader = CImMimeHeader::NewLC();
		mimeHeader->SetMimeCharset(KCharacterSetIdentifierUtf8);
		mimeHeader->StoreL(*iStore);
		iStore->CommitL();
		CleanupStack::PopAndDestroy(mimeHeader); 
		iFile.Close();
		delete iStore;
		iStore=NULL;
		
		TMsvId serviceId;
		TMsvEmailEntry textEntry;
		iMsvEntry->Session().GetEntry(iTextId, serviceId, textEntry);
		
		iMsvEntry->SetEntryL(iMultipartAlternativeId);

		if( iPartList & KMsvMessagePartBody && (textEntry.iSize > 0) && !(iOperation & EAttachOriginal) )
			iState = EPrepareToStoreHTMLEntryText;
		else 
			iState = ECheckAttachOriginalMessageRequired;
		}break;
	case EPrepareToStoreHTMLEntryText:
		{
		iFinishedConvertingHTML = EFalse;
		
		if(iUsePlainTextStorage && iRestoreErr == KErrNoMemory)
			{
			// If iRestoreErr is KErrNoMemory then complete the store message operation.
			iState = ECompleteEmailOperation;	
			}
		else
			{
			if( iHtmlId != KMsvNullIndexEntryId )
				iState = EStoreHTMLEntryText;
			else 
				iState = ECheckAttachOriginalMessageRequired;
			}
		} break;
	case EStoreHTMLEntryText:
		{
		if( iFinishedConvertingHTML )
			{
			iHtmlConverter->ResetStoreL();
			iState = EStoreHTMLTextInEntry;	
			}
		// Otherwise stay in this state until the HTML has been converted.
		} break;
	case EStoreHTMLTextInEntry:
		iState = ECheckAttachOriginalMessageRequired;
		break;
	case EAddMessageAttachmentInfo:
		{
		ResetStoreL();
		iState = ECompleteEmailOperation;
		}break;
	case ECheckAttachOriginalMessageRequired:
		{
		if( (iOperation & EAttachOriginal) && (iPartList & KMsvMessagePartBody || iPartList & KMsvMessagePartAttachments) )
			iState = EAttachOriginalMessage;
		else
			iState = ECheckHTMLPageRequired;
		} break;
	case EAttachOriginalMessage:
		{
		TPckgBuf<TMsvId> paramPack; 
		paramPack.Copy(iMsvOperation->FinalProgress());
		iAttachedMessageId = paramPack();
		iMsvEntry->SetEntryL(iAttachedMessageId);
		iState = EAddMessageAttachmentInfo;
		} break;
	case ECheckHTMLPageRequired:
		{
		TInt sizeOfBody = 0;
		if(iUsePlainTextStorage && iOperation != ENew)
			{
			iEmailMessage->GetBodyTextEntryIdL(iNewMessageId, CImEmailMessage::EThisMessageOnly);
			sizeOfBody = GetPlainBodyTextSizeL();
			}
		else
			{
			sizeOfBody = iRichText->DocumentLength();
			sizeOfBody = sizeOfBody * 2;
			}
		if( iPartList & KMsvMessagePartBody && iOrigMessageHtml &&
			(iOperation != ENew) &&	(((sizeOfBody) - iRichTextSize) == 0) )
			iState = EFindHTMLPage;
		else
			iState = ECheckAttachmentsRequired;
		} break;
	case EFindHTMLPage:
		{
		iState = ECopyHTMLPartToAttachment;
		} break;
	case ECopyHTMLPartToAttachment:
		{
		iState = ECheckAttachmentsRequired;
		} break;
	case ECheckAttachmentsRequired:
		{
		if( iPartList & KMsvMessagePartAttachments && (iTotalAttachments > 0) && (iOperation != ENew) )
			{
			iFileMan = CFileMan::NewL(iMsvSession.FileSession());
			iState = ECreateAttachmentEntry;
			}
		else
			iState = ECheckEmbeddedMessagesRequired;
		} break;
	case ECreateAttachmentEntry:
		{
		iState = EStoreAttachment;
		} break;
	case EStoreAttachment:
		{
		ResetStoreL();
		//	Are there more attachments to be added?
		if( ++iAttachmentCount < iTotalAttachments )
			iState = ECreateAttachmentEntry;
		else 
			iState = ECheckEmbeddedMessagesRequired;
		} break;
	case ECheckEmbeddedMessagesRequired:
		{
		if( iEmbeddedMessagesToProcess > 0 )
			iState = EAddEmbeddedMessagesAsAttachments;
		else
			iState = ECompleteEmailOperation;
		} break;
	case EAddEmbeddedMessagesAsAttachments:
		{
		if(iEmbeddedMessagesToProcess > 0)
   			{   			
   			iState = EAddEmbeddedMessagesAsAttachments;
   			}
   		else
   			{			
   			iState = ECompleteEmailOperation;
   			}
		} break;
	case ECompleteEmailOperation:
		{
		iState = EFinished;
		} break;
	default:
		__ASSERT_DEBUG( EFalse, User::Invariant() );
		break;
		}
	}

void CImEmailOperation::ProcessStateL()
	{
	CMsvOperation* op = NULL;
	switch( iState )
		{
	case ECreateNewHeader:
		{
		CreateNewHeaderL();
		} break;
	case EGetBodyText:
		{
		GetBodyTextL();
		} break;
	case EGetAttachmentList:
		{
		iEmailMessage->GetAttachmentsListL(iStatus, iOrigMessageId, CImEmailMessage::EAllAttachments, CImEmailMessage::EThisMessageOnly);
		} break;
	case EGetMessageDigest:
		{
		iEmailMessage->GetMessageDigestEntriesL(iStatus, iOrigMessageId);
		} break;
	case ECreateNewMessageEntry:
		{
		CreateNewMessageL(iDestinationId);
		} break;
	case ECreateMultipartMixedFolderEntry:
		{
		CreateMultipartMixedFolderEntryL();
		} break;
	case ECreateMultipartAlternativeFolderEntry:
		{
		CreateMultipartAlternativeFolderEntryL();
		} break;
	case ECreateTextEntry:
		{
		if (iMultipartAlternativeFolderCreated)
			CreateTextEntryL(iMultipartAlternativeId);
		else if (iMultipartMixedFolderCreated)
			CreateTextEntryL(iMultipartMixedId);
		else 
			CreateTextEntryL(iNewMessageId);

		} break;
	case EStoreBody:
		{
		StoreBodyL();
		} break;
	case ECreateVCardAttachment:
		{
		CreateVCardAttachmentL();
		} break;
	case EAddVCardAttachment:
		{
		AddVCardAttachmentL();
		} break;
	case ECreateHTMLEntry:
		{
		CreateHTMLEntryL();
		} break;
	case EPrepareToStoreHTMLEntryText:
		{
		TRAP(iRestoreErr ,iHtmlConverter->PrepareToStoreHTMLEntryTextL(iHtmlId, iTextId));
		// Do not leave if it is KErrNoMemory while creating HTML part for a message, since the 
		// plainbody text part of message is already created and that be used to send the message.
		if(iUsePlainTextStorage)
			{
			if(iRestoreErr != KErrNoMemory)
				{
				User::LeaveIfError(iRestoreErr);
				}
			}
		else
			{
			User::LeaveIfError(iRestoreErr);
			}	
		RequestComplete(KErrNone);
		} break;
	case EStoreHTMLEntryText:
		{
		iFinishedConvertingHTML = iHtmlConverter->StoreHTMLEntryTextAL(iStatus);
		if(!iFinishedConvertingHTML)
			RequestComplete(KErrNone);
		else
			{			
			iVCardAndHtmlSize += iHtmlConverter->Size();
			}
		} break;
	case ECreateDefaultHtmlAttachment:
		{
		CreateDefaultAttachmentL();	
		}break;
	case EStoreHTMLTextInEntry:
		op = iHtmlConverter->ChangeHTMLTextInEnrtyL(iStatus);
		if(!op)
			RequestComplete(KErrNone);	
		else
			{
			delete iMsvOperation;
			iMsvOperation = op;		
			}
		break;
	case EAddMessageAttachmentInfo:
		{
		AddMessageAttachmentInfoL(iAttachedMessageId);
		} break;	
	case EAttachOriginalMessage:
		{
		AttachOriginalMessageToNewMessageL();
		} break;
	case EFindHTMLPage:
		{
		iEmailMessage->FindFirstHTMLPageL(iOrigMessageId, iStatus);
		} break;
	case ECopyHTMLPartToAttachment:
		{
		AppendHtmlAttachmentL();
		} break;
	case ECreateAttachmentEntry:
		{
		CreateAttachmentEntryL();
		} break;
	case EStoreAttachment:
		{
		StoreAttachmentL();
		} break;
	case EAddEmbeddedMessagesAsAttachments:
		{
		AddMessageAsAttachmentL();
		} break;
	case ECompleteEmailOperation:
		{
		CompleteEmailOperationL();
		} break;
	case EFinished:
		{
		TRequestStatus* status=&iObserverRequestStatus;
		User::RequestComplete(status,KErrNone);
		} break;
	case ECheckMultipartMixedFolderRequired:
	case ECheckMultipartAlternativeFolderRequired:
	case ECheckTextEntryRequired:
	case ECheckVCardRequired:
	case ECheckHTMLEntryRequired:
	case ECheckAttachOriginalMessageRequired:
	case ECheckHTMLPageRequired:
	case ECheckAttachmentsRequired:
	case ECheckEmbeddedMessagesRequired:
		{
		RequestComplete(KErrNone);
		} break;
	default:
		__ASSERT_DEBUG( EFalse, User::Invariant() );
		break;
		}
	}

void CImEmailOperation::RequestComplete(TInt aError)
	{
	iStatus = KRequestPending;
	TRequestStatus* status=&iStatus;
	iStatus=KRequestPending;
	User::RequestComplete(status,aError);
	}

TMsvId CImEmailOperation::ServiceId()
	{
	// this functions returns the correct service id dependant on the Message Type.
	if (iMsgType == KUidMsgTypePCMail)
		return iPCMailServiceId;

	return iSmtpServiceId;
	}
	
void CImEmailOperation::CreateNewHeaderL()
	{
	//	1.	Create new header using the original message.
	//	2.	Change header details depending on the operation required.
	//	3.	Create the body header now since we have the original message
	if (iOriginalHeader==NULL)
		{
		RestoreOriginalHeaderL();
		}
		
	switch( iOperation )
		{
	case EForward:
	case EForwardAsAttachment:
		{
		iOriginalHeader->CreateForwardL(*iNewHeader, *iFormatString);
		} break;
	case EReply:
		{
		if( (iPartList & KMsvMessagePartOriginator) && !(iPartList & KMsvMessagePartRecipient) )
			iOriginalHeader->CreateReplyL(*iNewHeader, CImHeader::EOriginator, *iFormatString);
		else
			{
			if( (iPartList & KMsvMessagePartOriginator) && (iPartList & KMsvMessagePartRecipient) )
				iOriginalHeader->CreateReplyL(*iNewHeader, CImHeader::EAll, *iFormatString);
			else
				iOriginalHeader->CreateReplyL(*iNewHeader, CImHeader::ERecipients, *iFormatString);

			// Remove the user's email address (if provisioned) from the To: and Cc: headers
			// of the reply message.  But ensure there is at least one email
			// address in the To: header to account for the scenario when the
			// user may be replying to a message they sent themselves.
			if (iSmtpSettings->EmailAddress().Length())
   				{
				TInt i = 0;
				while (i < iNewHeader->ToRecipients().Count())
   					{
					if ((iNewHeader->ToRecipients().Count() > 1) && (iNewHeader->ToRecipients()[i]).FindF(iSmtpSettings->EmailAddress()) >= 0)
						// Only delete if there is more than one recipient because we do 
						// not want to create a reply with an empty To: header.
   						iNewHeader->ToRecipients().Delete(i);
					else
						++i;
   					}
				
				i = 0;
				while (i < iNewHeader->CcRecipients().Count())
   					{
					if ((iNewHeader->CcRecipients()[i]).FindF(iSmtpSettings->EmailAddress()) >= 0)
						iNewHeader->CcRecipients().Delete(i);
					else
						++i;
   					}
   				}
			}
		} break;
	case ECopy:
		{
		delete iNewHeader;
		iNewHeader = iOriginalHeader;
		} break;
	case EReceipt:
		{
		iOriginalHeader->CreateReceiptL(*iNewHeader, *iFormatString);
		} break;
	default:
		__ASSERT_DEBUG( EFalse, User::Invariant() );
		break;
		}

	if (iPartList & !KMsvMessagePartDescription)
		iNewHeader->SetSubjectL(KNullDesC);

	if ((iBodyHeaderFormatString != NULL ) && (iBodyHeaderFormatString->Find(KMiutFormatString)))
		{
		__ASSERT_DEBUG(iBodyHeaderDateTimeFormatString!=NULL, User::Invariant());
		
		TTime date = iMsvEntry->Entry().iDate;
		
		//Convert time to local time. 
		RTz myTZoneServer; 
		// Connect to the time zone server, leaves if fails to connect
		User::LeaveIfError(myTZoneServer.Connect()); 
		CleanupClosePushL(myTZoneServer);
		// Create a converter object.
		CTzConverter* myConverter = CTzConverter::NewL(myTZoneServer); 
		CleanupStack::PushL(myConverter);
		myConverter->ConvertToLocalTime(date); 
		CleanupStack::PopAndDestroy(2);  //myTZoneServer,myConverter
		
		TBuf<KMaxLongDateFormatSpec+KMaxTimeFormatSpec+1> parsedDateTime;
	        date.FormatL(parsedDateTime, *iBodyHeaderDateTimeFormatString);   

		// Numberconvertion to the local phone language
		TLocale localeInfo;
		localeInfo.Refresh();
		TDigitType dgType = localeInfo.DigitType();

		NumberConversion::ConvertDigits(parsedDateTime, dgType);

		TPtrC subject = iOriginalHeader->Subject();
		TPtrC author = iOriginalHeader->From();
		iOriginalHeader = NULL;
		
		switch (iBodyHeaderFormat)
			{
			case EToAndCc:
				{
				iBodyHeader = HBufC::NewL(subject.Length() + author.Length() + parsedDateTime.Length() + iBodyHeaderToString->Length() + iBodyHeaderCcString->Length() + iBodyHeaderFormatString->Length());
				TPtr bodyHeaderPtr = iBodyHeader->Des();
				bodyHeaderPtr.Format(TRefByValue<const TDesC>(*iBodyHeaderFormatString), &subject, &author, iBodyHeaderToString, iBodyHeaderCcString, &parsedDateTime);
				} break;				
			case EToOnly:
				{
				iBodyHeader = HBufC::NewL(subject.Length() + author.Length() + parsedDateTime.Length() + iBodyHeaderToString->Length() + iBodyHeaderFormatString->Length());
				TPtr bodyHeaderPtr = iBodyHeader->Des();
				bodyHeaderPtr.Format(TRefByValue<const TDesC>(*iBodyHeaderFormatString), &subject, &author, iBodyHeaderToString, &parsedDateTime);
				} break;
			case ECcOnly:
				{
				iBodyHeader = HBufC::NewL(subject.Length() + author.Length() + parsedDateTime.Length() + iBodyHeaderCcString->Length() + iBodyHeaderFormatString->Length());
				TPtr bodyHeaderPtr = iBodyHeader->Des();
				bodyHeaderPtr.Format(TRefByValue<const TDesC>(*iBodyHeaderFormatString), &subject, &author, iBodyHeaderCcString, &parsedDateTime);
				} break;
			case ENoToCcInfo:
			default:
				{
				iBodyHeader = HBufC::NewL(subject.Length() + author.Length() + parsedDateTime.Length() + iBodyHeaderFormatString->Length());
				TPtr bodyHeaderPtr = iBodyHeader->Des();
				bodyHeaderPtr.Format(TRefByValue<const TDesC>(*iBodyHeaderFormatString), &subject, &author, &parsedDateTime);
				}
			} // switch (iBodyHeaderFormat)
		}

	RequestComplete(KErrNone);
	}

void CImEmailOperation::RestoreOriginalHeaderL()
	{
	__ASSERT_DEBUG(iOriginalHeader==NULL, User::Invariant());
	iOriginalHeader = CImHeader::NewLC();
	CleanupStack::Pop(iOriginalHeader);

	// Restore the header from the original message.
	iMsvEntry->SetEntryL(iOrigMessageId);
	CMsvStore* store = iMsvEntry->ReadStoreL();
	CleanupStack::PushL(store);
	iOriginalHeader->RestoreL(*store);
	CleanupStack::PopAndDestroy(store);
	SetBodyHeaderFormatL();
	}
	
void CImEmailOperation::SetBodyHeaderFormatL()
	{
	// determine if To: and CC: fields are to be included in reply/forwarded email
	// body header. If so, prepare the string to be included.
	if (iSmtpSettings->ToCcIncludeLimit() > 0)
		{
		if (iOriginalHeader->ToRecipients().Count()>0)
			{
			if (iOriginalHeader->CcRecipients().Count()>0)
				{
				iBodyHeaderFormat=EToAndCc;
				CreateAddressListStringL(iBodyHeaderCcString, iOriginalHeader->CcRecipients());
				}
			else
				{
				iBodyHeaderFormat=EToOnly;
				}
			CreateAddressListStringL(iBodyHeaderToString, iOriginalHeader->ToRecipients());
			}
		else // no "to" recipients, any "cc"?
			{
			if (iOriginalHeader->CcRecipients().Count()>0)
				{
				iBodyHeaderFormat=ECcOnly;
				CreateAddressListStringL(iBodyHeaderCcString, iOriginalHeader->CcRecipients());
				}
			else
				{
				iBodyHeaderFormat=ENoToCcInfo;
				}
			}
		}
	else
		{
		iBodyHeaderFormat=ENoToCcInfo;
		}
	}

void CImEmailOperation::CreateAddressListStringL(HBufC*& aListBuffer, const CDesCArray& aAddressArray)
	{
	// Prepares a list of addresses to include in body header of reply/forwarded emails,
	// truncated if exceeding the set limit.
	// The original data may be either an array of individual addresses, or a single
	// descriptor containing multiple comma separated addresses.
	TBool truncate=EFalse;
	TInt  length=0;
	TInt  count=0;
	TInt  limit=iSmtpSettings->ToCcIncludeLimit();
	TInt  elements=aAddressArray.Count();
	
	if (elements==0)
		return;

	if (elements==1) // handle single string containing one or more addresses
		{
		// truncate the string if necessary
		TInt lastChar=0;
		truncate=iMessageField.TruncateAddressString(aAddressArray[0], limit, lastChar);
		if (!truncate)
			{
			aListBuffer = HBufC::NewL(aAddressArray[0].Length());
			TPtr addrStringPtr = aListBuffer->Des();
			addrStringPtr.Append(aAddressArray[0]);
			}
		else
			{
			aListBuffer = HBufC::NewL(lastChar+KEllipsesString.iTypeLength); // room for the truncated list + ellipses
			TPtr addrStringPtr = aListBuffer->Des();
			TPtrC16 leftString=aAddressArray[0].Left(lastChar);
			TInt temp1 = leftString.Length();
			TInt temp2 = aListBuffer->Length();
			addrStringPtr.Append(leftString);
			addrStringPtr.Append(KEllipsesString);
			}
		}
	else	// handle array of individual addresses.
		{
		// determine if list to be truncated
		if (limit<elements)
			{
			count=limit;
			truncate=ETrue;
			}
		else
			{
			count=elements;
			}
		
		// calculate length required for the address string
		TInt n;
		for (n=0;n<count-1;++n)
			{
			length+=aAddressArray[n].Length()+KSeparatorString.iTypeLength;
			}
		length+=aAddressArray[n].Length();
		if (truncate)
			{
			length+=KEllipsesString.iTypeLength;
			}

		delete aListBuffer;
		aListBuffer = NULL;
		aListBuffer = HBufC::NewL(length);
		TPtr addrStringPtr = aListBuffer->Des();

		// populate the address buffer
		for (n=0;n<count-1;++n)
			{
			addrStringPtr.Append(aAddressArray[n]);
			addrStringPtr.Append(KSeparatorString);
			}
		addrStringPtr.Append(aAddressArray[n]);			
		if (truncate)
			{
			addrStringPtr.Append(KEllipsesString);
			}
		}
	}
	
void CImEmailOperation::GetBodyTextL()
	{
	//	1.	Insert body header
	//	2.	Get the Body Text from the original message

	if (iBodyHeaderFormatString != NULL)
		iRichText->InsertL(iRichText->DocumentLength(), *iBodyHeader);

	// DocumentLength() gives no. of chars and since unicode we multiply by 2
	iRichTextSize = (iRichText->DocumentLength() * 2);
	
	iEmailMessage->GetBodyTextEntryIdL(iOrigMessageId,CImEmailMessage::EThisMessageOnly);
	TMsvId textId = 0;
	TBool isPlainText = EFalse;
	
	// Check if the body text of the message restored here was stored as rich text or not.
	if( iEmailMessage->Selection().Count() )
		{
		textId = iEmailMessage->Selection().At(0);
		iMsvEntry->SetEntryL(textId);
		CMsvStore* store = iMsvEntry->ReadStoreL();
		CleanupStack::PushL(store);
		// Check if the original message was stored as plain text.
		if(iUsePlainTextStorage)
			{
			isPlainText = (store->IsPresentL(KMsvPlainBodyText16)|| store->IsPresentL(KMsvPlainBodyText8));
			}		
		CleanupStack::PopAndDestroy(store);
		store = NULL;
		}
		
	// If the original message was not rich text then need not populate bodytext here.
	// Body text will be retrieved and stored when StoreBodyL is called .
	if(isPlainText)
		{
		TRequestStatus* myStatus=&iStatus;
		iStatus=KRequestPending;
		User::RequestComplete(myStatus,KErrNone);	
		}
	else
		{
		iEmailMessage->GetBodyTextL(iStatus, iOrigMessageId, CImEmailMessage::EThisMessageOnly, *iRichText, *iParaLayer, *iCharLayer);
		}
	
	
	}

TInt CImEmailOperation::RemoveIncompleteAttachments()
	{
	TInt completeAttachmentCount = 0;
	while (completeAttachmentCount < iAttachmentInfoList.Count())
		{
		if (!(iAttachmentInfoList[completeAttachmentCount]->Complete()))
			{
			delete iAttachmentInfoList[completeAttachmentCount];
			iAttachmentInfoList.Remove(completeAttachmentCount);
			}
		else
			++completeAttachmentCount;
		}
	return iAttachmentInfoList.Count();	
	}

void CImEmailOperation::CreateEntryDetails(TMsvEmailEntry& entry)
	{
	entry.iMtm = iMsgType;
	entry.iServiceId = ServiceId();
	entry.iType = KUidMsvMessageEntry;
	entry.iDate.UniversalTime();
	if ((iOperation != ENew) && (iPartList & KMsvMessagePartAttachments))
		iTotalAttachments = RemoveIncompleteAttachments();
	entry.SetNew(ETrue);
	if (iEmbeddedMessagesToProcess!=0)
		entry.SetAttachment(ETrue);
	entry.SetVisible(!(iMsvEmailTypeList & KMsvEmailTypeListInvisibleMessage));
	entry.SetMHTMLEmail(iCreateHtmlMessage);
	entry.SetInPreparation(iMsvEmailTypeList & KMsvEmailTypeListMessageInPreparation);
	}

void CImEmailOperation::CreateNewMessageL(TMsvId aDestinationId)
	{
	//	Create a new message so that the different sections can be added to it:
	iMsvEntry->SetEntryL(aDestinationId);
	delete iMsvOperation;
	iMsvOperation = NULL;
	TMsvEmailEntry entry;
	CreateEntryDetails(entry);
	iMsvOperation = iMsvEntry->CreateL(entry, iStatus);
	}

TBool CImEmailOperation::NeedMultipartMixedFolder() const
	{
	if( (iPartList & KMsvMessagePartAttachments && (iTotalAttachments > 0)) &&
		(iPartList & KMsvMessagePartBody || iSignatureText || iCreateHtmlMessage || iOperation & EAttachOriginal) )
		{
		// In this case the operation is attaching 1 or more attachments. As there
		// is either message body (signature text implies) or at least another
		// attachment (HTML body or the original message) then a mixed folder is
		// needed.
		return ETrue;
		}

	if( iPartList & KMsvMessagePartAttachments && (iTotalAttachments > 0) )
		{
		// If this case is reached then although the message body is not required
		// but it has attachment/s a mixed folder is needed.
		return ETrue;
		}

	if( iOperation & EAttachOriginal && iPartList & KMsvMessagePartBody )
		{
		// In this case the operation was attaching the original message as an 
		// attachment (e.g ForwardAsAttachment). As the message body is required
		// then mixed folder needed.
		return ETrue;
		}

	if( (iNeedToAddVCardAttachment || iHtmlNoTextAlt || iEmbeddedMessagesToProcess) && 
		(iPartList & KMsvMessagePartBody || iPartList & KMsvMessagePartAttachments))
		{
		// A V-card is required or the body text is added as an attachment or 
		// there are embedded messages to attach - all imply an attachment of
		// some description. As the message body or the attachments are required
		// then a mixed folder is needed.
		return ETrue;
		}
	
	if( iNeedToAddVCardAttachment )
		{
		//  Now this case is probably not needed - added here because a text 
		// entry is always added. Therefore if defect sorted so that text entry
		// onlu added when required this case can be removed.
		return ETrue;
		}

	// Got here therefore no mixed folder needed.
	return EFalse;
	}

void CImEmailOperation::CreateMultipartMixedFolderEntryL()
	{
	iMsvEntry->SetEntryL(iNewMessageId);
	TMsvEmailEntry entry;
	entry.iType = KUidMsvFolderEntry;
	entry.iMtm = iMsvEntry->Entry().iMtm;
	entry.iServiceId = iMsvEntry->Entry().iServiceId;
	entry.SetMessageFolderType(EFolderTypeMixed);
	entry.iSize = 0;
	delete iMsvOperation;
	iMsvOperation = NULL;
	iMsvOperation = iMsvEntry->CreateL(entry, iStatus);
	}

void CImEmailOperation::CreateTextEntryL(TMsvId aFolderId)
	{
	iMsvEntry->SetEntryL(aFolderId);
	TMsvEntry entry;
	entry.iType = KUidMsvEmailTextEntry;
	entry.iMtm = iMsvEntry->Entry().iMtm;
	entry.iServiceId = ServiceId();
	entry.iDate.UniversalTime();
	entry.iSize = 0;
	delete iMsvOperation;
	iMsvOperation=NULL;
	iMsvOperation=iMsvEntry->CreateL(entry, iStatus);
	}

void CImEmailOperation::StoreBodyL()
	{
	if(iUsePlainTextStorage)
		{
		StorePlainBodyL();
		return;
		}
	iMsvEntry->SetEntryL(iTextId);	//pointing to the text entry
	CMsvStore* store = iMsvEntry->EditStoreL();
	CleanupStack::PushL(store);
	
	store->StoreBodyTextL(*iRichText);
	store->CommitL();
	
	CleanupStack::PopAndDestroy(store);	
	// update size of text entry
	TMsvEmailEntry entry(iMsvEntry->Entry());

	// DocumentLength() gives no. of chars and since unicode, multiply by 2
	entry.iSize = (iRichText->DocumentLength() * 2);	
	entry.iDate.UniversalTime();
	
	delete iMsvOperation;
	iMsvOperation = NULL;
	iMsvOperation = iMsvEntry->ChangeL(entry, iStatus);	
	}


void CImEmailOperation::StorePlainBodyL()
	{
	TUint charset = 0;
	TUint defaultCharset = 0;
	TBool override = EFalse;
	
	RFs& fileSvrSession = iMsvSession.FileSession();
	CCnvCharacterSetConverter* characterConverter = CCnvCharacterSetConverter::NewL();
	CleanupStack::PushL(characterConverter);
	
	CImConvertCharconv* charConv = CImConvertCharconv::NewL(*characterConverter, fileSvrSession);
	CleanupStack::PushL(charConv);
	defaultCharset = charConv->SystemDefaultCharset();
	CleanupStack::PopAndDestroy(2);
	
	
	// To store the ids of all the text part of a email message.
	RArray<TMsvId> textIdArray;
	CleanupClosePushL (textIdArray);
	
	RPointerArray<CMsvPlainBodyText> plainTextArray;
	TCleanupItem closePlainTextArray (CImEmailOperation::ClosePlainBodyTextArray, &plainTextArray);
	CleanupStack::PushL(closePlainTextArray);
	TInt origSize = 0;
	
	// Chunk storage mechanism is used and we are doing a replyto OR fwding a mail then 
	// get the size of the message.
	if(iOperation == EForward || iOperation == EReply)
		{
		iEmailMessage->GetCharacterSetL(iOrigMessageId,charset,override);
		
		iEmailMessage->GetBodyTextEntryIdL(iOrigMessageId,CImEmailMessage::EThisMessageOnly);
		TInt count = iEmailMessage->Selection().Count();
		for(TInt i=0; i<count; ++i)
			{
			User::LeaveIfError (textIdArray.Append( iEmailMessage->Selection().At(i) ));
			}
		TInt textIdCount = 	textIdArray.Count();
		for(TInt i=0; i<textIdCount; ++i)
			{
			// Pointing to the text entry.
			iMsvEntry->SetEntryL(textIdArray[i]);	
			CMsvStore* bodyTextStore = iMsvEntry->ReadStoreL();
			CleanupStack::PushL(bodyTextStore);
		
			plainTextArray.Append( bodyTextStore->InitialisePlainBodyTextForReadL(KMsvDecodeChunkLength));
			origSize += plainTextArray[i]->Size();
				
			CleanupStack::PopAndDestroy(bodyTextStore);
			bodyTextStore = NULL;
			}
		}
	
	iMsvEntry->SetEntryL(iTextId);	//pointing to the text entry
	
	CMsvStore* bodyTextStore = iMsvEntry->EditStoreL();
	CleanupStack::PushL(bodyTextStore);
	
	// If chunk storage mechanism is used then get CMsvPlainBodyText.
	// Body text is stored as 16 bit so set iIs8Bit to EFalse.When a message is created from
	// the client side data is stored as 16 bit.
	CMsvPlainBodyText* text = bodyTextStore->InitialisePlainBodyTextForWriteL(EFalse,charset,defaultCharset);	
	CleanupStack::PushL(text);
	
	// Add signature if it exists.
	if(iSignatureText)
		{
		TInt len = iSignatureText->DocumentLength();
		HBufC16* signatureText = HBufC16::NewLC(len);
		TPtr signatureTextPtr = signatureText->Des();
		iSignatureText->Extract(signatureTextPtr, 0, len);
		text->StoreChunkL(signatureTextPtr);
		CleanupStack::PopAndDestroy(signatureText);
		}
	
	if(iBodyHeader)
		{
		if(iOperation == EForward || iOperation == EReply)
			{
			text->StoreChunkL(iBodyHeader->Des());
			}
		}
			
	// If email is Fwd'ed or ReplyTo then need to read from the existing mail and add it to
	// the newly created mail since this was not done in GetBodyTextL.
	TInt plainTextCount = plainTextArray.Count();
	for(TInt i=0; i<plainTextCount; ++i)
		{
		TInt size = plainTextArray[i]->Size();
		while( size > 0)
			{
			TBuf <KMsvDecodeChunkLength> buf;
			plainTextArray[i]->NextChunkL(buf);
			text->StoreChunkL(buf);
			size -= KMsvDecodeChunkLength;
			}	
		delete plainTextArray[i];
		plainTextArray[i] = NULL;
		}
	text->CommitL();
	CleanupStack::PopAndDestroy(4, &textIdArray);// text, bodyTextStore, plainTextArray
	
	TMsvEmailEntry entry(iMsvEntry->Entry());

	entry.iSize = origSize;	
	entry.iDate.UniversalTime();
	
	delete iMsvOperation;
	iMsvOperation = NULL;
	iMsvOperation = iMsvEntry->ChangeL(entry, iStatus);	
	}


void CImEmailOperation::CheckForSignatureOrVCardL()
   	{
   	if (!iSmtpServiceId || iSmtpServiceId==KMsvUnknownServiceIndexEntryId)
   		return; // No smtp service set.
   
	if( iSmtpSettings->AddVCardToEmail() )
   		{
		CContactDatabase* db = NULL;
   		// check whether the actual VCard exists
		TRAPD(error, db = CContactDatabase::OpenL());
		if( error == KErrNone )
			{
			CleanupStack::PushL(db);
			TContactItemId contactId = db->OwnCardId();
			if( contactId != KNullContactId )
				iNeedToAddVCardAttachment = ETrue;
			CleanupStack::PopAndDestroy(db);
			}
		else
			{
			if( db )
				delete db;
			}
   		}
   	
	if( iSmtpSettings->AddSignatureToEmail() )
   		{
		// Check the SMTP settings entry for the signature text.
		iMsvEntry->SetEntryL(iSmtpServiceId);
		CMsvStore* store = iMsvEntry->ReadStoreL();
		CleanupStack::PushL(store);

		if( store->IsPresentL(KMsvEntryRichTextBody) )
			{
   			iSignatureText=CRichText::NewL(iParaLayer,iCharLayer);	
   			store->RestoreBodyTextL(*iSignatureText);
			}
		CleanupStack::PopAndDestroy(store);
   		}
   	else
   		iSignatureText=NULL;
   	}

void CImEmailOperation::CreateVCardAttachmentL()
	{
	// Get the vcard from contact database, need owncard entry..

	CContactDatabase* db = CContactDatabase::OpenL();
	CleanupStack::PushL(db);
	TContactItemId contactId = db->OwnCardId();
	CContactIdArray *ids=CContactIdArray::NewLC();
	ids->AddL(contactId);
	if(contactId == KNullContactId)
		{
		CleanupStack::PopAndDestroy(2); // db, ids
		RequestComplete(KErrNone);
		return; // vcard not present, do nothing.
		}
	
	// Retrieve vcard to a temp store. 

	TUid uid;
	uid.iUid=KUidVCardConvDefaultImpl;

	delete iVcardStore;
	iVcardStore=NULL;
	iVcardStore=CBufStore::NewL(128);
	RStoreWriteStream tmpWrite;
	iVcardStoreId = tmpWrite.CreateLC(*iVcardStore);

	db->ExportSelectedContactsL(uid,*ids, tmpWrite, CContactDatabase::ETTFormat);
	tmpWrite.CommitL();
	iVcardStore->CommitL();

	// create attachment CMsvEntry entry
	TBuf<KVCardFilenameLength> textDef;
	CContactTextDef* contactTextDef = CContactTextDef::NewLC();
	TLanguage language = User::Language();
	if( language == ELangTaiwanChinese || 
		language == ELangHongKongChinese || 
		language == ELangPrcChinese )
		{
		contactTextDef->AppendL(TContactTextDefItem(KUidContactFieldFamilyName));
		contactTextDef->AppendL(TContactTextDefItem(KUidContactFieldGivenName));
		}
	else
		{
		contactTextDef->AppendL(TContactTextDefItem(KUidContactFieldGivenName));
		contactTextDef->AppendL(TContactTextDefItem(KUidContactFieldFamilyName));
		}
	contactTextDef->SetExactMatchOnly(ETrue);
	db->ReadContactTextDefL(contactId,textDef,contactTextDef);
	CleanupStack::PopAndDestroy(contactTextDef);
	if (textDef.Length())
		iFileName.Copy(textDef);
	else
		iFileName.Copy(*iDefaultVCardNameFormatString);
	iFileName.Append(KMimeVCardExtension);

	if (iMultipartMixedFolderCreated)
		iMsvEntry->SetEntryL(iMultipartMixedId);
	else 
		iMsvEntry->SetEntryL(iNewMessageId);
	CleanupStack::PopAndDestroy(3); // db, ids, tmpWrite,

	CreateAttachmentEntryL(); //Async
	}

void CImEmailOperation::AddVCardAttachmentL()
	{
	iMsvEntry->SetEntryL(iVcardId);
	if (!iFileName.Length())
		{
		RequestComplete(KErrNone);
		return; // ergo no attachment.
		}

	// Put vcard into a file.
	TFileName filePath;
	CMsvStore* store = iMsvEntry->EditStoreL();
	CleanupStack::PushL(store);
		
	MMsvAttachmentManagerSync& attachmentMgrSync = store->AttachmentManagerExtensionsL();
	CMsvAttachment* attachment = CMsvAttachment::NewL(CMsvAttachment::EMsvFile);
	RFile file;
	attachmentMgrSync.CreateAttachmentL(iFileName,file,attachment);
	store->CommitL();
	CleanupStack::PopAndDestroy(store);
	
	// set the file attachment file handle with write mode
	iAttachmentFile.SetFileHandle(file,TImAttachmentFile::EImFileWrite);
	RStoreReadStream dummy;
	dummy.OpenLC(*iVcardStore,iVcardStoreId);
	TInt size = (dummy.Source()->SizeL());
	
	HBufC8* textBuffer=HBufC8::NewLC(size);
	TPtr8 bufPtr=textBuffer->Des();
	dummy.ReadL(bufPtr,size);
	iAttachmentFile.WriteFile(bufPtr);
	iAttachmentFile.CloseFile();

	delete iVcardStore;
	iVcardStore=NULL;
	CleanupStack::PopAndDestroy(2); // dummy, textBuffer

	iMsvEntry->SetEntryL(iVcardId);
	TMsvEmailEntry entry = iMsvEntry->Entry();
	entry.iSize = size;
	entry.iDetails.Set(iFileName);
	entry.SetVCard(ETrue);
	iVCardAndHtmlSize+=entry.iSize;

	delete iMsvOperation;
	iMsvOperation=NULL;
	iMsvOperation=iMsvEntry->ChangeL(entry, iStatus);
	}

void CImEmailOperation::CreateAttachmentEntryL()
	{
	if (iMultipartMixedFolderCreated)
		iMsvEntry->SetEntryL(iMultipartMixedId);
	else 
		iMsvEntry->SetEntryL(iNewMessageId);

	// create the index entry
	TMsvEntry entry;
	entry.iMtm = iMsgType;
	entry.iServiceId = ServiceId();
	entry.iType = KUidMsvAttachmentEntry;
	entry.iDate.UniversalTime();
	entry.SetAttachment(ETrue);

	if (iAttachmentInfoList.Count())
		{
		// set the size and attachment name of the entry
		CMsvAttachment* attachment = iAttachmentInfoList[iAttachmentCount];
		entry.iSize = iAttachmentInfoList[iAttachmentCount]->Size();
		if(iAttachmentInfoList[iAttachmentCount]->AttachmentName().Length() == 0)
			{
			TParse parseFile;
			parseFile.Set(iAttachmentInfoList[iAttachmentCount]->FilePath(),NULL,NULL);
			entry.iDetails.Set(parseFile.NameAndExt());
			}
		else 
			{
			entry.iDetails.Set(iAttachmentInfoList[iAttachmentCount]->AttachmentName());
			}	
		}
	delete iMsvOperation;
	iMsvOperation = NULL;
	iMsvOperation = iMsvEntry->CreateL(entry, iStatus);
	}

void CImEmailOperation::StoreAttachmentL()
	{
	RFile origAttachmentFile;
	if (iAttachmentInfoList[iAttachmentCount]->Type() == CMsvAttachment::EMsvFile)
		{
		// Need to get a file handle for the original attachment.
		TMsvId origAttachId = iAttachmentInfoList[iAttachmentCount]->Id();
		iMsvEntry->SetEntryL(origAttachId);
		CMsvStore* store = iMsvEntry->ReadStoreL();
		CleanupStack::PushL(store);
		MMsvAttachmentManager& origAttachmentMgr = store->AttachmentManagerL();

		// This is an email attachment entry, so it only has one attachment per entry
		origAttachmentFile = origAttachmentMgr.GetAttachmentFileL(0);
		CleanupStack::PopAndDestroy(store);
		CleanupClosePushL(origAttachmentFile);
		}

	TMsvId msvId = McliUtils::GetProgressIdL(*iMsvOperation);
	iMsvEntry->SetEntryL(msvId);//pointing to the attachment
	iStore = iMsvEntry->EditStoreL();

	MMsvAttachmentManager& attachmentMgr = iStore->AttachmentManagerL();
	// Reset the Id to 1, email attachments have one entry 
	iAttachmentInfoList[iAttachmentCount]->SetId(1);
	CMsvAttachment* attachment = CMsvAttachment::NewL(*iAttachmentInfoList[iAttachmentCount]);

	CleanupStack::PushL(attachment);

 	if(attachment->Type() == CMsvAttachment::EMsvFile)
 		{
		attachmentMgr.AddAttachmentL(origAttachmentFile, attachment, iStatus);
		CleanupStack::Pop(2, &origAttachmentFile);   // attachment, origAttachmentFile
 		}
 	else if(attachment->Type() == CMsvAttachment::EMsvLinkedFile)
 		{
 		attachmentMgr.AddLinkedAttachmentL(attachment->FilePath(), attachment, iStatus);
 		CleanupStack::Pop(attachment);
 		}
 	else if(attachment->Type() == CMsvAttachment::EMsvMessageEntry)
 		{
 		attachmentMgr.AddEntryAsAttachmentL(attachment->EntryAttachmentId(), attachment, iStatus);
 		CleanupStack::Pop(attachment);
 		}
 	else
 		{
 		// If we have hit this line then a new attachment type has been added
 		// so this function will need to be updated.
 		__ASSERT_DEBUG(0, gPanic(EImEmailOpUnknownAttachmentType));
 		}
	}

void CImEmailOperation::AddMessageAsAttachmentL()
	{
	TMsvId serviceId;
	TMsvEmailEntry entry;

	iMsvEntry->Session().GetEntry(iEmailMessage->Selection()[--iEmbeddedMessagesToProcess], serviceId, entry);
	iMsvEntry->SetEntryL(entry.Parent());

	delete iMsvOperation;
	iMsvOperation = NULL;
	if (iMultipartMixedFolderCreated)
		iMsvOperation = iMsvEntry->CopyL(iEmailMessage->Selection()[iEmbeddedMessagesToProcess], iMultipartMixedId, iStatus);
	else 
		iMsvOperation = iMsvEntry->CopyL(iEmailMessage->Selection()[iEmbeddedMessagesToProcess], iNewMessageId, iStatus);
	}

void CImEmailOperation::CreateMultipartAlternativeFolderEntryL()
	{
	if (iMultipartMixedFolderCreated)
		iMsvEntry->SetEntryL(iMultipartMixedId);
	else 
		iMsvEntry->SetEntryL(iNewMessageId);

	TMsvEmailEntry entry;
	entry.iType = KUidMsvFolderEntry;
	entry.iMtm = iMsvEntry->Entry().iMtm;
	entry.iServiceId = iMsvEntry->Entry().iServiceId;
	entry.SetMessageFolderType(EFolderTypeAlternative);
	entry.iSize = 0;

	delete iMsvOperation;
	iMsvOperation = NULL;
	iMsvOperation = iMsvEntry->CreateL(entry, iStatus);
	}

void CImEmailOperation::AppendHtmlAttachmentL()
	{
	TBool copyHtml = EFalse;
	HBufC* temp = NULL;
	temp = iEmailMessage->GetUniversalResourceIdentifierL(iHtmlId, copyHtml);
	delete temp;
	temp = NULL;
	if (copyHtml)
		{
		iMsvEntry->SetEntryL(iHtmlId);
		TMsvEmailEntry entry(iMsvEntry->Entry());
			CMsvStore* store = iMsvEntry->ReadStoreL();
			CleanupStack::PushL(store);
			MMsvAttachmentManager& attachmentMgr = store->AttachmentManagerL();
			if(attachmentMgr.AttachmentCount())
				{
				// as this is email attachment entry , it has one attachment per entry
				CMsvAttachment* attachment = attachmentMgr.GetAttachmentInfoL(0);
				CleanupStack::PushL(attachment);
				
				attachment->SetSize(entry.iSize);
				attachment->SetComplete(entry.Complete());
				// def070915 - propagated fix
				attachment->SetId(iHtmlId);
				CleanupStack::Pop(attachment);
				iAttachmentInfoList.AppendL(attachment);
				++iTotalAttachments;
				iPartList=iPartList|KMsvMessagePartAttachments;
				}
			CleanupStack::PopAndDestroy(store);	
		}
	RequestComplete(KErrNone);
	}

/**
Creates another CImEmailOperation object that creates a copy of the original
message as an attachment to this message.

The partlist supplied to the current CImEmailOperation is passed to the new one
but ensuring that the orginator and recipient lists are included in the copy.

The copied message is attached as a child of the Multipart Mixed folder entry if
it exists or as a child of this message's message entry.
*/
void CImEmailOperation::AttachOriginalMessageToNewMessageL()
	{
	TMsvPartList partList = iPartList | KMsvMessagePartRecipient | KMsvMessagePartOriginator;

	TInt typeList = 0;
	if( iOrigMessageHtml )
		typeList |= KMsvEmailTypeListMHTMLMessage;

	delete iMsvOperation;
	iMsvOperation = NULL;
	if( iMultipartMixedFolderCreated )
		iMsvOperation = CImEmailOperation::CreateCopyL(iStatus, iMsvSession, iOrigMessageId, iMultipartMixedId, partList, typeList, iMsgType);
	else 
		iMsvOperation = CImEmailOperation::CreateCopyL(iStatus, iMsvSession, iOrigMessageId, iNewMessageId, partList, typeList, iMsgType);
	}

void CImEmailOperation::CompleteEmailOperationL()
	{
	//	1.	Set the Details (and Description if applicable)
	//	2.	Set the Size = Size of Header + Size of Body + Size of Attachments
	
	iMsvEntry->SetEntryL(iNewMessageId);	//pointing to the message
	TMsvEmailEntry entry(iMsvEntry->Entry());

	// Set the service id and the related id if the message we have created is a PCMail
	// Note that this only needs to be done for Reply as otherwise we don't know the service.
	// The service id will already be set for New and Forward PCMails if the service id was passed
	// into the Constructor.
	if( iMsgType == KUidMsgTypePCMail )
		entry.iServiceId = entry.iRelatedId = ServiceId();

	// if there are multiple recipients then set the flag
	if( (iNewHeader->ToRecipients().Count() + iNewHeader->CcRecipients().Count() + iNewHeader->BccRecipients().Count()) > 1 )
		entry.SetMultipleRecipients(ETrue);

	if( iNewHeader->ToRecipients().Count() )
		entry.iDetails.Set(iNewHeader->ToRecipients()[0]);
	else if( iNewHeader->CcRecipients().Count() )
		entry.iDetails.Set(iNewHeader->CcRecipients()[0]);
	else if( iNewHeader->BccRecipients().Count() )
		entry.iDetails.Set(iNewHeader->BccRecipients()[0]);
	//else do nothing as there are no recipients yet!

	entry.iDescription.Set(iNewHeader->Subject());
	entry.iDate.UniversalTime();
	
	TInt sizeOfHeader = iNewHeader->DataSize();
	
	// only add the size of the rich text if the body is required or a signature has been added
	// as otherwise it is not added
	TInt sizeOfBody = 0;
	if( iSignatureText || (iPartList & KMsvMessagePartBody) )
		{
		if(iUsePlainTextStorage)
			{
			iEmailMessage->GetBodyTextEntryIdL(iNewMessageId,CImEmailMessage::EThisMessageOnly);
			sizeOfBody = GetPlainBodyTextSizeL();
			iMsvEntry->SetEntryL(iNewMessageId);
			}
		else
			{
			// DocumentLength() gives no. of chars and since unicode we multiply by 2
			sizeOfBody = (iRichText->DocumentLength() * 2);
			}
			
		}
	
	TInt sizeOfAttachments = 0;
	if( (iOperation != ENew) && (iPartList & KMsvMessagePartAttachments) )
		{
		for( TInt i=0; i<iTotalAttachments; ++i )
			{
			sizeOfAttachments += iAttachmentInfoList[i]->Size();
			}
		}

	TInt sizeOfAttachedMessage = 0;
	if( iOperation & EAttachOriginal )
		{
		TMsvId serviceId;
		TMsvEmailEntry attachedMessageEntry;
		iMsvEntry->Session().GetEntry(iAttachedMessageId, serviceId, attachedMessageEntry);
		sizeOfAttachedMessage = attachedMessageEntry.iSize;
		}

	entry.iSize = sizeOfHeader + sizeOfBody + sizeOfAttachments + sizeOfAttachedMessage + iVCardAndHtmlSize;

	// For receipt messages set the sending state to KMsvSendStateWaiting as receipts can be sent
	// immediately
	// For all others messages, use the setting in the SMTP settings
	if( iOperation == EReceipt )
		entry.SetSendingState(KMsvSendStateWaiting);
	else if( iMsgType == KUidMsgTypeSMTP && iOperation != ECopy )
		{
		TUint sendState = 0;
		switch( iSmtpSettings->SendMessageOption() )
			{
		case ESendMessageImmediately: // No send state for this !!
		case ESendMessageOnNextConnection:
			sendState = KMsvSendStateWaiting;
			break;
		case ESendMessageOnRequest:
		default:
			sendState = KMsvSendStateUponRequest;
			break;
			}
		entry.SetSendingState(sendState);
		}

	// If a VCard has been added then the attachment flag needs to be set
	if( iVcardId || (iTotalAttachments > 0) )
		{
		entry.SetAttachment(ETrue);
		}

	if(iVcardId)
		{
		// The vcard is set on the actual email entry.
		entry.SetVCard(ETrue);
		}

	if( iOperation != ECopy )
		{
		// Adding a receipt flag or adding body encoding to the header does not
		// apply to ECopy operations.

		// Add receipt flag if required (check SMTP settings)
		if( (iMsgType == KUidMsgTypeSMTP) && (iOperation != EReceipt) && (iSmtpServiceId != KMsvUnknownServiceIndexEntryId) )
			{
			if( iSmtpSettings->RequestReceipts() )
				{
				if( iSmtpSettings->ReceiptAddress().Length() )
					iNewHeader->SetReceiptAddressL(iSmtpSettings->ReceiptAddress());
				else if( iSmtpSettings->ReplyToAddress().Length() )
					iNewHeader->SetReceiptAddressL(iSmtpSettings->ReplyToAddress());
				// else don't know who to send a receipt to - receipt will not be sent.
				
				if( iNewHeader->ReceiptAddress().Length() )
					entry.SetReceipt(ETrue);
				}
			}

		// Store the body encoding in the header from the SMTP account settings
		// if the user is creating a plaintext message even though the settings
		// specify HTML message, the body encoding is set to EMsgOutboxMIME
		if( iCreateHtmlMessage )
			iNewHeader->SetBodyEncoding(EMsgOutboxMHTMLAlternativeAsMIME);
		else if( (iMsgType == KUidMsgTypeSMTP) || (iSmtpSettings->BodyEncoding() != EMsgOutboxMHTMLAlternativeAsMIME) )
			iNewHeader->SetBodyEncoding(iSmtpSettings->BodyEncoding());	
		else
			iNewHeader->SetBodyEncoding(EMsgOutboxMIME);
		}

	CMsvStore* store = iMsvEntry->EditStoreL();
	CleanupStack::PushL(store);
	// store the header - this has not been done previously
	iNewHeader->StoreL(*store);

	if( iMultipartMixedFolderCreated || iMultipartAlternativeFolderCreated )
		{
		CImMimeHeader* mimeHeader = CImMimeHeader::NewLC();
		mimeHeader->SetContentTypeL(KMimeMultipart);
		if( iMultipartMixedFolderCreated )
			mimeHeader->SetContentSubTypeL(KMimeMixed);
		else
			mimeHeader->SetContentSubTypeL(KMimeAlternative);
		mimeHeader->SetContentTransferEncodingL(KMimeQuotedPrintable);
		mimeHeader->StoreL(*store);
		entry.iSize += mimeHeader->Size();
		CleanupStack::PopAndDestroy(mimeHeader);
		}
	store->CommitL();
	CleanupStack::PopAndDestroy(store);
	delete iMsvOperation;
	iMsvOperation=NULL;
	iMsvOperation=iMsvEntry->ChangeL(entry, iStatus);
	}

void CImEmailOperation::CreateHTMLEntryL()
	{
	iMsvEntry->SetEntryL(iMultipartAlternativeId);

	TMsvEntry entry;
	entry.iServiceId = ServiceId();
	entry.iType = KUidMsvEmailHtmlEntry;
	entry.iMtm = iMsvEntry->Entry().iMtm;
	entry.iDate.UniversalTime();
	entry.iSize = 0;

	delete iMsvOperation;
	iMsvOperation = NULL;
	iMsvOperation = iMsvEntry->CreateL(entry, iStatus);
	iHtmlId = iMsvEntry->Entry().Id();
	}
	
void CImEmailOperation::AddMessageAttachmentInfoL(TMsvId aAttachmentMessageId)
	{
	iMsvEntry->SetEntryL(aAttachmentMessageId);
	
	iStore = iMsvEntry->EditStoreL();
	MMsvAttachmentManager& attachmentMgr = iStore->AttachmentManagerL();

	CMsvAttachment* attachment = CMsvAttachment::NewL(CMsvAttachment::EMsvMessageEntry); 
	CleanupStack::PushL(attachment);
	attachmentMgr.AddEntryAsAttachmentL(aAttachmentMessageId,attachment,iStatus);
	CleanupStack::Pop(attachment);
	}
void CImEmailOperation::CreateDefaultAttachmentL()
	{
	MMsvAttachmentManager& attachmentMgr = iStore->AttachmentManagerL();
	CMsvAttachment* attachment = CMsvAttachment::NewL(CMsvAttachment::EMsvFile);
	TFileName fileName;
	ReadDefaultHtmlAttachmentNameL(fileName);
	attachmentMgr.CreateAttachmentL(fileName,iFile,attachment,iStatus);
	}

void CImEmailOperation::ReadDefaultHtmlAttachmentNameL(TDes& aFileName)
	{
	RResourceFile resourceFile;
	OpenResourceFileL(resourceFile, iMsvSession.FileSession());
	CleanupClosePushL(resourceFile);
	
	TResourceReader reader;
	HBufC8* buf = resourceFile.AllocReadLC( DEFAULT_ATTACHMENT_NAME );
	reader.SetBuffer(buf);
	
	TPtrC attachmentName = reader.ReadTPtrC();
	const TDesC& ext = KMimeHtmlExtension;
	HBufC* defaultAttachmentName = HBufC::NewL(attachmentName.Length() + ext.Length());
	CleanupStack::PushL(defaultAttachmentName);

	defaultAttachmentName->Des().Copy(attachmentName);
	defaultAttachmentName->Des().Append(ext);
	aFileName.Append(*defaultAttachmentName);

	CleanupStack::PopAndDestroy(3,&resourceFile ); //buf, defaultAttachmentName,resourceFile (Close resourceFile)
	}
	
void CImEmailOperation::ResetStoreL()	
	{
	iStore->CommitL();
	delete iStore;
	iStore = NULL;
	}

/**
Method that will be called when object in RPointerArray<CMsvPlainBodyText> is deleted.
@param aPtr		Pointer to the RPointerArray.
@return void.
*/
void CImEmailOperation::ClosePlainBodyTextArray( TAny* aPtr )
	{
	RPointerArray<CMsvPlainBodyText>* plainBodyTextArray = reinterpret_cast<RPointerArray<CMsvPlainBodyText>*> (aPtr);
	plainBodyTextArray->ResetAndDestroy();
	}

/**
Return the size of the body text for a email message.
*/
TInt CImEmailOperation::GetPlainBodyTextSizeL()
	{
	TInt count = iEmailMessage->Selection().Count();
	TInt sizeOfBody = 0;		
	for(TInt i=0; i< count ;++i)
		{
		iMsvEntry->SetEntryL(iEmailMessage->Selection().At(i));	//pointing to the text entry	
		CMsvStore* bodyTextStore = iMsvEntry->ReadStoreL();
		CleanupStack::PushL(bodyTextStore);
			
		CMsvPlainBodyText* plainBodyText = bodyTextStore->InitialisePlainBodyTextForReadL(KMaxChunkLength);
		sizeOfBody += plainBodyText->Size();
				
		delete plainBodyText;
		plainBodyText = NULL;
			
		CleanupStack::PopAndDestroy(bodyTextStore);
		bodyTextStore = NULL;
		}
	return 	sizeOfBody;
	}
	
void CImHtmlConverter::ResetStoreL()
	{
	if(iStore != NULL )
		{
		iStore->CommitL();
		delete iStore;
		iStore = NULL;
		}
	}
	
 void CImHtmlConverter::ResetStoreWithoutCommit()
 	{
 	if (this != NULL)
	 	{
	 	if(iStore != NULL )
	 		{
	 		delete iStore;
	 		iStore = NULL;
	 		}	
	 	}
  	}

//-----------------------------------------------------------------

//-----------------------------------------------------------------

CImHtmlConverter* CImHtmlConverter::NewL(CMsvEntry& aMsvEntry, 
								   CParaFormatLayer& aParaLayer, CCharFormatLayer& aCharLayer)
	{
	CImHtmlConverter* self = new (ELeave) CImHtmlConverter(aMsvEntry, aParaLayer, aCharLayer);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

CImHtmlConverter::CImHtmlConverter(CMsvEntry& aMsvEntry, CParaFormatLayer& aParaLayer, CCharFormatLayer& aCharLayer)
:iMsvEntry(aMsvEntry), iParaLayer(aParaLayer), iCharLayer(aCharLayer)
{
}

void CImHtmlConverter::ConstructL()
	{
	_LIT8(KHtmlInMimeType, "epoc32/crichtext");
	_LIT8(KHtmlOutMimeType, "text/html");

	TDataType inMimeType(KHtmlInMimeType);
	TDataType outMimeType(KHtmlOutMimeType);

	iConverterList = CCnaConverterList::NewL();
	iConverterList->UpdateL();

	TUid uid = iConverterList->ConverterL(inMimeType, outMimeType);

	iToHTMLConverter = iConverterList->NewConverterL(uid);
	User::LeaveIfNull(iToHTMLConverter);
	User::LeaveIfError(iToHTMLConverter->Capabilities() & CConverterBase::EConvertsFiles);
	}

void CImHtmlConverter::PrepareToStoreHTMLEntryTextL (TMsvId& aHtmlId, const TMsvId aTextId)
	{
	// 0. Check html entry exists.
	if (!aHtmlId)
		{
		iMsvEntry.SetEntryL(iMsvEntry.Entry().Parent());	//pointing to the text entry
		if ( ((TMsvEmailEntry)iMsvEntry.Entry()).MessageFolderType() != EFolderTypeAlternative)
			return;
		TInt i = iMsvEntry.Count();
		while (i--)
			{
			if (aTextId!=iMsvEntry[i].Id())
				{
				aHtmlId=iMsvEntry[i].Id();
				break;
				}
			}
		}

	if (aHtmlId <=0)
		return;

	// 1. Create file containing Rich text.

	iMsvEntry.SetEntryL(aTextId);	//pointing to the text entry
	TFileName filepath;
// Create the file with Rich text in non secure version 

	CMsvStore* store=NULL;
    TRAPD(err,store=iMsvEntry.ReadStoreL());
 	if (err)
		{
		aHtmlId=0;
	    User::Leave(err);
		}
	CleanupStack::PushL(store);

	if ( !store->IsPresentL(KMsvEntryRichTextBody) && !store->IsPresentL(KMsvPlainBodyText16) && !store->IsPresentL(KMsvPlainBodyText8))
		{
		aHtmlId=0;
		User::Leave(KErrNotFound); // Rich text not found
		}

	CRichText* bodyText=CRichText::NewL(&iParaLayer,&iCharLayer);
	CleanupStack::PushL(bodyText);
	store->RestoreBodyTextL(*bodyText);
	
	if (bodyText->PictureCount())
		{
		RMsvReadStream in;
		in.OpenL(*store, KMsvEntryRichTextBody);
		CEmbeddedStore* embeddedStore = CEmbeddedStore::FromL(in);
	
		// Create store resolver
		CImStoreResolver* storeResolver = new(ELeave)CImStoreResolver(embeddedStore); // Takes ownership
		CleanupStack::PushL(storeResolver);
	
		// Set the picture factory
		__ASSERT_ALWAYS(CEikonEnv::Static(), gPanic(EImcmNoEikonEnvironment));
		MPictureFactory* factory = CEikonEnv::Static()->PictureFactory();
		bodyText->SetPictureFactory(factory, storeResolver);
		bodyText->DetachFromStoreL(CPicture::EDetachFull);
		CleanupStack::PopAndDestroy(storeResolver);
		}
	CleanupStack::Pop(bodyText);
	delete iRichText;
	iRichText = NULL;
	iRichText = bodyText;
	TPckgBuf<CRichText*> bufferRichText(iRichText); //text to be converted
	iSourceStream.Open(bufferRichText);
	
	CleanupStack::PopAndDestroy(store);
	// 2. Convert to HTML file
	iMsvEntry.SetEntryL(aHtmlId);
	iStore = iMsvEntry.EditStoreL();
	MMsvAttachmentManager& attachmentMgr = iStore->AttachmentManagerL();
	iFile = attachmentMgr.GetAttachmentFileForWriteL(0);
	// open the write stream setting with file handle
	iTargetStream.Attach(iFile);
	// Prepares for asyn conversion. 	
	iHtmlId=aHtmlId;
	iTextId=aTextId;
	
	iToHTMLConverter->ConvertObjectAL(iSourceStream,iTargetStream);
	}
	
TBool CImHtmlConverter::StoreHTMLEntryTextAL(TRequestStatus& aStatus)
	{
	TBool finishedConvertingHTML = (!iToHTMLConverter->DoConvertL());
			
	if (finishedConvertingHTML)
		{
		TFileName filepath;
		RDir dir;
		TEntry entry;
		TParse dirPath;
		iToHTMLConverter->CancelConvert();
		iSourceStream.Close();
		iTargetStream.CommitL();
		iTargetStream.Close();
		iFile.Close();
		ResetStoreL();

		// Set to html attachment entry
		iMsvEntry.SetEntryL(iHtmlId);
		iStore = iMsvEntry.EditStoreL();
		MMsvAttachmentManager& attachmentMgr = iStore->AttachmentManagerL();
		
		// Email Mtm will have one attachment per attachment entry , ehnce index 0
		CMsvAttachment* attachment = attachmentMgr.GetAttachmentInfoL(0);
		RFile file = attachmentMgr.GetAttachmentFileL(0);
		CleanupStack::PushL(attachment);
		TInt error = file.Size(iSize);
		if(error == KErrNone)
			attachment->SetSize(iSize);
		file.Close();
		TFileName filename = HtmlFilename(iMsvEntry, GetDefaultAttachmentName());
		if(filename.Length() == 0)
		    {
		    User::Leave(KErrArgument);
		    }

		attachment->SetAttachmentNameL(filename);
		CleanupStack::Pop(attachment); // ownership passed to attachment manager 
		
		attachmentMgr.ModifyAttachmentInfoL(attachment,aStatus);
		return 	finishedConvertingHTML; // true
		}
	return finishedConvertingHTML; // false
	}	
	
CMsvOperation*CImHtmlConverter::ChangeHTMLTextInEnrtyL(TRequestStatus& aStatus)
	{
	TMsvEmailEntry emailEntry = iMsvEntry.Entry();
	
	TFileName filename = HtmlFilename(iMsvEntry, GetDefaultAttachmentName());
	if(filename.Length() == 0)
		{
		User::Leave(KErrArgument);
		}

	emailEntry.iDetails.Set(filename);
	emailEntry.iSize = iSize;	
	return iMsvEntry.ChangeL(emailEntry, aStatus);	
	}

TFileName CImHtmlConverter::HtmlFilename(CMsvEntry& aEntry, TPtrC aFileName)
	{
	TFileName name;
	name.Zero();

	TMsvEntry entry = aEntry.Entry();	
// this is not used at all , need to get rid of this 
	if (entry.iDetails.Length())
		name=entry.iDetails;
	else if (aFileName.Length())
		name.Copy(aFileName);
	return name;
	}

void CImHtmlConverter::ReadDefaultAttachmentNameL(RResourceFile& resourceFile)
	{	
	TResourceReader reader;
	HBufC8* buf;

	buf = resourceFile.AllocReadLC( DEFAULT_ATTACHMENT_NAME );
	reader.SetBuffer(buf);
	TPtrC attachmentName = reader.ReadTPtrC();
	const TDesC& ext = KMimeHtmlExtension;
	iDefaultAttachmentName = HBufC::NewL(attachmentName.Length() + ext.Length());
	iDefaultAttachmentName->Des().Copy(attachmentName);
	iDefaultAttachmentName->Des().Append(ext);
	CleanupStack::PopAndDestroy(); // buf
	}

TPtrC CImHtmlConverter::GetDefaultAttachmentName()
	{
	return (iDefaultAttachmentName) ? iDefaultAttachmentName->Des() : TPtrC();
	}

CImHtmlConverter::~CImHtmlConverter()
	{
	delete iToHTMLConverter;
	delete iDefaultAttachmentName;
	delete iConverterList;
	delete iRichText;
	iSourceStream.Close();
	iTargetStream.Close();
	iFile.Close();
	delete iStore;
	}

TInt CImHtmlConverter::Size() const
	{
	return iSize;
	}


//**********************************
// CImStoreResolver
//**********************************

CImStoreResolver::CImStoreResolver(CStreamStore* aStore)
: iStore(aStore)
	{
	}

CImStoreResolver::~CImStoreResolver()
	{
	delete iStore;
	}

const CStreamStore& CImStoreResolver::StreamStoreL(TInt) const
	{
	return *iStore;
	}

