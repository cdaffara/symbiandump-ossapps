/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Defines the CBtSoapCodec class to create and read SOAP-encoded messages.
*
*/


#include "cxmlhandler.h"
#include "cbtsoapcodec.h"
#include "imageprint.h"
#include "clog.h"
#include "tbtmapper.h"
#include "rsutils.h"

//--------------------------------------------------------------------------------------------
//
// CBtSoapCodec::NewL
//
//--------------------------------------------------------------------------------------------
CBtSoapCodec* CBtSoapCodec::NewL()
	{
		CBtSoapCodec *self = NewLC();
		CleanupStack::Pop();	// self

		return self;
	}

//--------------------------------------------------------------------------------------------
//
// CBtSoapCodec::NewLC
//
//--------------------------------------------------------------------------------------------
CBtSoapCodec* CBtSoapCodec::NewLC()
	{
		CBtSoapCodec *self = new (ELeave) CBtSoapCodec();
		CleanupStack::PushL(self);
		self->ConstructL();
		return self;
	}

//--------------------------------------------------------------------------------------------
//
// CBtSoapCodec::~CBtSoapCodec
//
//--------------------------------------------------------------------------------------------
CBtSoapCodec::~CBtSoapCodec()
{
	if(iXmlHandler)
		delete iXmlHandler;
	iXmlHandler = NULL;

	if(iActionBuffer)
		delete iActionBuffer;
	iActionBuffer = NULL;
}

//--------------------------------------------------------------------------------------------
//
// CBtSoapCodec::ConstructL
//
//--------------------------------------------------------------------------------------------
void CBtSoapCodec::ConstructL()
{
	LOG("[CBtSoapCodec::ConstructL]\t");
	iXmlHandler = CXmlHandler::NewL();
}

//--------------------------------------------------------------------------------------------
//
// CBtSoapCodec::Reset
//
//--------------------------------------------------------------------------------------------
void CBtSoapCodec::Reset()
{
	LOG("[CBtSoapCodec::Reset]\t");

	if(iXmlHandler)
		delete iXmlHandler;
	iXmlHandler = NULL;

	if(iActionBuffer)
		delete iActionBuffer;
	iActionBuffer = NULL;

}

//--------------------------------------------------------------------------------------------
//
// CBtSoapCodec::CreateSoapL
//
//--------------------------------------------------------------------------------------------
TPtrC8 CBtSoapCodec::CreateSoapL(const TInt aAction, const TInt aJobId)
{
	LOG1("[CBtSoapCodec::CreateSoapL]\t action: %d", aAction);
	// Open the template file and read it into buffer

	TPtrC8 soap(KNullDesC8);
	TBtIntString8 jobId;
	jobId.AppendNum(aJobId);

	if(iActionBuffer)
		delete iActionBuffer;
	iActionBuffer = NULL;

	switch(aAction)
	{
		case EBtCreateJob:
			CreateJobSoapL(aJobId);
			break;
		case EBtGetPrinterAttributes:
			GetPrinterAttributesSoapL();
			break;
		case EBtGetPrinterState:
			GetPrinterStateSoapL();
			break;
		case EBtGetPrinterCapabilities:
			GetCapabilitiesSoapL();
			break;
		case EBtCancelJob:
			CancelJobSoapL(jobId);
			break;
		case EBtGetJobAttributes:
			GetJobAttributesSoapL(jobId);
			break;
		case EBtGetEvent:
			GetEventSoapL(jobId);
			break;
		default:
			break;
	}

	TInt offset;
	TPtrC8 ptr = iXmlHandler->GetDataL(KBtBodyStartTag, KBtBodyEndTag, offset);
	iXmlHandler->ComposeDataL(ptr.Length(), KBtBytesInBody());

	return iXmlHandler->FinalizeBufferL(KBtEnvelopeEndTag());

}

//--------------------------------------------------------------------------------------------
//
// CBtSoapCodec::ReadSoapL
//
//--------------------------------------------------------------------------------------------
void CBtSoapCodec::ReadSoapL(const TDesC8& aSoapBuffer)
{
	LOG("[CBtSoapCodec::ReadSoapL(TDesC8&)]\t ");
	// Open the template file and read it into buffer
	
	TInt action = 0;
	ReadSoapL(action, aSoapBuffer);
}

//--------------------------------------------------------------------------------------------
//
// CBtSoapCodec::ReadSoapL
//
//--------------------------------------------------------------------------------------------
void CBtSoapCodec::ReadSoapL(const CBufBase& aSoapBuffer)
{
	LOG("[CBtSoapCodec::ReadSoapL(CBufBase&)]\t ");
	// Open the template file and read it into buffer
	
	TInt action = 0;
	ReadSoapL(action, aSoapBuffer);
}

//--------------------------------------------------------------------------------------------
//
// CBtSoapCodec::ReadSoapL
//
//--------------------------------------------------------------------------------------------
void CBtSoapCodec::ReadSoapL(TInt& aAction, const CBufBase& aSoapBuffer)
{
	LOG("[CBtSoapCodec::ReadSoapL(TInt&, CBufBase&)]\t ");
	// Open the template file and read it into buffer

	if(iActionBuffer)
		delete iActionBuffer;
	iActionBuffer = NULL;

	iActionBuffer = static_cast<HBufC8*>(User::LeaveIfNull(HBufC8::NewL(aSoapBuffer.Size())));
	TPtr8 ptr = iActionBuffer->Des();
	aSoapBuffer.Read(0, ptr, aSoapBuffer.Size());

	ReadSoapL(aAction, iActionBuffer->Des());
}

//--------------------------------------------------------------------------------------------
//
// CBtSoapCodec::ReadSoapL
//
//--------------------------------------------------------------------------------------------
void CBtSoapCodec::ReadSoapL(TInt& aAction, const TDesC8& aSoapBuffer)
{
	LOG("[CBtSoapCodec::ReadSoapL(TInt&, TDesC8&)]\t ");
	// Open the template file and read it into buffer

#ifdef _DEBUG	
	TInt len = aSoapBuffer.Length() < 50 ? aSoapBuffer.Length() : 50;
	TPtrC8 p = aSoapBuffer.Mid(0, len);

#endif

	if(!iXmlHandler)
		iXmlHandler = CXmlHandler::NewL();
	iXmlHandler->InitFromBufferL(aSoapBuffer);

	if(iActionBuffer)
		delete iActionBuffer;
	iActionBuffer = NULL;
	
	TInt offset = 0;

	TPtrC8 ptr = iXmlHandler->GetDataL(KBtBodyStartTag, KBtBodyEndTag, offset);

	iActionBuffer = static_cast<HBufC8*>(User::LeaveIfNull(ptr.AllocL()));

	aAction = SoapAction();

	iXmlHandler->Reset();
	iXmlHandler->InitFromBufferL(iActionBuffer->Des(), aAction);
}

//------------------------------
// Responses
//------------------------------
//--------------------------------------------------------------------------------------------
//
// CBtSoapCodec::CreateJobResponseL
//
//--------------------------------------------------------------------------------------------
void CBtSoapCodec::CreateJobResponseL(TInt& aJobId, TInt& aErrorCode)
{
	LOG("[CBtSoapCodec::CreateJobResponseL]\t ");

	if(EBtCreateJobResponse != SoapAction())
	{
		User::Leave(KErrInvalidSequence);
	}

	TBtIntString8 jobId;
	jobId.Zero();
	TInt status = OperationStatusL();

	aErrorCode = KErrNotFound;
	GetAttributeL(KBtJobId(), jobId, aErrorCode);
	LOG82("[CBtSoapCodec::CreateJobResponseL]\t %S: %S",&KBtJobId(), &jobId);
	LOG2("[CBtSoapCodec::CreateJobResponseL]\t opStatus: %d, err %d",status, aErrorCode);
	if(aErrorCode == KErrNone)
		aErrorCode = status;
	
	aJobId = TBtMapper::Int(jobId);
}

//--------------------------------------------------------------------------------------------
//
// CBtSoapCodec::GetPrinterAttributesResponseL
//
//--------------------------------------------------------------------------------------------
void CBtSoapCodec::GetPrinterAttributesResponseL(TInt& aState, TInt& aStateReasons,
								 			   RArray<TPrintCapability>& aCapabilities,
								 			   TInt& aOperationStatus)
{
	LOG("[CBtSoapCodec::GetPrinterAttributesResponseL]\t");
	
	// same operationStatus got twce but no matter.
	GetPrinterStateResponseL(aState, aStateReasons, aOperationStatus);
	GetPrinterCapabilitiesResponseL(aCapabilities, aOperationStatus);
}

//--------------------------------------------------------------------------------------------
//
// CBtSoapCodec::GetPrinterStateResponseL
//
//--------------------------------------------------------------------------------------------
void CBtSoapCodec::GetPrinterStateResponseL(TInt& aState, TInt& aStateReasons, TInt& aOperationStatus)
{
	LOG("[CBtSoapCodec::GetPrinterStateResponseL]\t");

	if(EBtGetPrinterAttributesResponse != SoapAction())
	{
		User::Leave(KErrInvalidSequence);
	}

	aState = EBtStateUnknown;
	aStateReasons = EPbCheckPrinter;
	aOperationStatus = OperationStatusL();

	/*get attribute*/
	TBtAttrString8 value(KNullDesC8());

	/* Get printer status and reason */
	TInt err = KErrNotFound;
	GetAttributeL(KBtPrinterState(), value, err);
	LOG82("[CBtSoapCodec::GetPrinterStateResponseL]\t ** %S: %S",&KBtPrinterState(), &value);
	if(KErrNone == err)
	{
		aState = TBtMapper::State(value);
		GetAttributeL(KBtPrinterStateReasons(), value, err);
		LOG82("[CBtSoapCodec::GetPrinterStateResponseL]\t ** %S: %S",&KBtPrinterStateReasons(), &value);
		if(KErrNone == err)
		{
			aStateReasons = TBtMapper::PrintError(value);
		}
	}
}



//--------------------------------------------------------------------------------------------
//
// CBtSoapCodec::GetCapabilitiesResponseL
//
//--------------------------------------------------------------------------------------------
void CBtSoapCodec::GetPrinterCapabilitiesResponseL(RArray<TPrintCapability>& aCapabilities,
								 			   TInt& aOperationStatus)
{
	LOG("[CBtSoapCodec::GetPrinterCapabilitiesResponseL]\t");

	if(EBtGetPrinterAttributesResponse != SoapAction())
	{
		User::Leave(KErrInvalidSequence);
	}

	aOperationStatus = OperationStatusL();
	aCapabilities.Reset();

	aCapabilities.Append(ParseLayoutL());
	aCapabilities.Append(ParseQualityL());
	aCapabilities.Append(ParsePaperSizeL());

}

//--------------------------------------------------------------------------------------------
//
// CBtSoapCodec::GetJobAttributesResponseL
//
//--------------------------------------------------------------------------------------------
void CBtSoapCodec::GetJobAttributesResponseL(const TInt aJobId, TInt& aJobState, TBtAttrString8& aJobName,
										   TBtAttrString8& aOriginator, TInt& aSheetsCompleted, 
										   TInt& aPendingJobs, TInt& aErrorCode )

{
	LOG("[CBtSoapCodec::GetJobAttributesResponseL]\t");
	if(EBtGetJobAttributesResponse != SoapAction())
	{
		User::Leave(KErrInvalidSequence);
	}
	
	aJobState = EBtStateUnknown;
	aJobName = KNullDesC8();
	aOriginator = KNullDesC8();
	aSheetsCompleted = KErrNotFound;
	aPendingJobs = KErrNotFound;
	aErrorCode = KErrNotFound;

	TBtAttrString8 value(KNullDesC8());
	TInt status = OperationStatusL();

	GetAttributeL(KBtJobId(), value, aErrorCode);
	LOG82("[CBtSoapCodec::GetJobAttributesResponseL]\t %S: %S",&KBtJobId(), &value);
	LOG2("[CBtSoapCodec::GetJobAttributesResponseL]\t opStatus: %d, err %d",status, aErrorCode);
	if(aErrorCode == KErrNone)
	{
		if(	TBtMapper::Int(value) != aJobId)
			aErrorCode = KErrArgument;
		else
			aErrorCode = status;
	}
	
	// then just get the rest of the arguments. Success or fails, just continue.
	GetAttributeL(KBtJobState(), value, status);
	if(KErrNone == status)
		aJobState = TBtMapper::State(value);
	else
	{
		LOG82("[CBtSoapCodec::GetJobAttributesResponseL]\t %S: %S",&KBtJobState(), &value);
		LOG1("[CBtSoapCodec::GetJobAttributesResponseL]\t err %d",status);
	}

	GetAttributeL(KBtJobMediaSheetsCompleted(), value, status);
	if(KErrNone == status)
		aSheetsCompleted = TBtMapper::Int(value);
	else
	{
		LOG82("[CBtSoapCodec::GetJobAttributesResponseL]\t %S: %S",&KBtJobMediaSheetsCompleted(), &value);
		LOG1("[CBtSoapCodec::GetJobAttributesResponseL]\t err %d",status);
	}

	GetAttributeL(KBtJobPending(), value, status);
	if(KErrNone == status)
		aPendingJobs = TBtMapper::Int(value);
	else
	{
		LOG82("[CBtSoapCodec::GetJobAttributesResponseL]\t %S: %S",&KBtJobPending(), &value);
		LOG1("[CBtSoapCodec::GetJobAttributesResponseL]\t err %d",status);
	}
	
	GetAttributeL(KBtJobName(), aJobName, status);
	if(KErrNone != status)
	{
		LOG82("[CBtSoapCodec::GetJobAttributesResponseL]\t %S: %S",&KBtJobName(), &aJobName);
		LOG1("[CBtSoapCodec::GetJobAttributesResponseL]\t err %d",status);
	}

	GetAttributeL(KBtJobOriginatingUserName(), aOriginator, status);
	if(KErrNone != status)
	{
		LOG82("[CBtSoapCodec::GetJobAttributesResponseL]\t %S: %S",&KBtJobOriginatingUserName(), &aOriginator);
		LOG1("[CBtSoapCodec::GetJobAttributesResponseL]\t err %d",status);
	}
}

//--------------------------------------------------------------------------------------------
//
// CBtSoapCodec::CancelJobResponseL
//
//--------------------------------------------------------------------------------------------
void CBtSoapCodec::CancelJobResponseL(const TInt aJobId, TInt& aErrorCode)
{
	LOG("[CBtSoapCodec::CancelJobResponseL]\t");
	TBtIntString8 jobId;
	jobId.Append(aJobId);

	if(EBtCancelJobResponse != SoapAction())
	{
		User::Leave(KErrInvalidSequence);
	}

	TBuf8<KDefaultRealWidth> respJobId;
	TInt status = OperationStatusL();

	aErrorCode = KErrNotFound;
	GetAttributeL(KBtJobId(), respJobId, aErrorCode);
	LOG82("[CBtSoapCodec::CancelJobResponseL]\t %S: %S",&KBtJobId(), &respJobId);
	LOG2("[CBtSoapCodec::CancelJobResponseL]\t opStatus: %d, err %d",status, aErrorCode);
	if(aErrorCode == KErrNone)
	{
		if(TBtMapper::Int(respJobId) != aJobId)
			aErrorCode = KErrArgument;
		else
			aErrorCode = status;
	}
}

//--------------------------------------------------------------------------------------------
//
// CBtSoapCodec::GetEventResponseL
//
//--------------------------------------------------------------------------------------------
void CBtSoapCodec::GetEventResponseL(const TInt aJobId,
											  TInt& aJobState,
											  TInt& aPrinterState,
											  TInt& aStateReasons,
											  TInt& aErrorCode)
{
	LOG("[CBtSoapCodec::GetEventResponseL]\t");
	if(EBtGetEventResponse != SoapAction())
	{
		User::Leave(KErrInvalidSequence);
	}

	aJobState = EBtStateUnknown;
	aPrinterState = KErrNotFound;
	aStateReasons = KErrNotFound;
	aErrorCode = KErrNotSupported;

	TBtAttrString8 value(KNullDesC8());
	TInt status = OperationStatusL();

	GetAttributeL(KBtJobId(), value, aErrorCode);
	LOG82("[CBtSoapCodec::GetEventResponseL]\t %S: %S",&KBtJobId(), &value);
	LOG2("[CBtSoapCodec::GetEventResponseL]\t opStatus: %d, err %d",status, aErrorCode);
	if(aErrorCode == KErrNone)
	{
		if(TBtMapper::Int(value) != aJobId)
			aErrorCode = KErrArgument;
		else
			aErrorCode = status;
	}
	
	// then just get the rest of the arguments. Success or fails, just continue.
	GetAttributeL(KBtJobState(), value, status);
	LOG82("[CBtSoapCodec::GetEventResponseL]\t %S: %S",&KBtJobState(), &value);
	LOG1("[CBtSoapCodec::GetEventResponseL]\t err %d",status);
	if(KErrNone == status)
		aJobState = TBtMapper::Int(value);

	GetAttributeL(KBtPrinterState(), value, status);
	LOG82("[CBtSoapCodec::GetEventResponseL]\t %S: %S",&KBtPrinterState(), &value);
	LOG1("[CBtSoapCodec::GetEventResponseL]\t err %d",status);
	if(KErrNone == status)
		aPrinterState = TBtMapper::Int(value);
	
	GetAttributeL(KBtPrinterStateReasons(), value, status);
	LOG82("[CBtSoapCodec::GetEventResponseL]\t %S: %S",&KBtPrinterStateReasons(), &value);
	LOG1("[CBtSoapCodec::GetEventResponseL]\t err %d",status);
	if(KErrNone == status)
		aStateReasons = TBtMapper::Int(value);
}

//--------------------------------------------------------------------------------------------
//
// CBtSoapCodec::OperationStatusL
//
//--------------------------------------------------------------------------------------------
TInt CBtSoapCodec::OperationStatusL()
{
	LOG("[CBtSoapCodec::GetOperationStatus]\t");

	/* Get operation status. If NOK or not found, no reason to continue */
	TBtAttrString8 value(KNullDesC8());
	TInt status = KErrNotFound;

	GetAttributeL(KBtOperationStatus(), value, status);
	LOG82("[CBtSoapCodec::GetOperationStatus]\t %S: %S",&KBtOperationStatus(), &value);

	status = TBtMapper::OperationStatus(value);

	LOG1("[CBtSoapCodec::GetOperationStatus]\t return status %d",status);
	return status;
}

//------------------------------

//--------------------------------------------------------------------------------------------
//
// CBtSoapCodec::GetAttributeL
//
//--------------------------------------------------------------------------------------------
void CBtSoapCodec::GetAttributeL(const TDesC8& aName, TDes8& aValue, TInt& aError)
{
	LOG("[CBtSoapCodec::GetAttributeL()]\t ");

	if(KErrNotFound == iXmlHandler->BufferId())
		User::Leave(KErrInvalidSequence);

	aError = KErrNotFound;

	TBtAttrString8 startTag;
	TBtAttrString8 endTag;

	startTag.Format(KBtStartTag(), &aName);
	endTag.Format(KBtEndTag(), &aName);

	TInt offset = KErrNotFound;
	TPtrC8 ptr = iXmlHandler->GetDataL(startTag, endTag, offset);

	if(0 < ptr.Length())
	{
		aError = KErrNone;
		aValue.Copy(ptr);
	}
	LOG("[CBtSoapCodec::GetAttributeL]\t end");
}

//--------------------------------------------------------------------------------------------
//
// CBtSoapCodec::GetAttributeListL
//
//--------------------------------------------------------------------------------------------
void CBtSoapCodec::GetAttributeListL(const TDesC8& aName, CDesC8ArrayFlat& aList, TInt& aError)
{
	LOG("[CBtSoapCodec::GetAttributeL()]\t ");

	if(KErrNotFound == iXmlHandler->BufferId())
		User::Leave(KErrInvalidSequence);

	aError = KErrNotFound;
	aList.Reset();

	// Read the list of tag aName
	TInt offset = KErrNotFound;
	HBufC8* listData = HBufC8::NewLC(iXmlHandler->Buffer().Length());
	TPtr8 dataPtr = listData->Des();
	GetAttributeL(aName, dataPtr, offset);
	if(0 >= dataPtr.Length())
	{
		aError = KErrNotFound;
		CleanupStack::PopAndDestroy(listData);
		return;
	}
	
	CXmlHandler* tmpHandler = CXmlHandler::NewLC();
	tmpHandler->InitFromBufferL(dataPtr);
	
	// Now find the first tag data to identify the tag of the entry in the list
	TPtrC8 newNamePtr = tmpHandler->GetDataL(KBtLessThan(), KBtGreaterThan(), offset);
	
	TBtAttrString8 startTag;
	TBtAttrString8 endTag;

	startTag.Format(KBtStartTag(), &newNamePtr);
	endTag.Format(KBtEndTag(), &newNamePtr);

	tmpHandler->GetDataListL(startTag, endTag, aList);
	CleanupStack::PopAndDestroy(2); //listData, tmpHandler

	if(0 < aList.Count())
	{
		aError = KErrNone;
	}
}


// PRIVATE
//--------------------------------------------------------------------------------------------
//
// CBtSoapCodec::CreateSoapTemplateL
//
//--------------------------------------------------------------------------------------------
void CBtSoapCodec::CreateSoapTemplateL(const TDesC8& aStartTag, const TDesC8& aEndTag)
{
	LOG("[CBtSoapCodec::CreateSoapTemplateL]\t");

	// Open the template file and read it into buffer
	iXmlHandler->Reset();
	iXmlHandler->InitFromFileL(KBtXmlTemplate());

	// Separate the template element from the template buffer
	if(iActionBuffer)
		delete iActionBuffer;
	iActionBuffer = NULL;
	
	TInt offset = 0;

	TPtrC8 ptr = iXmlHandler->GetDataL(aStartTag, aEndTag, offset);

	iActionBuffer = static_cast<HBufC8*>(User::LeaveIfNull(ptr.AllocL()));

	iXmlHandler->DeleteBufferData(offset, iActionBuffer->Length()+aStartTag.Length()+aEndTag.Length());
	iXmlHandler->ComposeDataL(iActionBuffer->Des(), KBtActionData());
}

//--------------------------------------------------------------------------------------------
//
// CBtSoapCodec::CreateJobSoapL
//
//--------------------------------------------------------------------------------------------
void CBtSoapCodec::CreateJobSoapL(const TInt aJobId)
{
	LOG("[CBtSoapCodec::CreateJobSoapL]\t");
	CreateSoapTemplateL(KBtCreateJobStart(), KBtCreateJobEnd());

	TBuf8<KMaxFileName> filename;
	filename.Format(KXHTMLFileName8(), aJobId);

	TBtAttrString8 nameTempl, userNameTempl;
	nameTempl.Format(KBtValueTemplate(), &KBtJobName());
	userNameTempl.Format(KBtValueTemplate(), &KBtJobOriginatingUserName());
	nameTempl.Format(KBtValueTemplate(), &KBtJobName());

	iXmlHandler->ComposeDataL(KBtCreateJob(), KBtAction());
	iXmlHandler->ComposeDataL(filename, nameTempl);
	
	TBuf8<0x100> name;
	RsUtils::GetLocalBTNameL(name);
	iXmlHandler->ComposeDataL(name, userNameTempl);
}

//--------------------------------------------------------------------------------------------
//
// CBtSoapCodec::GetCapabilitiesSoapL
//
//--------------------------------------------------------------------------------------------
void CBtSoapCodec::GetCapabilitiesSoapL()
{
	LOG("[CBtSoapCodec::GetCapabilitiesSoapL]\t");
	CreateSoapTemplateL(KBtGetPrinterCapabilitiesStart(), KBtGetPrinterCapabilitiesEnd());

	iXmlHandler->ComposeDataL(KBtGetPrinterAttributes(), KBtAction());
}

//--------------------------------------------------------------------------------------------
//
// CBtSoapCodec::GetPrinterStateSoapL
//
//--------------------------------------------------------------------------------------------
void CBtSoapCodec::GetPrinterStateSoapL()
{
	LOG("[CBtSoapCodec::GetPrinterStateSoapL]\t");
	CreateSoapTemplateL(KBtGetPrinterStateStart(), KBtGetPrinterStateEnd());

	iXmlHandler->ComposeDataL(KBtGetPrinterAttributes(), KBtAction());
}

//--------------------------------------------------------------------------------------------
//
// CBtSoapCodec::GetPrinterAttributesSoapL
//
//--------------------------------------------------------------------------------------------
void CBtSoapCodec::GetPrinterAttributesSoapL()
{
	LOG("[CBtSoapCodec::GetPrinterAttributesSoapL]\t");
	CreateSoapTemplateL(KBtGetPrinterAttributesAllStart(), KBtGetPrinterAttributesAllEnd());

	iXmlHandler->ComposeDataL(KBtGetPrinterAttributes(), KBtAction());
}

//--------------------------------------------------------------------------------------------
//
// CBtSoapCodec::GetJobAttributesSoapL
//
//--------------------------------------------------------------------------------------------
void CBtSoapCodec::GetJobAttributesSoapL(const TDesC8&  aJobId)
{
	LOG("[CBtSoapCodec::GetJobAttributesSoapL]\t");
	CreateSoapTemplateL(KBtJobIdStart(), KBtJobIdEnd());

	iXmlHandler->ComposeDataL(KBtGetJobAttributes(), KBtAction());

	TBtAttrString8 templ;
	templ.Format(KBtValueTemplate(), &KBtJobId());

	iXmlHandler->ComposeDataL(aJobId, templ);
}

//--------------------------------------------------------------------------------------------
//
// CBtSoapCodec::CancelJobSoapL
//
//--------------------------------------------------------------------------------------------
void CBtSoapCodec::CancelJobSoapL(const TDesC8&  aJobId)
{
	LOG("[CBtSoapCodec::CancelJobSoapL]\t");
	CreateSoapTemplateL(KBtJobIdStart(), KBtJobIdEnd());

	iXmlHandler->ComposeDataL(KBtCancelJob(), KBtAction());

	TBtAttrString8 templ;
	templ.Format(KBtValueTemplate(), &KBtJobId());

	iXmlHandler->ComposeDataL(aJobId, templ);
}

//--------------------------------------------------------------------------------------------
//
// CBtSoapCodec::GetEventSoapL
//
//--------------------------------------------------------------------------------------------
void CBtSoapCodec::GetEventSoapL(const TDesC8&  aJobId)
{
	LOG("[CBtSoapCodec::GetEventSoapL]\t");
	CreateSoapTemplateL(KBtJobIdStart(), KBtJobIdEnd());

	iXmlHandler->ComposeDataL(KBtGetEvent(), KBtAction());

	TBtAttrString8 templ;
	templ.Format(KBtValueTemplate(), &KBtJobId());

	iXmlHandler->ComposeDataL(aJobId, templ);
}

//--------------------------------------------------------------------------------------------
//
// CBtSoapCodec::GetMarginsSoapL
//
//--------------------------------------------------------------------------------------------
void CBtSoapCodec::GetMarginsSoapL()
{
	LOG("[CBtSoapCodec::GetMarginsSoapL]\t NOT SUPPORTED -> leave");
	User::Leave(KErrNotSupported);
	
	CreateSoapTemplateL(KBtGetMarginsStart(), KBtGetMarginsEnd());

	iXmlHandler->ComposeDataL(KBtGetMargins(), KBtAction());
}

//--------------------------------------------------------------------------------------------
//
// CBtSoapCodec::SoapAction
//
//--------------------------------------------------------------------------------------------
TInt CBtSoapCodec::SoapAction()
{
	LOG1("[CBtSoapCodec::SoapAction()]\t iActionBuffer: %d", iActionBuffer);
	if(!iActionBuffer || 0 >= iActionBuffer->Length())
		return EBtUnknownAction;
	
	TInt len = iActionBuffer->Length() < 50 ? iActionBuffer->Length() : 50;
	TPtrC8 ptr = iActionBuffer->Mid(0, len);
	LOG81("[CBtSoapCodec::SoapAction()]\t ptr: \"%S\"", &ptr);

	/* read action name - the order in list below matters: 
	   be aware the substrings (as CreateJob is for CreateJobResponse) aren't found!*/
	if(KErrNotFound < ptr.Find(KBtCreateJobResponse()))
	{
		return EBtCreateJobResponse;
	}
	else if(KErrNotFound < ptr.Find(KBtCreateJob()))
	{
		return EBtCreateJob;
	}
	else if(KErrNotFound < ptr.Find(KBtGetPrinterAttributesResponse()))
	{
		return EBtGetPrinterAttributesResponse;
	}
	else if(KErrNotFound < ptr.Find(KBtGetPrinterAttributes()))
	{
		return EBtGetPrinterAttributes;
	}
	else if(KErrNotFound < ptr.Find(KBtGetJobAttributesResponse()))
	{
		return EBtGetJobAttributesResponse;
	}
	else if(KErrNotFound < ptr.Find(KBtGetJobAttributes()))
	{
		return EBtGetJobAttributes;
	}
	else if(KErrNotFound < ptr.Find(KBtCancelJobResponse()))
	{
		return EBtCancelJobResponse;
	}
	else if(KErrNotFound < ptr.Find(KBtCancelJob()))
	{
		return EBtCancelJob;
	}
	else if(KErrNotFound < ptr.Find(KBtGetEventResponse()))
	{
		return EBtGetEventResponse;
	}
	else if(KErrNotFound < ptr.Find(KBtGetEvent()))
	{
		return EBtGetEvent;
	}
	else if(KErrNotFound < ptr.Find(KBtGetMarginsResponse()))
	{
		return EBtGetMarginsResponse;
	}
	else if(KErrNotFound < ptr.Find(KBtGetMargins()))
	{
		return EBtGetMargins;
	}

	return EBtUnknownAction;

}

//--------------------------------------------------------------------------------------------
//
// CBtSoapCodec::CBtSoapCodec
//
//--------------------------------------------------------------------------------------------
CBtSoapCodec::CBtSoapCodec()
{
}

//--------------------------------------------------------------------------------------------
//
// CBtSoapCodec::ParseLayout
//
//--------------------------------------------------------------------------------------------
TPrintCapability CBtSoapCodec::ParseLayoutL()
{
	LOG("[CBtSoapCodec::ParseLayout]\t");

	TPrintCapability layout;
	layout.iCapabilityID = EPrintCapabLayout;
	layout.iType = TPrintCapability::Enum;
	layout.iLow = 0;
	layout.iHigh = 0;
	layout.iEnumCount = 0;

	/* Get layout(s) */
	TBtAttrString8 value(KNullDesC8());
	TInt err = KErrNotFound;
	GetAttributeL(KBtNumberUpSupported(), value, err);
	LOG82("[CBtSoapCodec::ParseLayout]\t %S: %S",&KBtNumberUpSupported(), &value);
	LOG1("[CBtSoapCodec::ParseLayout]\t error: %d",err);
	if(KErrNone != err)
	{
		layout.iEnumCodes[0] = EPrintCapabLayout1UpBorderless;
		layout.iEnumCount = 1;
	}
	else
	{
		LOG("[CBtSoapCodec::ParseLayout]\t get layout values...");
		RArray<TInt> arr;
		TBtMapper::Layout(value, arr);
		LOG1("[CBtSoapCodec::ParseLayout]\t layout count: %d", arr.Count());
		for(TInt i = 0; i < arr.Count(); ++i)
		{
			LOG2("[CBtSoapCodec::ParseLayout]\t arr[%d]: %d", i, arr[i]);
			layout.iEnumCodes[i] = arr[i];
		}
		layout.iEnumCount = arr.Count();
		arr.Close();
	}

	RArray<TInt> tmpArr;
	tmpArr.Append(EPrintCapabLayout1UpBorderless);
	tmpArr.Append(EPrintCapabLayout1UpBorder);
	tmpArr.Append(EPrintCapabLayout2Up);
	tmpArr.Append(EPrintCapabLayout4Up);
	tmpArr.Append(EPrintCapabLayout6Up);
	tmpArr.Append(EPrintCapabLayout9Up);
	tmpArr.Append(EPrintCapabLayout12Up);
	tmpArr.Append(EPrintCapabLayout16Up);

	for(TInt i = 0; i < tmpArr.Count(); ++i)
	{
		TInt ix = ValuePosition(layout, tmpArr[i]);
		if(KErrNotFound == ix)
		{
			layout.iEnumCodes[layout.iEnumCount] = tmpArr[i];
			++layout.iEnumCount;
		}
	}
	tmpArr.Close();

	layout.iDefaultValue = layout.iEnumCodes[0];
	
	TInt ix = ValuePosition(layout, EPrintCapabLayout1UpBorderless);
	if(KErrNotFound < ix)
		layout.iDefaultValue = layout.iEnumCodes[ix];

	LOG1("[CBtSoapCodec::ParseLayout]\t return default: %d", layout.iDefaultValue);
	return layout;
}

//--------------------------------------------------------------------------------------------
//
// CBtSoapCodec::ParseQuality
//
//--------------------------------------------------------------------------------------------
TPrintCapability CBtSoapCodec::ParseQualityL()
{
	LOG("[CBtSoapCodec::ParseQuality]\t");

	TPrintCapability quality;
	quality.iCapabilityID = EPrintCapabQuality;
	quality.iType = TPrintCapability::Enum;
	quality.iLow = 0;
	quality.iHigh = 0;
	quality.iDefaultValue = EPrintCapabQualityDefault;
	quality.iEnumCodes[0] = EPrintCapabQualityDefault;
	quality.iEnumCount = 1;

	/* Get quality(s) */
	CDesC8ArrayFlat *qualities = new (ELeave) CDesC8ArrayFlat(10);
	CleanupStack::PushL(qualities);
	
	TInt err = KErrNotFound;
	GetAttributeListL(KBtPrintQualitySupported(), *qualities, err);
	LOG82("[CBtSoapCodec::ParseQuality]\t %S: count: %d",&KBtPrintQualitySupported(), qualities->MdcaCount());
	if(KErrNone != err)
	{
		LOG1("[CBtSoapCodec::ParseQuality]\t return with error %d",err);
		return quality;
	}

	for(TInt i = 0; i < qualities->MdcaCount(); ++i)
	{
		TInt tmp = TBtMapper::Quality( qualities->MdcaPoint(i) );
		LOG2("[CBtSoapCodec::ParseQuality]\t qualities[%2d]: %d",i, tmp);
		if(KErrNotSupported != tmp)
		{
			quality.iEnumCodes[quality.iEnumCount] = tmp;
			++quality.iEnumCount;
		}
		if(KMaxEnumAmount < quality.iEnumCount)
		{
			LOG1("[CBtSoapCodec::ParseQuality]\t max enum count reached: %d",quality.iEnumCount);
			--quality.iEnumCount;
			break;
		}
	}
	CleanupStack::PopAndDestroy(qualities);

	LOG1("[CBtSoapCodec::ParseQuality]\t return default: %d", quality.iDefaultValue);
	return quality;
}

//--------------------------------------------------------------------------------------------
//
// CBtSoapCodec::ParsePaperSize
//
//--------------------------------------------------------------------------------------------
TPrintCapability CBtSoapCodec::ParsePaperSizeL()
{
	LOG("[CBtSoapCodec::ParsePaperSize]\t");

	TPrintCapability size;
	size.iCapabilityID = EPrintCapabPaperSize;
	size.iType = TPrintCapability::Enum;
	size.iLow = 0;
	size.iHigh = 0;
	size.iDefaultValue = EPrintCapabPaperSize4x6;

	/* Get size(s) */
	CDesC8ArrayFlat *sizes = new (ELeave) CDesC8ArrayFlat(10);
	CleanupStack::PushL(sizes);

	TInt err = KErrNotFound;
	GetAttributeListL(KBtMediaSizesSupported(), *sizes, err);
	LOG82("[CBtSoapCodec::ParsePaperSize]\t %S: count %d",&KBtMediaSizesSupported(), sizes->MdcaCount());
	if(KErrNone != err)
	{
		size.iEnumCodes[0] = size.iDefaultValue;
		size.iEnumCount = 1;
		return size;
	}

	TInt enumPos = 0;
	for(TInt i = 0; i < sizes->MdcaCount(); ++i)
	{
//		LOG82("[CBtSoapCodec::ParsePaperSize]\t sizes[%2d]: %S",i, &sizes[i]);
		TInt tmp = TBtMapper::Size(sizes->MdcaPoint(i));
		if(KErrNotSupported != tmp)
		{
			LOG2("[CBtSoapCodec::ParsePaperSize]\t sizes[%2d]: %d",i, tmp);
			size.iEnumCodes[enumPos] = tmp;
			size.iEnumCount = ++enumPos;
		}
		if(KMaxEnumAmount < enumPos)
		{
			LOG1("[CBtSoapCodec::ParsePaperSize]\t max enum count reached: %d",enumPos);
			break;
		}
	}
	CleanupStack::PopAndDestroy(sizes);

	// Get default value from loaded media
	ParseDefaultSizeL(size, enumPos);
	
	// Sort the paper sizes in order 'smallest to biggest'
	RArray<TInt> tmpArr;
	tmpArr.Append(EPrintCapabPaperSizeAuto);
	tmpArr.Append(EPrintCapabPaperSize4x6);
	tmpArr.Append(EPrintCapabPaperSize5x7);
	tmpArr.Append(EPrintCapabPaperSizeA6);
	tmpArr.Append(EPrintCapabPaperSizeA4);
	tmpArr.Append(EPrintCapabPaperSizeLetter);	
	
	for(TInt s = 0; s < tmpArr.Count(); ++s)
	{
		TInt ix = ValuePosition(size,tmpArr[s]);
		if(KErrNotFound == ix)
			tmpArr.Remove(s);
	}

	// Move back to enum array
	for(TInt s2 = 0; s2 < tmpArr.Count(); ++s2)
	{
		size.iEnumCodes[s2] = tmpArr[s2];
	}
	tmpArr.Close();

#ifdef ENABLE_LOGGING
	_LIT(KTab, "|");
	TFileName tmp(KTab);
	
	for(TInt l = 0; l < size.iEnumCount; ++l)
	{
		tmp.AppendNum(size.iEnumCodes[l]);
		tmp.Append(KTab);
	}
	LOG1("[CBtSoapCodec::ParsePaperSizeL]\t order: %S", &tmp); 
#endif

	return size;
}

//--------------------------------------------------------------------------------------------
//
// CBtSoapCodec::ParseDefaultSizeL
//
//--------------------------------------------------------------------------------------------
void CBtSoapCodec::ParseDefaultSizeL(TPrintCapability& aSize, TInt& aPos)
{
	LOG("[CBtSoapCodec::ParseDefaultSizeL]\t");

	//reserve more space for this: there are several long text lines
	TBuf8<KAttrStrLen*10> value(KNullDesC8());
	TInt err = KErrNotFound;
	
	GetAttributeL(KBtMediaLoaded(), value, err);

#ifdef ENABLE_LOGGING
	TInt len = value.Length() < 50 ? value.Length() : 50;
	TPtrC8 p = value.Mid(0, len);
	LOG82("[CBtSoapCodec::ParseDefaultValues]\t %S: ptr: \"%S\"",&KBtMediaLoaded(), &p);
#endif

	if(KErrNone != err)
	{
		LOG82("[CBtSoapCodec::ParseDefaultSizeL]\t %S not found. Error: %d. Return.",&KBtMediaLoaded(), err);
		return;
	}

	CXmlHandler* tmpHandler = CXmlHandler::NewLC();
	tmpHandler->InitFromBufferL(value);
	TBtAttrString8 start;
	TBtAttrString8 end;
	start.Format(KBtStartTag(), &KBtLoadedMediumSize());
	end.Format(KBtEndTag(), &KBtLoadedMediumSize());

	TPtrC8 defValue = tmpHandler->GetDataL(start, end, err);
	TInt tmpDefault = TBtMapper::Size(defValue);

	CleanupStack::PopAndDestroy(tmpHandler);

	if(KErrNotSupported != tmpDefault)
	{
		aSize.iDefaultValue = tmpDefault;
	}

	// Check the default value exists...
	for(TInt i = 0; i < aSize.iEnumCount; ++i)
	{
		if(aSize.iDefaultValue == aSize.iEnumCodes[i])
		{
			return;
		}
	}

	// ...if not, append it.
	if(KMaxEnumAmount < aPos)
	{
		--aPos;
	}
	aSize.iEnumCodes[aPos] = aSize.iDefaultValue;
	aSize.iEnumCount = ++aPos;

	LOG1("[CBtSoapCodec::ParseDefaultSizeL]\t return default: %d", aSize.iDefaultValue);
}


//--------------------------------------------------------------------------------------------
//
// CBtSoapCodec::ValuePosition
//
//--------------------------------------------------------------------------------------------
TInt CBtSoapCodec::ValuePosition(TPrintCapability aCapab, TInt aValue)
{
	TInt pos = KErrNotFound;
	for(TInt i = 0; i < aCapab.iEnumCount; ++i)
		if(aValue == aCapab.iEnumCodes[i])
			pos = i;
		
	return pos;
}

////// TEST METHODS ///////////
//--------------------------------------------------------------------------------------------
void CBtSoapCodec::TestMeL()
{

    _LIT8(KSepar, "\n-------------------------\n");
    _LIT8(KKErrNotFound, "KErrNotFound");
    _LIT8(KLF, "\n%d\n");

	RFs fs;
	CleanupClosePushL(fs);
	User::LeaveIfError(fs.Connect());

    /*create*/
    CBtSoapCodec* codec = CBtSoapCodec::NewL();
    TPtrC8 ptr = codec->CreateSoapL(EBtGetPrinterAttributes);

	_LIT(KTstFile, "c:\\data\\tst.txt");
	RFile f;
	CleanupClosePushL(f);
	f.Replace(fs,KTstFile, EFileWrite);
	f.Write(ptr);
	f.Write(KSepar());

   	/*read*/
	TInt act;
	CBufFlat* data = CBufFlat::NewL(8);
	data->InsertL(0, GetPrinterAttributesResponseTestSoap());

	codec->ReadSoapL(act, *data);

	/*get attribute*/
	TBtAttrString8 val, errStr;
	_LIT8(KPrinterState,		"PrinterState");
	_LIT8(KPrinterStateReasons,	"PrinterStateReasons");
	TInt err = KErrNoMemory;

	codec->GetAttributeL(KPrinterState(), val, err);
	TInt s;
	errStr.Format(KLF(), err);
	f.Size(s);
	f.Write(s+1, KPrinterState);
	f.Write(errStr);
	f.Write(val);
	f.Write(KSepar());

	codec->GetAttributeL(KPrinterStateReasons(), val, err);
	errStr.Format(KLF(), err);
	f.Size(s);
	f.Write(s+1, KPrinterStateReasons);
	f.Write(errStr);
	f.Write(val);
	f.Write(KSepar());

	codec->GetAttributeL(KKErrNotFound(), val, err);
	errStr.Format(KLF(), err);
	f.Size(s);
	f.Write(s+1, KKErrNotFound);
	f.Write(errStr);
	f.Write(val);
	f.Write(KSepar());

	CleanupStack::PopAndDestroy(2);	// f, fs

}


TPtrC8 CBtSoapCodec::CreateJobResponseTestSoap()
{
    _LIT8(KTestSoap, "CONTENT-LENGTH:200\nCONTENT-TYPE:text/xml; charset=\"utf-8\""
   				"<s:Envelope\nxmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\""
   				"s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\">"
   				"<s:Body>"
				"<u:CreateJobResponse xmlns:u=\"urn:schemas-bluetooth-org:service:Printer:1\">"
					"<JobId>12345</JobId>"
					"<OperationStatus>0x0001</OperationStatus>"
				"</u:CreateJobResponse>"
				"</s:Body>"
				"</s:Envelope>");
				   				
	TPtrC8 response = KTestSoap();
	return response;
}
TPtrC8 CBtSoapCodec::CancelJobResponseTestSoap()
{
    _LIT8(KTestSoap, "CONTENT-LENGTH:200\nCONTENT-TYPE:text/xml; charset=\"utf-8\""
   				"<s:Envelope\nxmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\""
   				"s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\">"
   				"<s:Body>"
				"<u:CancelJobResponse xmlns:u=\"urn:schemas-bluetooth-org:service:Printer:1\">"
					"<JobId>12345</JobId>"
					"<OperationStatus>0x0406</OperationStatus>"
				"</u:CancelJobResponse>"
				"</s:Body>"
				"</s:Envelope>");
   				
	TPtrC8 response = KTestSoap();
	return response;
}
TPtrC8 CBtSoapCodec::GetPrinterAttributesResponseTestSoap()
{
    _LIT8(KTestSoap, "CONTENT-LENGTH:200\nCONTENT-TYPE:text/xml; charset=\"utf-8\""
   				"<s:Envelope\nxmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\""
   				"s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\">"
   				"<s:Body>"
   				"<u:GetPrinterAttributesResponse xmlns:u=\"urn:schemas-bluetooth-org:service:Printer:1\">"
    				"<PrinterName>MyPrinter</PrinterName>"
    				"<PrinterLocation>MyLocation</PrinterLocation>"
    				"<PrinterState>idle</PrinterState>"
    				"<PrinterStateReasons>none</PrinterStateReasons>"
    				"<PrinterGeneralCurrentOperator></PrinterGeneralCurrentOperator>"
    				"<DocumentFormatsSupported>"
	    				"<DocumentFormat> application/vnd.pwg-xhtml-print+xml:0.95 </DocumentFormat>"
	    				"<DocumentFormat> application/vnd.hp-PCL:5E</DocumentFormat>"
	    				"<DocumentFormat> text/plain</DocumentFormat>"
	    				"<DocumentFormat> application/PostScript:3</DocumentFormat>"
    				"</DocumentFormatsSupported>"
    				"<ImageFormatsSupported>"
	    				"<ImageFormat>image/jpeg</ImageFormat>"
	    				"<ImageFormat>image/gif</ImageFormat>"
    				"</ImageFormatsSupported>"
    				"<ColorSupported>true</ColorSupported>"
    				"<MaxCopiesSupported>1</MaxCopiesSupported>"
    				"<SidesSupported>"
	    				"<Sides> one-sided</Sides>"
	    				"<Sides> two-sided-long-edge</Sides>"
	    				"<Sides> two-sided-short-edge</Sides>"
    				"</SidesSupported>"
    				"<NumberUpSupported>4</NumberUpSupported>"
    				"<OrientationsSupported>"
	    				"<Orientation>portrait</Orientation>"
	    				"<Orientation>landscape</Orientation>"
    				"</OrientationsSupported>"
    				"<MediaSizesSupported>"
	    				"<MediaSize>iso_a4_210x297mm</MediaSize>"
	    				"<MediaSize>iso_a3_297x420mm</MediaSize>"
    				"</MediaSizesSupported>"
    				"<MediaTypesSupported>"
	    				"<MediaType>stationery</MediaType>"
	    				"<MediaType>photographic</MediaType>"
	    				"<MediaType>cardstock</MediaType>"
    				"</MediaTypesSupported>"
    				"<MediaLoaded>"
	    				"<LoadedMediumDetails>"
		    				"<LoadedMediumSize> iso_a4_210x297mm</LoadedMediumSize>"
		    				"<LoadedMediumType>stationery</LoadedMediumType>"
	    				"</LoadedMediumDetails>"
	    				"<LoadedMediumDetails>"
		    				"<LoadedMediumSize> iso_a4_210x297mm</LoadedMediumSize>"
		    				"<LoadedMediumType>photographic</LoadedMediumType>"
	    				"</LoadedMediumDetails>"
    				"</MediaLoaded>"
    				"<PrintQualitySupported>"
	    				"<PrintQuality>draft</PrintQuality>"
	    				"<PrintQuality>normal</PrintQuality>"
	    				"<PrintQuality>fine</PrintQuality>"
    				"</PrintQualitySupported>"
    				"<QueuedJobCount>1</QueuedJobCount>"
    				"<OperationStatus>0x0000</OperationStatus>"
   				"</u:GetPrinterAttributesResponse></s:Body></s:Envelope>");

	TPtrC8 response = KTestSoap();
	return response;
}
TPtrC8 CBtSoapCodec::GetJobAttributesResponseTestSoap()
{
    _LIT8(KTestSoap, "CONTENT-LENGTH:200\nCONTENT-TYPE:text/xml; charset=\"utf-8\""
   				"<s:Envelope\nxmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\""
   				"s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\">"
   				"<s:Body>"
				"<u:GetJobAttributesResponse xmlns:u=\"urn:schemas-bluetooth-org:service:Printer:1\">"
					"<JobId>12345</JobId>"
					"<JobState>printing</JobState>"
					"<JobName>MyExpenseReport</JobName>"
					"<JobOriginatingUserName>mailto:MyEmail</JobOriginatingUserName>"
					"<JobMediaSheetsCompleted>2</JobMediaSheetsCompleted>"
					"<NumberOfInterveningJobs>0</NumberOfInterveningJobs>"
					"<OperationStatus>0x0000</OperationStatus>"
				"</u:GetJobAttributesResponse>"
				"</s:Body>"
				"</s:Envelope>");	
	
	TPtrC8 response = KTestSoap();
	return response;
}
TPtrC8 CBtSoapCodec::GetEventsResponseTestSoap()
{
    _LIT8(KTestSoap, "CONTENT-LENGTH:200\nCONTENT-TYPE:text/xml; charset=\"utf-8\""
   				"<s:Envelope\nxmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\""
   				"s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\">"
   				"<s:Body>"
				"<u:GetEventResponse xmlns:u=\"urn:schemas-bluetooth-org:service:Printer:1\">"
					"<JobId>12345</JobId>"
					"<JobState>stopped</JobState>"
					"<PrinterState>stopped</PrinterState>"
					"<PrinterStateReasons>media-empty</PrinterStateReasons>"
					"<OperationStatus>0x0000</OperationStatus>"
				"</u:GetEventResponse>"
				"</s:Body>"
				"</s:Envelope>");
   				
	TPtrC8 response = KTestSoap();
	return response;
}
TPtrC8 CBtSoapCodec::GetMarginsResponseTestSoap()
{
    _LIT8(KTestSoap, "CONTENT-LENGTH:200\nCONTENT-TYPE:text/xml; charset=\"utf-8\""
   				"<s:Envelope\nxmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\""
   				"s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\">"
   				"<s:Body>"
				"<u:GetMarginsResponse xmlns:u=\"urn:schemas-bluetooth-org:service:Printer:1\">"
				"	<Margins>.25in,.25in,0in,.25in</Margins>"
				"	<OperationStatus>0x0000</OperationStatus>"
				"</u:GetMarginsResponse>"
				"</s:Body>"
				"</s:Envelope>");
   				
	TPtrC8 response = KTestSoap();
	return response;
}

// End of file
