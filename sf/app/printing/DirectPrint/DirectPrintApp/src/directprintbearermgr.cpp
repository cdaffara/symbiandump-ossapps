/*
* Copyright (c) 2010 Kanrikogaku Kenkyusho, Ltd.
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Kanrikogaku Kenkyusho, Ltd. - Initial contribution
*
* Contributors:
*
* Description:
* {Description of the file}
*
*/

// User includes
#include "clog.h"
#include "DirectPrintApp.pan"
#include "DirectPrintBearerMgr.h"
#include "DirectPrintModel.h"
#include "directprintbearerdata.h"
#include "directprintprinterdata.h"

#include "directprintprintsettings.h"
#include "directprinttintcapability.h"
#include "directprintlistcapability.h"
#include "directprintfloatcapability.h"
#include "directprinttextcapability.h"
#include "directprintsettingsconverter.h"
#include "directprintcapability.h"

const TInt PROTOCOL_ID_BIT_COUNT = 6;

CDirectPrintBearerMgr::CDirectPrintBearerMgr(MDirectPrintBearerMngObserver* aObserver,
											 CDirectPrintModel& aModel)
	: CActive( CActive::EPriorityHigh )
	, iObserver(aObserver)
	, iModel(aModel)
	{
	LOG("CDirectPrintBearerMgr::CDirectPrintBearerMgr()");
	CActiveScheduler::Add(this);
	}

CDirectPrintBearerMgr::~CDirectPrintBearerMgr()
	{
	LOG("CDirectPrintBearerMgr::~CDirectPrintBearerMgr()");
	if (IsActive())
		{
		Cancel();
		}
	iBearer.Close();
	delete iPrintSettings;
	}

CDirectPrintBearerMgr* CDirectPrintBearerMgr::NewL(MDirectPrintBearerMngObserver* aObserver,
										CDirectPrintModel& aModel)
	{
	CDirectPrintBearerMgr* self = CDirectPrintBearerMgr::NewLC(aObserver, aModel);
	CleanupStack::Pop(self);
	return self;
	}

CDirectPrintBearerMgr* CDirectPrintBearerMgr::NewLC(MDirectPrintBearerMngObserver* aObserver,
										CDirectPrintModel& aModel)
	{
	CDirectPrintBearerMgr* self = new(ELeave) CDirectPrintBearerMgr(aObserver, aModel);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

void CDirectPrintBearerMgr::ConstructL()
	{
	LOG("CDirectPrintBearerMgr::ConstructL BEGIN");
	__ASSERT_ALWAYS(iObserver, Panic(EDirectPrintNoObserver));
	iPrintSettings = new ( ELeave ) CDirectPrintPrintSettings;
	iBearer.ConnectL();
	SetBearerStatus(EBearerMngConnected);
	LOG("CDirectPrintBearerMgr::ConstructL END");
	}

void CDirectPrintBearerMgr::CreateJobL()
	{
	LOG("CDirectPrintBearerMgr::CreateJobL BEGIN");
	RPointerArray<TDesC> cmdline;
	CleanupClosePushL(cmdline);

	_LIT(KDPAppName, "DirectPrint");
	cmdline.AppendL(&(KDPAppName()));

	TDirectPrintBearerData bearerData;
	TDirectPrintPrinterData printerData;
	TInt indexBearer = iModel.CurrentBearer();
	TInt indexPrinter = iModel.CurrentPrinter();
	if (indexBearer >= 0 && indexPrinter >= 0)
		{
		iModel.GetBearerDataL(indexBearer, bearerData);
		iModel.GetPrinterDataL(indexPrinter, printerData);
		}
	else
		{
		LOG("CDirectPrintBearerMgr::CreateJobL LEAVE1 KErrNotFound");
		User::Leave(KErrNotFound);
		}

	HBufC* apname = HBufC::NewLC(50);
	TPtr apnamePtr(apname->Des());
	_LIT(KDPApName, "-S%S");
	apnamePtr.AppendFormat(KDPApName(), &(bearerData.iHostName));
	cmdline.AppendL(apname);

	HBufC* printername = HBufC::NewLC(50);
	TPtr printernamePtr(printername->Des());
	_LIT(KDPPrinterName, "-P000000439163");
	printernamePtr.Copy(KDPPrinterName());
	cmdline.AppendL(printername);

	HBufC* filename = HBufC::NewLC(50);
	TPtr filenamePtr(filename->Des());
#ifdef __WINSCW__
		_LIT(KDPFileName, "c:\\fpr.pcl");
#else
//		_LIT(KDPFileName, "e:\\fpr.pcl");
		_LIT(KDPFileName, "c:\\fpr.pcl");
#endif
	filenamePtr.Copy(KDPFileName());
	cmdline.AppendL(filename);

	SetBearerStatus(EBearerMngCreatingJob);

	// bearer ID
	TInt printerID = 128;
	TInt err = iBearer.CreateJob(printerID, iJobGuardData,
						cmdline, iStatus);
	if( !err )
		{
		LOG("Start create JOB");
		SetActive();
		}
	else
		{
		LOG1("CDirectPrintBearerMgr::CreateJobL LEAVE2[%d]", err);
		User::Leave(err);
		}

	CleanupStack::PopAndDestroy(filename);
	CleanupStack::PopAndDestroy(printername);
	CleanupStack::PopAndDestroy(apname);
	CleanupStack::PopAndDestroy(&cmdline);
	LOG("CDirectPrintBearerMgr::CreateJobL END");
	}

void CDirectPrintBearerMgr::RunL()
	{
	LOG1("CDirectPrintBearerMgr::RunL begin with iStatus.Int(): %d", iStatus.Int());
	if( iStatus == KErrNone && iObserver )
		{
		switch( iJobGuardData.iCb )
			{
			case TDirectPrintJobGuardData::EProgressEventCb:
				LOG("CJobGuard::RunL TDirectPrintJobGuardData::EProgressEventCb");
				LOG1("CJobGuard::RunL iJobGuardData.iStatus: %d", iJobGuardData.iStatus);
				LOG1("CJobGuard::RunL iJobGuardData.iPercentCompletion: %d", iJobGuardData.iPercentCompletion);
				LOG1("CJobGuard::RunL iJobGuardData.iJobStateCode: %d", iJobGuardData.iJobStateCode);
				iObserver->PrintJobProgressEvent( iJobGuardData.iStatus, iJobGuardData.iPercentCompletion, iJobGuardData.iJobStateCode );
				break;
			case TDirectPrintJobGuardData::EErrorEventCb:
				LOG("CJobGuard::RunL TDirectPrintJobGuardData::EErrorEventCb");
				LOG1("CJobGuard::RunL iJobGuardData.iError: %d", iJobGuardData.iError);
				LOG1("CJobGuard::RunL iJobGuardData.iErrorStringCode: %d", iJobGuardData.iErrorStringCode);
				iObserver->PrintJobErrorEvent( iJobGuardData.iError, iJobGuardData.iErrorStringCode );
				break;
			case TDirectPrintJobGuardData::EStatusEventCb:
				LOG("CJobGuard::RunL TDirectPrintJobGuardData::EStatusEventCb");
				LOG1("CJobGuard::RunL iJobGuardData.iError: %d", iJobGuardData.iError);
				LOG1("CJobGuard::RunL iJobGuardData.iErrorStringCode: %d", iJobGuardData.iErrorStringCode);
				iObserver->PrintJobErrorEvent( iJobGuardData.iError, iJobGuardData.iErrorStringCode );
				break;
			case TDirectPrintJobGuardData::EImageEventCb:
				LOG("CJobGuard::RunL TDirectPrintJobGuardData::EImageEventCb");
				LOG1("CJobGuard::RunL iJobGuardData.iFsBitmapHandle: %d", iJobGuardData.iFsBitmapHandle);
				iObserver->PreviewImageEvent( iJobGuardData.iFsBitmapHandle );
				break;
			default:
				break;
			}

		LOG1("CDirectPrintBearerMgr::RunL BearerStatus=[%d]", iBearerStatus);

		switch (iBearerStatus)
			{
			case EBearerMngNotConnected:
			case EBearerMngConnected:
			case EBearerMngCreatingJob:
			case EBearerMngPrinting:
				{
				LOG("CDirectPrintBearerMgr::RunL ContinueCreateJob");
				iBearer.ContinueCreateJob( iJobGuardData, iStatus );
				SetActive();
				}
				break;
			case EBearerMngClosing:
			case EBearerMngCanceling:
			default:
				break;
			}

		}
	LOG("CDirectPrintBearerMgr::RunL end");
	}

void CDirectPrintBearerMgr::SubmitPrintJobL()
	{
	LOG("CDirectPrintBearerMgr::SubmitPrintJobL BEGIN");
	SetBearerStatus(EBearerMngPrinting);
	// Submit print job
	TInt err = iBearer.SubmitPrintJob();
	if( err != KErrNone )
		{
		User::Leave(err);
		}
	LOG("CDirectPrintBearerMgr::SubmitPrintJobL END");
	}

void CDirectPrintBearerMgr::CloseSession()
	{
	LOG("CDirectPrintBearerMgr::CloseSession BEGIN");
	SetBearerStatus(EBearerMngClosing);
	Cancel();
//	iBearer.Close();
	SetBearerStatus(EBearerMngConnected);
	LOG("CDirectPrintBearerMgr::CloseSession END");
	}

void CDirectPrintBearerMgr::DoCancel()
	{
	LOG("CDirectPrintBearerMgr::DoCancel BEGIN");
	SetBearerStatus(EBearerMngCanceling);
	iBearer.CancelCreateJob();
	LOG("CDirectPrintBearerMgr::DoCancel END");
	}

void CDirectPrintBearerMgr::SetBearerStatus(TBearerMngStatus aStatus)
	{
	LOG1("CDirectPrintBearerMgr::SetBearerStatus [%d]", aStatus);
	iBearerStatus = aStatus;
	}

CDirectPrintBearerMgr::TBearerMngStatus CDirectPrintBearerMgr::BearerStatus()
	{
	return iBearerStatus;
	}

TInt CDirectPrintBearerMgr::SupportedProtocols()
	{
	return iBearer.SupportedProtocols();
	}

TInt CDirectPrintBearerMgr::GetProtocolNames(RSelectItemArray& aNames)
	{
	return iBearer.GetProtocolNames(aNames);
	}

void CDirectPrintBearerMgr::InitCapabilitiesL(TInt aIndex)
	{
	TInt printerUid = CreatePrinterUid(aIndex);

	RArray<TInt> capabIDs;
	CleanupClosePushL( capabIDs );

	iBearer.GetPrinterCapabilityIDs( printerUid, capabIDs );
	CDirectPrintPrintSettings* printSettings = new ( ELeave ) CDirectPrintPrintSettings;
	CleanupStack::PushL( printSettings );

	TInt i( 0 );
	TInt num( capabIDs.Count() );
	for ( i = 0; i < num; i++ )
		{
		TDirectPrintCapability capab;
		iBearer.GetPrinterCapability( printerUid, capabIDs[i], capab );
		//capab.iType = TDirectPrintCapability::Enum;

		CDirectPrintBaseCapability* capability = 0;

		// Create capability
		switch ( capab.iType )
			{
			case TDirectPrintCapability::Int:
				capability = DirectPrintSettingsConverter::AddTIntCapabilityLC( capab, i, capabIDs[i] );
				break;
			case TDirectPrintCapability::Enum:
				capability = DirectPrintSettingsConverter::AddListCapabilityLC( capab, i, capabIDs[i] );
				break;
			case TDirectPrintCapability::Float:
				capability = DirectPrintSettingsConverter::AddFloatCapabilityLC( capab, i, capabIDs[i] );
				break;
			case TDirectPrintCapability::Text:
				capability = DirectPrintSettingsConverter::AddTextCapabilityLC( capab, i, capabIDs[i] );
				break;
			default:
				User::Leave( KErrUnknown );
				break;
			}
		//iBearer.GetJobSetting( capabIDs[i], capability->iValue );

		// Add the capability to the list
		User::LeaveIfError( printSettings->iCapabilities.Append( capability ) );
		CleanupStack::Pop();    // capability
		}

	delete iPrintSettings;
	iPrintSettings = printSettings;

	CleanupStack::Pop();    // printSettings
	CleanupStack::PopAndDestroy();  // capabIDs
	}

TInt CDirectPrintBearerMgr::GetJobSetting(TInt aCapabilityID, TInt& aValue)
	{
	return iBearer.GetJobSetting(aCapabilityID, aValue);
	}

TInt CDirectPrintBearerMgr::SetJobSetting(TInt aCapabilityID, TInt aValue, TInt& aAffectedCapability)
	{
	return iBearer.SetJobSetting(aCapabilityID, aValue, aAffectedCapability);
	}

void CDirectPrintBearerMgr::SetJobSettings()
	{
	TInt i( 0 );
	TInt num( iPrintSettings->iCapabilities.Count() );
	for ( i = 0; i < num; i++ )
		{
		TInt getSettings;
		iBearer.SetJobSetting(iPrintSettings->iCapabilities[i]->iUid, iPrintSettings->iCapabilities[i]->iValue, getSettings);
		}
	}

CDirectPrintPrintSettings* CDirectPrintBearerMgr::PrinterSettingsL()
	{
	//return iPrintSettings->CloneL();
	return iPrintSettings;
	}

TInt CDirectPrintBearerMgr::CreatePrinterUid(TInt aIndex)
	{
	TInt printerUid = 0;
	TInt protocolIndex = aIndex;
	protocolIndex <<= sizeof(TInt)*8 - PROTOCOL_ID_BIT_COUNT;
	printerUid |= protocolIndex;
	return printerUid;
	}

// End of file
