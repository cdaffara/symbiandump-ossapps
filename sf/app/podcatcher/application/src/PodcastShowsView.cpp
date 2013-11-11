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

#include "PodcastShowsView.h"
#include "PodcastAppUi.h"
#include "ShowEngine.h"
#include "SettingsEngine.h"
#include "PodcastApp.h"
#include "imagehandler.h"
#include "constants.h"

#include <akntitle.h>
#include <podcast.rsg>
#include <podcast.mbg>
#include <gulicon.h>
#include <barsread.h>
#include <aknnotedialog.h>
#include <aknmessagequerydialog.h>

#define KMaxMessageLength 200

_LIT(KSizeDownloadingOf, "%.1f/%.1f MB");
_LIT(KShowsSizeFormatS60, "%.1f MB");

_LIT(KShowFormat, "%d\t%S\t%S%S\t");

// these must correspond with TShowsIconIndex

const TUint KShowIconArrayIds[] =
	{
			EMbmPodcastAudio,
			EMbmPodcastAudio_mask,
			EMbmPodcastAudio_new,
			EMbmPodcastAudio_new_mask,
			EMbmPodcastAudio_queued,
			EMbmPodcastAudio_queued_mask,
			EMbmPodcastAudio_downloading,
			EMbmPodcastAudio_downloading_mask,
			EMbmPodcastAudio_downloaded,
			EMbmPodcastAudio_downloaded_mask,
			EMbmPodcastAudio_downloaded_new,
			EMbmPodcastAudio_downloaded_new_mask,
			EMbmPodcastAudio_failed,
			EMbmPodcastAudio_failed_mask,
			EMbmPodcastAudio_suspended,
			EMbmPodcastAudio_suspended_mask,
			EMbmPodcastVideo,
			EMbmPodcastVideo_mask,
			EMbmPodcastVideo_new,
			EMbmPodcastVideo_new_mask,
			EMbmPodcastVideo_queued,
			EMbmPodcastVideo_queued_mask,
			EMbmPodcastVideo_downloading,
			EMbmPodcastVideo_downloading_mask,
			EMbmPodcastVideo_downloaded,
			EMbmPodcastVideo_downloaded_mask,
			EMbmPodcastVideo_downloaded_new,
			EMbmPodcastVideo_downloaded_new_mask,
			EMbmPodcastVideo_failed,
			EMbmPodcastVideo_failed_mask,
			EMbmPodcastVideo_suspended,
			EMbmPodcastVideo_suspended_mask,
			EMbmPodcastFeed,
			EMbmPodcastFeed_mask,
			0,
			0
	};

const TInt KVideoIconOffset = 8;

CPodcastShowsView* CPodcastShowsView::NewL(CPodcastModel& aPodcastModel)
	{
	CPodcastShowsView* self = CPodcastShowsView::NewLC(aPodcastModel);
	CleanupStack::Pop(self);
	return self;
	}

CPodcastShowsView* CPodcastShowsView::NewLC(CPodcastModel& aPodcastModel)
	{
	CPodcastShowsView* self = new ( ELeave ) CPodcastShowsView(aPodcastModel);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CPodcastShowsView::CPodcastShowsView(CPodcastModel& aPodcastModel) :
	iPodcastModel(aPodcastModel)
	{
	}

void CPodcastShowsView::ConstructL()
	{
	BaseConstructL(R_PODCAST_SHOWSVIEW);
	CPodcastListView::ConstructL();
	
	CreateIconsL();
	
	iListContainer->Listbox()->SetListBoxObserver(this);
	
	iPodcastModel.FeedEngine().AddObserver(this);
	iPodcastModel.ShowEngine().AddObserver(this);
	
	iStylusPopupMenu = CAknStylusPopUpMenu::NewL( this , TPoint(0,0));
	TResourceReader reader;
	iCoeEnv->CreateResourceReaderLC(reader,R_SHOWVIEW_POPUP_MENU);
	iStylusPopupMenu->ConstructFromResourceL(reader);

	CleanupStack::PopAndDestroy();	
	}

void CPodcastShowsView::CreateIconsL()
	{
	CArrayPtr< CGulIcon>* icons = new(ELeave) CArrayPtrFlat< CGulIcon>(1);
	CleanupStack::PushL(icons);
	TInt pos = 0;
	while (KShowIconArrayIds[pos] > 0)
		{
		// Load the icon	
		CFbsBitmap* bitmap= NULL;
		CFbsBitmap* mask=  NULL;
		AknIconUtils::CreateIconL(bitmap,
					                          mask,
					                          iEikonEnv->EikAppUi()->Application()->BitmapStoreName(),
					                          KShowIconArrayIds[pos],
					                          KShowIconArrayIds[pos+1]);	
		CleanupStack::PushL(bitmap);		
		CleanupStack::PushL(mask);
		
		// Append the icon to icon array
		icons->AppendL(CGulIcon::NewL(bitmap, mask) );
		CleanupStack::Pop(2); // bitmap, mask
		pos+=2;
		}
		
	iListContainer->Listbox()->ItemDrawer()->FormattedCellData()->SetIconArrayL(icons);
	CleanupStack::Pop(icons); // icons
	}

TKeyResponse CPodcastShowsView::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType)
	{
	if (aType == EEventKey)
		{
		CShowInfo *activeShow = NULL;

		TInt index = iListContainer->Listbox()->CurrentItemIndex();
		if(index >= 0 && index < iPodcastModel.ActiveShowList().Count())
		{
			activeShow = iPodcastModel.ActiveShowList()[index];
		}
		
		if (activeShow != NULL) {
			DP1("aKeyEvent.iCode=%d", aKeyEvent.iCode);
			switch (aKeyEvent.iCode) {
			case 117: 
			case '*':
			case EKeySpace:
				if (activeShow->PlayState() == EPlayed) {
					HandleCommandL(EPodcastMarkAsUnplayed);
				} else {
					HandleCommandL(EPodcastMarkAsPlayed);
				}
				break;
			case 106:
			case '#':
				if (activeShow->DownloadState() == ENotDownloaded) {
					HandleCommandL(EPodcastDownloadShow);
				}
				break;
			case EKeyBackspace:
			case EKeyDelete:
					HandleCommandL(EPodcastDeleteShow);
				break;
			default:
				break;
			}
			UpdateToolbar();
		}
	}
		return CPodcastListView::OfferKeyEventL(aKeyEvent, aType);
	}

CPodcastShowsView::~CPodcastShowsView()
	{
	iPodcastModel.ShowEngine().RemoveObserver(this);
	iPodcastModel.FeedEngine().RemoveObserver(this);
	
    if(iStylusPopupMenu)
        delete iStylusPopupMenu, iStylusPopupMenu = NULL;
	}


TUid CPodcastShowsView::Id() const
	{
	return KUidPodcastShowsViewID;
	}

void CPodcastShowsView::DoActivateL(const TVwsViewId& aPrevViewId,
		TUid aCustomMessageId, const TDesC8& aCustomMessage)
	{
	DP("CPodcastShowsView::DoActivateL BEGIN");
	CPodcastListView::DoActivateL(aPrevViewId, aCustomMessageId, aCustomMessage);
	
	iPreviousView = TVwsViewId(KUidPodcast, KUidPodcastFeedViewID);
	
	UpdateViewTitleL();
	UpdateFeedUpdateStateL();
	UpdateToolbar();
	DP("CPodcastShowsView::DoActivateL END");
	}

void CPodcastShowsView::DoDeactivate()
	{
	CPodcastListView::DoDeactivate();
	}

// Engine callback when new shows are available
void CPodcastShowsView::ShowListUpdatedL()
	{
	UpdateListboxItemsL();
	}

void CPodcastShowsView::ShowDownloadUpdatedL(TInt aBytesOfCurrentDownload, TInt /*aBytesTotal*/)
	{
	if (!iListContainer->IsVisible())
		{
		return;
		}
	
	CShowInfo *info = iPodcastModel.ShowEngine().ShowDownloading();
	if (info) 
		{
		UpdateShowItemL(info->Uid(), aBytesOfCurrentDownload);
		}		
	}

void CPodcastShowsView::ShowDownloadFinishedL(TUint aShowUid, TInt aError)
	{
	iProgressAdded = EFalse;
	CShowInfo *info = NULL;
	RShowInfoArray &fItems = iPodcastModel.ActiveShowList();
	
	for (TInt i=0;i<fItems.Count();i++)
		{
		if (fItems[i]->Uid() == aShowUid)
			{
			info = fItems[i];
			}
		}
	
	if (info == NULL) {
		return;
	}
	
	switch(aError)
		{
		case KErrCouldNotConnect:
			{
			TBuf<KMaxMessageLength> message;
			iEikonEnv->ReadResourceL(message, R_PODCAST_CONNECTION_ERROR);
			ShowErrorMessageL(message);
			}
			break;
		default:
			break;
		}
	UpdateListboxItemsL();
	}


void CPodcastShowsView::FeedDownloadStartedL(TFeedState /*aState*/, TUint aFeedUid)
	{
	// TODO make use of the fact that we know that the feed download is
	// started instead of checking feed engine states in UpdateFeedUpdateStateL.
	if (iPodcastModel.ActiveFeedInfo() != NULL
			&& iPodcastModel.ActiveFeedInfo()->Uid() == aFeedUid)
		{
		UpdateFeedUpdateStateL();
		UpdateToolbar();
		}	
	}

void CPodcastShowsView::FeedDownloadFinishedL(TFeedState /*aState*/, TUint aFeedUid, TInt /*aError*/)
	{
	DP("CPodcastShowsView::FeedDownloadFinishedL BEGIN");
	if (iListContainer->IsVisible() &&
			iPodcastModel.ActiveFeedInfo() != NULL &&
			iPodcastModel.ActiveFeedInfo()->Uid() == aFeedUid)
		{
		UpdateFeedUpdateStateL();
		UpdateViewTitleL();
		}
	DP("CPodcastShowsView::FeedDownloadFinishedL END");
	}

void CPodcastShowsView::HandleListBoxEventL(CEikListBox* /*aListBox*/,
		TListBoxEvent aEventType)
	{
	switch (aEventType)
		{
		case EEventItemSingleClicked:
		case EEventEnterKeyPressed:		
		case EEventItemActioned:
		case EEventItemDoubleClicked:
			{
			RShowInfoArray &fItems = iPodcastModel.ActiveShowList();
			TInt index = iListContainer->Listbox()->CurrentItemIndex();
			if (index>=0 && index< fItems.Count())
				{
				DP2("Handle event for podcast %S, downloadState is %d", &(fItems[index]->Title()), fItems[index]->DownloadState());
				CShowInfo *showInfo = fItems[index];
				
				switch (showInfo->DownloadState()) {
				case ENotDownloaded:
				case EFailedDownload:
					HandleCommandL(EPodcastDownloadShow);
					break;
				case EDownloading:
				case EQueued:
					AppUi()->ActivateLocalViewL(KUidPodcastQueueViewID,  TUid::Uid(0), KNullDesC8());
					((CPodcastAppUi*)AppUi())->SetActiveTab(KTabIdQueue);
					break;
				case EDownloaded:
					{
					TRAPD(err, iPodcastModel.PlayPausePodcastL(showInfo, ETrue));
					if (err != KErrNone)
						{
						HBufC *error = iEikonEnv->AllocReadResourceLC(R_ERROR_PLAYBACK_FAILED);
						ShowErrorMessageL(*error);
						CleanupStack::PopAndDestroy(error);
						}
					UpdateListboxItemsL();
					}
					break;
				default:
					break;
					}
				}
			}
			break;
		default:
			break;
		}
		UpdateToolbar();
	}

void CPodcastShowsView::GetShowIcons(CShowInfo* aShowInfo, TInt& aIconIndex)
	{
	TBool dlStop = iPodcastModel.SettingsEngine().DownloadSuspended();

	switch (aShowInfo->DownloadState())
		{
		case EDownloaded:
			if (aShowInfo->PlayState() == ENeverPlayed) {
				aIconIndex = EDownloadedNewShowIcon;
			} else {
				aIconIndex = EDownloadedShowIcon;
			}
			break;
		case ENotDownloaded:
			if (aShowInfo->PlayState() == ENeverPlayed) {
				aIconIndex = ENewShowIcon;
			} else {
				aIconIndex = EShowIcon;
			}
			break;
		case EQueued:
			aIconIndex = dlStop ? ESuspendedShowIcon : EQuedShowIcon;
			break;
		case EDownloading:
			aIconIndex = dlStop ? ESuspendedShowIcon : EDownloadingShowIcon;		
			break;
		case EFailedDownload:
			aIconIndex = EFailedShowIcon;
			break;
		}
	
	if (aShowInfo->ShowType() == EVideoPodcast)
		{
		aIconIndex += KVideoIconOffset;
		}
	}
	

void CPodcastShowsView::UpdateFeedUpdateStateL()
	{
	TBool listboxDimmed = EFalse;

	if (iPodcastModel.FeedEngine().ClientState() != EIdle && iPodcastModel.ActiveFeedInfo()
			!= NULL && iPodcastModel.FeedEngine().ActiveClientUid() == iPodcastModel.ActiveFeedInfo()->Uid())
		{
		listboxDimmed = ETrue;
		}

	if ((iListContainer->Listbox()->IsDimmed() && !listboxDimmed) || (!iListContainer->Listbox()->IsDimmed() && listboxDimmed))
		{
		iListContainer->Listbox()->SetDimmed(listboxDimmed);
		}
	UpdateListboxItemsL();
	UpdateToolbar();
	}

void CPodcastShowsView::FormatShowInfoListBoxItemL(CShowInfo& aShowInfo, TInt aSizeDownloaded)
	{
	TBuf<32> infoSize;
	TInt iconIndex = 0;	
	TBuf<KMaxShortDateFormatSpec*2> showDate;
	GetShowIcons(&aShowInfo, iconIndex);	
	
	if(aSizeDownloaded > 0)
		{
		if (aShowInfo.ShowSize() > 0)
			{
				infoSize.Format(KSizeDownloadingOf(), ((float) aSizeDownloaded / (float) KSizeMb),
						((float)aShowInfo.ShowSize() / (float)KSizeMb));
			}
		else
			{
			infoSize.Format(KShowsSizeFormatS60(), (float)aSizeDownloaded / (float)KSizeMb);
			}
		}
	else if (aShowInfo.ShowSize() > 0)
		{
		infoSize.Format(KShowsSizeFormatS60(), (float)aShowInfo.ShowSize() / (float)KSizeMb);
		} 
	else {
		infoSize = KNullDesC();	
	}

	if (aShowInfo.PubDate().Int64() == 0)
		{
		showDate = KNullDesC();
		}
	else
		{
		aShowInfo.PubDate().FormatL(showDate, KDateFormatShort());
		}

	if(aShowInfo.LastError() != KErrNone)
		{
		GetShowErrorText(infoSize, aShowInfo.LastError());
		showDate.Zero();
		}
	
	if (infoSize.Length() > 0 && showDate.Length() > 0)
		{
		infoSize.Insert(0,_L(", "));
		}
		
	iListboxFormatbuffer.Format(KShowFormat(), iconIndex, &aShowInfo.Title(), &showDate, &infoSize);
	}

void CPodcastShowsView::GetShowErrorText(TDes &aErrorMessage, TInt aErrorCode)
	{
	TRAP_IGNORE(((CPodcastAppUi*)AppUi())->GetErrorTextL(aErrorMessage,aErrorCode));
	}

void CPodcastShowsView::UpdateShowItemDataL(CShowInfo* aShowInfo,TInt aIndex, TInt aSizeDownloaded)
{
	FormatShowInfoListBoxItemL(*aShowInfo, aSizeDownloaded);
	iItemArray->Delete(aIndex);
	if(aIndex>= iItemArray->MdcaCount())
		{
		iItemArray->AppendL(iListboxFormatbuffer);
		}
	else
		{
		iItemArray->InsertL(aIndex, iListboxFormatbuffer);
		}
}

void CPodcastShowsView::UpdateShowItemL(TUint aUid, TInt aSizeDownloaded)
{
	RShowInfoArray& array = iPodcastModel.ActiveShowList();
	
	for (int i=0;i<array.Count();i++) {
		if (array[i]->Uid() == aUid) {
			UpdateShowItemDataL(array[i], i, aSizeDownloaded);
			if (iListContainer->Listbox()->TopItemIndex() <= i &&
				iListContainer->Listbox()->BottomItemIndex() >= i) {
					iListContainer->Listbox()->DrawItem(i);
			}
		}
	}
}

void CPodcastShowsView::UpdateListboxItemsL()
	{
	if (iListContainer->IsVisible())
		{
		TListItemProperties itemProps;
		TInt len = 0;

		iPodcastModel.GetShowsByFeedL(iPodcastModel.ActiveFeedInfo()->Uid());

		RShowInfoArray &fItems = iPodcastModel.ActiveShowList();
		len = fItems.Count();

		if (iListContainer->Listbox() != NULL)
			{
			TBool allUidsMatch = EFalse;

			if (len == iListContainer->Listbox()->Model()->NumberOfItems())
				{
				allUidsMatch = ETrue;
				TUint itemId = 0;
				for (TInt loop = 0; loop< len; loop++)
					{
					itemId = iItemIdArray[loop];
					if (fItems[loop]->Uid() != itemId)
						{
						allUidsMatch = EFalse;
						break;
						}
					}
				}

			if (allUidsMatch && len > 0)
				{
				for (TInt loop = 0; loop< len; loop++)
					{
					UpdateShowItemDataL(fItems[loop], loop);
					}
				iListContainer->Listbox()->DrawNow();
				}
			else
				{
				iListContainer->Listbox()->ItemDrawer()->ClearAllPropertiesL();
				iListContainer->Listbox()->Reset();
				iItemIdArray.Reset();
				iItemArray->Reset();

				if (len > 0)
					{
					for (TInt i=0; i<len; i++)
						{
						CShowInfo *si = fItems[i];
						FormatShowInfoListBoxItemL(*si);
						iItemIdArray.Append(si->Uid());						
						iItemArray->AppendL(iListboxFormatbuffer);
						}
					}
				else
					{
					iItemArray->Reset();
					iItemIdArray.Reset();
					
					itemProps.SetDimmed(ETrue);
					itemProps.SetHiddenSelection(ETrue);
					}
				iListContainer->Listbox()->HandleItemAdditionL();
				}				
			}
		}
	}

/** 
 * Command handling function intended for overriding by sub classes. 
 * Default implementation is empty.  
 * @param aCommand ID of the command to respond to. 
 */
void CPodcastShowsView::HandleCommandL(TInt aCommand)
	{
	switch (aCommand)
		{
		case EPodcastMarkAsPlayed:
			HandleSetShowPlayedL(ETrue);
			break;
		case EPodcastMarkAsUnplayed:
			HandleSetShowPlayedL(EFalse);
			break;
		case EPodcastMarkAllPlayed:
			iPodcastModel.MarkSelectionPlayedL();
			UpdateListboxItemsL();
			break;
		case EPodcastDeleteShow:
			HandleDeleteShowL();
			break;
		case EPodcastDownloadShow:
			{
			TInt index = iListContainer->Listbox()->CurrentItemIndex();
			
			if (index >= 0 && index < iPodcastModel.ActiveShowList().Count())
				{
				iPodcastModel.ShowEngine().AddDownloadL(*iPodcastModel.ActiveShowList()[index]);
				UpdateShowItemL(iPodcastModel.ActiveShowList()[index]->Uid(),-1);
				}
			}
			break;
		case EPodcastUpdateFeed:
			{

			if (iPodcastModel.ActiveFeedInfo()->Url().Length()>0)
				{
				TRAPD(error, iPodcastModel.FeedEngine().UpdateFeedL(iPodcastModel.ActiveFeedInfo()->Uid()));

				if (error != KErrNone)
					{
					HBufC* str =
							iEikonEnv->AllocReadResourceLC(R_PODCAST_FEEDS_UPDATE_ERROR);
					User::InfoPrint(*str);
					CleanupStack::PopAndDestroy(str);
					}
				}
			}
			break;
		case EPodcastCancelUpdateAllFeeds:
			iPodcastModel.FeedEngine().CancelUpdateAllFeeds();
			break;
		case EPodcastShowInfo:
			{
			DisplayShowInfoDialogL();
			}break;
		default:
			CPodcastListView::HandleCommandL(aCommand);
			break;
		}
	iListContainer->SetLongTapDetectedL(EFalse);

	UpdateToolbar();
	}
	
void CPodcastShowsView::DynInitMenuPaneL(TInt aResourceId,CEikMenuPane* aMenuPane)
{
	if(aResourceId == R_PODCAST_SHOWSVIEW_MENU)
		{
		TBool updatingState = iPodcastModel.FeedEngine().ClientState() != EIdle && iPodcastModel.FeedEngine().ActiveClientUid() == iPodcastModel.ActiveFeedInfo()->Uid();
		aMenuPane->SetItemDimmed(EPodcastMarkAllPlayed, updatingState);
		}
}
	
void CPodcastShowsView::DisplayShowInfoDialogL()
	{
	TInt index = iListContainer->Listbox()->CurrentItemIndex();
	if (index >= 0 && index < iPodcastModel.ActiveShowList().Count())
		{
		CShowInfo* info = iPodcastModel.ActiveShowList()[index];

		HBufC *title = info->Title().AllocL();
		HBufC *description = info->Description().AllocL();
		
		CAknMessageQueryDialog* note = new ( ELeave ) CAknMessageQueryDialog( description, title );
							
		note->PrepareLC( R_SHOW_INFO_NOTE ); // Adds to CleanupStack
		note->RunLD();
		}
	}

void CPodcastShowsView::UpdateToolbar(TBool aVisible)
{
	CAknToolbar* toolbar = Toolbar();

	if (toolbar) {
		if (iListContainer->IsVisible()) {
			toolbar->SetToolbarVisibility(aVisible);
		}
	
		TBool updatingState = iPodcastModel.FeedEngine().ClientState() != EIdle && iPodcastModel.ActiveFeedInfo() && 
				iPodcastModel.FeedEngine().ActiveClientUid() == iPodcastModel.ActiveFeedInfo()->Uid();
	
		toolbar->HideItem(EPodcastUpdateFeed, updatingState, ETrue ); 
		toolbar->HideItem(EPodcastCancelUpdateAllFeeds, !updatingState, ETrue );
		// there seems to be drawing bugs in the toolbar if there is only
		// one or two buttons defined in the resource, so we have download
		// there but always hidden
		toolbar->HideItem(EPodcastDownloadShow, ETrue, ETrue );
	}
}

void CPodcastShowsView::HandleLongTapEventL( const TPoint& aPenEventLocation, const TPoint& /* aPenEventScreenLocation */)
{
	DP("CPodcastShowsView::HandleLongTapEventL BEGIN");

	iListContainer->SetLongTapDetectedL(ETrue);

	const TInt KListboxDefaultHeight = 19; // for some reason it returns 19 for an empty listbox in S^1
	TInt lbHeight = iListContainer->Listbox()->CalcHeightBasedOnNumOfItems(
			iListContainer->Listbox()->Model()->NumberOfItems()) - KListboxDefaultHeight;

    if(iStylusPopupMenu && aPenEventLocation.iY < lbHeight)
    {
		TInt index = iListContainer->Listbox()->CurrentItemIndex();
		if (index >= 0 && index < iPodcastModel.ActiveShowList().Count())
			{
			CShowInfo *info = iPodcastModel.ActiveShowList()[index];
			TBool hideDownloadShowCmd = info->DownloadState() != ENotDownloaded;
			TBool hideDeleteShowCmd = info->DownloadState() != EDownloaded;
			TBool hideMarkOld = info->PlayState() == EPlayed;
			
			iStylusPopupMenu->SetItemDimmed(EPodcastMarkAsPlayed, hideMarkOld);
			iStylusPopupMenu->SetItemDimmed(EPodcastMarkAsUnplayed, !hideMarkOld);
						
			iStylusPopupMenu->SetItemDimmed(EPodcastDownloadShow, hideDownloadShowCmd);
			iStylusPopupMenu->SetItemDimmed(EPodcastDeleteShow, hideDeleteShowCmd);
			}

		iStylusPopupMenu->ShowMenu();
		iStylusPopupMenu->SetPosition(aPenEventLocation);
    }
	DP("CPodcastShowsView::HandleLongTapEventL END");
}

void CPodcastShowsView::HandleSetShowPlayedL(TBool aPlayed)
	{

	TInt index = iListContainer->Listbox()->CurrentItemIndex();
				
	if (index >= 0 && index < iPodcastModel.ActiveShowList().Count())
		{
		CShowInfo *info = iPodcastModel.ActiveShowList()[index];
		info->SetPlayState(aPlayed ? EPlayed : ENeverPlayed);
		iPodcastModel.ShowEngine().UpdateShowL(*info);
		UpdateShowItemDataL(iPodcastModel.ActiveShowList()[index], index, 0);
		iListContainer->Listbox()->DrawItem(index);					
		}
	}

void CPodcastShowsView::HandleDeleteShowL()
	{
	TInt index = iListContainer->Listbox()->CurrentItemIndex();

	if (index >= 0 && index < iPodcastModel.ActiveShowList().Count())
		{
		CShowInfo *info = iPodcastModel.ActiveShowList()[index];
		TBuf<KMaxMessageLength> msg;
		TBuf<KMaxMessageLength> templ;
		iEikonEnv->ReadResourceL(templ, R_PODCAST_DELETE_SHOW_PROMPT);
		msg.Format(templ, &(info->Title()));
		if (ShowQueryMessageL(msg))
			{
			iPodcastModel.ShowEngine().DeleteShowL(iPodcastModel.ActiveShowList()[index]->Uid());
			
			// and mark as played, and not downloaded
			
			info->SetDownloadState(ENotDownloaded);
			info->SetPlayState(EPlayed);
			iPodcastModel.ShowEngine().UpdateShowL(*info);
			
			UpdateShowItemDataL(iPodcastModel.ActiveShowList()[index], index, 0);
			iListContainer->Listbox()->DrawItem(index);					
			}
		}
	}

void CPodcastShowsView::DownloadQueueUpdatedL(TInt aDownloadingShows, TInt aQueuedShows)
	{
	((CPodcastAppUi*)AppUi())->UpdateQueueTabL(aDownloadingShows+aQueuedShows);
	UpdateListboxItemsL();
	}

void CPodcastShowsView::FeedUpdateAllCompleteL(TFeedState /*aState*/)
	{
	UpdateListboxItemsL();
	UpdateToolbar();
	}

void CPodcastShowsView::UpdateViewTitleL()
	{
	DP("CPodcastShowsView::UpdateViewTitleL BEGIN");
	 CAknTitlePane* titlePane = static_cast<CAknTitlePane*>
		      ( StatusPane()->ControlL( TUid::Uid( EEikStatusPaneUidTitle ) ) );
		 
		TBool updatingState = iPodcastModel.FeedEngine().ClientState() != EIdle && 
				iPodcastModel.FeedEngine().ActiveClientUid() == iPodcastModel.ActiveFeedInfo()->Uid();

		if (updatingState) {
			SetEmptyTextL(R_PODCAST_EMPTY_LIST_UPDATING);		
		} else {
			SetEmptyTextL(R_PODCAST_EMPTY_LIST);
		}
		
		if(iPodcastModel.ActiveFeedInfo())
			{
			if (iPodcastModel.ActiveFeedInfo()->Title() != KNullDesC)
				{
				titlePane->SetTextL( iPodcastModel.ActiveFeedInfo()->Title(), ETrue );
				}
			}
		else
			{
			titlePane->SetTextToDefaultL();
			}
		
	DP("CPodcastShowsView::UpdateViewTitleL END");
	}
