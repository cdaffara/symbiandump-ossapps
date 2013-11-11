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

// System include files
#include <aknviewappui.h>
#include <akntoolbar.h>
#include <barsread.h>
#include <DirectPrintApp.rsg>
#include <AknSettingItemList.h>
#include <eiklbx.h>
#include <bautils.h>
#include <driveinfo.h>
#include <pathinfo.h>

// User include files
#include "clog.h"
#include "directprintapp.hrh"
#include "DirectPrintAppAppUi.h"
#include "DirectPrintEngine.h"
#include "DirectPrintModel.h"
#include "directprintsettingview.h"
#include "DirectPrintBearerMgr.h"

#include "directprintbasecapability.h"
#include "directprintlistcapability.h"
#include "directprinttextcapability.h"
#include "directprintprintsettings.h"
#include "imageprint.h"
#include "directprintcapabilitycodes.h"

CDirectPrintPrinterSettingView::CDirectPrintPrinterSettingView()
	{
	iListBox = NULL;
	}

CDirectPrintPrinterSettingView::~CDirectPrintPrinterSettingView()
	{
	AppUi()->RemoveFromViewStack(*this, iListBox);
	delete iListBox;
	}

CDirectPrintPrinterSettingView* CDirectPrintPrinterSettingView::NewL()
	{
	CDirectPrintPrinterSettingView* self = CDirectPrintPrinterSettingView::NewLC();
	CleanupStack::Pop(self);
	return self;
	}

CDirectPrintPrinterSettingView* CDirectPrintPrinterSettingView::NewLC()
	{
	CDirectPrintPrinterSettingView* self = new(ELeave) CDirectPrintPrinterSettingView();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

void CDirectPrintPrinterSettingView::ConstructL()
	{
	LOG("CDirectPrintPrinterSettingView::ConstructL BEGIN");
	BaseConstructL(R_DIRECTPRINT_PRINTERSETTING_VIEW);

	iViewId = TUid::Uid(EDirectPrintPrinterSettingViewId);
	LOG("CDirectPrintPrinterSettingView::ConstructL END");
	}

void CDirectPrintPrinterSettingView::HandleCommandL(TInt aCommand)
	{
	switch (aCommand)
		{
		case EAknSoftkeyBack:
			{
			iListBox->StoreSettingsL();
			StoreDriverInfoL();
			StoreTypeInfoL();
			StoreBearerInfoL();
			StoreExtendSettingsL();
			WriteDataL(); // write to model
			GetBearerMgr()->SetJobSettings(); // write to bearer
			GetModel().SetIsNewData(EFalse);

			AppUi()->HandleCommandL(EDirectPrintCmdIdPrinterSettingViewBack);
			}
			break;
		default:
			{
			AppUi()->HandleCommandL(aCommand);
			}
			break;
		}
	}

void CDirectPrintPrinterSettingView::HandleEditItemL(TInt aIndex, TBool aChanged)
	{
	if (aChanged)
		{
		TBuf<25> titleBearer;
		CEikonEnv::Static()->ReadResource(titleBearer, R_DIRECTPRINT_PRINTERSETTING_ITEM_BEARER);

		TPtrC name = (*(iListBox->SettingItemArray()))[aIndex]->SettingName();
		if (name.Compare(titleBearer) == 0)
			{
			ReloadListL();
			}
		}
	}

void CDirectPrintPrinterSettingView::DoActivateL(const TVwsViewId& /*aPrevViewId*/, TUid /*aCustomMessageId*/, const TDesC8& /*aCustomMessage*/)
	{
	LOG("CDirectPrintPrinterSettingView::DoActivateL BEGIN");

	if (GetModel().IsNewData())
		{
		NewDataL();
		}
	else
		{
		ReadDataL();
		}

	if (!iListBox)
		{
		// Create list box
		iListBox = CDirectPrintSettingListBox::NewL(ClientRect());
		iListBox->SetMopParent(this);
		iListBox->SetSettingListObserver(this);
		iListBox->ConstructFromResourceL(R_DIRECTPRINT_PRINTERSETTING_ITEM_LIST);
		iListBox->ActivateL();
		}
	else
		{
		iListBox->SettingItemArray()->ResetAndDestroy();
		}

	LoadListL();
	LoadDriverInfoL();
	LoadTypeInfoL();
	LoadBearerInfoL();
	LoadExtendListL();
	iListBox->LoadSettingsL();
	AppUi()->AddToStackL(*this, iListBox);
	iListBox->MakeVisible(ETrue);
	iListBox->DrawNow();

	// set the title pane
	SetActiveTitleL(R_DIRECTPRINT_PRINTERSETTING_TITLE);
	LOG("CDirectPrintPrinterSettingView::DoActivateL END");
	}

void CDirectPrintPrinterSettingView::DoDeactivate()
	{
	LOG("CDirectPrintPrinterSettingView::DoDeactivate BEGIN");
	iListBox->MakeVisible(EFalse);
	AppUi()->RemoveFromViewStack(*this, iListBox);
	LOG("CDirectPrintPrinterSettingView::DoDeactivate END");
	}

void CDirectPrintPrinterSettingView::LoadListL()
	{
	LOG("CDirectPrintPrinterSettingView::LoadListL BEGIN");

	TBuf<25> itemTitle;

	// Common setting item
	CAknTextSettingItem* item1 = new (ELeave) CAknTextSettingItem(ESettingViewItemPrinterName, iPrinterName);
	CleanupStack::PushL(item1);
	item1->SetEmptyItemTextL( _L("None") );
	CEikonEnv::Static()->ReadResource(itemTitle, R_DIRECTPRINT_PRINTERSETTING_ITEM_PRINTERNAME);
	item1->ConstructL(EFalse, ESettingViewItemPrinterName, itemTitle, NULL, R_DIRECTPRINT_PRINTERSETTING_TEXT_SETTING_PAGE, -1);
	iListBox->SettingItemArray()->AppendL(item1);
	CleanupStack::Pop(item1);

	LOG("CDirectPrintPrinterSettingView::LoadListL 1");

	iDriver = 0;
	CAknEnumeratedTextPopupSettingItem* item2 = new (ELeave) CAknEnumeratedTextPopupSettingItem(ESettingViewItemDriver, iDriver);
	CleanupStack::PushL(item2);
	CEikonEnv::Static()->ReadResource(itemTitle, R_DIRECTPRINT_PRINTERSETTING_ITEM_DRIVER);
	item2->ConstructL(EFalse, ESettingViewItemDriver, itemTitle, NULL, R_DIRECTPRINT_PRINTERSETTING_POPUP_SETTING_PAGE, -1, 0, R_DIRECTPRINT_PRINTERSETTING_POPUP_SETTING_TEXTS);

	LOG("CDirectPrintPrinterSettingView::LoadListL 2");
	// set printer driver
		{
		const TInt KSettingItemArrayGranularity = 5;
		// Construct the array
		CArrayPtr<CAknEnumeratedText>* enumeratedTextArray; 
		enumeratedTextArray = new( ELeave ) CArrayPtrFlat<CAknEnumeratedText>( KSettingItemArrayGranularity );
		CleanupStack::PushL( enumeratedTextArray );

		// Construct a dummy array for popup text, needed to avoid a panic
		CArrayPtr<HBufC>* popupTextArray; 
		popupTextArray = new( ELeave ) CArrayPtrFlat<HBufC>( KSettingItemArrayGranularity );
		CleanupStack::PushL( popupTextArray );

		CDirectPrintAppAppUi* appui = static_cast<CDirectPrintAppAppUi*>( AppUi() );
		CDirectPrintEngine* engine = appui->Engine();

		// Read in the texts to be used in the setting item list
		TInt index;
		// Loop through all the texts 

		LOG1("ModelCount=[%d]", engine->ModelCount());

		for (index=0; index < engine->ModelCount(); index++)
			{
			HBufC* thisText = HBufC::NewLC(KMaxPrinterModelNameLength); 
			TPtr thisTextDes = thisText->Des();
			engine->ModelName(index, thisTextDes);
			CAknEnumeratedText* enumeratedText = new( ELeave ) CAknEnumeratedText( index, thisText );
			CleanupStack::Pop( thisText ); 
			CleanupStack::PushL( enumeratedText );
			enumeratedTextArray->AppendL( enumeratedText );
			CleanupStack::Pop( enumeratedText ); 
			}
		if (engine->ModelCount() > 0)
			{
			item2->SetEnumeratedTextArrays( enumeratedTextArray, popupTextArray );
			}
		CleanupStack::Pop( popupTextArray );
		CleanupStack::Pop( enumeratedTextArray );
		}
	LOG("CDirectPrintPrinterSettingView::LoadListL 2-2");

	iListBox->SettingItemArray()->AppendL(item2);
	CleanupStack::Pop(item2);

	LOG("CDirectPrintPrinterSettingView::LoadListL 3");
	iType = 0;
	CAknEnumeratedTextPopupSettingItem* item3 = new (ELeave) CAknEnumeratedTextPopupSettingItem(ESettingViewItemType, iType);
	CleanupStack::PushL(item3);
	CEikonEnv::Static()->ReadResource(itemTitle, R_DIRECTPRINT_PRINTERSETTING_ITEM_TYPE);
	item3->ConstructL(EFalse, ESettingViewItemType, itemTitle, NULL, R_DIRECTPRINT_PRINTERSETTING_POPUP_SETTING_PAGE, -1, 0, R_DIRECTPRINT_PRINTERSETTING_TYPE_POPUP_SETTING_TEXTS);
	iListBox->SettingItemArray()->AppendL(item3);
	CleanupStack::Pop(item3);

	iBearer = 0;
	LOG("CDirectPrintPrinterSettingView::LoadListL 4");
	CAknEnumeratedTextPopupSettingItem* item4 = new (ELeave) CAknEnumeratedTextPopupSettingItem(ESettingViewItemBearer, iBearer);
	CleanupStack::PushL(item4);
	CEikonEnv::Static()->ReadResource(itemTitle, R_DIRECTPRINT_PRINTERSETTING_ITEM_BEARER);
	item4->ConstructL(EFalse, ESettingViewItemBearer, itemTitle, NULL, R_DIRECTPRINT_PRINTERSETTING_POPUP_SETTING_PAGE, -1, 0, R_DIRECTPRINT_PRINTERSETTING_BEARER_POPUP_SETTING_TEXTS);

	SetBearerListL(item4);

	iListBox->SettingItemArray()->AppendL(item4);
	CleanupStack::Pop(item4);

	LOG("CDirectPrintPrinterSettingView::LoadListL xxx");

	iListBox->ListBox()->SetCurrentItemIndex(0);
	LOG("CDirectPrintPrinterSettingView::LoadListL xx");
	iListBox->HandleChangeInItemArrayOrVisibilityL(); 
	LOG("CDirectPrintPrinterSettingView::LoadListL END");
	}

void CDirectPrintPrinterSettingView::LoadExtendListL()
	{
	// Initialize capabilities for bearer
	GetBearerMgr()->InitCapabilitiesL(iBearer);

	// Extend setting item
	CDirectPrintPrintSettings* printSettings = GetBearerMgr()->PrinterSettingsL();
	TInt count = printSettings->iCapabilities.Count();
	for (TInt i=0; i<count; i++)
		{
		CAknSettingItem* item = printSettings->iCapabilities[i]->SettingItemLC(ESettingViewItemExtent+i);
		iListBox->SettingItemArray()->AppendL(item);
		CleanupStack::Pop(item);
		}

	LoadExtendSettingsL();

	iListBox->HandleChangeInItemArrayOrVisibilityL(); 
	}

void CDirectPrintPrinterSettingView::ReloadListL()
	{
	iListBox->StoreSettingsL();
	StoreDriverInfoL();
	StoreTypeInfoL();
	StoreBearerInfoL();
	StoreExtendSettingsL();

	iListBox->SettingItemArray()->ResetAndDestroy();
	LoadListL();
	LoadDriverInfoL();
	LoadTypeInfoL();
	LoadBearerInfoL();

	LoadExtendListL();

	iListBox->LoadSettingsL();
	iListBox->DrawNow();
	}

void CDirectPrintPrinterSettingView::SetBearerListL(CAknEnumeratedTextPopupSettingItem* item)
	{
	TInt err = GetBearerMgr()->GetProtocolNames(iBearerList);
	if (!err)
		{
		CArrayPtr<CAknEnumeratedText>* textArray = item->EnumeratedTextArray();
		textArray->ResetAndDestroy();

		TInt count = iBearerList.Count();
		for (TInt i=0; i<count; i++)
			{
			CAknEnumeratedText* enumText = new (ELeave) CAknEnumeratedText(i, iBearerList[i].iName.AllocLC());
			CleanupStack::Pop();
			CleanupStack::PushL(enumText);
			textArray->AppendL(enumText);
			CleanupStack::Pop();
			}
		}
	}

void CDirectPrintPrinterSettingView::ReadDataL()
	{
	LOG("CDirectPrintPrinterSettingView::ReadDataL BEGIN");
	CDirectPrintModel& model = GetModel();
	TDirectPrintPrinterData printerData;
	TInt index = model.SelectPrinter();
	if (index >= 0)
		{
		model.GetPrinterDataL(index, printerData);
		}
	else
		{
		model.SetCurrentPrinter(0);
		}

	TDirectPrintBearerData bearerData;
	index = model.CurrentBearer();
	if (index >= 0)
		{
		//model.GetBearerDataL(index, bearerData);
		model.GetSelectedBearerDataL(bearerData);
		}
	else
		{
		model.SetCurrentBearer(0);
		}

	iPrinterId = printerData.iPrinterId;
	iPrinterName.Copy(printerData.iPrinterName);
	iDriverName.Copy(printerData.iDriverName);
	iTypeName.Copy(printerData.iTypeName);
	//iDriver = 1;
	//iBearer = 1;
	//iAccessPoint = 0;
	iAccessPointName.Copy(bearerData.iAccessPoint);
	iBearerName.Copy(bearerData.iBearerName);
	iPort = bearerData.iPort;
	if (bearerData.iHostName.Length() > 0)
		{
		iHost.Copy(bearerData.iHostName);
		}
	iUserName.Copy(bearerData.iUserName);
	iQue.Copy(bearerData.iQueName);

	LOG("CDirectPrintPrinterSettingView::ReadDataL END");
	}

void CDirectPrintPrinterSettingView::WriteDataL()
	{
	LOG("CDirectPrintPrinterSettingView::WriteDataL BEGIN");
	TDirectPrintPrinterData printerData;
	printerData.iPrinterId = iPrinterId;
	printerData.iPrinterName.Copy(iPrinterName);
//	printerData.iPrinterDriver = iDriverUid;
	printerData.iDriverName.Copy(iDriverName);
//	printerData.iPrinterType = iType;
	printerData.iTypeName.Copy(iTypeName);

	TDirectPrintBearerData bearerData;
	bearerData.iPrinterId = iPrinterId;
	bearerData.iBearerName.Copy(iBearerName);
	bearerData.iAccessPoint.Copy(iAccessPointName);
	bearerData.iHostName.Copy(iHost);
	bearerData.iPort = iPort;
	bearerData.iUserName.Copy(iUserName);
	bearerData.iQueName.Copy(iQue);

	CDirectPrintModel& model = GetModel();
	if (model.IsNewData())
		{
		if (model.PrinterCount() == 0)
			{
			model.SetCurrentPrinter(0);
			model.SetCurrentBearer(0);
			}
		model.SetPrinterDataL(model.PrinterCount(), printerData);
		model.SetBearerDataL(model.BearerCount(), bearerData);
		}
	else
		{
		model.SetPrinterDataL(model.SelectPrinter(), printerData);
		//model.SetBearerDataL(model.CurrentBearer(), bearerData);
		model.SetBearerDataL(bearerData);
		}
	LOG("CDirectPrintPrinterSettingView::WriteDataL END");
	}

void CDirectPrintPrinterSettingView::NewDataL()
	{
	CDirectPrintModel& model = GetModel();
	TInt count = model.PrinterCount();

	TBuf<50> buf;
	TBool IsNewName = EFalse;
	for (TInt i=1; !IsNewName; i++)
		{
		buf.Format(_L("Printer%d"), count+i);
		IsNewName = ETrue;

		for (TInt j=0; j<count; j++)
			{
			TDirectPrintPrinterData printerData;
			model.GetPrinterDataL(j, printerData);
			if (printerData.iPrinterName.Compare(buf) == 0)
				{
				IsNewName = EFalse;
				break;
				}
			}
		}

	iPrinterId = model.NewPrinterId();
	iPrinterName.Copy(buf);
	iDriver = 0;
	iBearer = 0;
	iAccessPoint = 0;
//	iAccessPointName.Copy(_L("WLAN_ap1"));
//	iHostIp.SetAddress(3232235530);
	iPort = 80;
	iUserName.Copy(_L(""));
	iQue.Copy(_L(""));
	}

void CDirectPrintPrinterSettingView::LoadDriverInfoL()
	{
	TBuf<25> itemTitle;
	CEikonEnv::Static()->ReadResource(itemTitle, R_DIRECTPRINT_PRINTERSETTING_ITEM_DRIVER);
	CAknEnumeratedTextPopupSettingItem* item = static_cast<CAknEnumeratedTextPopupSettingItem*>(GetSettingItemL(itemTitle));
	CArrayPtr<CAknEnumeratedText>* textArray = item->EnumeratedTextArray();
	TInt count = textArray->Count();

	for (TInt i=0; i<count; i++)
		{
		if (iDriverName.Compare((*textArray)[i]->Value()->Des()) == 0)
			{
			iDriver = i;
			}
		}
	}

void CDirectPrintPrinterSettingView::LoadTypeInfoL()
	{
	TBuf<25> itemTitle;
	CEikonEnv::Static()->ReadResource(itemTitle, R_DIRECTPRINT_PRINTERSETTING_ITEM_TYPE);
	CAknEnumeratedTextPopupSettingItem* item = static_cast<CAknEnumeratedTextPopupSettingItem*>(GetSettingItemL(itemTitle));
	CArrayPtr<CAknEnumeratedText>* textArray = item->EnumeratedTextArray();
	TInt count = textArray->Count();

	for (TInt i=0; i<count; i++)
		{
		if (iTypeName.Compare((*textArray)[i]->Value()->Des()) == 0)
			{
			iType = i;
			}
		}
	}

void CDirectPrintPrinterSettingView::LoadBearerInfoL()
	{
	TBuf<25> itemTitle;
	CEikonEnv::Static()->ReadResource(itemTitle, R_DIRECTPRINT_PRINTERSETTING_ITEM_BEARER);
	CAknEnumeratedTextPopupSettingItem* item = static_cast<CAknEnumeratedTextPopupSettingItem*>(GetSettingItemL(itemTitle));
	CArrayPtr<CAknEnumeratedText>* textArray = item->EnumeratedTextArray();
	TInt count = textArray->Count();

	for (TInt i=0; i<count; i++)
		{
		if (iBearerName.Compare((*textArray)[i]->Value()->Des()) == 0)
			{
			iBearer = i;
			}
		}
	}

void CDirectPrintPrinterSettingView::LoadItemInfoL(TInt aResouceId, const TDesC& aInfo, TInt& aIndex)
	{
	TBuf<25> itemTitle;
	CEikonEnv::Static()->ReadResource(itemTitle, aResouceId);
	CAknEnumeratedTextPopupSettingItem* item = static_cast<CAknEnumeratedTextPopupSettingItem*>(GetSettingItemL(itemTitle));
	CArrayPtr<CAknEnumeratedText>* textArray = item->EnumeratedTextArray();
	TInt count = textArray->Count();

	for (TInt i=0; i<count; i++)
		{
		if (aInfo.Compare((*textArray)[i]->Value()->Des()) == 0)
			{
			aIndex = i;
			}
		}
	}

void CDirectPrintPrinterSettingView::LoadExtendSettingsL()
	{
	CDirectPrintPrintSettings* printSettings = GetBearerMgr()->PrinterSettingsL();
	TInt count = printSettings->iCapabilities.Count();
	for (TInt i=0; i<count; i++)
		{
		switch (printSettings->iCapabilities[i]->iUid)
			{
			case EDirectPrintCapabAP:
				{
				CAknEnumeratedTextPopupSettingItem* item = static_cast<CAknEnumeratedTextPopupSettingItem*>(GetSettingItemL(printSettings->iCapabilities[i]->iTitle));
				CArrayPtr<CAknEnumeratedText>* textArray = item->EnumeratedTextArray();
				TInt count = textArray->Count();

				for (TInt j=0; j<count; j++)
					{
					if (iAccessPointName.Compare((*textArray)[j]->Value()->Des()) == 0)
						{
						CDirectPrintListCapability* capability = static_cast<CDirectPrintListCapability*>(printSettings->iCapabilities[i]);
						capability->iEnumIndex = j;
						capability->iValue = capability->iEnumIDs[j];
						break;
						}
					}
				}
				break;
			case EDirectPrintCapabHost:
				{
				CDirectPrintTextCapability* capability = static_cast<CDirectPrintTextCapability*>(printSettings->iCapabilities[i]);
				capability->iText.Copy(iHost);
				}
				break;
			case EDirectPrintCapabPort:
				{
				printSettings->iCapabilities[i]->iValue = iPort;
				}
				break;
			case EDirectPrintCapabUser:
				{
				CDirectPrintTextCapability* capability = static_cast<CDirectPrintTextCapability*>(printSettings->iCapabilities[i]);
				capability->iText.Copy(iUserName);
				}
				break;
			case EDirectPrintCapabQue:
				{
				CDirectPrintTextCapability* capability = static_cast<CDirectPrintTextCapability*>(printSettings->iCapabilities[i]);
				capability->iText.Copy(iQue);
				}
				break;
			default:
				break;
			}
		}
	}

void CDirectPrintPrinterSettingView::StoreDriverInfoL()
	{
	TBuf<25> itemTitle;
	CEikonEnv::Static()->ReadResource(itemTitle, R_DIRECTPRINT_PRINTERSETTING_ITEM_DRIVER);
	iDriverName.Copy(GetSettingItemL(itemTitle)->SettingTextL());
	}

void CDirectPrintPrinterSettingView::StoreTypeInfoL()
	{
	TBuf<25> itemTitle;
	CEikonEnv::Static()->ReadResource(itemTitle, R_DIRECTPRINT_PRINTERSETTING_ITEM_TYPE);
	iTypeName.Copy(GetSettingItemL(itemTitle)->SettingTextL());
	}

void CDirectPrintPrinterSettingView::StoreBearerInfoL()
	{
	TBuf<25> itemTitle;
	CEikonEnv::Static()->ReadResource(itemTitle, R_DIRECTPRINT_PRINTERSETTING_ITEM_BEARER);
	iBearerName.Copy(GetSettingItemL(itemTitle)->SettingTextL());
	}

void CDirectPrintPrinterSettingView::StoreItemInfoL(TInt aResouceId, TDes& aInfo)
	{
	TBuf<25> itemTitle;
	CEikonEnv::Static()->ReadResource(itemTitle, aResouceId);
	aInfo.Copy(GetSettingItemL(itemTitle)->SettingTextL());
	}

void CDirectPrintPrinterSettingView::StoreExtendSettingsL()
	{
	CDirectPrintPrintSettings* printSettings = GetBearerMgr()->PrinterSettingsL();
	TInt count = printSettings->iCapabilities.Count();
	for (TInt i=0; i<count; i++)
		{
		switch (printSettings->iCapabilities[i]->iUid)
			{
			case EDirectPrintCapabAP:
				{
				CDirectPrintListCapability* capability = static_cast<CDirectPrintListCapability*>(printSettings->iCapabilities[i]);
				iAccessPointName.Copy(GetSettingItemL(capability->iTitle)->SettingTextL());
				capability->iValue = capability->iEnumIDs[capability->iEnumIndex];
				}
				break;
			case EDirectPrintCapabHost:
				{
				iHost.Copy(GetSettingItemL(printSettings->iCapabilities[i]->iTitle)->SettingTextL());
				}
				break;
			case EDirectPrintCapabPort:
				{
				iPort = printSettings->iCapabilities[i]->iValue;
				}
				break;
			case EDirectPrintCapabUser:
				{
				iUserName.Copy(GetSettingItemL(printSettings->iCapabilities[i]->iTitle)->SettingTextL());
				}
				break;
			case EDirectPrintCapabQue:
				{
				iQue.Copy(GetSettingItemL(printSettings->iCapabilities[i]->iTitle)->SettingTextL());
				}
				break;
			default:
				break;
			}
		}
	}

CAknSettingItem* CDirectPrintPrinterSettingView::GetSettingItemL(const TDesC& aItemTitle)
	{
	CAknSettingItem* item = NULL;
	CAknSettingItemArray* array = iListBox->SettingItemArray();
	TInt count = array->Count();

	for (TInt i=0; i<count; i++)
		{
		item = (*array)[i];
		TPtrC name = item->SettingName();
		if (name.Compare(aItemTitle) == 0)
			{
			break;
			}
		item = NULL;
		}

	return item;
	}

CDirectPrintBearerMgr* CDirectPrintPrinterSettingView::GetBearerMgr()
	{
	return (static_cast<CDirectPrintAppAppUi*>(AppUi()))->BearerMgr();
	}

