// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

#include "TestCalInterimApiParserAttachment.h"
#include <f32file.h>
#include <bautils.h>
#include <calentry.h>
#include <calattachment.h>

_LIT(KCmdMIMEType,						"ATTACHMIMETYPE");
_LIT(KCmdLabel,							"ATTACHNAME");
_LIT(KCmdDrive,							"ATTACHDRIVE");
_LIT(KCmdSetAttributes,					"SETATTRIBUTES");
_LIT(KCmdClearAttributes,				"CLEARATTRIBUTES");
_LIT(KCmdSetLastModifiedDate,			"SETLASTMODIFIEDDATE");

_LIT(KWarnMimeLongerThanMaxLength,		"MimeType is longer than the max.length");
_LIT(KWarnLabelLongerThanMaxLength,		"Label is longer than the max.length");
_LIT(KWarnInvalidData,					"Invalid Data set");

_LIT(KErrInvalidData,					"Invalid Data set");


CTestCalInterimApiParserAttachment* CTestCalInterimApiParserAttachment::NewLC(const TDesC& aData, CCalEntry& aEntry, CTestStep& aTestStep, MParserCallback& aParserCallback, CCalAttachment::TType& aAttachType,
										 RFs& aFs, const TBool& aBinaryData)
	{
	CTestCalInterimApiParserAttachment* ret = new (ELeave) CTestCalInterimApiParserAttachment(aEntry, aTestStep, aParserCallback, aAttachType, aBinaryData);
	CleanupStack::PushL(ret);
	ret->ConstructL(aFs, aData);
	return ret;
	}
	
CTestCalInterimApiParserAttachment* CTestCalInterimApiParserAttachment::NewL(const TDesC& aData, CCalEntry& aEntry, CTestStep& aTestStep, MParserCallback& aParserCallback, CCalAttachment::TType& aAttachType,
									RFs& aFs, const TBool& aBinaryData)
	{
	CTestCalInterimApiParserAttachment* ret = CTestCalInterimApiParserAttachment::NewLC(aData, aEntry, aTestStep, aParserCallback, aAttachType, aFs, aBinaryData);
	CleanupStack::Pop(ret);
	return ret;
	}

CTestCalInterimApiParserAttachment::CTestCalInterimApiParserAttachment(CCalEntry& aEntry, CTestStep& aTestStep, MParserCallback& aParserCallback, CCalAttachment::TType& aAttachType, const TBool& aBinaryData)
:	CTestCalInterimApiParserBase(aTestStep, aParserCallback)
,	iEntry(aEntry)
,	iAttachType(aAttachType)
,	iBinaryData(aBinaryData)
	{
	}
	
CTestCalInterimApiParserAttachment::~CTestCalInterimApiParserAttachment()
	{
	delete iAttachment;	
	}

void CTestCalInterimApiParserAttachment::ConstructL(RFs& aFs, const TDesC& aData)
	{
	if ( iAttachType == CCalAttachment::EFile )
		{
		// If BinaryData is present, then create an Inline attachment by passing the binary data
		if ( iBinaryData )
			{
			HBufC8* binaryData = HBufC8::NewLC(aData.Length());
			binaryData->Des().Copy(aData);
			iAttachment = CCalAttachment::NewFileL(binaryData);
			// Get the size of the binary data
			iSize = binaryData->Length();
			CleanupStack::Pop(binaryData);
			}
		// If filehandle is present,then Create an Inline attachment by passing the file handle
		else
			{
			RFile file;
			CleanupClosePushL(file);
			User::LeaveIfError(file.Open(aFs, aData, EFileRead|EFileWrite|EFileShareExclusive));

			iAttachment = CCalAttachment::NewFileL(file);
			// Get the size of the file
			file.Size(iSize);
			CleanupStack::PopAndDestroy(&file);
			}
		}

	else if ( iAttachType == CCalAttachment::EUri )
		{
		HBufC8*	uriData = HBufC8::NewLC(aData.Length());
		uriData->Des().Copy(aData);
		// Create a new Uri type attachment
		iAttachment = CCalAttachment::NewUriL(uriData->Des());
		CleanupStack::Pop(uriData);
		iSize = uriData->Length();
		}
	}

TInt CTestCalInterimApiParserAttachment::ParseL(const TDesC& aCommand, const TDesC& aData)
	{
	TInt	ret = KErrNone;

	if ( aCommand == KCmdMIMEType )
		{
		ProcessMIMETypeL(aData);
		}
	else if ( aCommand == KCmdLabel )
		{
		ProcessLabelL(aData);
		}
	else if ( aCommand == KCmdDrive )
		{
		ProcessDriveL(aData);
		}
	else if ( aCommand == KCmdSetAttributes )
		{
		ProcessSetAttributes(aData);
		}
	else if ( aCommand == KCmdClearAttributes )
		{
		ProcessClearAttributes(aData);
		}
	else if ( aCommand == KCmdSetLastModifiedDate )
		{
		ProcessSetLastModifiedDate(aData);
		}
	else
		{
		// Add the attachment to the entry
		iEntry.AddAttachmentL(*iAttachment);
		GetAttachmentInfo();
		iAttachment = NULL;
		ret = KErrNotFound;
		}
	return ret;
	}

void CTestCalInterimApiParserAttachment::ProcessMIMETypeL(const TDesC& aData)
	{
	iMimeType = HBufC8::NewL(aData.Length());
	iMimeType->Des().Copy(aData);
	// Set the Mime type of the attachment. If the Mime type is > 256, it leaves with KErrArgument
	TRAPD( err, iAttachment->SetMimeTypeL(iMimeType->Des()) );
	/* Due to restriction of existing test cases, we cant leave here
	   and set it to fail */
	if ( err == KErrArgument ) 
		{
		iTestStep.WARN_PRINTF1(KWarnMimeLongerThanMaxLength);
		iMimeType = KNullDesC8().AllocL();
		}
	else if ( err != KErrNone )
		{
		iTestStep.ERR_PRINTF2(KErrInvalidData, err);
		}
	}

void CTestCalInterimApiParserAttachment::ProcessLabelL(const TDesC& aData)
	{
	/* Set the name for the attachment. Setting the label has no effect on the actual filename.
	   If the label is greater than 256, it leaves with KErrArgument */
	TRAPD( err, iAttachment->SetLabelL(aData) );
	iLabel = aData.AllocL();
	/* Due to restriction of existing test cases, we cant leave here
	   and set the test to fail */
	if ( err == KErrArgument ) 
		{
		iTestStep.WARN_PRINTF1(KWarnLabelLongerThanMaxLength);
		iLabel = KNullDesC16().AllocL();
		}
	else if ( err != KErrNone )
		{
		iTestStep.ERR_PRINTF2(KErrInvalidData, err);
		}
	}

void CTestCalInterimApiParserAttachment::ProcessDriveL(const TDesC& aData)
	{
	if ( iAttachment->FileAttachment() )
		{
		iDriveName.Copy(aData);
		iDriveName.UpperCase();
		// Set the drive for storing the attachment
		TRAPD( err, iAttachment->FileAttachment()->SetDriveL(iDriveName) );
		if ( err == KErrArgument ) // due to restriction of existing test cases, we cant leave here
			{
			iTestStep.WARN_PRINTF1(KWarnInvalidData);
			iDriveName.Copy(_L("c:"));
			}
		else if ( err != KErrNone )
			{
			iTestStep.ERR_PRINTF2(KErrInvalidData, err);
			}
		}
	}

void CTestCalInterimApiParserAttachment::ProcessSetAttributes(const TDesC& aData)
	{
	TLex	lex(aData);
	TInt	result;
	lex.Val(result);
	// Set the attribute for the attachment
	iAttachment->SetAttribute((CCalAttachment::TAttributes)result);
	iAttributes = result;
	}

void CTestCalInterimApiParserAttachment::ProcessClearAttributes(const TDesC& aData)
	{
	TLex	lex(aData);
	TInt	result;
	lex.Val(result);
	// Clear the attribute that is set for the attachment
	iAttachment->ClearAttribute((CCalAttachment::TAttributes)result);
	}

void CTestCalInterimApiParserAttachment::ProcessSetLastModifiedDate(const TDesC& aData)
	{
	TTime	time(aData);
	iAttachment->FileAttachment()->SetLastModifiedTimeUtc(time);
	iLastModifiedDate = aData;
	}

void CTestCalInterimApiParserAttachment::GetAttachmentInfo()
	{
	iTestStep.INFO_PRINTF1(_L("KInfoCheckAttachmentData"));
	iTestStep.TEST1( iAttachment->Type() == iAttachType, ETrue );
	if ( *iMimeType != KNullDesC8() )
		{
		iTestStep.TEST1( iAttachment->MimeType() == iMimeType->Des(), ETrue );	
		}
	if ( iLabel != NULL )
		{
		iTestStep.TEST1( iAttachment->Label() == iLabel->Des(), ETrue );	
		}
	
	TBool	ret = iAttachment->IsAttributeSet((CCalAttachment::TAttributes)iAttributes);
	// If the attachment has file attachment, then check the properties of file attachment
	if ( iAttachment->FileAttachment() && iBinaryData == EFalse )
		{
		if ( iDriveName != KNullDesC() )
			{
			iTestStep.TEST1( iAttachment->FileAttachment()->Drive()  == iDriveName, ETrue );	
			}
		if ( iBinaryData == EFalse && iLastModifiedDate != 0 )
			{
			iTestStep.TEST1( iAttachment->FileAttachment()->LastModifiedTimeUtc() == iLastModifiedDate , ETrue );	
			}
		}
	}
	
