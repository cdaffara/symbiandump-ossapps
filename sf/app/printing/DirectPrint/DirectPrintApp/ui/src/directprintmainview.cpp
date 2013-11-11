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

#include "clog.h"
#include "directprintapp.hrh"
#include "directprintmodel.h"
#include "directprintmainview.h"

CDirectPrintMainView::CDirectPrintMainView()
	{
	iListBox = NULL;
	}

CDirectPrintMainView::~CDirectPrintMainView()
	{
	AppUi()->RemoveFromViewStack(*this, iListBox);
	delete iListBox;
	iItemArray->Reset();
	delete iItemArray;
	}

CDirectPrintMainView* CDirectPrintMainView::NewL()
	{
	CDirectPrintMainView* self = CDirectPrintMainView::NewLC();
	CleanupStack::Pop(self);
	return self;
	}

CDirectPrintMainView* CDirectPrintMainView::NewLC()
	{
	CDirectPrintMainView* self = new(ELeave) CDirectPrintMainView();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

void CDirectPrintMainView::ConstructL()
	{
	LOG("CDirectPrintMainView::ConstructL BEGIN");
	BaseConstructL(R_DIRECTPRINT_MAINVIEW);

	iViewId = TUid::Uid(EDirectPrintMainViewId);

	iItemArray = new(ELeave) CDesCArrayFlat(3);
	LOG("CDirectPrintMainView::ConstructL END");
	}

void CDirectPrintMainView::HandleCommandL(TInt aCommand)
	{
	switch (aCommand)
		{
		case EAknSoftkeyBack:
			{
			AppUi()->HandleCommandL(EDirectPrintCmdIdMainViewExit);
			}
			break;
		case EDirectPrintCmdIdMainViewPrinterList:
		case EDirectPrintCmdIdMainViewPrint:
		case EDirectPrintCmdIdMainViewPreview:
		case EDirectPrintCmdIdMainViewPropertySetting:
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

void CDirectPrintMainView::HandleListIndexL(TInt aIndex)
	{
	switch (aIndex)
		{
		case EItemPrinter:
			{
			AppUi()->HandleCommandL(EDirectPrintCmdIdMainViewPrinterList);
			}
			break;
		case EItemBearer:
			{
			CDirectPrintModel& model = GetModel();
			model.SetSelectPrinter(model.CurrentPrinter());
			AppUi()->HandleCommandL(EDirectPrintCmdIdMainViewPrinterSetting);
			}
			break;
		case EItemSetting:
			{
			AppUi()->HandleCommandL(EDirectPrintCmdIdMainViewPropertySetting);
			}
			break;
		default:
			break;
		}
	}

void CDirectPrintMainView::OfferToolbarEventL(TInt aCommand)
	{
	switch (aCommand)
		{
		case EDirectPrintCmdIdMainViewPrint:
		case EDirectPrintCmdIdMainViewPreview:
		case EDirectPrintCmdIdMainViewPropertySetting:
			{
			AppUi()->HandleCommandL(aCommand);
			}
			break;
		default:
			break;
		}
	}

void CDirectPrintMainView::DoActivateL(const TVwsViewId& /*aPrevViewId*/, TUid /*aCustomMessageId*/, const TDesC8& /*aCustomMessage*/)
	{
	LOG("CDirectPrintMainView::DoActivateL BEGIN");
	CAknToolbar* toolbar = AppUi()->CurrentFixedToolbar();
	toolbar->SetToolbarObserver(this);

	iItemArray->Reset();
	LoadListDataL();

	// Create list box
	if (!iListBox)
		{
		iListBox = CDirectPrintListBox::NewL(ClientRect(), iItemArray);
		iListBox->SetMopParent(this);
		iListBox->SetListObserver(this);
		}
	AppUi()->AddToStackL(*this, iListBox);
	iListBox->MakeVisible(ETrue);
	iListBox->RedrawListL();

	// set the title pane
	SetActiveTitleL(R_DIRECTPRINT_MAINVIEW_TITLE);
	LOG("CDirectPrintMainView::DoActivateL END");
	}

void CDirectPrintMainView::DoDeactivate()
	{
	LOG("CDirectPrintMainView::DoDeactivate BEGIN");
	iListBox->MakeVisible(EFalse);
	AppUi()->RemoveFromViewStack(*this, iListBox);
	LOG("CDirectPrintMainView::DoDeactivate END");
	}

void CDirectPrintMainView::LoadListDataL()
	{
	CDirectPrintModel& model = GetModel();

	if (model.PrinterCount() > 0)
		{
		TBuf<50> buf;
		TBuf<25> itemTitle;

		TDirectPrintPrinterData printerData;
		model.GetPrinterDataL(0, printerData);
		CEikonEnv::Static()->ReadResource(itemTitle, R_DIRECTPRINT_MAINVIEW_ITEM_PRINTERNAME);
		buf.Format(_L("\t%S\t%S"), &itemTitle, &printerData.iPrinterName);
		iItemArray->AppendL(buf);

		TDirectPrintBearerData bearerData;
		model.GetCurrentBearerDataL(bearerData);
		CEikonEnv::Static()->ReadResource(itemTitle, R_DIRECTPRINT_MAINVIEW_ITEM_BEARER);
		buf.Format(_L("\t%S\t%S"), &itemTitle, &bearerData.iBearerName);
		iItemArray->AppendL(buf);

		TBuf<25> property;
		MakePropertyStringL(property);
		CEikonEnv::Static()->ReadResource(itemTitle, R_DIRECTPRINT_MAINVIEW_ITEM_PROPERTYSETTINGS);
		buf.Format(_L("\t%S\t%S"), &itemTitle, &property);
		iItemArray->AppendL(buf);
		}
	else
		{
		TBuf<50> buf;
		TBuf<30> itemTitle;
		CEikonEnv::Static()->ReadResource(itemTitle, R_DIRECTPRINT_MAINVIEW_ITEM_PRINTERNAME);
		buf.Format(_L("\t%S\t"), &itemTitle);
		iItemArray->AppendL(buf);
		CEikonEnv::Static()->ReadResource(itemTitle, R_DIRECTPRINT_MAINVIEW_ITEM_BEARER);
		buf.Format(_L("\t%S\t"), &itemTitle);
		iItemArray->AppendL(buf);
		CEikonEnv::Static()->ReadResource(itemTitle, R_DIRECTPRINT_MAINVIEW_ITEM_PROPERTYSETTINGS);
		buf.Format(_L("\t%S\t"), &itemTitle);
		iItemArray->AppendL(buf);
		}
	}

void CDirectPrintMainView::MakePropertyStringL(TDes& aProperty)
	{
	CDirectPrintModel& model = GetModel();
	CPrinterProperty* property = model.GetCurrentPrinterPropertyL(TUid::Uid(0));

	if (property)
		{
		TInt paperSize(0), orientation(0), copy(0);
		TInt count = property->iPropertyItems.Count();
		for (TInt i=0; i<count; i++)
			{
			if (property->iPropertyItems[i].iItemType == EDirectPrintPageSettingTab)
				{
				switch (property->iPropertyItems[i].iItemId)
					{
					case EDirectPrintPaperPaperSize:
						{
						paperSize = property->iPropertyItems[i].iItemValue;
						}
						break;
					case EDirectPrintPaperOrientation:
						{
						orientation = property->iPropertyItems[i].iItemValue;
						}
						break;
					case EDirectPrintPageCopy:
						{
						copy = property->iPropertyItems[i].iItemValue;
						}
						break;
					default:
						break;
					}
				}
			}

		//aProperty.Format(_L("%S/%S/%d"), paperSize, orientation, copy);
		aProperty.Format(_L("%d/%d/%d"), paperSize, orientation, copy);
		}
	}

