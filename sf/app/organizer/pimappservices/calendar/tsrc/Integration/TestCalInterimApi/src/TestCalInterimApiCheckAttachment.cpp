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

#include "TestCalInterimApiCheckAttachment.h"

#include <calsession.h>
#include <calentry.h>
#include <calentryview.h>
#include <calattachment.h>

_LIT(KGUid,									"guid");
_LIT(KRecurrenceId,							"recurrenceid");
_LIT(KExpectedAttachmentCount,				"expectedattachmentcount");

_LIT(KMIMEType,								"mimetype%d");
_LIT(KAttachType,							"attachtype%d");
_LIT(KValue,								"value%d");
_LIT(KName,									"filename%d");
_LIT(KSize,									"filesize%d");
_LIT(KAttributes,							"attributes%d");
_LIT(KDrive,								"drive%d");
_LIT(KLastModifiedDate,						"lastmodifieddate%d");
_LIT(KContentId,							"contentid%d");

_LIT(KInfoFileAttachment,					"File Attachment is present");
_LIT(KInfoUriAttachment,					"Uri Attachment is present");


_LIT(KExpectedAndFoundAttachType,			"Expected AttachType:%d, Retrieved AttachType:%d");
_LIT(KExpectedAndFoundMIME,					"Expected MIMEType:%S, Retrieved MIMEType:%S");
_LIT(KExpectedAndFoundValue,				"Expected Value:%S, Retrieved Value:%S");
_LIT(KExpectedAndFoundDrive,				"Expected Drive:%S, Retrieved Drive:%S");
_LIT(KExpectedAndFoundName,					"Expected Name:%S, Retrieved Name:%S");
_LIT(KExpectedAndFoundSize,					"Expected Size:%d, Retrieved Size:%d");
_LIT(KExpectedAndFoundLastModifiedDate,		"Expected lastmodifieddate and Retrieved Modified date are :");
_LIT(KExpectedAndFoundAttachCount,			"Expected Attachmentcount:%d, Retrieved AttachmentCount:%d");
_LIT(KExpectedNoAttachNoEntry,				"Expected Attachmentcount:0, Entry not found");
_LIT(KExpectedAndFoundContentId,			"Expected ContentId:%S, Retrieved ContentId:%S");


_LIT(KErrExpectedAndFoundAttachmentCount,	"ExpectedCount %d, AttachmentCount %d ");
_LIT(KErrFetchFileHandle,					"Error while getting file handle");

CTestCalInterimApiCheckAttachment::CTestCalInterimApiCheckAttachment()
:	CTestCalInterimApiSuiteStepBase()
,	iHasSize(EFalse)
,	iHasAttribute(EFalse)

/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KTestCalInterimApiCheckAttachment);
	}

CTestCalInterimApiCheckAttachment::~CTestCalInterimApiCheckAttachment()
/**
 * Destructor
 */
	{
	delete iCalEntryView;
	}


/**
Base class pure virtual.
@return		EPass or EFail indicating the result of the test step.
*/
TVerdict CTestCalInterimApiCheckAttachment::doTestStepL()
	{
	iCalEntryView = CCalEntryView::NewL(GetSession(), *this);
	CActiveScheduler::Start();
	
	TPtrC	gUid;
	TESTL(GetStringFromConfig(ConfigSection(), KGUid, gUid));			
	TPtrC	recurrenceId;
	GetStringFromConfig(ConfigSection(), KRecurrenceId, recurrenceId);

	// Fetch the entry
	RPointerArray<CCalEntry>	entriesFetched;	
	CleanupStack::PushL(TCleanupItem(ResetAndDestroyEntryArray, &entriesFetched));
	HBufC8*		entryId = HBufC8::NewLC(gUid.Length());
	entryId->Des().Copy(gUid);
	iCalEntryView->FetchL(entryId->Des(), entriesFetched);
	CleanupStack::PopAndDestroy(entryId);

	// Get the entry we are interested in
	TCalTime	recurrenceIdTime;
	if ( recurrenceId == KNullDesC )
		{
		recurrenceIdTime.SetTimeUtcL(Time::NullTTime());
		}
	else
		{
		recurrenceIdTime.SetTimeUtcL(TTime(recurrenceId));
		}
		
	CCalEntry*	entry = GetMatchingEntryL(entriesFetched, recurrenceIdTime);
	
	// Get the attachment from the entry and check the properties
	GetAttachmentAndCheckPropertiesL(entry);	
		
	CleanupStack::PopAndDestroy(&entriesFetched);

	return TestStepResult();
	}

/** Get the attachment from the parent or child entry and check the attachment properties of
the attachment
@param	aEntry Pointer to CCalEntry
*/
void CTestCalInterimApiCheckAttachment::GetAttachmentAndCheckPropertiesL(CCalEntry* aEntry)
	{
	TInt	expectedAttachmentCount;
	TESTL(GetIntFromConfig(ConfigSection(), KExpectedAttachmentCount, expectedAttachmentCount));
	if (aEntry == NULL && expectedAttachmentCount == 0)
		{
		INFO_PRINTF3(KExpectedNoAttachNoEntry, expectedAttachmentCount, 0);
		}
	else if ( expectedAttachmentCount == aEntry->AttachmentCountL() )
		{
		INFO_PRINTF3(KExpectedAndFoundAttachCount, expectedAttachmentCount, aEntry->AttachmentCountL());
		TInt	index = 0;
		TInt	attachmentCount = 0;
		// Iterates through all the attachments and checks the properties
		while ( attachmentCount < aEntry->AttachmentCountL())
			{
			index++;
			CCalAttachment*	attachment = aEntry->AttachmentL(attachmentCount);
			GetAttachmentDataFromIni(index);
			// Check the attachment properties of the parent and child entry
			CheckAttachmentPropertiesL(attachment);
			attachmentCount++;
			}
		}
	else
		{
		ERR_PRINTF3(KErrExpectedAndFoundAttachmentCount, expectedAttachmentCount, aEntry->AttachmentCountL());
		SetTestStepResult(EFail);
		}
	}

/** Get the properties of the attachment from the ini and store it in the variables
@param	aIndex Number of attachment
*/
void CTestCalInterimApiCheckAttachment::GetAttachmentDataFromIni(const TInt& aIndex)
	{
	TBuf<KMaxTestExecuteCommandLength>	tempStore;
	tempStore.Format(KAttachType(), aIndex);
	iHasAttachType = GetIntFromConfig(ConfigSection(), tempStore, iAttachProperties.iAttachType);
	tempStore.Format(KMIMEType(), aIndex);
	GetStringFromConfig(ConfigSection(), tempStore, iAttachProperties.iMimeType);
	tempStore.Format(KValue(), aIndex);
	GetStringFromConfig(ConfigSection(), tempStore, iAttachProperties.iValue);
	tempStore.Format(KName(), aIndex);
	GetStringFromConfig(ConfigSection(), tempStore, iAttachProperties.iName);
	tempStore.Format(KSize(), aIndex);
	iHasSize = GetIntFromConfig(ConfigSection(), tempStore, iAttachProperties.iSize);
	tempStore.Format(KLastModifiedDate(), aIndex);
	GetStringFromConfig(ConfigSection(), tempStore, iAttachProperties.iLastModifiedDate);
	tempStore.Format(KAttributes(), aIndex);
	iHasAttribute = GetIntFromConfig(ConfigSection(), tempStore, iAttachProperties.iAttribute);
	tempStore.Format(KDrive(), aIndex);
	GetStringFromConfig(ConfigSection(), tempStore, iAttachProperties.iDrive);
	tempStore.Format(KContentId(), aIndex);
	GetStringFromConfig(ConfigSection(), tempStore, iAttachProperties.iContentId);
	}

/** Check the attachment has file handle and based on that output check the properties
@param	aCalAttachment Pointer to CCalAttachment
*/
void CTestCalInterimApiCheckAttachment::CheckAttachmentPropertiesL(CCalAttachment* aCalAttachment)
	{
	// Get the file handle of the attachment
	CCalAttachmentFile*	fileAttachment = aCalAttachment->FileAttachment();
	// If the file attachment has some value then, check the file attachment properties
	if ( fileAttachment != NULL )
		{
		INFO_PRINTF1(KInfoFileAttachment);
		CheckPropertiesOfAttachmentL(aCalAttachment);
		CheckFileAttachmentPropertiesL(fileAttachment);
		}
	else // Uri attachment
		{
		INFO_PRINTF1(KInfoUriAttachment);
		CheckPropertiesOfAttachmentL(aCalAttachment);
		}
	}
	
/** Check the attachment properties 
@param	aCalAttachment Pointer to CCalAttachment
*/
void CTestCalInterimApiCheckAttachment::CheckPropertiesOfAttachmentL(CCalAttachment* aCalAttachment)
	{
	// Get the MIME type of the attachment and compare it with the expected value
	if ( iAttachProperties.iMimeType != KNullDesC16() )
		{
		const TDesC8&	retrievedMimeType(aCalAttachment->MimeType());
		TBuf<128>	buf;
		buf.Copy(retrievedMimeType);
		INFO_PRINTF3(KExpectedAndFoundMIME, &iAttachProperties.iMimeType, &buf);
		HBufC8*	expectedMimeType = HBufC8::NewLC((iAttachProperties.iMimeType).Length());
		expectedMimeType->Des().Copy(iAttachProperties.iMimeType);
		TEST1( aCalAttachment->MimeType() == expectedMimeType->Des(), ETrue);	
		CleanupStack::PopAndDestroy(expectedMimeType);
		iAttachProperties.iMimeType.Set(KNullDesC());
		}

	// Get the type of the attachment and compare it with the expected value
	if ( iHasAttachType )
		{
		CCalAttachment::TType	attachType(aCalAttachment->Type());
		INFO_PRINTF3(KExpectedAndFoundAttachType, iAttachProperties.iAttachType, attachType);
		TEST1( attachType == CCalAttachment::TType(iAttachProperties.iAttachType), ETrue);
		iHasAttachType = EFalse;
		}

	// Test if the value of the attachment is set correctly
	if ( iAttachProperties.iValue != KNullDesC16() )
		{
		HBufC8*	expectedValue = HBufC8::NewLC((iAttachProperties.iValue).Length());
		expectedValue->Des().Copy(iAttachProperties.iValue);
		// if it is a file attachment, then load the binary data of the file attachment
		if ( aCalAttachment->FileAttachment() )
			{
			aCalAttachment->FileAttachment()->LoadBinaryDataL();
			TEST1( aCalAttachment->Value() == expectedValue->Des(), ETrue);
			}
		else // For uri attachments
			{
			const TDesC8&	attachValue(aCalAttachment->Value());
			TBuf<256>	buf;
			buf.Copy(attachValue);
			INFO_PRINTF3(KExpectedAndFoundValue, &iAttachProperties.iValue, &buf);	
			TEST1( aCalAttachment->Value() == expectedValue->Des(), ETrue);
			}
		CleanupStack::PopAndDestroy(expectedValue);
		iAttachProperties.iValue.Set(KNullDesC());
		}

	// Test if the attribute is set correctly
	if ( iHasAttribute )
		{
		TBool	hasAttribute = aCalAttachment->IsAttributeSet(CCalAttachment::TAttributes(iAttachProperties.iAttribute));
		TEST1( hasAttribute == iAttachProperties.iAttribute, ETrue);	
		iHasAttribute = EFalse;
		}

	// Test if the name of the attachment matches with the expected name
	if ( iAttachProperties.iName != KNullDesC16() )
		{
		const TDesC&	name(aCalAttachment->Label());
		TBuf<256>	buf;
		buf.Copy(name);
		INFO_PRINTF3(KExpectedAndFoundName, &iAttachProperties.iName, &buf);
		HBufC*	expectedName = (iAttachProperties.iName).AllocLC();
		TEST1( name == expectedName->Des(), ETrue);
		CleanupStack::PopAndDestroy(expectedName);
		iAttachProperties.iName.Set(KNullDesC());
		}
	}

/** Check the file attachment properties 
@param	aFileAttachment Pointer to CCalAttachmentFile
*/
void CTestCalInterimApiCheckAttachment::CheckFileAttachmentPropertiesL(CCalAttachmentFile* aFileAttachment)
	{
	// Test if the last modified date is set correctly
	if ( iAttachProperties.iLastModifiedDate != KNullDesC16() )
		{
		TTime	lastModifiedDate(aFileAttachment->LastModifiedTimeUtc());
		TTime	expectedLastModifiedDate;
		expectedLastModifiedDate.Set(iAttachProperties.iLastModifiedDate);
		INFO_PRINTF1(KExpectedAndFoundLastModifiedDate);
		PrintTimeL(lastModifiedDate);
		PrintTimeL(expectedLastModifiedDate);
		TEST1( lastModifiedDate == expectedLastModifiedDate, ETrue);
		iAttachProperties.iLastModifiedDate.Set(KNullDesC16());
		}

	// Test if the drive of the attachment is set correctly
	if ( iAttachProperties.iDrive != KNullDesC16() )
		{
		TDriveName driveName;
		driveName.Copy(aFileAttachment->Drive());
		driveName.UpperCase();
		INFO_PRINTF3(KExpectedAndFoundDrive, &iAttachProperties.iDrive, &driveName);
		TEST1( driveName == iAttachProperties.iDrive, ETrue);
		iAttachProperties.iDrive.Set(KNullDesC16());	
		}

	// Fetch the file handle from the attachment
	RFile	fileHandle;
	CleanupClosePushL(fileHandle);
	TRAPD( err, aFileAttachment->FetchFileHandleL(fileHandle) );
	if ( err != KErrNone )
		{
		ERR_PRINTF1(KErrFetchFileHandle);
		SetTestStepResult(EFail);
		}
			
	// Test if the size of the attachment is equal
	if ( iHasSize )
		{
		INFO_PRINTF3(KExpectedAndFoundSize, iAttachProperties.iSize, aFileAttachment->Size());
		TEST1(aFileAttachment->Size() == iAttachProperties.iSize, ETrue);
		// Get the size of the file handle and check it with the expected value
		TInt	size;
		User::LeaveIfError(fileHandle.Size(size));
		TESTL( iAttachProperties.iSize == size );
		iHasSize = EFalse;
		}
		
	// Check the content id of the attachment
	if ( iAttachProperties.iContentId != KNullDesC16() )
		{
		const TDesC8&	contentId(aFileAttachment->ContentId());
		TBuf<256>	buf;
		buf.Copy(contentId);
		INFO_PRINTF3(KExpectedAndFoundContentId, &iAttachProperties.iContentId, &buf);
		HBufC8*	expectedContentId = HBufC8::NewLC((iAttachProperties.iContentId).Length());
		TEST1( contentId == expectedContentId->Des(), ETrue);
		CleanupStack::PopAndDestroy(expectedContentId);
		iAttachProperties.iContentId.Set(KNullDesC());
		}
	
	CleanupStack::PopAndDestroy(&fileHandle);

	}

