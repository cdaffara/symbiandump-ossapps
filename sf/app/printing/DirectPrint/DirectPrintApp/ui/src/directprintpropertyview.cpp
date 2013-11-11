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
#include "directprintpropertyview.h"
#include "DirectPrintUiPluginMgr.h"
#include "DirectPrintUiPlugin.h"

CDirectPrintPropertyView::CDirectPrintPropertyView()
	{
	iListBox = NULL;
	}

CDirectPrintPropertyView::~CDirectPrintPropertyView()
	{
	AppUi()->RemoveFromViewStack(*this, iListBox);
	delete iListBox;
	}

CDirectPrintPropertyView* CDirectPrintPropertyView::NewL()
	{
	CDirectPrintPropertyView* self = CDirectPrintPropertyView::NewLC();
	CleanupStack::Pop(self);
	return self;
	}

CDirectPrintPropertyView* CDirectPrintPropertyView::NewLC()
	{
	CDirectPrintPropertyView* self = new(ELeave) CDirectPrintPropertyView();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

void CDirectPrintPropertyView::ConstructL()
	{
	LOG("CDirectPrintPropertyView::ConstructL BEGIN");
	BaseConstructL(R_DIRECTPRINT_PROPERTYSETTING_VIEW);

	iViewId = TUid::Uid(EDirectPrintPropertyViewId);
	LOG("CDirectPrintPropertyView::ConstructL END");
	}

void CDirectPrintPropertyView::HandleCommandL(TInt aCommand)
	{
	switch (aCommand)
		{
		case EAknSoftkeyBack:
			{
			AppUi()->HandleCommandL(EDirectPrintCmdIdPropertySettingViewBack);
			}
			break;
		default:
			{
			AppUi()->HandleCommandL(aCommand);
			}
			break;
		}
	}

void CDirectPrintPropertyView::OfferToolbarEventL(TInt aCommand)
	{
	switch (aCommand)
		{
		case EDirectPrintCmdIdPropertySettingViewPreview:
			{
			AppUi()->HandleCommandL(aCommand);
			}
			break;
		default:
			break;
		}
	}

void CDirectPrintPropertyView::ChangeTabL(TInt aTabIndex)
	{
	iTabIndex = aTabIndex;
	iListBox->SettingItemArray()->ResetAndDestroy();
	LoadListL();
	iListBox->LoadSettingsL();
	iListBox->DrawNow();
	}

void CDirectPrintPropertyView::DoActivateL(const TVwsViewId& /*aPrevViewId*/, TUid /*aCustomMessageId*/, const TDesC8& /*aCustomMessage*/)
	{
	LOG("CDirectPrintPropertyView::DoActivateL BEGIN");
	CAknToolbar* toolbar = AppUi()->CurrentFixedToolbar();
	toolbar->SetToolbarObserver(this);

	ReadDataL();

	// Create list box
	if (!iListBox)
		{
		iListBox = CDirectPrintSettingListBox::NewL(ClientRect());
		iListBox->SetMopParent(this);
		iListBox->ConstructFromResourceL(R_DIRECTPRINT_PROPERTYSETTING_SETTING_ITEM_LIST);
		iListBox->ActivateL();
		}
	ChangeTabL(EDirectPrintPaperSettingTab);
	iListBox->LoadSettingsL();
	AppUi()->AddToStackL(*this, iListBox);
	iListBox->MakeVisible(ETrue);
	iListBox->DrawNow();

	// set the title pane
	SetActiveTitleL(R_DIRECTPRINT_PROPERTYSETTING_TITLE);
	LOG("CDirectPrintPropertyView::DoActivateL END");
	}

void CDirectPrintPropertyView::DoDeactivate()
	{
	LOG("CDirectPrintPropertyView::DoDeactivate BEGIN");
	TInt err = 0;

	if (iListBox)
		{
		TRAP(err, iListBox->StoreSettingsL());
		TRAP(err, WriteDataL());

		iListBox->MakeVisible(EFalse);
		AppUi()->RemoveFromViewStack(*this, iListBox);
		}
	LOG("CDirectPrintPropertyView::DoDeactivate END");
	}

void CDirectPrintPropertyView::LoadListL()
	{
	LOG("CDirectPrintPropertyView::LoadListL BEGIN");

	switch (iTabIndex)
		{
		case EDirectPrintPaperSettingTab:
			{
			LoadPaperTabL();
			}
			break;
		case EDirectPrintPageSettingTab:
			{
			LoadPageTabL();
			}
			break;
		case EDirectPrintMarginSettingTab:
			{
			LoadMarginTabL();
			}
			break;
		case EDirectPrintHeaderSettingTab:
			{
			LoadHeaderTabL();
			}
			break;
		case EDirectPrintFooterSettingTab:
			{
			LoadFooterTabL();
			}
			break;
		default:
			break;
		}

	// Set extend setting item
	CDirectPrintUiPlugin* plugin = GetPluginMgr().CurrentPluginL();
	if (plugin)
		{
		plugin->LoadExtendItemsL(*(iListBox->SettingItemArray()), iTabIndex);
		}

	iListBox->HandleChangeInItemArrayOrVisibilityL();
	LOG("CDirectPrintPropertyView::LoadListL END");
	}

void CDirectPrintPropertyView::ReadDataL()
	{
	LOG("CDirectPrintPropertyView::ReadDataL BEGIN");

	CDirectPrintModel& model = GetModel();
	CPrinterProperty* property = model.GetCurrentPrinterPropertyL(TUid::Uid(0));

	if (property)
		{
		TInt count = property->iPropertyItems.Count();
		for (TInt i=0; i<count; i++)
			{
			switch (property->iPropertyItems[i].iItemId)
				{
				case EDirectPrintPaperPaperSize:
					{
					iPaperSize = property->iPropertyItems[i].iItemValue;
					}
					break;
				case EDirectPrintPaperOrientation:
					{
					iOrientation = property->iPropertyItems[i].iItemValue;
					}
					break;
				case EDirectPrintPaperHight:
					{
					iHight = property->iPropertyItems[i].iItemValue;
					}
					break;
				case EDirectPrintPaperWidth:
					{
					iWidth = property->iPropertyItems[i].iItemValue;
					}
					break;
				case EDirectPrintPagePrintPage:
					{
					iPrintPage = property->iPropertyItems[i].iItemValue;
					}
					break;
				case EDirectPrintPagePrintRange:
					{
					iPrintRange.Copy(property->iPropertyItems[i].iItemTextValue);
					}
					break;
				case EDirectPrintPageCopy:
					{
					iCopy = property->iPropertyItems[i].iItemValue;
					}
					break;
				case EDirectPrintMarginLeftMargin:
					{
					iLeftMargin = property->iPropertyItems[i].iItemValue;
					}
					break;
				case EDirectPrintMarginRightMargin:
					{
					iRightMargin = property->iPropertyItems[i].iItemValue;
					}
					break;
				case EDirectPrintMarginTopMargin:
					{
					iTopMargin = property->iPropertyItems[i].iItemValue;
					}
					break;
				case EDirectPrintMarginBottomMargin:
					{
					iBottomMargin = property->iPropertyItems[i].iItemValue;
					}
					break;
				case EDirectPrintHeaderHeaderText:
					{
					iHeaderText.Copy(property->iPropertyItems[i].iItemTextValue);
					}
					break;
				case EDirectPrintHeaderDistance:
					{
					iHeaderDistance = property->iPropertyItems[i].iItemValue;
					}
					break;
				case EDirectPrintHeaderFirstPage:
					{
					iHeaderFirstPage = property->iPropertyItems[i].iItemValue;
					}
					break;
				case EDirectPrintFooterFooterText:
					{
					iFooterText.Copy(property->iPropertyItems[i].iItemTextValue);
					}
					break;
				case EDirectPrintFooterDistance:
					{
					iFooterDistance = property->iPropertyItems[i].iItemValue;
					}
					break;
				case EDirectPrintFooterFirstPage:
					{
					iFooterFirstPage = property->iPropertyItems[i].iItemValue;
					}
					break;
				default:
					{
					// Set extend setting item
					CDirectPrintUiPlugin* plugin = GetPluginMgr().CurrentPluginL();
					if (plugin)
						{
						plugin->ReadDataL(property->iPropertyItems[i]);
						}
					}
					break;
				}
			}
		}
	else
		{
		model.SetCurrentProperty(model.PropertyCount());
		NewDataL();
		}

	LOG("CDirectPrintPropertyView::ReadDataL END");
	}

void CDirectPrintPropertyView::WriteDataL()
	{
	LOG("CDirectPrintPropertyView::WriteDataL BEGIN");

	CDirectPrintModel& model = GetModel();
	CPrinterProperty* property = new (ELeave) CPrinterProperty();
	CleanupStack::PushL(property);
	property->iPrinterId = model.GetCurrentPrinterIdL();
	property->iApplicationId = 0;

	TDirectPrintPropertyData data1;
	data1.iItemType = EDirectPrintPaperSettingTab;
	data1.iItemId = EDirectPrintPaperPaperSize;
	//data1.iItemName.Copy();
	data1.iItemValue = iPaperSize;
	property->iPropertyItems.Append(data1);

	TDirectPrintPropertyData data2;
	data2.iItemType = EDirectPrintPaperSettingTab;
	data2.iItemId = EDirectPrintPaperOrientation;
	//data2.iItemName.Copy();
	data2.iItemValue = iOrientation;
	property->iPropertyItems.Append(data2);

	TDirectPrintPropertyData data3;
	data3.iItemType = EDirectPrintPaperSettingTab;
	data3.iItemId = EDirectPrintPaperHight;
	//data3.iItemName.Copy();
	data3.iItemValue = iHight;
	property->iPropertyItems.Append(data3);

	TDirectPrintPropertyData data4;
	data4.iItemType = EDirectPrintPaperSettingTab;
	data4.iItemId = EDirectPrintPaperWidth;
	//data4.iItemName.Copy();
	data4.iItemValue = iWidth;
	property->iPropertyItems.Append(data4);

	TDirectPrintPropertyData data5;
	data5.iItemType = EDirectPrintPageSettingTab;
	data5.iItemId = EDirectPrintPagePrintPage;
	//data5.iItemName.Copy();
	data5.iItemValue = iPrintPage;
	property->iPropertyItems.Append(data5);

	TDirectPrintPropertyData data6;
	data6.iItemType = EDirectPrintPageSettingTab;
	data6.iItemId = EDirectPrintPagePrintRange;
	//data6.iItemName.Copy();
	data6.iItemTextValue.Copy(iPrintRange);
	property->iPropertyItems.Append(data6);

	TDirectPrintPropertyData data7;
	data7.iItemType = EDirectPrintPageSettingTab;
	data7.iItemId = EDirectPrintPageCopy;
	//data7.iItemName.Copy();
	data7.iItemValue = iCopy;
	property->iPropertyItems.Append(data7);

	TDirectPrintPropertyData data8;
	data8.iItemType = EDirectPrintMarginSettingTab;
	data8.iItemId = EDirectPrintMarginLeftMargin;
	//data8.iItemName.Copy();
	data8.iItemValue = iLeftMargin;
	property->iPropertyItems.Append(data8);

	TDirectPrintPropertyData data9;
	data9.iItemType = EDirectPrintMarginSettingTab;
	data9.iItemId = EDirectPrintMarginRightMargin;
	//data9.iItemName.Copy();
	data9.iItemValue = iRightMargin;
	property->iPropertyItems.Append(data9);

	TDirectPrintPropertyData data10;
	data10.iItemType = EDirectPrintMarginSettingTab;
	data10.iItemId = EDirectPrintMarginTopMargin;
	//data10.iItemName.Copy();
	data10.iItemValue = iTopMargin;
	property->iPropertyItems.Append(data10);

	TDirectPrintPropertyData data11;
	data11.iItemType = EDirectPrintMarginSettingTab;
	data11.iItemId = EDirectPrintMarginBottomMargin;
	//data11.iItemName.Copy();
	data11.iItemValue = iBottomMargin;
	property->iPropertyItems.Append(data11);

	TDirectPrintPropertyData data12;
	data12.iItemType = EDirectPrintHeaderSettingTab;
	data12.iItemId = EDirectPrintHeaderHeaderText;
	//data12.iItemName.Copy();
	data12.iItemTextValue.Copy(iHeaderText);
	property->iPropertyItems.Append(data12);

	TDirectPrintPropertyData data13;
	data13.iItemType = EDirectPrintHeaderSettingTab;
	data13.iItemId = EDirectPrintHeaderDistance;
	//data13.iItemName.Copy();
	data13.iItemValue = iHeaderDistance;
	property->iPropertyItems.Append(data13);

	TDirectPrintPropertyData data14;
	data14.iItemType = EDirectPrintHeaderSettingTab;
	data14.iItemId = EDirectPrintHeaderFirstPage;
	//data14.iItemName.Copy();
	data14.iItemValue = iHeaderFirstPage;
	property->iPropertyItems.Append(data14);

	TDirectPrintPropertyData data15;
	data15.iItemType = EDirectPrintFooterSettingTab;
	data15.iItemId = EDirectPrintFooterFooterText;
	//data15.iItemName.Copy();
	data15.iItemTextValue.Copy(iFooterText);
	property->iPropertyItems.Append(data15);

	TDirectPrintPropertyData data16;
	data16.iItemType = EDirectPrintFooterSettingTab;
	data16.iItemId = EDirectPrintFooterDistance;
	//data16.iItemName.Copy();
	data16.iItemValue = iFooterDistance;
	property->iPropertyItems.Append(data16);

	TDirectPrintPropertyData data17;
	data17.iItemType = EDirectPrintFooterSettingTab;
	data17.iItemId = EDirectPrintFooterFirstPage;
	//data17.iItemName.Copy();
	data17.iItemValue = iFooterFirstPage;
	property->iPropertyItems.Append(data17);

	// Set extend setting item
	CDirectPrintUiPlugin* plugin = GetPluginMgr().CurrentPluginL();
	if (plugin)
		{
		plugin->WriteDataL(property->iPropertyItems);
		}

	CleanupStack::Pop(property);
	model.SetPrinterPropertyL(property);

	LOG("CDirectPrintPropertyView::WriteDataL END");
	}

void CDirectPrintPropertyView::NewDataL()
	{
	LOG("CDirectPrintPropertyView::NewDataL BEGIN");

	iPaperSize = 0;
	iOrientation = 0;
	iHight = 0;
	iWidth = 0;
	iPrintPage = 0;
	iPrintRange.Copy(_L(""));
	iCopy = 1;
	iLeftMargin = 2;
	iRightMargin = 2;
	iTopMargin = 2;
	iBottomMargin = 2;
	iHeaderText.Copy(_L(""));
	iHeaderDistance = 0;
	iHeaderFirstPage = 0;
	iFooterText.Copy(_L(""));
	iFooterDistance = 0;
	iFooterFirstPage = 0;

	// Set extend setting item
	CDirectPrintUiPlugin* plugin = GetPluginMgr().CurrentPluginL();
	if (plugin)
		{
		plugin->NewDataL();
		}

	LOG("CDirectPrintPropertyView::NewDataL END");
	}

void CDirectPrintPropertyView::LoadPaperTabL()
	{
	TBuf<25> itemTitle;

	// Set common setting item
	CAknEnumeratedTextPopupSettingItem* item1 = new (ELeave) CAknEnumeratedTextPopupSettingItem(1, iPaperSize);
	CleanupStack::PushL(item1);
	CEikonEnv::Static()->ReadResource(itemTitle, R_DIRECTPRINT_PROPERTYSETTING_ITEM_PAPERSIZE);
	item1->ConstructL(EFalse, 1, itemTitle, NULL, R_DIRECTPRINT_PROPERTYSETTING_POPUP_SETTING_PAGE, -1, 0, R_DIRECTPRINT_PROPERTYSETTING_SIZE_POPUP_SETTING_TEXTS);
	iListBox->SettingItemArray()->AppendL(item1);
	CleanupStack::Pop(item1);

	CAknEnumeratedTextPopupSettingItem* item2 = new (ELeave) CAknEnumeratedTextPopupSettingItem(2, iOrientation);
	CleanupStack::PushL(item2);
	CEikonEnv::Static()->ReadResource(itemTitle, R_DIRECTPRINT_PROPERTYSETTING_ITEM_ORIENTATION);
	item2->ConstructL(EFalse, 2, itemTitle, NULL, R_DIRECTPRINT_PROPERTYSETTING_POPUP_SETTING_PAGE, -1, 0, R_DIRECTPRINT_PROPERTYSETTING_ORIENTATION_POPUP_SETTING_TEXTS);
	iListBox->SettingItemArray()->AppendL(item2);
	CleanupStack::Pop(item2);

	CAknEnumeratedTextPopupSettingItem* item3 = new (ELeave) CAknEnumeratedTextPopupSettingItem(3, iMediaType);
	CleanupStack::PushL(item3);
	CEikonEnv::Static()->ReadResource(itemTitle, R_DIRECTPRINT_PROPERTYSETTING_ITEM_MEDIATYPE);
	item3->ConstructL(EFalse, 3, itemTitle, NULL, R_DIRECTPRINT_PROPERTYSETTING_POPUP_SETTING_PAGE, -1, 0, R_DIRECTPRINT_PROPERTYSETTING_MEDIATYPE_POPUP_SETTING_TEXTS);
	iListBox->SettingItemArray()->AppendL(item3);
	CleanupStack::Pop(item3);
	}

void CDirectPrintPropertyView::LoadPageTabL()
	{
	TBuf<25> itemTitle;

	// Set common setting item
	CAknEnumeratedTextPopupSettingItem* item1 = new (ELeave) CAknEnumeratedTextPopupSettingItem(1, iPrintPage);
	CleanupStack::PushL(item1);
	CEikonEnv::Static()->ReadResource(itemTitle, R_DIRECTPRINT_PROPERTYSETTING_ITEM_PRINTPAGE);
	item1->ConstructL(EFalse, 1, itemTitle, NULL, R_DIRECTPRINT_PROPERTYSETTING_POPUP_SETTING_PAGE, -1, 0, R_DIRECTPRINT_PROPERTYSETTING_RANGE_POPUP_SETTING_TEXTS);
	iListBox->SettingItemArray()->AppendL(item1);
	CleanupStack::Pop(item1);

	CAknTextSettingItem* item2 = new (ELeave) CAknTextSettingItem(2, iPrintRange);
	CleanupStack::PushL(item2);
	item2->SetEmptyItemTextL( _L("None") );
	CEikonEnv::Static()->ReadResource(itemTitle, R_DIRECTPRINT_PROPERTYSETTING_ITEM_PRINTRANGE);
	item2->ConstructL(EFalse, 2, itemTitle, NULL, R_DIRECTPRINT_PROPERTYSETTING_TEXT_SETTING_PAGE, -1);
	iListBox->SettingItemArray()->AppendL(item2);
	CleanupStack::Pop(item2);

	CAknIntegerEdwinSettingItem* item3 = new (ELeave) CAknIntegerEdwinSettingItem(3, iCopy);
	CleanupStack::PushL(item3);
	CEikonEnv::Static()->ReadResource(itemTitle, R_DIRECTPRINT_PROPERTYSETTING_ITEM_COPY);
	item3->ConstructL(EFalse, 3, itemTitle, NULL, R_DIRECTPRINT_PROPERTYSETTING_INTEGER_SETTING_PAGE, -1);
	iListBox->SettingItemArray()->AppendL(item3);
	CleanupStack::Pop(item3);
	}

void CDirectPrintPropertyView::LoadMarginTabL()
	{
	TBuf<25> itemTitle;

	// Set common setting item
	CAknIntegerEdwinSettingItem* item1 = new (ELeave) CAknIntegerEdwinSettingItem(1, iLeftMargin);
	CleanupStack::PushL(item1);
	CEikonEnv::Static()->ReadResource(itemTitle, R_DIRECTPRINT_PROPERTYSETTING_ITEM_LEFTMARGIN);
	item1->ConstructL(EFalse, 1, itemTitle, NULL, R_DIRECTPRINT_PROPERTYSETTING_INTEGER_SETTING_PAGE, -1);
	iListBox->SettingItemArray()->AppendL(item1);
	CleanupStack::Pop(item1);

	CAknIntegerEdwinSettingItem* item2 = new (ELeave) CAknIntegerEdwinSettingItem(2, iRightMargin);
	CleanupStack::PushL(item2);
	CEikonEnv::Static()->ReadResource(itemTitle, R_DIRECTPRINT_PROPERTYSETTING_ITEM_RIGHTMARGIN);
	item2->ConstructL(EFalse, 2, itemTitle, NULL, R_DIRECTPRINT_PROPERTYSETTING_INTEGER_SETTING_PAGE, -1);
	iListBox->SettingItemArray()->AppendL(item2);
	CleanupStack::Pop(item2);

	CAknIntegerEdwinSettingItem* item3 = new (ELeave) CAknIntegerEdwinSettingItem(3, iTopMargin);
	CleanupStack::PushL(item3);
	CEikonEnv::Static()->ReadResource(itemTitle, R_DIRECTPRINT_PROPERTYSETTING_ITEM_TOPMARGIN);
	item3->ConstructL(EFalse, 3, itemTitle, NULL, R_DIRECTPRINT_PROPERTYSETTING_INTEGER_SETTING_PAGE, -1);
	iListBox->SettingItemArray()->AppendL(item3);
	CleanupStack::Pop(item3);

	CAknIntegerEdwinSettingItem* item4 = new (ELeave) CAknIntegerEdwinSettingItem(4, iBottomMargin);
	CleanupStack::PushL(item4);
	CEikonEnv::Static()->ReadResource(itemTitle, R_DIRECTPRINT_PROPERTYSETTING_ITEM_BOTTOMMARGIN);
	item4->ConstructL(EFalse, 4, itemTitle, NULL, R_DIRECTPRINT_PROPERTYSETTING_INTEGER_SETTING_PAGE, -1);
	iListBox->SettingItemArray()->AppendL(item4);
	CleanupStack::Pop(item4);
	}

void CDirectPrintPropertyView::LoadHeaderTabL()
	{
	TBuf<25> itemTitle;

	// Set common setting item
	CAknTextSettingItem* item1 = new (ELeave) CAknTextSettingItem(1, iHeaderText);
	CleanupStack::PushL(item1);
	item1->SetEmptyItemTextL( _L("None") );
	CEikonEnv::Static()->ReadResource(itemTitle, R_DIRECTPRINT_PROPERTYSETTING_ITEM_HEADERTEXT);
	item1->ConstructL(EFalse, 1, itemTitle, NULL, R_DIRECTPRINT_PROPERTYSETTING_TEXT_SETTING_PAGE, -1);
	iListBox->SettingItemArray()->AppendL(item1);
	CleanupStack::Pop(item1);

	CAknIntegerEdwinSettingItem* item2 = new (ELeave) CAknIntegerEdwinSettingItem(2, iHeaderDistance);
	CleanupStack::PushL(item2);
	CEikonEnv::Static()->ReadResource(itemTitle, R_DIRECTPRINT_PROPERTYSETTING_ITEM_HEADERDISTANCE);
	item2->ConstructL(EFalse, 2, itemTitle, NULL, R_DIRECTPRINT_PROPERTYSETTING_INTEGER_SETTING_PAGE, -1);
	iListBox->SettingItemArray()->AppendL(item2);
	CleanupStack::Pop(item2);

	CAknEnumeratedTextPopupSettingItem* item3 = new (ELeave) CAknEnumeratedTextPopupSettingItem(3, iHeaderFirstPage);
	CleanupStack::PushL(item3);
	CEikonEnv::Static()->ReadResource(itemTitle, R_DIRECTPRINT_PROPERTYSETTING_ITEM_HEADERFIRSTPAGE);
	item3->ConstructL(EFalse, 3, itemTitle, NULL, R_DIRECTPRINT_PROPERTYSETTING_POPUP_SETTING_PAGE, -1, 0, R_DIRECTPRINT_PROPERTYSETTING_PRINT_POPUP_SETTING_TEXTS);
	iListBox->SettingItemArray()->AppendL(item3);
	CleanupStack::Pop(item3);
	}

void CDirectPrintPropertyView::LoadFooterTabL()
	{
	TBuf<25> itemTitle;

	// Set common setting item
	CAknTextSettingItem* item1 = new (ELeave) CAknTextSettingItem(1, iFooterText);
	CleanupStack::PushL(item1);
	item1->SetEmptyItemTextL( _L("None") );
	CEikonEnv::Static()->ReadResource(itemTitle, R_DIRECTPRINT_PROPERTYSETTING_ITEM_FOOTERTEXT);
	item1->ConstructL(EFalse, 1, itemTitle, NULL, R_DIRECTPRINT_PROPERTYSETTING_TEXT_SETTING_PAGE, -1);
	iListBox->SettingItemArray()->AppendL(item1);
	CleanupStack::Pop(item1);

	CAknIntegerEdwinSettingItem* item2 = new (ELeave) CAknIntegerEdwinSettingItem(2, iFooterDistance);
	CleanupStack::PushL(item2);
	CEikonEnv::Static()->ReadResource(itemTitle, R_DIRECTPRINT_PROPERTYSETTING_ITEM_FOOTERDISTANCE);
	item2->ConstructL(EFalse, 2, itemTitle, NULL, R_DIRECTPRINT_PROPERTYSETTING_INTEGER_SETTING_PAGE, -1);
	iListBox->SettingItemArray()->AppendL(item2);
	CleanupStack::Pop(item2);

	CAknEnumeratedTextPopupSettingItem* item3 = new (ELeave) CAknEnumeratedTextPopupSettingItem(3, iFooterFirstPage);
	CleanupStack::PushL(item3);
	CEikonEnv::Static()->ReadResource(itemTitle, R_DIRECTPRINT_PROPERTYSETTING_ITEM_FOOTERFIRSTPAGE);
	item3->ConstructL(EFalse, 3, itemTitle, NULL, R_DIRECTPRINT_PROPERTYSETTING_POPUP_SETTING_PAGE, -1, 0, R_DIRECTPRINT_PROPERTYSETTING_PRINT_POPUP_SETTING_TEXTS);
	iListBox->SettingItemArray()->AppendL(item3);
	CleanupStack::Pop(item3);
	}

