/*
* Copyright (c) 2007-2010 Sebastian Brannstrom, Lars Persson, EmbedDev AB
*
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* EmbedDev AB - initial contribution.
*
* Contributors:
*
* Description:
*
*/

#include "PodcastSettingsView.h"
#include "PodcastAppUi.h"
#include "Podcast.hrh"
#include <aknlists.h>
#include <aknsettingitemlist.h>
#include <aknnavide.h> 
#include <podcast.rsg>
#include "SettingsEngine.h"

#include <akncommondialogsdynmem.h> 
#include <pathinfo.h>
#include <aknquerydialog.h>


class CIapSetting: public CAknEnumeratedTextPopupSettingItem 
{ 
public:
	CIapSetting(TInt aResourceId, TInt& aValue, CPodcastModel &aPodcastModel) :
		CAknEnumeratedTextPopupSettingItem(aResourceId, aValue), iPodcastModel(aPodcastModel), iIap(aValue)
		{
		}

	~CIapSetting()
		{
		}

	void RefreshConnectionListL()
		{	
		CArrayPtr< CAknEnumeratedText > * enumeratedArr = EnumeratedTextArray();
		CArrayPtr< HBufC > * poppedUpTextArray = PoppedUpTextArray();
		enumeratedArr->ResetAndDestroy();
		poppedUpTextArray->ResetAndDestroy();		
			
		CDesCArrayFlat *iapArray = iPodcastModel.IAPNames();

		TBool valueExists = EFalse;
		DP2("InternalValue=%d, ExternalValue=%d", InternalValue(), ExternalValue());
		for (int i=0;i<iapArray->Count();i++) {
		HBufC *buf = (*iapArray)[i].AllocL();
		poppedUpTextArray->AppendL(buf);

		TInt iapId = iPodcastModel.IAPIds()[i].iIapId;
		DP2("IAP name='%S', id=%d", buf, iapId);

		if (iapId == ExternalValue()) {
		valueExists = ETrue;
		}
		// both arrays destroy themselves, so we need two copies to prevent USER 44
		HBufC *buf2 = (*iapArray)[i].AllocL();

		CAknEnumeratedText *enumerated = new CAknEnumeratedText(iapId, buf2);
		enumeratedArr->AppendL(enumerated);
		}

		DP1("valueExists=%d", valueExists);
		if (!valueExists && iPodcastModel.IAPIds().Count() > 0 ) {
		DP1("Setting iIap=%d", iPodcastModel.IAPIds()[0].iIapId);
		iIap = iPodcastModel.IAPIds()[0].iIapId;
		LoadL();
		DP2("InternalValue=%d, ExternalValue=%d", InternalValue(), ExternalValue());
		}

		TRAPD(err, HandleTextArrayUpdateL());

		if (err != KErrNone) {
		DP1("Leave in HandleTextArrayUpdateL, err=%d", err);
		}
		}

	void CompleteConstructionL()
		{
		DP("CIapSetting::CompleteConstructionL BEGIN");
		CAknEnumeratedTextPopupSettingItem::CompleteConstructionL();
		RefreshConnectionListL();
		DP("CIapSetting::CompleteConstructionL END");
		}

	void EditItemL(TBool aCalledFromMenu)
		{
		DP("CIapSetting::EditItemL BEGIN");
		LoadL();
		CAknEnumeratedTextPopupSettingItem::EditItemL(aCalledFromMenu);
		StoreL();		
		DP2("InternalValue=%d, ExternalValue=%d", InternalValue(), ExternalValue());
		DP("CIapSetting::EditItemL END");
		}

	void HandleSettingPageEventL(CAknSettingPage* aSettingPage, TAknSettingPageEvent aEventType)
		{
		DP("CIapSetting::HandleSettingPageEventL BEGIN");
		CAknSettingItem::HandleSettingPageEventL(aSettingPage, aEventType);
		/*if (aEventType == EEventSettingOked) 
			{
			DP2("InternalValue=%d, ExternalValue=%d", InternalValue(), ExternalValue());
			StoreL();
			}*/
		DP("CIapSetting::HandleSettingPageEventL END");
		}

protected:
  	CPodcastModel& iPodcastModel;
  	TInt& iIap;
};


class CConnectionSetting: public CAknEnumeratedTextPopupSettingItem 
{ 
public:
	CConnectionSetting(TInt aResourceId, TInt& aValue, CPodcastModel &aPodcastModel) :
		CAknEnumeratedTextPopupSettingItem(aResourceId, aValue), iPodcastModel(aPodcastModel)
		{
		}

	~CConnectionSetting()
		{
		}

	void RefreshConnectionListL()
		{	
		CArrayPtr< CAknEnumeratedText > * enumeratedArr = EnumeratedTextArray();
		CArrayPtr< HBufC > * poppedUpTextArray = PoppedUpTextArray();						
			
		CDesCArrayFlat *snapArray = iPodcastModel.SNAPNames();

		TBool valueExists = EFalse;
		DP2("InternalValue=%d, ExternalValue=%d", InternalValue(), ExternalValue());
		for (int i=0;i<snapArray->Count();i++) {
			HBufC *buf = (*snapArray)[i].AllocL();
			poppedUpTextArray->InsertL(EConnectionUseNetwork+i,buf);

			TInt snapId = iPodcastModel.SNAPIds()[i].iIapId;
			DP2("SNAP name='%S', id=%d", buf, snapId);
	
			if (snapId == ExternalValue()) {
			valueExists = ETrue;
		}
		// both arrays destroy themselves, so we need two copies to prevent USER 44
		HBufC *buf2 = (*snapArray)[i].AllocL();

		CAknEnumeratedText *enumerated = new CAknEnumeratedText(EConnectionUseNetwork+i, buf2);
		enumeratedArr->InsertL(EConnectionUseNetwork+i, enumerated);
		}

		DP1("valueExists=%d", valueExists);
		if (!valueExists && iPodcastModel.SNAPIds().Count() > 0 ) {
		DP1("Setting i=%d", iPodcastModel.SNAPIds()[0].iIapId);	
		LoadL();
		DP2("InternalValue=%d, ExternalValue=%d", InternalValue(), ExternalValue());
		}

		TRAPD(err, HandleTextArrayUpdateL());

		if (err != KErrNone) {
		DP1("Leave in HandleTextArrayUpdateL, err=%d", err);
		}
		}

	void CompleteConstructionL()
		{
		DP("CIapSetting::CompleteConstructionL BEGIN");
		CAknEnumeratedTextPopupSettingItem::CompleteConstructionL();
		RefreshConnectionListL();
		DP("CIapSetting::CompleteConstructionL END");
		}

	void EditItemL(TBool aCalledFromMenu)
		{
		DP("CIapSetting::EditItemL BEGIN");
		LoadL();
		CAknEnumeratedTextPopupSettingItem::EditItemL(aCalledFromMenu);
		StoreL();		
		DP2("InternalValue=%d, ExternalValue=%d", InternalValue(), ExternalValue());
		DP("CIapSetting::EditItemL END");
		}

	void HandleSettingPageEventL(CAknSettingPage* aSettingPage, TAknSettingPageEvent aEventType)
		{
		DP("CIapSetting::HandleSettingPageEventL BEGIN");
		CAknSettingItem::HandleSettingPageEventL(aSettingPage, aEventType);
		/*if (aEventType == EEventSettingOked) 
			{
			DP2("InternalValue=%d, ExternalValue=%d", InternalValue(), ExternalValue());
			StoreL();
			}*/
		DP("CIapSetting::HandleSettingPageEventL END");
		}

protected:
  	CPodcastModel& iPodcastModel;  
};

class CPodcastSettingItemList:public CAknSettingItemList
	{
public:
	CPodcastSettingItemList(CPodcastModel& aPodcastModel) : iPodcastModel(aPodcastModel)
		{
		}
	
	~CPodcastSettingItemList()
		{
		DP("CPodcastSettingItemList~");
		}

	void StoreSettings() {
		DP("StoreSettings BEGIN");
		StoreSettingsL();
		CSettingsEngine &se = iPodcastModel.SettingsEngine();
		se.SetBaseDir(iShowDir);
		DP1("Base Dir: %S", &iShowDir);
		se.SetUpdateAutomatically((TAutoUpdateSetting)iAutoUpdate);
		DP1("Update automatically: %d", iAutoUpdate);		
		switch(iConnection)
			{
			case EConnectionDefault:
				{
				DP("Specific IAP: 0 Ask user");
				se.SetSpecificIAP(0);	
				}break;
			case EConnectionAlwaysAsk:
				{
				DP("Specific IAP: -1 Ask user");
				se.SetSpecificIAP(-1);		
				}break;
			
			case EConnectionUseIap:
				{		
				DP1("Specific NETWORK: %d", iIap);		
				se.SetSpecificIAP((iIap|KUseIAPFlag));	
				}break;
			default:
			case EConnectionUseNetwork:
				{
				DP1("Specific IAP: %d", iIap);		
				TInt snapIndex = iConnection-EConnectionUseNetwork;
				se.SetSpecificIAP(iPodcastModel.SNAPIds()[snapIndex].iIapId);	
				}break;
			}		
		
		DP1("Download automatically: %d", iAutoDownload);
		se.SetDownloadAutomatically(iAutoDownload);
		se.SaveSettingsL();
		DP("StoreSettings END");
	}
	
	void HandleListBoxEventL(CEikListBox* aListBox, TListBoxEvent aEventType)
	{
		DP1("CPodcastSettingItemList::HandleListBoxEventL event %d", aEventType)
		CAknSettingItemList::HandleListBoxEventL(aListBox, aEventType);
	}
	
	void UpdateSettingVisibility()
		{
		DP("UpdateSettingVisibility BEGIN");
		LoadSettingsL();
		TBool dimAutoUpdate = iConnection == EConnectionAlwaysAsk;
		TBool dimIAP = iConnection < EConnectionUseIap;
	
		iSettingAutoUpdate->SetHidden(dimAutoUpdate);
		iSettingIAP->SetHidden(dimIAP);
		if(!dimIAP)
			{
			iSettingIAP->RefreshConnectionListL();
			}
		ListBox()->ScrollToMakeItemVisible(0);
		
		TRAP_IGNORE(HandleChangeInItemArrayOrVisibilityL());
		DP("UpdateSettingVisibility END");
		}

	void  EditItemL (TInt aIndex, TBool aCalledFromMenu)
		{
		DP("EditItemL BEGIN");
		if (aIndex == 0) {
			TFileName selectedFolder;
			selectedFolder.Copy(iShowDir);
			TFileName startFolder;
			startFolder.Zero();
			TInt types = AknCommonDialogsDynMem::EMemoryTypePhone | AknCommonDialogsDynMem::EMemoryTypeMMC |AknCommonDialogsDynMem::EMemoryTypeInternalMassStorage| AknCommonDialogsDynMem::EMemoryTypeRemote;
			
			HBufC *title = iCoeEnv->AllocReadResourceLC(R_PODCAST_SELECT_FOLDER);
			if (AknCommonDialogsDynMem::RunFolderSelectDlgLD (types, selectedFolder,
					startFolder, NULL, NULL, *title))
				{
				_LIT(KPodcastsDir, "Podcasts");
				TInt pos = selectedFolder.Find(KPodcastsDir);
				if (pos == KErrNotFound || pos != selectedFolder.Length()-9)
					{
					// append "Podcasts" if the folder isn't already called this
					selectedFolder.Append(KPodcastsDir);
					}
				
				if (selectedFolder[selectedFolder.Length()-1] != '\\')
					{
					selectedFolder.Append(_L("\\"));
					}
				
				iShowDir.Copy(selectedFolder);
				LoadSettingsL();
				}
			CleanupStack::PopAndDestroy(title);
			}
		else {
			CAknSettingItemList::EditItemL(aIndex,aCalledFromMenu);
		}
			StoreSettingsL();
			UpdateSettingVisibility();
		DP("EditItemL END");
		}
	
	/**
	 * Framework method to create a setting item based upon the user id aSettingId. The 
	 * client code decides what type to contruct. new (ELeave) must then be used and the resulting 
	 * pointer returned. Ownership is thereafter base class's responsiblity.
	 *
	 * @param aSettingId	ID to use to determine the type of the setting item
	 * @return a constructed (not 2nd-stage constructed) setting item.
	 */
	CAknSettingItem* CreateSettingItemL( TInt aSettingId )
		{
		DP1("CreateSettingItemL BEGIN, aSettingId=%d", aSettingId);

		CSettingsEngine &se = iPodcastModel.SettingsEngine();
		iShowDir.Copy(se.BaseDir());
		iAutoUpdate = se.UpdateAutomatically();
		iIap = se.SpecificIAP();
		
		if( iIap == 0)
			{
			iConnection = EConnectionDefault;
			}
		else if ( iIap == -1)
			{
			iConnection = EConnectionAlwaysAsk;	
			}
		else if ( (iIap & KUseIAPFlag))
			{
			iConnection = EConnectionUseIap;
			iIap = iIap& KUseIAPMask;
			}
		else
			{
			TInt snapIndex = 0;
			TInt cnt = iPodcastModel.SNAPIds().Count();
			while(snapIndex < cnt && ((TInt)iPodcastModel.SNAPIds()[snapIndex].iIapId) != iIap)
				{
				snapIndex++;
				}
			
			if(snapIndex != cnt)
				{
				iConnection = EConnectionUseNetwork+snapIndex;
				}
			else
				{
				iConnection = EConnectionAlwaysAsk;
				}
			}
									
		iAutoDownload = se.DownloadAutomatically();
			
		switch(aSettingId)
			{
			case EPodcastSettingShowDir:
				DP("EPodcastSettingShowDir");
				return new (ELeave) CAknTextSettingItem(aSettingId, iShowDir);
				break;
			case EPodcastSettingAutoUpdate:
				DP("EPodcastSettingAutoUpdate");
				iSettingAutoUpdate = new (ELeave) CAknEnumeratedTextPopupSettingItem(aSettingId, iAutoUpdate);
				return iSettingAutoUpdate;
				break;
			case EPodcastSettingConnection:
				DP("EPodcastSettingConnection");
				return new (ELeave) CConnectionSetting (aSettingId, iConnection, iPodcastModel);
				break;
			case EPodcastSettingIAPList:
				DP("EPodcastSettingIAPList");
				iSettingIAP = new (ELeave) CIapSetting (aSettingId, iIap, iPodcastModel);
				return iSettingIAP;
				break;
			case EPodcastSettingAutoDownload:
				DP("EPodcastSettingAutoDownload");
				iSettingAutoDownload = new (ELeave) CAknBinaryPopupSettingItem (aSettingId, iAutoDownload);
				return iSettingAutoDownload;
				break;
			default:
				return CAknSettingItemList::CreateSettingItemL(aSettingId);
				break;
			}
		DP("CreateSettingItemL END");
		return NULL;	
		}
	
	TFileName iShowDir;
	
	TInt iAutoUpdate;
	CAknSettingItem *iSettingAutoUpdate;
	
	TInt iAutoDownload;
	CAknSettingItem *iSettingAutoDownload; 

	TInt iConnection;
	
	TInt iIap;
	CIapSetting *iSettingIAP; 
		
	
	CPodcastModel &iPodcastModel;
	};


CPodcastSettingsView* CPodcastSettingsView::NewL(CPodcastModel& aPodcastModel)
    {
    CPodcastSettingsView* self = CPodcastSettingsView::NewLC(aPodcastModel);
    CleanupStack::Pop( self );
    return self;
    }

CPodcastSettingsView* CPodcastSettingsView::NewLC(CPodcastModel& aPodcastModel)
    {
    CPodcastSettingsView* self = new ( ELeave ) CPodcastSettingsView(aPodcastModel);
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

CPodcastSettingsView::CPodcastSettingsView(CPodcastModel& aPodcastModel):iPodcastModel(aPodcastModel)
{
}

void CPodcastSettingsView::ConstructL()
{
	BaseConstructL(R_PODCAST_SETTINGSVIEW);	
}
    
CPodcastSettingsView::~CPodcastSettingsView()
    {
    DP("CPodcastSettingsView::~CPodcastSettingsView()");
	delete iListbox;
    }

TUid CPodcastSettingsView::Id() const
{
	return KUidPodcastSettingsViewID;
}
		
void CPodcastSettingsView::DoActivateL(const TVwsViewId& aPrevViewId,
	                                  TUid /*aCustomMessageId*/,
	                                  const TDesC8& /*aCustomMessage*/)
{
	DP("CPodcastSettingsView::DoActivateL BEGIN");
	iPreviousView = aPrevViewId;
	
	if (iListbox) {
		delete iListbox;
		iListbox = NULL;
	}
	
	DP("Creating listbox");
	iPodcastModel.UpdateSNAPListL();
	iPodcastModel.UpdateIAPListL();
	
	iListbox =new (ELeave) CPodcastSettingItemList(iPodcastModel);
	iListbox->SetMopParent( this );
	iListbox->ConstructFromResourceL(R_PODCAST_SETTINGS);
	iListbox->SetRect(ClientRect());
	iListbox->ActivateL();   
	
	DP("Creating navipane");
	iNaviPane =( CAknNavigationControlContainer * ) StatusPane()->ControlL( TUid::Uid( EEikStatusPaneUidNavi ) );
		
	HBufC *titleBuffer = iEikonEnv->AllocReadResourceL(R_SETTINGS_TITLE);
	iNaviDecorator  = iNaviPane->CreateNavigationLabelL(*titleBuffer);
	delete titleBuffer;

	DP("Updating listbox");
	AppUi()->AddToStackL(*this, iListbox);
	iListbox->UpdateSettingVisibility();
	iListbox->MakeVisible(ETrue);
	iListbox->DrawNow();
	iListbox->SetFocus(ETrue);
	
	if(iNaviDecorator && iNaviPane)
		{
		iNaviPane->PushL(*iNaviDecorator);
		}

	DP("CPodcastSettingsView::DoActivateL END");
}

void CPodcastSettingsView::DoDeactivate()
	{
	DP("CPodcastSettingsView::DoDeactivate BEGIN");
	if (iListbox) {
		iListbox->MakeVisible(EFalse);
		
		AppUi()->RemoveFromViewStack( *this, iListbox );
		
	}
	
	if(iNaviDecorator && iNaviPane)
		{
		iNaviPane->Pop(iNaviDecorator);
		delete iNaviDecorator;
		iNaviDecorator = NULL;
		}
	DP("CPodcastSettingsView::DoDeactivate END");
	}

/** 
* Command handling function intended for overriding by sub classes. 
* Default implementation is empty.  
* @param aCommand ID of the command to respond to. 
*/
void CPodcastSettingsView::HandleCommandL(TInt aCommand)
{
	CAknView::HandleCommandL(aCommand);
	DP1("CPodcastListView::HandleCommandL=%d", aCommand);
	switch(aCommand)
	{
	case EAknSoftkeyBack:
		{
		iListbox->StoreSettings();
		AppUi()->ActivateViewL(iPreviousView);
		}
		break;
	case EPodcastResetDb:
		{
		CAknQueryDialog* dlg= new(ELeave) CAknQueryDialog();
		
		CleanupStack::PushL(dlg);
		HBufC *text = iCoeEnv->AllocReadResourceLC(R_RESET_DB_QUERY);
		dlg->SetPromptL(*text);
		CleanupStack::PopAndDestroy(text);
		CleanupStack::Pop(dlg);
		if(dlg->ExecuteLD(R_QUERYDLG))
			{
			iPodcastModel.DropDB();
			AppUi()->Exit();
			}
		}
		break;
	default:
		{
		AppUi()->HandleCommandL(aCommand);
		}
		break;
	}
}

void CPodcastSettingsView::HandleStatusPaneSizeChange()
{
	CAknView::HandleStatusPaneSizeChange();

	DP2("CPodcastSettingsView::HandleStatusPaneSizeChange() width=%d, height=%d", ClientRect().Width(), ClientRect().Height());
	if (iListbox) {
		iListbox->SetRect( ClientRect());	
	}
}
