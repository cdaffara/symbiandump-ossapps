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
*
*/

// INCLUDE FILES
#include <avkon.hrh>
#include <aknnotewrappers.h>
#include <akntabgrp.h>
#include <barsread.h>
#include <stringloader.h>
#include <DirectPrintApp.rsg>
#include <f32file.h>
#include <s32file.h>
#include <f32file.h>
#include <aiwmenu.h>					// CAiwGenericParamList

//not use #include <featmgr.h>			// FeatureManager

#include <xmlengdomparser.h>			// RXmlEngDOMParser
#include <xmlengdomimplementation.h>	// RXmlEngDOMImplementation
#include <xmlengerrors.h>

// Start ImagePrint Provide
#include "imageprint.h"  
#include "printmessagecodes.h"
// End   ImagePrint Provide

#include "clog.h"
#include "aiwprintproperty.h"

#include "directprintapp.pan"
#include "directprintappappui.h"
#include "directprintapp.hrh"
#include "directprintappdocument.h"
#include "directprintmodel.h"

#include "directprintmainview.h"
#include "directprintlistview.h"
#include "directprintsettingview.h"
#include "directprintpreviewview.h"
#include "directprintpropertyview.h"
#include "directprintsearchresultview.h"
#include "directprintengine.h"
#include "directprintbearermgr.h"
#include "directprintmsgutil.h"
#include "directprintuipluginmgr.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CDirectPrintAppAppUi::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CDirectPrintAppAppUi::ConstructL()
	{
	LOG("CDirectPrintAppAppUi::ConstructL BEGIN");
	// Initialise app UI with standard value.
    //BaseConstructL( EAknEnableSkin | EAknEnableMSK );
    BaseConstructL( EAknEnableSkin );

//not use    FeatureManager::InitializeLibL();

    iDoc = static_cast<CDirectPrintAppDocument*>( CAknAppUi::Document() );

	TRAPD( err, InitializeEngineL() );
	if ( err == KErrInUse )
		{
		LOG1("InitializeEngineL Error [%d]", err);
		CDirectPrintMsgUtil::DisplayErrorNoteL(R_DIRECTPRINT_MSG_PRINT_IN_USE);
		Exit();
		}
	else
		{
		User::LeaveIfError( err );
		}

	TRAP( err, InitializeBearerMngL() );
	if ( err == KErrInUse || err == KErrAlreadyExists )
		{
		LOG1("InitializeBearerMngL Error [%d]", err);
		CDirectPrintMsgUtil::DisplayErrorNoteL(R_DIRECTPRINT_MSG_PRINT_IN_USE);
		Exit();
		}
	else
		{
		User::LeaveIfError( err );
		}

	InitializePluginMgrL();

	LOG("CDirectPrintAppAppUi::ConstructL StatusPane...");
	CEikStatusPane* statusPane = StatusPane();
	CAknNavigationControlContainer* naviPane = (CAknNavigationControlContainer*) statusPane->ControlL(TUid::Uid(EEikStatusPaneUidNavi));
		{
		TResourceReader reader;
		iCoeEnv->CreateResourceReaderLC(reader, R_DIRECTPRINT_PROPERTYSETTING_NAVI_DECORATOR);
		iNaviDecorator = naviPane->ConstructNavigationDecoratorFromResourceL(reader);
		CleanupStack::PopAndDestroy();
		}
	CAknTabGroup* tabGroup = static_cast<CAknTabGroup*>(iNaviDecorator->DecoratedControl());
	tabGroup->SetObserver(this);

	LOG("CDirectPrintAppAppUi::ConstructL aiw param...");
	// aiw param
	iAiwState = ENotAiwCall;
	TInt r = iAiwCall.Attach(KPrintPropertyCat,EPrintProperty,EOwnerProcess);
	if (r == KErrNone)
		{
		iAiwCall.Get(KPrintPropertyCat,EPrintProperty,iAiwState);
		}

	if ( iAiwState == EAiwPrintCall )
		{
		// Set the aiw call RProperty key back to default value so that
		// if application for some reason crashes the value is not left for AIW mode.
		iAiwCall.Set( KPrintPropertyCat,EPrintProperty, ENotAiwCall );
		TRAP( err, LoadParamL() );
		if ( err != KErrNone )
			{
			CDirectPrintMsgUtil::DisplayErrorNoteL(R_DIRECTPRINT_MSG_PARAM_INCORRECT);
			Exit();
			}
		}

	LOG("CDirectPrintAppAppUi::ConstructL InitializeViewsL...");
	InitializeViewsL();

    iCoeEnv->RootWin().EnableScreenChangeEvents();

	LOG("CDirectPrintAppAppUi::ConstructL END");
	}

// -----------------------------------------------------------------------------
// CDirectPrintAppAppUi::CDirectPrintAppAppUi()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CDirectPrintAppAppUi::CDirectPrintAppAppUi()
	{
	// No implementation required
	}

// -----------------------------------------------------------------------------
// CDirectPrintAppAppUi::~CDirectPrintAppAppUi()
// Destructor.
// -----------------------------------------------------------------------------
//
CDirectPrintAppAppUi::~CDirectPrintAppAppUi()
	{
//not use	FeatureManager::UnInitializeLib();

    if (iXmlEngDoc.NotNull()) 
        {
	    iXmlEngDoc.Close();               // closing all opened objects
	    iXmlParser.Close();
	    iXmlDomImpl.Close();
        }
	delete iXmlParam;
	delete iEngine;
	delete iBearerMgr;
	delete iPluginMgr;
	delete iNaviDecorator;
    iAiwCall.Close();
	}

// -----------------------------------------------------------------------------
// CDirectPrintAppAppUi::HandleCommandL()
// Takes care of command handling.
// -----------------------------------------------------------------------------
//
void CDirectPrintAppAppUi::HandleCommandL( TInt aCommand )
	{
	LOG1("CDirectPrintAppAppUi::HandleCommandL(%d) BEGIN", aCommand);
	switch( aCommand )
		{
		case EEikCmdExit:
		case EAknSoftkeyExit:
		case EDirectPrintCmdIdMainViewExit:
			{
			LOG1("BearerStatu=%d", iBearerMgr->BearerStatus());

			TBool isPrinting = EFalse;
			switch (iBearerMgr->BearerStatus())
				{
				case CDirectPrintBearerMgr::EBearerMngCreatingJob:
				case CDirectPrintBearerMgr::EBearerMngPrinting:
					isPrinting = ETrue;
					break;
				case CDirectPrintBearerMgr::EBearerMngClosing:
				case CDirectPrintBearerMgr::EBearerMngCanceling:
				case CDirectPrintBearerMgr::EBearerMngNotConnected:
				case CDirectPrintBearerMgr::EBearerMngConnected:
				default:
					isPrinting = EFalse;
					break;
				}

			if (!isPrinting)
				{
				iDoc->Model().ExternalizeDataL();
				Exit();
				}
			else
				{
				// Show information message
				HBufC* textResource = StringLoader::LoadLC(R_DIRECTPRINT_MSG_NOW_PRINTING);
				CAknInformationNote* informationNote;
				informationNote = new (ELeave) CAknInformationNote;
				informationNote->ExecuteLD(*textResource);
				CleanupStack::PopAndDestroy(textResource);
				}
			}
			break;
		case EDirectPrintCmdIdMainViewPrint:
		case EDirectPrintCmdIdPropertySettingViewPrint:
		case EDirectPrintCmdIdPreviewViewPrint:
			{
			MakeNaviPaneVisibleL(EFalse);
			//TInt modelcount = iEngine->ModelCount();
			iEngine->SetUseModel(0);
			iEngine->PrintL();
			}
			break;
		case EDirectPrintCmdIdMainViewPreview:
		case EDirectPrintCmdIdPropertySettingViewPreview:
			{
			TVwsViewId id;
			GetActiveViewId(id);
			iBackViewUid = id.iViewUid;
			//MakeStatusPaneVisible(EFalse);
			MakeNaviPaneVisibleL(EFalse);
			ActivateLocalViewL(TUid::Uid(EDirectPrintPreviewViewId));
			}
			break;
		case EDirectPrintCmdIdMainViewPropertySetting:
			{
			if (iDoc->Model().PrinterCount() > 0)
				{
				CAknTabGroup* tabGroup = static_cast<CAknTabGroup*>(iNaviDecorator->DecoratedControl());
				tabGroup->SetActiveTabByIndex(0);
			//	tabGroup->SetActiveTabByIndex(1);
				MakeNaviPaneVisibleL(ETrue);
			//	ActivateLocalViewL(TUid::Uid(EDirectPrintPropertySetting1ViewId));
			//	ActivateLocalViewL(TUid::Uid(EDirectPrintPropertySetting2ViewId));
				ActivateLocalViewL(TUid::Uid(EDirectPrintPropertyViewId));
				}
			}
			break;
		case EDirectPrintCmdIdMainViewPrinterList:
			{
			MakeNaviPaneVisibleL(EFalse);
			ActivateLocalViewL(TUid::Uid(EDirectPrintPrinterListViewId));
			}
			break;
		case EDirectPrintCmdIdPrinterSettingViewBack:
			{
			if (iBackViewUid != TUid::Uid(0))
				{
				ActivateLocalViewL(iBackViewUid);
				iBackViewUid = TUid::Uid(0);
				}
			else
				{
				ActivateLocalViewL(TUid::Uid(EDirectPrintPrinterListViewId));
				}
			}
			break;
		case EDirectPrintCmdIdMainViewPrinterSetting:
		case EDirectPrintCmdIdPrinterListViewNew:
		case EDirectPrintCmdIdPrinterListViewEdit:
			{
			if ((iDoc->Model().PrinterCount() > 0) || (iDoc->Model().IsNewData()))
				{
				TVwsViewId id;
				GetActiveViewId(id);
				iBackViewUid = id.iViewUid;
				MakeNaviPaneVisibleL(EFalse);
				ActivateLocalViewL(TUid::Uid(EDirectPrintPrinterSettingViewId));
				}
			}
			break;
		case EDirectPrintCmdIdPrinterListViewDone:
		case EDirectPrintCmdIdPropertySettingViewBack:
			{
			MakeNaviPaneVisibleL(EFalse);
			ActivateLocalViewL(TUid::Uid(EDirectPrintMainViewId));
			}
			break;
		case EDirectPrintCmdIdPrinterListViewSearch:
			{
			// Start printer search
			//SearchPrinterL();
			ActivateLocalViewL(TUid::Uid(EDirectPrintSearchResultViewId));
			}
			break;
		case EDirectPrintCmdIdSearchResultViewSearch:
			{
			// Start printer search
			//SearchPrinterL();
			}
			break;
		case EDirectPrintCmdIdSearchResultViewSelect:
		case EDirectPrintCmdIdSearchResultViewCancel:
			{
			ActivateLocalViewL(TUid::Uid(EDirectPrintPrinterListViewId));
			}
			break;
		case EDirectPrintCmdIdPreviewViewBack:
			{
			MakeStatusPaneVisible(ETrue);
			if (iBackViewUid != TUid::Uid(0))
				{
				if (iBackViewUid != TUid::Uid(EDirectPrintMainViewId))
					{
					MakeNaviPaneVisibleL(ETrue);
					}
				ActivateLocalViewL(iBackViewUid);
				iBackViewUid = TUid::Uid(0);
				}
			else
				{
				ActivateLocalViewL(TUid::Uid(EDirectPrintMainViewId));
				}
			}
			break;
		case EDirectPrintCmdIdPrinterListViewDelete:
		default:
			break;
		}
	LOG("CDirectPrintAppAppUi::HandleCommandL END");
	}

void CDirectPrintAppAppUi::HandleResourceChangeL( TInt aType )
	{
	// base-class call also
	CAknAppUi::HandleResourceChangeL(aType);
	if (aType == KEikDynamicLayoutVariantSwitch)
		{
		}
	}

void CDirectPrintAppAppUi::TabChangedL(TInt aIndex)
	{
	TVwsViewId id;
	GetActiveViewId(id);
	if (id.iViewUid.iUid == EDirectPrintPropertyViewId)
		{
		CDirectPrintPropertyView* view = static_cast<CDirectPrintPropertyView*>(View(id.iViewUid));
		view->ChangeTabL(aIndex);
		}
	}

void CDirectPrintAppAppUi::MakeNaviPaneVisibleL(TBool aVisible)
	{
	CEikStatusPane* statusPane = StatusPane();
	CAknNavigationControlContainer* naviPane = (CAknNavigationControlContainer*) statusPane->ControlL(TUid::Uid(EEikStatusPaneUidNavi));

	if (aVisible)
		{
		naviPane->PushL(*iNaviDecorator);
		}
	else
		{
		naviPane->Pop(iNaviDecorator);
		}
	}

void CDirectPrintAppAppUi::MakeStatusPaneVisible(TBool aVisible)
	{
	CEikStatusPane* statusPane = StatusPane();
	statusPane->MakeVisible(aVisible);
	}

void CDirectPrintAppAppUi::InitializeViewsL()
	{
	LOG("CDirectPrintAppAppUi::InitializeViewsL begin");
	// Create view
	LOG("-->CDirectPrintMainView");
	CDirectPrintMainView* view1 = CDirectPrintMainView::NewL();
	CleanupStack::PushL(view1);
	AddViewL(view1);
	CleanupStack::Pop(view1);

	LOG("-->CDirectPrintListView");
	CDirectPrintListView* view2 = CDirectPrintListView::NewL();
	CleanupStack::PushL(view2);
	AddViewL(view2);
	CleanupStack::Pop(view2);

	LOG("-->CDirectPrintSearchResultView");
	CDirectPrintSearchResultView* view3 = CDirectPrintSearchResultView::NewL();
	AddViewL(view3);

	LOG("-->CDirectPrintPrinterSettingView");
	CDirectPrintPrinterSettingView* view4 = CDirectPrintPrinterSettingView::NewL();
	CleanupStack::PushL(view4);
	AddViewL(view4);
	CleanupStack::Pop(view4);

	LOG("-->CDirectPrintPreviewView");
	CDirectPrintPreviewView* view5 = CDirectPrintPreviewView::NewL();
	CleanupStack::PushL(view5);
	AddViewL(view5);
	CleanupStack::Pop(view5);

	LOG("-->CDirectPrintPropertyView");
	CDirectPrintPropertyView* view6 = CDirectPrintPropertyView::NewL();
	CleanupStack::PushL(view6);
	AddViewL(view6);
	CleanupStack::Pop(view6);

	// Check plugins. Create view, if extended view exists.

	SetDefaultViewL( *view1 );

	iBackViewUid = TUid::Uid(0);
	LOG("CDirectPrintAppAppUi::InitializeViewsL end");
	}


void CDirectPrintAppAppUi::LoadParamL()
	{
	// get the xml parameter from AIW provider
	// after that iXmlEngDoc is set in the function.
	GetParamFromAiwProviderL();
	// set the xml param to the engine side, and 
	// the adapted band plugin is loaded and the xml
	// parameter is sent to the plugin.
	iEngine->SetParamL(&iXmlEngDoc);
	}

void CDirectPrintAppAppUi::GetParamFromAiwProviderL()
	{
	LOG("CDirectPrintAppAppUi::GetParamFromAiwProviderL() BEGIN");

	RFileReadStream readStream;
	TFileName paramFile;
	_LIT( KParamFile, "c:\\system\\data\\T_AIW_TEMP.DAT");
	paramFile.Copy(KParamFile);

	User::LeaveIfError( readStream.Open(iCoeEnv->FsSession(), paramFile, EFileRead) );
	readStream.PushL();
	CAiwGenericParamList* paramList = CAiwGenericParamList::NewLC(readStream);

	TInt index( 0 );
	const TAiwGenericParam* param = paramList->FindFirst(index,
			EGenericParamPrintXMLFile,
			EVariantTypeDesC8);
	TPtrC8 ptrParam = param->Value().AsData();
	if ( ptrParam.Length() == 0)
		{
		User::Leave(KErrArgument);
		}

	// set file name
	iXmlParam = ptrParam.AllocL();

	// parse XML param
	// opening DOM implementation object 
	iXmlDomImpl.OpenL();
	// opening parser object
	User::LeaveIfError(iXmlParser.Open( iXmlDomImpl ));
	// ParseL return KXmlEngErrParsing
	iXmlEngDoc =iXmlParser.ParseL( *iXmlParam );   // parsing buffer

	CleanupStack::PopAndDestroy(paramList); //paramList
	readStream.Close();
	readStream.Pop();
	LOG("CDirectPrintAppAppUi::GetParamFromAiwProviderL() END");
	}

// ---------------------------------------------------------------------------
// CDirectPrintAppAppUi::InitializeEngineL()
// Initialize print engine.
// ---------------------------------------------------------------------------
//
void CDirectPrintAppAppUi::InitializeEngineL()
	{
	iEngine = CDirectPrintEngine::NewL(iDoc->Model());
	iEngine->SetObserver(this);
	}


// ---------------------------------------------------------------------------
// CDirectPrintAppAppUi::HandlePrintEventL()
// Handle the events from engine side
// ---------------------------------------------------------------------------
//
void CDirectPrintAppAppUi::HandlePrintEventL(
							TDirectPrintEngineEvents aEvent,
							TInt /*aError*/, 
							const TDesC& /*aParam*/)
	{

	switch (aEvent)
		{
		case EDirectPrintEngineEventStartCreatePrintData:
			break;
		case EDirectPrintEngineEventFinishCreatePrintData:
			{
			//TInt msgid = R_DIRECTPRINT_MSG_BEARER_OPEN_ERR;
			TRAPD(err, iBearerMgr->CreateJobL());
			if (err == KErrNone)
				{
				//msgid = R_DIRECTPRINT_MSG_SUBMIT_PRINT_ERR;
				TRAP(err, iBearerMgr->SubmitPrintJobL());
				}

			if (err != KErrNone)
				{
				// show bearer open error
				CAknErrorNote* errornote = new (ELeave) CAknErrorNote( ETrue );
				HBufC* format = StringLoader::LoadLC( R_DIRECTPRINT_MSG_BEARER_OPEN_ERR );
				HBufC* msg = HBufC::NewLC(format->Length()+5);
				TPtr ptrMsg(msg->Des());
				ptrMsg.AppendFormat(*format, err);
				errornote->ExecuteLD( *msg );
				CleanupStack::PopAndDestroy( msg );
				CleanupStack::PopAndDestroy( format );
				}
			}
			break;
		case EDirectPrintEngineEventCancelCreatePrintData:
			break;
		case EDirectPrintEngineEventErrorCreatePrintData:
			break;
		default:
			break;
		}
	}

// ---------------------------------------------------------------------------
// CDirectPrintAppAppUi::InitializeBearerMngL()
// Initialize bearer manager.
// ---------------------------------------------------------------------------
//
void CDirectPrintAppAppUi::InitializeBearerMngL()
	{
	iBearerMgr = CDirectPrintBearerMgr::NewL(this, iDoc->Model());
	}

// ---------------------------------------------------------------------------
// CDirectPrintAppAppUi::InitializePluginMgrL()
// Initialize plugin manager.
// ---------------------------------------------------------------------------
//
void CDirectPrintAppAppUi::InitializePluginMgrL()
	{
	iPluginMgr = CDirectPrintUiPluginMgr::NewL(iDoc->Model());
	}

// ---------------------------------------------------------------------------
// CDirectPrintAppAppUi::HandleBearerMngEventL()
// Handle the events from engine side
// ---------------------------------------------------------------------------
//
void CDirectPrintAppAppUi::HandleBearerMngEventL(
							TDirectPrintBearerMngEvents aEvent,
							TInt /*aError*/, 
							const TDesC& /*aParam*/)
	{

	switch (aEvent)
		{
		case EDirectPrintBearerMngErrorConnectBearer:
			break;
		case EDirectPrintBearerMngConnectedBearer:
			break;
		default:
			break;
		}
	}

void CDirectPrintAppAppUi::PrintJobProgressEvent( TInt aStatus,
										TInt aPercentCompletion,
										TInt aJobStateCode )
	{
	LOG3("Start CDirectPrintAppAppUi::PrintJobProgressEvent[%d],[%d],[%d]", 
												aStatus, aPercentCompletion, aJobStateCode);
	if ( aStatus == EActive )
		{
		LOG("aStatus == EActive");

		}
	else
		{
		LOG("aStatus == EDone");
		if ( aJobStateCode == ECancelling && aStatus == EDone )
			{
			LOG("ECancelling");
			}
		else if ( aJobStateCode == ECancellingNoMessage && aStatus == EDone )
			{
			LOG("ECancellingNoMessage");
			}
		else
			{
			LOG("Finish");
			this->iBearerMgr->CloseSession();
			}
		}
	LOG("End   CDirectPrintAppAppUi::PrintJobProgressEvent");
	}

void CDirectPrintAppAppUi::PrintJobErrorEvent( TInt aError,
										TInt aErrorStringCode )
	{
	LOG2("Start CDirectPrintAppAppUi::PrintJobErrorEvent[%d], [%d]", aError, aErrorStringCode);
	LOG("Finish when error happen");
	this->iBearerMgr->CloseSession();
	LOG("End   CDirectPrintAppAppUi::PrintJobErrorEvent");
	}

void CDirectPrintAppAppUi::PrinterStatusEvent( TInt aError,
										TInt aErrorStringCode )
	{
	LOG2("Start CDirectPrintAppAppUi::PrinterStatusEvent[%d], [%d]", aError, aErrorStringCode);
	LOG("End   CDirectPrintAppAppUi::PrinterStatusEvent");
	}

void CDirectPrintAppAppUi::PreviewImageEvent( TInt /*aFsBitmapHandle*/ )
	{
	LOG("Start CDirectPrintAppAppUi::PreviewImageEvent");
	LOG("End   CDirectPrintAppAppUi::PreviewImageEvent");
	}

void CDirectPrintAppAppUi::ShowMessageL( TInt aMsgLine1Code,
										TInt aMsgLine2Code )
	{
	LOG2("Start CDirectPrintAppAppUi::ShowMessageL[%d], [%d]", aMsgLine1Code, aMsgLine2Code);
	LOG("End   CDirectPrintAppAppUi::ShowMessageL");
	}

TBool CDirectPrintAppAppUi::AskYesNoQuestionL( TInt aMsgLine1Code,
										TInt aMsgLine2Code )
	{
	LOG2("Start CDirectPrintAppAppUi::AskYesNoQuestionL[%d], [%d]", aMsgLine1Code, aMsgLine2Code);
	LOG("End   CDirectPrintAppAppUi::AskYesNoQuestionL");
	return EFalse;
	}

const TDesC& CDirectPrintAppAppUi::AskForInputL( TInt aMsgLine1Code,
										TInt aMsgLine2Code )
	{
	LOG2("Start CDirectPrintAppAppUi::AskForInputL[%d], [%d]", aMsgLine1Code, aMsgLine2Code);
	LOG("End   CDirectPrintAppAppUi::AskForInputL");
	return KNullDesC;
	}

// End of File

