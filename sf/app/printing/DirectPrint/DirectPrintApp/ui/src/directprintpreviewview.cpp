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

#include <aknviewappui.h>
#include <akntoolbar.h>
#include <barsread.h>
#include <DirectPrintApp.rsg>
#include <eikcore.rsg>
#include <prnprev.h> 

#include "clog.h"
#include "directprintapp.hrh"
#include "DirectPrintAppAppUi.h"
#include "DirectPrintEngine.h"
#include "directprintpreviewview.h"
#include "directprintpreviewcontrol.h"

CDirectPrintPreviewView::CDirectPrintPreviewView()
	{
	}

CDirectPrintPreviewView::~CDirectPrintPreviewView()
	{
	delete iPrevCtrl;
	}

CDirectPrintPreviewView* CDirectPrintPreviewView::NewL()
	{
	CDirectPrintPreviewView* self = CDirectPrintPreviewView::NewLC();
	CleanupStack::Pop(self);
	return self;
	}

CDirectPrintPreviewView* CDirectPrintPreviewView::NewLC()
	{
	CDirectPrintPreviewView* self = new(ELeave) CDirectPrintPreviewView();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

void CDirectPrintPreviewView::ConstructL()
	{
	LOG("CDirectPrintPreviewView::ConstructL BEGIN");
	BaseConstructL(R_DIRECTPRINT_PREVIEW_VIEW);
	LOG("CDirectPrintPreviewView::ConstructL END");
	}

void CDirectPrintPreviewView::HandleCommandL(TInt aCommand)
	{
	switch (aCommand)
		{
		case EAknSoftkeyBack:
			{
			AppUi()->HandleCommandL(EDirectPrintCmdIdPreviewViewBack);
			}
			break;
		case EDirectPrintCmdIdPreviewViewPrint:
		case EDirectPrintCmdIdPreviewViewExpand:
		case EDirectPrintCmdIdPreviewViewReduce:
		case EDirectPrintCmdIdPreviewViewBack:
			{
			AppUi()->HandleCommandL(aCommand);
			}
			break;
		default:
			{
			AppUi()->HandleCommandL(aCommand);
			}
			break;
		}
	}

TUid CDirectPrintPreviewView::Id() const
	{
	return TUid::Uid(EDirectPrintPreviewViewId);
	}

void CDirectPrintPreviewView::OfferToolbarEventL(TInt aCommand)
	{
	switch (aCommand)
		{
		case EDirectPrintCmdIdPreviewViewPrint:
			{
			AppUi()->HandleCommandL(aCommand);
			}
			break;
		case EDirectPrintCmdIdPreviewViewExpand:
			{
			iPrevCtrl->IncrementZoomLevel();
			ChangeVisibleState();
			}
			break;
		case EDirectPrintCmdIdPreviewViewReduce:
			{
			iPrevCtrl->DecrementZoomLevel();
			ChangeVisibleState();
			}
			break;
		default:
			break;
		}
	}

void CDirectPrintPreviewView::DoActivateL(const TVwsViewId& /*aPrevViewId*/, TUid /*aCustomMessageId*/, const TDesC8& /*aCustomMessage*/)
	{
	LOG("CDirectPrintPreviewView::DoActivateL BEGIN");
	CAknToolbar* toolbar = AppUi()->CurrentFixedToolbar();
	toolbar->SetToolbarObserver(this);
	// set the title pane
	SetActiveTitleL(R_DIRECTPRINT_PREVIEW_TITLE);

	if (!iPrevCtrl)
		{
		CDirectPrintAppAppUi* appui = static_cast<CDirectPrintAppAppUi*>( AppUi() );
		CDirectPrintEngine* engine = appui->Engine();

		const TInt KPanelGutter=5;
		const TInt KShadowWidth=2;
		const TInt KLabelFontHeight=12;

		TFontSpec normalFontSpec = CEikonEnv::Static()->NormalFont()->FontSpecInTwips();
		TFontSpec fontSpec(normalFontSpec);
		fontSpec.iHeight = KLabelFontHeight;
		iPrintPreviewFormat.iLabelFontSpecInPixels=fontSpec;
		iPrintPreviewFormat.iMinPanelGutterInPixels=TSize(KPanelGutter,KPanelGutter);
		iPrintPreviewFormat.iPanelShadowWidthInPixels=KShadowWidth;
		iPrintPreviewFormat.iPanelShadowColor=KRgbDarkGray;
		iPrintPreviewFormat.iPanelMarginColor=KRgbGray;
		iPrintPreviewFormat.iLabelOffsetInPixels=5;

		iPrevCtrl = CDirectPrintPreviewControl::NewL(NULL);
		iPrevCtrl->SetMopParent(this);

		//iNumPagesInDoc = engine->MaxPageNum();
		iNumPagesInDoc = 3;
		iFirstPageToDisplay = 1;
		iNumPagesToView = 1;
		iNumBands = 2;
		iMarginState = CPrintPreviewImage::EShowMargins;

		engine->StartPrintPreviewL();

		iPrevCtrl->InitializeFormat(*(engine->PrintSetup()),
									*(engine->PageRegionPrinter()),
									iPrintPreviewFormat,
									iNumPagesInDoc,
									iFirstPageToDisplay,
									iNumPagesToView,
									iMarginState);
		iPrevCtrl->ActivateL();
		iPrevCtrl->SetNumBands(2);
		}
	iPrevCtrl->SetRect(ClientRect());
	iPrevCtrl->SetDrawFlag(ETrue);
	iPrevCtrl->MakeVisible(ETrue);
	iPrevCtrl->DrawDeferred();
	AppUi()->AddToStackL(*this, iPrevCtrl);
	ChangeVisibleState();
	LOG("CDirectPrintPreviewView::DoActivateL END");
	}

void CDirectPrintPreviewView::DoDeactivate()
	{
	LOG("CDirectPrintPreviewView::DoDeactivate BEGIN");
	AppUi()->RemoveFromViewStack(*this, iPrevCtrl);
	if (iPrevCtrl)
		{
		CDirectPrintAppAppUi* appui = static_cast<CDirectPrintAppAppUi*>( AppUi() );
		CDirectPrintEngine* engine = appui->Engine();
		engine->EndPrintPreview();
		delete iPrevCtrl;
		iPrevCtrl = NULL;
		}
	LOG("CDirectPrintPreviewView::DoDeactivate END");
	}


void CDirectPrintPreviewView::NotifyBandPrinted(TInt /*aPercentageOfPagePrinted*/, 
												TInt /*aCurrentPageNum*/, 
												TInt /*aCurrentCopyNum*/)
    {
	LOG("CDirectPrintPreviewView::NotifyBandPrinted");
	} 


void CDirectPrintPreviewView::NotifyPrintStarted(TPrintParameters /*aPrintParams*/)
	{
	LOG("CDirectPrintPreviewView::NotifyPrintStarted");
	}

void CDirectPrintPreviewView::NotifyPrintEnded(TInt anErrorCode)
	// ignore any errors before fully initialized
    {
	LOG1("CDirectPrintPreviewView::NotifyPrintEnded [%d]", anErrorCode);
	if (anErrorCode!=KErrNone && anErrorCode!=KErrCancel)
		{
		TBuf<80> previewError;
		if (anErrorCode==KErrNoMemory)
			iCoeEnv->ReadResource(previewError,R_EIK_TBUF_NOT_ENOUGH_MEMORY);
		else
			iCoeEnv->ReadResource(previewError,R_DIRECTPRINT_MSG_PREVIEW_ERR);
		iEikonEnv->AlertWin(previewError);
		// ToDo: Go back to Main view
		//delete this;
		}
	}

void CDirectPrintPreviewView::ChangeVisibleState()
	{
	CAknToolbar* toolbar = AppUi()->CurrentFixedToolbar();

	TBool isDimmedExpand = EFalse;
	TBool isDimmedReduce = EFalse;
	switch (iPrevCtrl->GetZoomLebel())
		{
		case CDirectPrintPreviewControl::EPreviewZoomLevel1:
			isDimmedReduce = ETrue;
			break;
		case CDirectPrintPreviewControl::EPreviewZoomLevel5:
			isDimmedExpand = ETrue;
			break;
		default:
			break;
		}
	toolbar->SetItemDimmed(EDirectPrintCmdIdPreviewViewExpand, isDimmedExpand, ETrue);
	toolbar->SetItemDimmed(EDirectPrintCmdIdPreviewViewReduce, isDimmedReduce, ETrue);
	}

// End of file
