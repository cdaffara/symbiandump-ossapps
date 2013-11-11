// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
// cimmessagepart.h
//

/**
 * @file 
 * @
 * @released
 */

#ifndef __CIMMESSAGEPART_H__
#define __CIMMESSAGEPART_H__

class CImEmailMessage;
class CImMimeHeader;
class CMsvMimeHeaders;
class CImHtmlConverter;

class TImCreateMessageOptions
/**
@internalComponent
@released
*/
	{
public:
	TMsvPartList		iMsvPartList;
	TMsvEmailTypeList	iMsvEmailTypeList;
	TUid				iMessageType;
	};


class CImRemoveMessagePart : public CMsvOperation
/**
@internalComponent
@released
*/
	{
public:
	static CImRemoveMessagePart* DeleteAttachmentL(TRequestStatus &aStatus, CMsvEntry& aMsvEntry, TMsvId aMessageId, TMsvId aAttachmentId);
	static CImRemoveMessagePart* DeleteAttachedMessageL(TRequestStatus &aStatus, CMsvEntry& aMsvEntry, TMsvId aMessageId, TMsvId aAttachedMessageId);
	~CImRemoveMessagePart();
	const TDesC8& FinalProgress();
	void DoCancel();
	void RunL();
	const TDesC8& ProgressL();		// returns NULL until operation completed - then returns MessageId
	TMsvId RemovedAttachmentId();	
private:
	CImRemoveMessagePart(TRequestStatus& aStatus, CMsvEntry& aMsvEntry, TMsvId aMessageId);
	void ConstructL(TMsvId aAttachmentId);
	void ErrorRecovery(TInt aError);
	void SelectNextStateL();					// selects next state to go to
	void ChangeStateL();					// initiates the next state operation
	void SelectAndChangeToNextStateL();
	void RequestComplete(TInt aError);
	void Recover();							// leave message in a 'good' state

	void CheckAttachmentParentTypeL();
	void MoveOtherEntryToParentOfFolderL();
	void DeleteAttachmentEntryL();
	void DeleteFolderEntryL();
	void CompleteRemoveMessagePartL();
private:
	enum TImRemoveMessagePartState
		{
		ECheckAttachmentParentType,
		EMoveOtherEntryToParentOfFolder,
		EDeleteAttachmentEntry,
		EDeleteFolderEntry,
		ECompleteRemoveMessagePart,
		EFinished
		};
	CMsvEntry&			iMsvEntry;
	CMsvOperation*		iMsvOperation;

	CMsvEntrySelection*	iMessageEntrySelection;
	CImEmailMessage*	iEmailMessage;

	TInt				iState;
	TPckgBuf<TMsvId>	iDataMember;
	TMsvId				iMessageId;
	TMsvId				iFolderToDeleteId;
	TInt				iAttachmentSize;
	TMsvId				iAttachmentId;
	TMsvId				iEntryToMoveId;
	};

class CImStoreMessagePart : public CMsvOperation
/**
@internalComponent
@released
*/
	{
public:
	static CImStoreMessagePart* AddAttachmentL(TMsvId aMessageId, const TDesC& aFilePath, CMsvEntry& aMsvEntry,CMsvAttachment* aAttachmentInfo,TRequestStatus& aStatus );
	static CImStoreMessagePart* AddEntryAsAttachmentL(TMsvId aMessageId,TMsvId aAttachmentMessageId, CMsvEntry& aMsvEntry, CMsvAttachment* aAttachmentInfo,TRequestStatus& aStatus);
	static CImStoreMessagePart* AddAttachmentL(TMsvId aMessageId,RFile& aFile, CMsvEntry& aMsvEntry, CMsvAttachment* aAttachmentInfo, TRequestStatus& aStatus);
	static CImStoreMessagePart* AddLinkedAttachmentL(TMsvId aMessageId,const TDesC& aFilePath, CMsvEntry& aMsvEntry,CMsvAttachment* aAttachmentInfo,TRequestStatus& aStatus );	
	static CImStoreMessagePart* CreateAttachmentL(TMsvId aMessageId,const TDesC& aFileName, RFile& aAttachmentFile,CMsvEntry& aMsvEntry, CMsvAttachment* aAttachmentInfo, TRequestStatus& aStatus);
	static CImStoreMessagePart* AddRelatedPartL(TRequestStatus &aStatus, CMsvEntry& aMsvEntry, TMsvId aMessageId, const TDesC& aAttachmentFullName, TMsvId aRelatedPartId, const TDesC8& aContentId);
	static CImStoreMessagePart* StoreBodyTextL(TRequestStatus& aStatus, CMsvEntry& aMsvEntry, TMsvId aMessageId, CRichText& aRichText, TBool aUsePlainTextStorage = EFalse);
	static CImStoreMessagePart* StoreBodyTextWithMimeHeaderL(TRequestStatus& aStatus, CMsvEntry& aMsvEntry, TMsvId aMessageId, CRichText& aRichText, const CImMimeHeader& aMimeHeader, TBool aUsePlainTextStorage = EFalse);
	static CImStoreMessagePart* StorePlainBodyTextL(TRequestStatus& aStatus, CMsvEntry& aMsvEntry, TMsvId aMessageId, TBool aUsePlainTextStorage = ETrue);
	static CImStoreMessagePart* StorePlainBodyTextL(TRequestStatus& aStatus, CMsvEntry& aMsvEntry, TMsvId aMessageId, CImMimeHeader& aMimeHeader, TBool aUsePlainTextStorage = ETrue);
	~CImStoreMessagePart();
	const TDesC8& FinalProgress();
	void DoCancel();
	void RunL();
	const TDesC8& ProgressL();
private:
	enum TImMessagePart
		{
		EMessagePartAttachment,
		EMessagePartBody,
		EMessagePartMessageAttachment
		};
	void ConstructL(RFile& aFile);
	void ConstructL(RFile& aFile,const TDesC& aAttachmentName);
	void ConstructL(const TDesC& aAttachmentFullName, const TDesC8& aContentId);
	void ConstructAttachmentInfo(CMsvAttachment* aAttachmentInfo,TInt aAttachmentCreationState);
	CImStoreMessagePart(TRequestStatus& aStatus, CMsvEntry& aMsvEntry, TMsvId aMessageId, TBool aUsePlainTextStorage = EFalse);
	
	void ConstructL();
	void ConstructL(const TDesC& aAttachmentFullName);
	void ConstructL(TMsvId aAttachmentMessageId);
	void ConstructL(const TDesC& aAttachmentFullName, TMsvId aRelatedPartId, const TDesC8& aContentId);
	void ConstructL(CRichText& aRichText);
	void ConstructL(CRichText& aRichText, const CImMimeHeader& aMimeHeader);
	void ConstructL(CImMimeHeader& aMimeHeader);
	void ConstructL(TImMessagePart aMessagePart);
	void ErrorRecovery(TInt error);
	void SelectNextStateL();				// selects next state to go to
	void ChangeStateL();					// initiates the next state operation
	void SelectAndChangeToNextStateL();
	void RequestComplete(TInt aError);
	void Recover();

	TBool HTMLMessage() const;
	void SetHTMLMessage(TBool aFlag);
	TBool MultipartMixedExists() const;
	void SetMultipartMixedExists(TBool aFlag);
	TBool MultipartMixedCreated() const;
	void SetMultipartMixedCreated(TBool aFlag);
	TBool MultipartAlternativeExists() const;
	void SetMultipartAlternativeExists(TBool aFlag);
	TBool MultipartAlternativeCreated() const;
	void SetMultipartAlternativeCreated(TBool aFlag);
	TBool MultipartRelatedExists() const;
	void SetMultipartRelatedExists(TBool aFlag);
	TBool MultipartRelatedCreated() const;
	void SetMultipartRelatedCreated(TBool aFlag);
	TBool TextPartExists() const;
	void SetTextPartExists(TBool aFlag);
	TBool TextPartCreated() const;
	void SetTextPartCreated(TBool aFlag);
 	TBool AttachmentEntryCreated() const;
	void SetAttachmentEntryCreated(TBool aFlag);

	void FindMultipartRelatedFolderL();
	void CheckForSubfolderL();
	void CreateAttachmentEntryL();
	void CopyOrigMessageToMessageL();
	void CreateMultipartMixedFolderEntryL();
	void CreateMultipartAlternativeFolderEntryL();
	void CreateMultipartRelatedFolderEntryL();
	void MoveOriginalMessageEntryChildrenToNewFolderL();
	void CreateTextEntryL();
	void StoreRichTextL();
	void CompleteStoreMessagePartL();
	void CreateHTMLEntryL();
	void PrepareToStoreHTMLEntryTextL();
	void StoreHTMLEntryTextL();
	void RemoveHTMLEntryL();
	void MoveTextEntryToAltFolderL();
	TPtrC GetDefaultAttachmentName();
	void OpenAndReadResourceFileL();
	void StoreMimeHeaderL();
	TBool CreateAttachmentMimeHeaderL(CImMimeHeader& aMimeHeader, const TDesC& aDetails);
	void CreateFolderMimeHeaderL(CImMimeHeader& aMimeHeader);
	void CreateMessageMimeHeaderL(CImMimeHeader& aMimeHeader);
	void AddAttachmentL();
	void AddAttachmentUsingFileHandleL();
	void AddLinkedAttachmentL();
	void AddEntryAsAttachmentL();	
	void CreateAttachmentL();
	void DoAttachmentStoreL();
	TInt CheckMimeInAttachmentInfoL(CMsvMimeHeaders& aMsvMimeHeaders);
	CImMimeHeader* ProcessAttachmentMimeHeadersL();
	void SetEntryDetailsL(TMsvEntry& aEntry);
	void StorePlainTextL();
private:
	enum TImStoreMessagePartState
		{
		EFindMultipartRelatedFolder,
		ECheckForSubfolder,
		ECreateAttachmentEntry,
		EStoreAttachment,
		ECopyOrigMessageToMessage,
		ECreateMultipartMixedFolderEntry,
		ECreateMultipartRelatedFolderEntry,
		ECreateMultipartAlternativeFolderEntry,
		EMoveOriginalMessageEntryChildrenToNewFolder,
		ECreateTextEntry,
		EStoreRichText,
		ERemoveHTMLEntry,
		EMoveTextToAlternativeFolder,
		ECreateHTMLEntry,
		EPrepareToStoreHTMLEntryText,
		EStoreHTMLEntryText,
		ECompleteStoreMessage,
		EFinished
		,EStoreHTMLTextInEntry,
		EAddEntryAttachment	
		};

	enum TImStoreMessagePartFlags
		{
		KStoreMessagePartClearFlag							= 0x00000000,
		KStoreMessagePartHTMLMessage						= 0x00000001,
		KStoreMessagePartMultipartMixedExists				= 0x00000002,
		KStoreMessagePartMultipartMixedCreated				= 0x00000004,
		KStoreMessagePartMultipartAlternativeExists			= 0x00000008,
		KStoreMessagePartMultipartAlternativeCreated		= 0x00000010,
		KStoreMessagePartTextPartExists						= 0x00000020,
		KStoreMessagePartTextPartCreated					= 0x00000040,
		KStoreMessagePartAttachmentEntryCreated				= 0x00000080,
		KStoreMessagePartOrigMessageEntryChildrenCopied		= 0x00000100,
		KStoreMessagePartOrigMessageEntryChildrenDeleted	= 0x00000200,
		KStoreMessagePartMultipartRelatedExists				= 0x00000400,
		KStoreMessagePartMultipartRelatedCreated			= 0x00000800
		};
	CMsvEntry&			iMsvEntry;
	CMsvOperation*		iMsvOperation;
	
	CRichText*			iRichTextToStore; // I don't own this object!
	CImMimeHeader*		iMimeHeaderOfBodyText;
	CRichText*			iRichText;
	CFileMan*			iFileMan;
	CMsvEntrySelection* iMessageEntrySelection;

	TInt				iState;
	TPckgBuf<TMsvId>	iDataMember;
	TMsvId				iMessageId;
	TImMessagePart		iMessagePart;
	TMsvId				iMixFolderId;
	TMsvId				iAltFolderId;
	TMsvId				iRelFolderId;
	HBufC*				iAttachmentFullName;
	TInt				iAttachmentSize;
	TMsvId				iAttachmentId;
	TMsvId				iTextId;
	TMsvId				iHtmlId;
	TMsvId				iRelatedPartId;
	TMsvId				iAttachmentMessageId;
	TUint32				iFlags;
	HBufC8*				iContentId;
	CImEmailMessage*	iEmailMessage;

	// Used when converting rich text to html
	CParaFormatLayer*	iParaLayer;
	CCharFormatLayer*	iCharLayer;
	CImHtmlConverter* 	iHtmlConverter;
	TBool iFinishedConvertingHTML;
	CMsvAttachment*		iAttachmentInfo;
	CMsvStore* 			iStore;
	TInt 				iAttachmentCreateState;
	TBool				iIsAddByFileHandle;
	RFile*				iFileHandle;
	RFile               iFile;
	// Indicates whether to store message body as CRichText or plaintext.
	TBool 				iUsePlainTextStorage;
	// Size of the plain text body.
	TInt				iSizeOfBody;
	// Size of the Fwd'ed/ReplyTo plaintext body part.
	TInt				iSizeFwdReplyBody;
	// The error value returned when CRichText is populated with message body text.
	TInt 				iRestoreErr;
	enum TImAttachmentCreation
		{
		EAddAttachment,
		EAddAttachmentUsingFileHandle,
		EAddLinkedAttachment,
		EAddEntryAsAttachment,
		ECreateAttachment
		};
	};

class CImHtmlConverter : public CBase
/**
@internalComponent
@released
*/
	{
public:
	static CImHtmlConverter* NewL(CMsvEntry& aMsvEntry, CParaFormatLayer& aParaLayer, CCharFormatLayer& aCharLayer);
	~CImHtmlConverter();
	void PrepareToStoreHTMLEntryTextL(TMsvId& aHtmlId, const TMsvId aTextId);
	void ResetStoreL();
	void ResetStoreWithoutCommit();
	TBool StoreHTMLEntryTextAL(TRequestStatus& aStatus);
	CMsvOperation* ChangeHTMLTextInEnrtyL(TRequestStatus& aStatus);
	
	void ReadDefaultAttachmentNameL( RResourceFile& resourceFile);
	TInt Size() const;

private:
	CImHtmlConverter(CMsvEntry&	aMsvEntry, CParaFormatLayer& aParaLayer, CCharFormatLayer& aCharLayer);
	TFileName HtmlFilename(CMsvEntry& aEntry, TPtrC aFileName);
	TPtrC GetDefaultAttachmentName();
	void ConstructL();

private:
	CMsvEntry&			iMsvEntry;
	CParaFormatLayer&	iParaLayer;
	CCharFormatLayer&	iCharLayer;

	CConverterBase*		iToHTMLConverter;
	CCnaConverterList*	iConverterList;

	TMsvId				iTextId;
	TMsvId				iHtmlId;
	TInt				iSize;
	HBufC*				iDefaultAttachmentName;
	CRichText*			iRichText;
	RDesReadStream 		iSourceStream;
	RFileWriteStream	iTargetStream;
	CMsvStore*			iStore;
	RFile 				iFile;
	};

class CImStoreResolver : public CBase, public MRichTextStoreResolver
/**
@internalComponent
@released
*/
	{
public:
	CImStoreResolver(CStreamStore* aStore);
	~CImStoreResolver();
	//
private:
	const CStreamStore& StreamStoreL(TInt aPos)const;
	//
private:
	CStreamStore* iStore;
	};

#endif //__CIMMESSAGEPART_H__
