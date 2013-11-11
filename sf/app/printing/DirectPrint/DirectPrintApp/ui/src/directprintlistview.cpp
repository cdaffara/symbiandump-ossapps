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

// System include files
#include <aknviewappui.h>
#include <akntoolbar.h>
#include <barsread.h>
#include <DirectPrintApp.rsg>

// User include files go here:
#include "clog.h"
#include "directprintapp.hrh"
#include "directprintmodel.h"
#include "directprintlistview.h"

CDirectPrintListView::CDirectPrintListView()
	{
	iListBox = NULL;
	}

CDirectPrintListView::~CDirectPrintListView()
	{
	AppUi()->RemoveFromViewStack(*this, iListBox);
	delete iListBox;
	delete iItemArray;
	}

CDirectPrintListView* CDirectPrintListView::NewL()
	{
	CDirectPrintListView* self = CDirectPrintListView::NewLC();
	CleanupStack::Pop(self);
	return self;
	}

CDirectPrintListView* CDirectPrintListView::NewLC()
	{
	CDirectPrintListView* self = new(ELeave) CDirectPrintListView();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

void CDirectPrintListView::ConstructL()
	{
	LOG("CDirectPrintListView::ConstructL BEGIN");
	BaseConstructL(R_DIRECTPRINT_PRINTERLIST_VIEW);

	LOG("CDirectPrintListView::ConstructL 1");
	iViewId = TUid::Uid(EDirectPrintPrinterListViewId);

	iItemArray = new(ELeave) CDesCArrayFlat(3);
	LOG("CDirectPrintListView::ConstructL END");
	}

void CDirectPrintListView::HandleCommandL(TInt aCommand)
	{
	switch (aCommand)
		{
		case EAknSoftkeyDone:
			{
			TInt index = iListBox->CurrentIndex();
			if (index >= 0)
				{
				GetModel().MoveTopPrinterDataL(index);
				}
			AppUi()->HandleCommandL(EDirectPrintCmdIdPrinterListViewDone);
			}
			break;
		case EDirectPrintCmdIdPrinterListViewDelete:
			{
			TInt index = iListBox->DeleteCurrentItemL();
			if (index >= 0)
				{
				GetModel().RemovePrinterDataL(index);
				iListBox->SetFocusL(0);
				}
			AppUi()->HandleCommandL(aCommand);
			}
			break;
		case EDirectPrintCmdIdPrinterListViewNew:
			{
			PrepareNewDataL();
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

void CDirectPrintListView::HandleListIndexL(TInt aIndex)
	{
	GetModel().SetSelectPrinter(aIndex);
	AppUi()->HandleCommandL(EDirectPrintCmdIdPrinterListViewEdit);
	}

void CDirectPrintListView::OfferToolbarEventL(TInt aCommand)
	{
	switch (aCommand)
		{
		case EDirectPrintCmdIdPrinterListViewEdit:
			{
			TInt index = iListBox->CurrentIndex();
			GetModel().SetSelectPrinter(index);
			AppUi()->HandleCommandL(aCommand);
			}
			break;
		case EDirectPrintCmdIdPrinterListViewSearch:
			{
			AppUi()->HandleCommandL(aCommand);
			}
			break;
		case EDirectPrintCmdIdPrinterListViewNew:
			{
			PrepareNewDataL();
			AppUi()->HandleCommandL(aCommand);
			}
			break;
		default:
			break;
		}
	}

void CDirectPrintListView::DoActivateL(const TVwsViewId& /*aPrevViewId*/, TUid /*aCustomMessageId*/, const TDesC8& /*aCustomMessage*/)
	{
	CAknToolbar* toolbar = AppUi()->CurrentFixedToolbar();
	toolbar->SetToolbarObserver(this);

	iItemArray->Reset();

	// Create list box
	if (!iListBox)
		{
		iListBox = CDirectPrintListBox::NewL(ClientRect(), iItemArray);
		iListBox->SetMopParent(this);
		iListBox->SetListObserver(this);
		}
	AppUi()->AddToStackL(*this, iListBox);
	iListBox->MakeVisible(ETrue);

	LoadListDataL();

	iListBox->RedrawListL();

	// set the title pane
	SetActiveTitleL(R_DIRECTPRINT_PRINTERLIST_TITLE);
	}

void CDirectPrintListView::DoDeactivate()
	{
	iListBox->MakeVisible(EFalse);
	AppUi()->RemoveFromViewStack(*this, iListBox);
	}

void CDirectPrintListView::LoadListDataL()
	{
	CDirectPrintModel& model = GetModel();
	TInt count = model.PrinterCount();
	for (TInt i=0; i<count; i++)
		{
		TDirectPrintPrinterData printerData;
		model.GetPrinterDataL(i, printerData);

		TBuf<50> buf;
		buf.Format(_L("\t%S\t%S"), &printerData.iPrinterName, &printerData.iDriverName);
		iItemArray->AppendL(buf);
		}
	}

void CDirectPrintListView::PrepareNewDataL()
	{
	CDirectPrintModel& model = GetModel();
	model.SetIsNewData(ETrue);
	model.SetSelectPrinter(0);
	if (model.PrinterCount() == 0)
		{
		model.SetCurrentPrinter(0);
		}
	}

