// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// CIMPLAINBODYTEXT.CPP
//

#include "MIUTMSG.H"
#include "MIUTCONV.H"
#include "MIUT_ERR.H"
#include "CIMPLAINBODYTEXT.H"
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS 
#include "cimmessagepart.h" 
#include "cimconvertcharconv.h"
#include "miut_errconsts.h"
#endif
#include <cmsvplainbodytext.h>

/**
NewL Factory method.
@param	aEmailMessage.   The CImEmailMessage refernce corresponding to the message whose body text is being processed.
@param	aMsvEntry.		 The CMsvEntry correspoding to the message being processed.
@param  aEntryType.		 The TImEmailEntryType for the message.
@param	aChunkLength.	 The length of the chunk that will be stored/restored in a single operation.
@param  aWriteMode.		 Indicates whether CMsvStore needs to be opened in Read/Write mode.
						 ETrue indicates Write mode.
						 EFalse indicates Read mode.
@return CImPlainBodyText.Pointer to CImPlainBodyText.
*/	
CImPlainBodyText* CImPlainBodyText::NewL(CImEmailMessage& aEmailMessage, CMsvEntry& aMsvEntry, CImEmailMessage::TImEmailEntryType aEntryType, TInt aChunkLength, TBool aWriteMode)
	{
	CImPlainBodyText* self = new (ELeave) CImPlainBodyText(aEmailMessage, aMsvEntry);
	CleanupStack::PushL(self);
	self->ConstructL(aChunkLength, aEntryType, aWriteMode);
	CleanupStack::Pop();
	return self;
	}

/**
Constructor
@param	aEmailMessage. The CImEmailMessage refernce corresponding to the message whose body text is 
@param	aMsvEntry.	   The CMsvEntry correspoding to the message being processed.
@return None.
*/
CImPlainBodyText::CImPlainBodyText(CImEmailMessage& aEmailMessage, CMsvEntry& aMsvEntry)
: CMsgActive(EPriorityNormal), iEmailMessage(aEmailMessage), iMsvEntry(aMsvEntry)
	{
	}

/**
The 2nd Phase construction.
*/	
void CImPlainBodyText::ConstructL(TInt aChunkLength, CImEmailMessage::TImEmailEntryType aEntryType, TBool aWriteMode)
	{
	iMessageId = iMsvEntry.EntryId();
	RArray<TMsvId> textIdArray;
	CleanupClosePushL (textIdArray);
	iEmailMessage.GetBodyTextEntryIdL(iMessageId, aEntryType);	
	
	// A message may contain more than one text part.
	TInt count = iEmailMessage.Selection().Count();
	for(TInt i=0;i<count;++i)
		{
		textIdArray.AppendL(iEmailMessage.Selection().At(i));
		}
		
	TUint charset = 0; // Will contain the charset when GetCharacterSetL returns.
	TBool override = EFalse; // Will be set to ETrue if the existing charset was overidden.
	iEmailMessage.GetCharacterSetL(iMessageId,charset,override);	
	
	if(aWriteMode)
		{
		RFs& fileSvrSession = iMsvEntry.Session().FileSession();
		
		CCnvCharacterSetConverter* characterConverter = CCnvCharacterSetConverter::NewL();
		CleanupStack::PushL(characterConverter);
		
		CImConvertCharconv* charConv = CImConvertCharconv::NewL(*characterConverter, fileSvrSession);
		CleanupStack::PushL(charConv);
		
		TUint defaultCharset = charConv->DefaultCharset();
		CleanupStack::PopAndDestroy(2,characterConverter); // charConv, characterConverter
		
		iMsvEntry.SetEntryL(textIdArray[0]);
		iStore = iMsvEntry.EditStoreL();
		// Body text is stored in MailStore as 16 bit so set iIs8Bit to EFalse.
		iPlainTextArray.AppendL(iStore->InitialisePlainBodyTextForWriteL(EFalse, charset, defaultCharset));	
		}	
	else
		{
		TInt textIdCount = textIdArray.Count();
		for(TInt i=0; i<textIdCount; ++i)
			{
			// Pointing to the text entry.
			iMsvEntry.SetEntryL(textIdArray[i]);	
			CMsvStore* store = iMsvEntry.ReadStoreL();
			CleanupStack::PushL(store);
			
			iPlainTextArray.AppendL( store->InitialisePlainBodyTextForReadL(aChunkLength));
			// if the existing charset was overidden by calling CImEmailMessage::SetCharacterSetL
			if(override)
				{
				iPlainTextArray[i]->SetCharacterSetL(charset);
				}
			CleanupStack::PopAndDestroy(store);
			store = NULL;
			}
		}
	CleanupStack::PopAndDestroy(&textIdArray);
	CActiveScheduler::Add(this);
	}

/**
Destructor
*/
EXPORT_C CImPlainBodyText::~CImPlainBodyText()
	{
	delete iStoreMessagePart;
	iPlainTextArray.ResetAndDestroy();
	iPlainTextArray.Close();	
	delete iStore;
	}

/**
Store the plain body text part in chunks.
@param aChunk. 				The 8 bit chunk that is to be stored.
@param aStatus. 			The TRequestStatus parameter for this request.
@leave KErrAccessDenied.	If CMsvStore was opened in Read mode or
							If CommitL is already called.
@leave Other 				Standard system-wide error codes.
@return void.
*/
EXPORT_C void CImPlainBodyText::StoreChunkL(const TDesC8& aChunk, TRequestStatus& aStatus)
	{
	iPlainTextArray[0]->StoreChunkL(aChunk, aStatus);
	}


/**
Store the plain body text part in chunks,synchronous version.
@param aChunk. 				The 8 bit chunk that is to be stored.
@leave KErrAccessDenied.	If CMsvStore was opened in Read mode or
							If CommitL is already called.
@leave Other 				Standard system-wide error codes.
@return void.
*/
EXPORT_C void CImPlainBodyText::StoreChunkL(const TDesC8& aChunk)
	{
	iPlainTextArray[0]->StoreChunkL(aChunk);
	}

/**
Store the body part in chunks(16 bit version).
@param aChunk 			 The 16 bit chunk that is to be stored.
@param aStatus	 		 The TRequestStatus parameter for this request.
@leave KErrNotSupported  If the 8-bit storage was enabled.
@leave KErrAccessDenied  If CMsvStore was opened in Read mode or
						 IfCommitL is already called.
@leave Other 			 Standard system-wide error codes.
@return void
*/
EXPORT_C void CImPlainBodyText::StoreChunkL(const TDesC16& aChunk, TRequestStatus& aStatus)
	{
	iPlainTextArray[0]->StoreChunkL(aChunk, aStatus);
	}

/**
Store the body part in chunks(16 bit synchronous version).
@param aChunk 			 The 16 bit chunk that is to be stored.
@leave KErrNotSupported  If the 8-bit storage was enabled.						 
@leave KErrAccessDenied  If CMsvStore was opened in Read mode or
						 If CommitL is already called.
@leave Other 			 Standard system-wide error codes.
@return void
*/
EXPORT_C void CImPlainBodyText::StoreChunkL(const TDesC16& aChunk)
	{
	iPlainTextArray[0]->StoreChunkL(aChunk);
	}
	


/**
Converts and stores the CRichText contents to a plain text.
@param aRichText 		 The CRichText object that will be stored as plain body text.
@leave KErrNotSupported  If the 8-bit storage was enabled.
@leave KErrAccessDenied  If CMsvStore was opened in Read mode or
						 If CommitL is already called.
@leave Other 			 Standard system-wide error codes.
@return void.
*/
EXPORT_C void CImPlainBodyText::StoreRichTextAsPlainTextL(CRichText& aRichText)
	{
	iPlainTextArray[0]->StoreRichTextAsPlainTextL(aRichText);
	}

/**
Retrieve the next chunk of the plain body text.
@param aChunk 			 The output parameter contains the requested chunk.
@param aStatus.			 The TRequestStatus for this request.
@leave KErrAccessDenied  If CMsvStore was opened in Write mode.
@leave KErrNotSupported  If 16-bit storage is enabled.
@leave KErrUnderflow 	 If aChunk MaxLength is less than iChunkMaxLength.
@leave Other 				Standard system-wide error codes.
@return void
*/
EXPORT_C void CImPlainBodyText::NextChunkL(TDes8& aChunk, TRequestStatus& aStatus)
	{
	__ASSERT_DEBUG(iChunkRetrievalState == EIdleState, gPanic(EImcmNonIdleRetrievalState));
	iChunkRetrievalState = ENextChunk8Bit;
	
	iChunk8 = &aChunk;
	iChunk16 = NULL;	
	iPlainTextArray[iIndex]->NextChunkL(aChunk, iStatus);
	SetActive();
	Queue(aStatus);
	}

/**
Retrieve the next chunk of the plain body text.
@param aChunk 			 The output parameter contains the requested chunk.
@leave KErrNotSupported  If CMsvStore was opened in Write mode.
@leave KErrNotSupported  If 16-bit storage is enabled.
@leave KErrUnderflow	 If aChunk MaxLength is less than iChunkMaxLength.
@leave Other 			 Standard system-wide error codes.
@return void
*/
EXPORT_C void CImPlainBodyText::NextChunkL(TDes8& aChunk)
	{
	iPlainTextArray[iIndex]->NextChunkL(aChunk);
	// If there is more than one text part for the message move to next text part.
	if(aChunk.Length() == 0 && iIndex + 1 < iPlainTextArray.Count())
		{
		iPlainTextArray[++iIndex]->NextChunkL(aChunk);
		}
	}



/**
Restore the plain body text into chunks.
@param aChunk  			The output parameter contains the requested chunk on completion.
@param aStatus 			The TRequestStatus parameter for this request.
@leave KErrNotSupported If CMsvStore was opened in Write mode.
@leave KErrUnderflow	If aChunk MaxLength is less than iChunkMaxLength.
@leave Other 			Standard system-wide error codes.
@return void
*/
EXPORT_C void CImPlainBodyText::NextChunkL(TDes16& aChunk, TRequestStatus& aStatus)
	{
	__ASSERT_DEBUG(iChunkRetrievalState == EIdleState, gPanic(EImcmNonIdleRetrievalState));
	iChunkRetrievalState = ENextChunk16Bit;
			
	iChunk16 = &aChunk;
	iChunk8 = NULL;
	iPlainTextArray[iIndex]->NextChunkL(aChunk, iStatus);
	SetActive();
	Queue(aStatus);
	}

/**
Restores the plain body text into chunks.
@param aChunk  			 The output parameter contains the requested chunk on completion.
@leave KErrNotSupported  If CMsvStore was opened in Write mode.
@leave KErrUnderflow	 If aChunk MaxLength is less than iChunkMaxLength.
@leave Other 			 Standard system-wide error codes.
@return void
*/
EXPORT_C void CImPlainBodyText::NextChunkL(TDes16& aChunk)
	{
	iPlainTextArray[iIndex]->NextChunkL(aChunk);
	// If there is more than one text part for the message move to next text part.
	if(aChunk.Length() == 0 && iIndex + 1 < iPlainTextArray.Count())
		{
		iPlainTextArray[++iIndex]->NextChunkL(aChunk);
		}
	}

/**
Retrieve the next chunk of the plain body text.
@param aChunk 			 The output parameter contains the requested chunk.
@leave KErrNotSupported  If CMsvStore was opened in Write mode.
@leave KErrNotSupported  If 16-bit storage is enabled.
@leave KErrUnderflow	 If aChunk MaxLength is less than iChunkMaxLength.
@leave Other 			 Standard system-wide error codes.
@return void
*/
EXPORT_C void CImPlainBodyText::PreviousChunkL(TDes8& aChunk, TRequestStatus& aStatus)
	{
	__ASSERT_DEBUG(iChunkRetrievalState == EIdleState, gPanic(EImcmNonIdleRetrievalState));
	iChunkRetrievalState = EPreviousChunk8Bit;
	
	iChunk8 = &aChunk;
	iChunk16 = NULL;	
	iPlainTextArray[iIndex]->PreviousChunkL(aChunk, iStatus);
	SetActive();
	Queue(aStatus);
	}


/**
Retrieve the next chunk of the plain body text.
@param aChunk 			 The output parameter contains the requested chunk.
@leave KErrNotSupported  If CMsvStore was opened in Write mode.
@leave KErrNotSupported  If 16-bit storage is enabled.
@leave KErrUnderflow	 If aChunk MaxLength is less than iChunkMaxLength.
@leave Other 			 Standard system-wide error codes.
@return void
*/
EXPORT_C void CImPlainBodyText::PreviousChunkL(TDes8& aChunk)
	{
	iPlainTextArray[iIndex]->PreviousChunkL(aChunk);
	// If message had more than one text part move to previous text part.
	if(aChunk.Length() == 0 && iIndex - 1 >= 0)
		{
		iPlainTextArray[--iIndex]->PreviousChunkL(aChunk);
		}
	}	

/**
Restore the plain body text into chunks.
@param aChunk            The output parameter contains the requested chunk on completion.
@param aStatus	         The TRequestStatus parameter for this request.
@leave KErrNotSupported  If CMsvStore was opened in Write mode.
@leave KErrUnderflow	 If aChunk MaxLength is less than iChunkMaxLength.
@leave Other 			 Standard system-wide error codes.
@return void
*/
EXPORT_C void CImPlainBodyText::PreviousChunkL(TDes16& aChunk, TRequestStatus& aStatus)
	{
	__ASSERT_DEBUG(iChunkRetrievalState == EIdleState, gPanic(EImcmNonIdleRetrievalState));
	iChunkRetrievalState = EPreviousChunk16Bit;
	
	iChunk16 = &aChunk;
	iChunk8 = NULL;
	iPlainTextArray[iIndex]->PreviousChunkL(aChunk, iStatus);
	SetActive();
	Queue(aStatus);
	}

/**
Restore the plain body text into chunks.
@param aChunk            The output parameter contains the requested chunk on completion.
@leave KErrNotSupported  If CMsvStore was opened in Write mode.
@leave KErrUnderflow	 If aChunk MaxLength is less than iChunkMaxLength.
@leave Other 			 Standard system-wide error codes.
@return void
*/
EXPORT_C void CImPlainBodyText::PreviousChunkL(TDes16& aChunk)
	{
	iPlainTextArray[iIndex]->PreviousChunkL(aChunk);
	// If message had more than one text part move to previous text part.
	if(aChunk.Length() == 0 && iIndex - 1 >= 0)
		{
		iPlainTextArray[--iIndex]->PreviousChunkL(aChunk);
		}
	}	



void CImPlainBodyText::DoRunL()
	{
	__ASSERT_DEBUG(iChunkRetrievalState != EIdleState, gPanic(EImcmIdleRetrievalState));
	
	TBool callComplete = ETrue;
	switch(iChunkRetrievalState)
		{
		// DoRunL is called as a result to a call to 8-bit asynchronous NextChunkL.
		case ENextChunk8Bit:
			{ 
			// Check if there are more than one body text part.
			if(iChunk8->Length() == 0 && iIndex + 1 < iPlainTextArray.Count())
				{
				iPlainTextArray[++iIndex]->NextChunkL(*iChunk8, iStatus);
				SetActive();
				callComplete = EFalse;
				}
			break;
			}
		case ENextChunk16Bit:
			{
			// Check if there are more than one body text part
			if(iChunk16->Length() == 0 && iIndex + 1 < iPlainTextArray.Count())
				{
				iPlainTextArray[iIndex]->NextChunkL(*iChunk16, iStatus);
				SetActive();
				callComplete = EFalse;
				}
			break;	
			}
	
		case EPreviousChunk8Bit:
			{
			// DoRunL is called as a result to a call to 8-bit asynchronous PreviousChunkL.
			if(iChunk8->Length() == 0 && iIndex - 1 >= 0)
				{
				iPlainTextArray[--iIndex]->NextChunkL(*iChunk8, iStatus);
				SetActive();
				callComplete = EFalse;
				}
			break;	
			}
		case EPreviousChunk16Bit:
			{
			if(iChunk16->Length() == 0 && iIndex - 1 >= 0)
				{
				iPlainTextArray[--iIndex]->PreviousChunkL(*iChunk16, iStatus);
				SetActive();
				callComplete = EFalse;
				}
			break;	
			}
		default:
			{
			__ASSERT_DEBUG(iChunkRetrievalState != EIdleState, gPanic(EImcmIdleRetrievalState));
			break;
			}
		
		}
	if(callComplete)	
		{
		Complete(iStatus.Int());
		iChunkRetrievalState = EIdleState;
		}
	}

/**
Commits the email message after it has been completed.
@param aStatus      The  TRequestStatus parameter for this request.
@return void
*/
EXPORT_C void CImPlainBodyText::CommitL(TRequestStatus& aStatus)
	{
	iPlainTextArray[0]->CommitL();
	delete iStore;
	iStore = NULL;
	iStoreMessagePart = CImStoreMessagePart::StorePlainBodyTextL(aStatus, iMsvEntry, iMessageId);
	}


/**
Commits the email message after it has been completed along with mime header for this message.
@param aStatus      The TRequestStatus parameter for this request.
@param aMimeHeader  The CMimeHeader for this message.
@return void
*/
EXPORT_C void CImPlainBodyText::CommitL(TRequestStatus& aStatus, CImMimeHeader& aMimeHeader)
	{
	// Commit the file.This will close the plainbody text file.
	iPlainTextArray[0]->CommitL();
	delete iStore;
	iStore = NULL;
	iStoreMessagePart = CImStoreMessagePart::StorePlainBodyTextL(aStatus, iMsvEntry, iMessageId, aMimeHeader);
	}

/**
Cancels the relavent CMsvPlainBodyText object
*/	
void CImPlainBodyText::DoCancel()
	{
	iPlainTextArray[iIndex]->Cancel();
	CMsgActive::DoCancel();
	}
