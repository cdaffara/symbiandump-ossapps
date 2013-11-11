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

#include "PodcastFeedView.h"
#include "PodcastAppUi.h"
#include "ShowEngine.h"
#include "SettingsEngine.h"
#include "PodcastApp.h"
#include "PodcastUtils.h"
#include "PodcastFeedViewUpdater.h"
#include "Podcast.hrh"
#include <caknfileselectiondialog.h> 
#include <podcast.rsg>
#include <podcast.mbg>
#include <gulicon.h>
#include <aknquerydialog.h>
#include <BAUTILS.H> 
#include <pathinfo.h> 
#include <akncommondialogsdynmem.h> 
#include "Podcatcher.pan"

const TInt KMaxFeedNameLength = 100;
const TInt KMaxUnplayedFeedsLength =64;
const TInt KADayInHours = 24;
#define KMaxMessageLength 200
#define KMaxTitleLength 100

_LIT(KFeedFormat, "%d\t%S\t%S%S");
enum 
{
 EFeedIcon
};

CPodcastFeedView* CPodcastFeedView::NewL(CPodcastModel& aPodcastModel)
    {
    CPodcastFeedView* self = CPodcastFeedView::NewLC(aPodcastModel);
    CleanupStack::Pop( self );
    return self;
    }

CPodcastFeedView* CPodcastFeedView::NewLC(CPodcastModel& aPodcastModel)
    {
    CPodcastFeedView* self = new ( ELeave ) CPodcastFeedView(aPodcastModel);
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

CPodcastFeedView::CPodcastFeedView(CPodcastModel& aPodcastModel):iPodcastModel(aPodcastModel)
	{
	iFirstActivateAfterLaunch = ETrue;
	}
#define KAsterisk iEikonEnv->EikAppUi()->Application()->BitmapStoreName()
void CPodcastFeedView::ConstructL()
	{
	DP("CPodcastFeedView::ConstructL BEGIN");
	//_LIT(KAsterisk, "*");
	BaseConstructL(R_PODCAST_FEEDVIEW);
	iNeverUpdated = iEikonEnv->AllocReadResourceL(R_PODCAST_FEEDS_NEVER_UPDATED);
	iFeedsFormat = iEikonEnv->AllocReadResourceL(R_PODCAST_FEEDS_STATUS_FORMAT);
	CPodcastListView::ConstructL();
	iPodcastModel.FeedEngine().AddObserver(this);
	CArrayPtr< CGulIcon >* icons = new(ELeave) CArrayPtrFlat< CGulIcon >(1);
	CleanupStack::PushL( icons );
	
	CFbsBitmap* bitmap = NULL;
	CFbsBitmap* mask = NULL;
	// Load the bitmap for empty icon	
	TFileName fname = KAsterisk;
	TParsePtr parser(fname);

	SetEmptyTextL(R_PODCAST_NO_FEEDS);
	
	// Load svg.-image and mask with a single call
		AknIconUtils::CreateIconL(bitmap,
		                          mask,
		                          iEikonEnv->EikAppUi()->Application()->BitmapStoreName(),
		                          EMbmPodcastFeed,
		                          EMbmPodcastFeed_mask);
	    
	/*bitmap = iEikonEnv->CreateBitmapL(KAsterisk,EMbmPodcastFeed_40x40);
	 * */
	CleanupStack::PushL( bitmap );		
	// Load the mask for feed icon	
	//mask = iEikonEnv->CreateBitmapL(KAsterisk,EMbmPodcastFeed_40x40m );	
	CleanupStack::PushL( mask );
	// Append the feed icon to icon array
	icons->AppendL( CGulIcon::NewL( bitmap, mask ) );
	CleanupStack::Pop(2); // bitmap, mask
	iListContainer->Listbox()->ItemDrawer()->FormattedCellData()->SetIconArrayL( icons );
	CleanupStack::Pop(icons); // icons

	iListContainer->Listbox()->SetListBoxObserver(this);
	
    iStylusPopupMenu = CAknStylusPopUpMenu::NewL( this , TPoint(0,0));
    TResourceReader reader;
    iCoeEnv->CreateResourceReaderLC(reader,R_FEEDVIEW_POPUP_MENU);
    iStylusPopupMenu->ConstructFromResourceL(reader);
    CleanupStack::PopAndDestroy();
    
    iUpdater = CPodcastFeedViewUpdater::NewL(*this);
	DP("CPodcastFeedView::ConstructL END");
	}
    
CPodcastFeedView::~CPodcastFeedView()
    {
	iPodcastModel.FeedEngine().RemoveObserver(this);
	delete iFeedsFormat;
	delete iNeverUpdated;
	delete iStylusPopupMenu;
	delete iUpdater;
	iFeedIdForIconArray.Close();
    }

void CPodcastFeedView::UpdateItemL(TInt aIndex)
	{
	__ASSERT_DEBUG(iListContainer->IsVisible(), Panic(EPodcatcherPanicFeedView));
	__ASSERT_ALWAYS(iItemIdArray.Count() > aIndex, Panic(EPodcatcherPanicFeedView));

	const RFeedInfoArray& sortedItems = iPodcastModel.FeedEngine().GetSortedFeeds();
	__ASSERT_ALWAYS(sortedItems.Count() > aIndex, Panic(EPodcatcherPanicFeedView));

	// Update UID of for the feed at aIndex
	iItemIdArray[aIndex] = sortedItems[aIndex]->Uid();
	
	// Prepare data to update the listbox item with
	FormatFeedInfoListBoxItemL(*sortedItems[aIndex], EFalse);
	
	// If nothing has changed, we are done here
	if (iListboxFormatbuffer == iItemArray->MdcaPoint(aIndex))
		{
		return;
		}

	// Something has changed, update the listbox item
	TListItemProperties itemProps;			
	itemProps.SetDimmed(EFalse);	
	iItemArray->Delete(aIndex);	
	iItemArray->InsertL(aIndex, iListboxFormatbuffer);
	iListContainer->Listbox()->ItemDrawer()->SetPropertiesL(aIndex, itemProps);
	// If item is visible, redraw it
	if (iListContainer->Listbox()->TopItemIndex() <= aIndex
			&& iListContainer->Listbox()->BottomItemIndex() >= aIndex)
		{
		iListContainer->Listbox()->DrawItem(aIndex);
		}
	}

TUid CPodcastFeedView::Id() const
	{
	return KUidPodcastFeedViewID;
	}
		
void CPodcastFeedView::DoActivateL(const TVwsViewId& aPrevViewId,
	                                  TUid aCustomMessageId,
	                                  const TDesC8& aCustomMessage)
	{
	CPodcastListView::DoActivateL(aPrevViewId, aCustomMessageId, aCustomMessage);
	
	if (aPrevViewId.iViewUid == KUidPodcastShowsViewID)
		{
		// back key from shows view
		iViewingShows = EFalse;
		}
	
		UpdateListboxItemsL();		
		UpdateToolbar();

	if (iFirstActivateAfterLaunch)
		{
		iFirstActivateAfterLaunch = EFalse;
		}
	}

void CPodcastFeedView::DoDeactivate()
	{
	iUpdater->StopUpdate();
	CPodcastListView::DoDeactivate();
	}


void CPodcastFeedView::HandleListBoxEventL(CEikListBox* /* aListBox */, TListBoxEvent aEventType)
	{
	DP("CPodcastFeedView::HandleListBoxEventL BEGIN");

	switch(aEventType)
		{
	case EEventItemSingleClicked:
	case EEventEnterKeyPressed:
	case EEventItemDoubleClicked:
	case EEventItemActioned:
			{
			const RFeedInfoArray* sortedItems = NULL;
			TInt index = iListContainer->Listbox()->CurrentItemIndex();
			sortedItems = &iPodcastModel.FeedEngine().GetSortedFeeds();

			if(index >= 0 && index < sortedItems->Count())
				{
				iPodcastModel.SetActiveFeedInfo((*sortedItems)[index]);			
				iViewingShows = ETrue;
				AppUi()->ActivateLocalViewL(KUidPodcastShowsViewID,  TUid::Uid(0), KNullDesC8());
				}
			}
		break;
	default:
		break;
		}
	DP("CPodcastFeedView::HandleListBoxEventL END");
	}

void CPodcastFeedView::FeedUpdateAllCompleteL(TFeedState /*aState*/)
	{
	iUpdatingRunning = EFalse;
	UpdateToolbar();
	}

void CPodcastFeedView::FeedDownloadStartedL(TFeedState /*aState*/, TUint aFeedUid)
	{
	// Update status text
	iUpdatingRunning = ETrue;
	UpdateFeedInfoStatusL(aFeedUid, ETrue);
	
	UpdateToolbar();
	}

void CPodcastFeedView::FeedDownloadFinishedL(TFeedState aState,TUint aFeedUid, TInt aError)
	{
	switch(aError)
		{
		case KErrCouldNotConnect:
			{
			if(aState == MFeedEngineObserver::EFeedManualUpdate)
				{
				TBuf<KMaxMessageLength> message;
				iEikonEnv->ReadResourceL(message, R_PODCAST_CONNECTION_ERROR);
				ShowErrorMessageL(message);
				}
			}
			break;
		default: // Do nothing
			break;
		}
	UpdateFeedInfoStatusL(aFeedUid, EFalse);
	}

void CPodcastFeedView::UpdateFeedInfoStatusL(TUint aFeedUid, TBool aIsUpdating)
	{
	const RFeedInfoArray& feeds = iPodcastModel.FeedEngine().GetSortedFeeds();

	// Find the index for the feed i both the feed-array and the listbox 
	TInt feedsIdx = KErrNotFound;
	TInt listboxIdx = KErrNotFound;
	for (TInt i = 0; i < feeds.Count(); i++)
		{
		if (feeds[i]->Uid() == aFeedUid)
			{
			feedsIdx = i;
			break;
			}
		}
	for (TInt j = 0; j < iItemIdArray.Count(); j++)
		{
		if (iItemIdArray[j] == aFeedUid)
			{
			listboxIdx = j;
			break;
			}
		}
		
	if (feedsIdx != KErrNotFound && listboxIdx != KErrNotFound)
		{
		// TODO In the long run we want to move the sorting resposibility from
		// CFeedEngine to CPodcastFeedView.
		// Hackish fix to make sure the listbox is sorted.
		if (listboxIdx != feedsIdx)
			{
			iItemIdArray.Remove(listboxIdx);
			iItemIdArray.InsertL(aFeedUid, feedsIdx);
			iItemArray->Delete(listboxIdx);
			iItemArray->InsertL(feedsIdx, KNullDesC);
			iListContainer->Listbox()->HandleItemAdditionL();
			}
		// Update the listbox info
		UpdateFeedInfoDataL(feeds[feedsIdx], feedsIdx, aIsUpdating);
		//TODO sort the listbox after update
		}
		// Update all visible listbox items that are affected by sorting and update.
		TInt minIdx = Max(Min(feedsIdx, listboxIdx), iListContainer->Listbox()->TopItemIndex());
		TInt maxIdx = Min(Max(feedsIdx, listboxIdx), iListContainer->Listbox()->BottomItemIndex());
		for (TInt k = minIdx; k <= maxIdx; k++)
			{
			iListContainer->Listbox()->DrawItem(k);
			}
	}

void CPodcastFeedView::FormatFeedInfoListBoxItemL(CFeedInfo& aFeedInfo, TBool aIsUpdating)
	{
	TBuf<KMaxShortDateFormatSpec*2> updatedDate;
	TBuf<KMaxUnplayedFeedsLength> unplayedShows;
	TUint unplayedCount = 0;
	TUint showCount = 0;
	TInt iconIndex = EFeedIcon;
	
	if(aIsUpdating)
		{
		iEikonEnv->ReadResourceL(updatedDate, R_PODCAST_FEEDS_IS_UPDATING);
		unplayedShows = KNullDesC();			
		}
	else
		{
		// we will get a leave if there are no shows for this feed, for instance, which is fine
		TRAP_IGNORE(iPodcastModel.FeedEngine().GetStatsByFeedL(aFeedInfo.Uid(), showCount, unplayedCount));	
		
		if (unplayedCount) {
			unplayedShows.Format(*iFeedsFormat, unplayedCount);
		} else {
			unplayedShows.Zero();
		}
		
		if (aFeedInfo.LastUpdated().Int64() == 0) 
			{
			updatedDate.Copy(*iNeverUpdated);					
			unplayedShows.Zero();
			}
		else 
			{
			TTime now;
			TTimeIntervalHours interval;
			now.HomeTime();
			now.HoursFrom(aFeedInfo.LastUpdated(), interval);
			if (interval.Int() < KADayInHours) 
				{
				aFeedInfo.LastUpdated().FormatL(updatedDate, KTimeFormat());
				}
			else 
				{
				aFeedInfo.LastUpdated().FormatL(updatedDate, KDateFormatShort());
				}
			}
		
		if(aFeedInfo.LastError() != KErrNone)
			{
			GetFeedErrorText(unplayedShows, aFeedInfo.LastError());
			updatedDate.Zero();
			}
		}
	CArrayPtr<CGulIcon>* icons = iListContainer->Listbox()->ItemDrawer()->FormattedCellData()->IconArray();
	iconIndex = iFeedIdForIconArray.Find(aFeedInfo.Uid());
	if(iconIndex == KErrNotFound && aFeedInfo.FeedIcon() != NULL && aFeedInfo.ImageFileName().Length() > 0 && 
			aFeedInfo.FeedIcon()->SizeInPixels().iHeight > 0 &&
			aFeedInfo.FeedIcon()->SizeInPixels().iWidth > 0)
		{
		// Hopefully temporary haxx to prevent double delete. I would prefer if
		// this could be solved with a little better design.
		CFbsBitmap* bmpCopy = new (ELeave) CFbsBitmap;
		CleanupStack::PushL(bmpCopy);
		bmpCopy->Duplicate(aFeedInfo.FeedIcon()->Handle());
		icons->AppendL( CGulIcon::NewL(bmpCopy, NULL));
		iFeedIdForIconArray.Append(aFeedInfo.Uid());
		CleanupStack::Pop(bmpCopy);			
		iconIndex = icons->Count()-1;
		}	
	else 
		{
		iconIndex++;
		}	

	if (unplayedShows.Length() > 0 && updatedDate.Length() > 0) {
		unplayedShows.Insert(0,_L(", "));
	}
	
	iListboxFormatbuffer.Format(KFeedFormat(), iconIndex, &(aFeedInfo.Title()), &updatedDate,  &unplayedShows);
	}

void CPodcastFeedView::ImageOperationCompleteL(TInt aError, TUint aHandle, CPodcastModel& /*aPodcastModel*/)
	{
	if (aError == KErrNone) {
		UpdateFeedInfoStatusL(aHandle, EFalse);
		}
	}

void CPodcastFeedView::UpdateFeedInfoDataL(CFeedInfo* aFeedInfo, TInt aIndex, TBool aIsUpdating )
	{			
	TListItemProperties itemProps;			
	itemProps.SetDimmed(aIsUpdating);	
	FormatFeedInfoListBoxItemL(*aFeedInfo, aIsUpdating);
	
	TPtrC compareTo((*iItemArray)[aIndex]);
	
	if (iListboxFormatbuffer.Compare(compareTo) != 0) {
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
	iListContainer->Listbox()->ItemDrawer()->SetPropertiesL(aIndex, itemProps);
	}


void CPodcastFeedView::UpdateListboxItemsL()
	{
	DP("CPodcastFeedView::UpdateListboxItemsL BEGIN");
	// No reason to do any work if it isn't going to show..
	if(!iListContainer->IsVisible())
		{
		return;
		}
	
	TInt nbrItems = iPodcastModel.FeedEngine().GetSortedFeeds().Count();
	if (nbrItems > 0)
		{
		// Ensure that there are as many elements in iItemIdArray as in FeedEngine
		while (iItemIdArray.Count() < nbrItems)
			{
			iItemIdArray.AppendL(0);
			}
		while (iItemIdArray.Count() > nbrItems)
			{
			iItemIdArray.Remove(iItemIdArray.Count() - 1);
			}
		
		// Ensure that there are as many elements in iItemArray as in FeedEngine
		while (iItemArray->Count() < nbrItems)
			{
			iItemArray->AppendL(KNullDesC);
			TListItemProperties itemProps;
			iListContainer->Listbox()->ItemDrawer()->SetPropertiesL(iItemArray->Count() - 1, itemProps);
			}
		while (iItemArray->Count() > nbrItems)
			{
			iItemArray->Delete(iItemArray->Count() - 1);
			}
		iUpdater->StartUpdate(nbrItems);
		}
	else
		{
		// No feeds at all in the list , add dummy list item
		TBuf<KMaxFeedNameLength> itemName;
		iEikonEnv->ReadResourceL(itemName, R_PODCAST_FEEDS_NO_FEEDS);
		iItemArray->Reset();
		iItemIdArray.Reset();
	
		TListItemProperties itemProps;
		itemProps.SetDimmed(ETrue);
		itemProps.SetHiddenSelection(ETrue);								
		iListContainer->Listbox()->ItemDrawer()->SetPropertiesL(0, itemProps);
		}
	iListContainer->Listbox()->HandleItemAdditionL();
	DP("CPodcastFeedView::UpdateListboxItemsL END");
	}

/** 
 * Command handling function intended for overriding by sub classes. 
 * Default implementation is empty.  
 * @param aCommand ID of the command to respond to. 
 */
void CPodcastFeedView::HandleCommandL(TInt aCommand)
	{
	DP("CPodcastFeedView::HandleCommandL BEGIN");

	switch(aCommand)
		{
        case EPodcastHide:
			AppUi()->HandleCommandL(EEikCmdExit);
			break;
		case EPodcastAddFeed:
			HandleAddFeedL();
			break;
		case EPodcastImportFeeds:
			HandleImportFeedsL();
			break;
		case EPodcastExportFeeds:
			HandleExportFeedsL();
			break;
		case EPodcastEditFeed:
			HandleEditFeedL();
			break;
		case EPodcastDeleteFeedHardware:
		case EPodcastDeleteFeed:
			HandleRemoveFeedL();
			break;
		case EPodcastUpdateAllFeeds:
			{
			iPodcastModel.FeedEngine().UpdateAllFeedsL();
			UpdateToolbar();
			}break;
		case EPodcastUpdateFeed:
			{
			HandleUpdateFeedL();
			}break;
		case EPodcastCancelUpdateAllFeeds:
			{
			if(iUpdatingRunning)
				{
				iPodcastModel.FeedEngine().CancelUpdateAllFeeds();
				}
			}break;
		case EAknSoftkeyExit:
			{
			RShowInfoArray dlQueue;
			iPodcastModel.ShowEngine().GetShowsDownloadingL(dlQueue);
			TUint queueCount = dlQueue.Count();
			dlQueue.ResetAndDestroy();
			dlQueue.Close();
			
			if (queueCount > 0 && !iPodcastModel.SettingsEngine().DownloadSuspended())
				{
				TBuf<KMaxMessageLength> message;
				iEikonEnv->ReadResourceL(message, R_EXIT_SHOWS_DOWNLOADING);
				if(ShowQueryMessageL(message))
					{
					// pass it on to AppUi, which will exit for us
					CPodcastListView::HandleCommandL(aCommand);
					}
				} 
			else
				{
					// nothing in queue, or downloading suspended
					CPodcastListView::HandleCommandL(aCommand);
				}
			}
			break;
		default:
			CPodcastListView::HandleCommandL(aCommand);
			break;
		}
	
	iListContainer->SetLongTapDetectedL(EFalse); // in case we got here by long tapping
	UpdateToolbar();
	DP("CPodcastFeedView::HandleCommandL END");
	}

void CPodcastFeedView::UpdateToolbar(TBool aVisible)
{
	DP("CPodcastFeedView::UpdateToolbar BEGIN");
	CAknToolbar* toolbar = Toolbar();
	
	if (toolbar)
		{
		if (iListContainer->IsVisible()) {
			toolbar->SetToolbarVisibility(aVisible);
		}
		toolbar->HideItem(EPodcastUpdateAllFeeds, iUpdatingRunning, ETrue);
		toolbar->HideItem(EPodcastCancelUpdateAllFeeds, !iUpdatingRunning, ETrue );
		toolbar->SetItemDimmed(EPodcastAddFeed, iUpdatingRunning, ETrue );
		toolbar->SetItemDimmed(EPodcastSettings, iUpdatingRunning, ETrue );
		}
	DP("CPodcastFeedView::UpdateToolbar END");
}

void CPodcastFeedView::HandleAddFeedL()
	{
	TBuf<KFeedUrlLength> url;
	url.Copy(_L("http://"));
	CAknTextQueryDialog * dlg =CAknTextQueryDialog::NewL(url);
	dlg->PrepareLC(R_PODCAST_ADD_FEED_DLG);
	HBufC* prompt = iEikonEnv->AllocReadResourceLC(R_PODCAST_ADDFEED_PROMPT);
	dlg->SetPromptL(*prompt);
	CleanupStack::PopAndDestroy(prompt);
	
	if(dlg->RunLD())
		{
		
		// if no :// we do a search
		if (url.Find(_L("://")) == KErrNotFound)
			{
			HBufC *waitText = iEikonEnv->AllocReadResourceLC(R_SEARCHING);
			ShowWaitDialogL(*waitText);
			CleanupStack::PopAndDestroy(waitText);	
	
			iOpmlState = EOpmlSearching;
			TRAPD(err, iPodcastModel.FeedEngine().SearchForFeedL(url));
			
			if (err != KErrNone)
				{
				delete iWaitDialog;
				iOpmlState = EOpmlIdle;
				}
			}
		else
			{
			PodcastUtils::FixProtocolsL(url);
	
			CFeedInfo* newFeedInfo = CFeedInfo::NewL();
			CleanupStack::PushL(newFeedInfo);
			newFeedInfo->SetUrlL(url);
			newFeedInfo->SetTitleL(newFeedInfo->Url());
			
			TBool added = iPodcastModel.FeedEngine().AddFeedL(*newFeedInfo);
			
			if (added)
				{
				UpdateListboxItemsL();
				
				// ask if users wants to update it now
				TBuf<KMaxMessageLength> message;
				iEikonEnv->ReadResourceL(message, R_ADD_FEED_SUCCESS);
				if(ShowQueryMessageL(message))
					{
					CFeedInfo *info = iPodcastModel.FeedEngine().GetFeedInfoByUid(newFeedInfo->Uid());
					
					iPodcastModel.SetActiveFeedInfo(info);			
					AppUi()->ActivateLocalViewL(KUidPodcastShowsViewID,  TUid::Uid(0), KNullDesC8());
					iPodcastModel.FeedEngine().UpdateFeedL(newFeedInfo->Uid());
					}
				}
			else
				{
				TBuf<KMaxMessageLength> message;
				iEikonEnv->ReadResourceL(message, R_ADD_FEED_EXISTS);
				ShowErrorMessageL(message);
				}		
			
			CleanupStack::PopAndDestroy(newFeedInfo);
			}
		}
	}

void CPodcastFeedView::HandleEditFeedL()
	{
	TInt index = iListContainer->Listbox()->CurrentItemIndex();
	
	if(index < iItemArray->MdcaCount() && index >= 0)
		{
		CFeedInfo *info = iPodcastModel.FeedEngine().GetSortedFeeds()[index];

		TBuf<KFeedTitleLength> title;
		title.Copy(info->Title());
		TBuf<KFeedUrlLength> url;
		url.Copy(info->Url());
		CAknMultiLineDataQueryDialog  *dlg = CAknMultiLineDataQueryDialog ::NewL(title, url);
	
		if (dlg->ExecuteLD(R_PODCAST_EDIT_FEED_DLG)) 
			{
			
			if(info->Url().Compare(url) != 0)
				{
				TBuf<KMaxMessageLength> dlgMessage;
				iEikonEnv->ReadResourceL(dlgMessage, R_ADD_FEED_REPLACE);

				// Ask the user if it is OK to remove all shows
				if ( ShowQueryMessageL(dlgMessage))
					{
					PodcastUtils::FixProtocolsL(url);
					
					//----- HACK ---- //
					CFeedInfo* temp = CFeedInfo::NewLC();
					temp->SetUrlL(url);
					TBool added = iPodcastModel.FeedEngine().AddFeedL(*temp);
					if (added) {
						// The Feed URL did not exist
						// Remove the temp entry so that the correct entry could be changed
						iPodcastModel.FeedEngine().RemoveFeedL(temp->Uid());	
						
						// we remove the existing feed
						iPodcastModel.FeedEngine().RemoveFeedL(info->Uid());	
						
						CFeedInfo* newFeed = CFeedInfo::NewLC();
						newFeed->SetUrlL(url);
						newFeed->SetTitleL(title);
						
						iPodcastModel.FeedEngine().AddFeedL(*newFeed);
						CleanupStack::PopAndDestroy(newFeed);
						UpdateListboxItemsL();
					} else {
						// the feed existed. Object deleted in AddFeed.	
						TBuf<KMaxMessageLength> dlgMessage;
						iEikonEnv->ReadResourceL(dlgMessage, R_ADD_FEED_EXISTS);
						ShowErrorMessageL(dlgMessage);
					}
					CleanupStack::PopAndDestroy(temp);
				}
			} else { // no url change, maybe title?
				// Update the title
				if (info->Title().Compare(title) != 0)
				{
					info->SetTitleL(title);
					info->SetCustomTitle();	
					iPodcastModel.FeedEngine().UpdateFeedInfoL(info);
					UpdateListboxItemsL();
				}
			}
		}
		}
	}

void CPodcastFeedView::HandleRemoveFeedL()
	{
	if(iListContainer->Listbox() != NULL)
		{
		TInt index = iListContainer->Listbox()->CurrentItemIndex();
	
		if(index < iItemArray->MdcaCount() && index >= 0)
			{
			CFeedInfo *info = iPodcastModel.FeedEngine().GetFeedInfoByUid(iItemIdArray[index]);
			TBuf<KMaxMessageLength> templ;
			TBuf<KMaxMessageLength> message;
			iEikonEnv->ReadResourceL(templ, R_PODCAST_REMOVE_FEED_PROMPT);
			message.Format(templ, &info->Title());					
			if(ShowQueryMessageL(message))
				{
				iPodcastModel.FeedEngine().RemoveFeedL(iItemIdArray[index]);
				iItemArray->Delete(index);
				iItemIdArray.Remove(index);
				iListContainer->Listbox()->HandleItemRemovalL();
				iListContainer->Listbox()->DrawNow();
				}					
			}
		UpdateListboxItemsL();
		}
	}

void CPodcastFeedView::HandleUpdateFeedL()
	{
	TInt index = iListContainer->Listbox()->CurrentItemIndex();
	
	if(index < iItemArray->MdcaCount() && index >= 0)
		{
		CFeedInfo *info = iPodcastModel.FeedEngine().GetSortedFeeds()[index];
		iPodcastModel.FeedEngine().UpdateFeedL(info->Uid());
		}
	}

void CPodcastFeedView::HandleImportFeedsL()
	{
	TFileName fileName;
	fileName.Zero();
	TFileName startFolder;
	startFolder.Zero();
	TInt types = AknCommonDialogsDynMem::EMemoryTypePhone | AknCommonDialogsDynMem::EMemoryTypeMMC |AknCommonDialogsDynMem::EMemoryTypeInternalMassStorage| AknCommonDialogsDynMem::EMemoryTypeRemote;
	
	HBufC *title = iCoeEnv->AllocReadResourceLC(R_PODCAST_SELECT_OPML);
	if (AknCommonDialogsDynMem::RunSelectDlgLD (types, fileName,
			startFolder, NULL, NULL, *title))
		{
		
		if(fileName.Length()>0)
			{
			HBufC *waitText = iEikonEnv->AllocReadResourceLC(R_IMPORTING);
			iOpmlState = EOpmlImporting;
			ShowWaitDialogL(*waitText);
			CleanupStack::PopAndDestroy(waitText);	

			TRAPD(err, iPodcastModel.FeedEngine().ImportFeedsL(fileName));
								
			if (err != KErrNone) {
				TBuf<KMaxMessageLength> message;
				iEikonEnv->ReadResourceL(message, R_IMPORT_FEED_FAILURE);
				ShowErrorMessageL(message);
				}
			}

		}
	CleanupStack::PopAndDestroy(title);
	}

void CPodcastFeedView::HandleExportFeedsL()
	{
	TFileName fileName;
	fileName.Copy(_L("feeds.opml"));
	TFileName startFolder;
	startFolder.Zero();
	TInt types = AknCommonDialogsDynMem::EMemoryTypePhone | AknCommonDialogsDynMem::EMemoryTypeMMC |AknCommonDialogsDynMem::EMemoryTypeInternalMassStorage| AknCommonDialogsDynMem::EMemoryTypeRemote;
	
	HBufC *title = iCoeEnv->AllocReadResourceLC(R_PODCAST_SELECT_FOLDER);
	if (AknCommonDialogsDynMem::RunSaveDlgLD (types, fileName,
			startFolder, NULL, NULL, *title))
		{
			TFileName temp;
			TRAPD(err, iPodcastModel.FeedEngine().ExportFeedsL(temp));						
			BaflUtils::CopyFile(iEikonEnv->FsSession(), temp, fileName);
			BaflUtils::DeleteFile(iEikonEnv->FsSession(),temp);	
			if (err == KErrNone) 
				{
				UpdateListboxItemsL();
				TInt numFeeds = iPodcastModel.FeedEngine().GetSortedFeeds().Count();
								
				TBuf<KMaxMessageLength> message;
				TBuf<KMaxMessageLength> templ;
				iEikonEnv->ReadResourceL(templ, R_EXPORT_FEED_SUCCESS);
				message.Format(templ, numFeeds);
				ShowOkMessageL(message);
				} 
			else 
				{
				TBuf<KMaxMessageLength> message;
				iEikonEnv->ReadResourceL(message, R_EXPORT_FEED_FAILURE);
				ShowErrorMessageL(message);
				}
		}
	CleanupStack::PopAndDestroy(title);
	}

void CPodcastFeedView::CheckResumeDownloadL()
	{
	// if there are shows queued for downloading, ask if we should resume now
	RShowInfoArray showsDownloading;
	iPodcastModel.ShowEngine().GetShowsDownloadingL(showsDownloading);

	if (showsDownloading.Count() > 0)
		{
		TBuf<KMaxMessageLength> msg;
		iEikonEnv->ReadResourceL(msg, R_PODCAST_ENABLE_DOWNLOADS_PROMPT);
	
		if (ShowQueryMessageL(msg))
			{
			// need to suspend downloads before ResumeDownloadL will work :)
			iPodcastModel.SettingsEngine().SetDownloadSuspended(ETrue);
			// resume downloading if user says yes
			iPodcastModel.ShowEngine().ResumeDownloadsL();
			}
		else
			{
			// we disable downloading if user says no
			iPodcastModel.SettingsEngine().SetDownloadSuspended(ETrue);
			}
		}
	
	// if no shows in queue, we keep whichever state suspend is in
	showsDownloading.ResetAndDestroy();
	}

void CPodcastFeedView::OpmlParsingComplete(TInt aError, TUint aNumFeedsImported)
	{
	TRAP_IGNORE(OpmlParsingCompleteL(aError, aNumFeedsImported));
	}

void CPodcastFeedView::OpmlParsingCompleteL(TInt aError, TUint aNumFeedsImported)
	{
	DP("CPodcastFeedView::OpmlParsingComplete BEGIN");
	
	switch (aError)
		{
		case KErrCouldNotConnect:
			{
			TBuf<KMaxMessageLength> message;
			iEikonEnv->ReadResourceL(message, R_PODCAST_CONNECTION_ERROR);
			delete iWaitDialog;
			iOpmlState = EOpmlIdle;
			ShowErrorMessageL(message);
			}
			break;
		case KErrNone: 
		default:			// we don't do more error handling here, just show 0 imported feeds
		switch (iOpmlState)
			{
			case EOpmlIdle:
				break;
			case EOpmlImporting:
				{
				UpdateListboxItemsL();
				delete iWaitDialog;
				iOpmlState = EOpmlIdle;
					
				TBuf<KMaxMessageLength> message;
				TBuf<KMaxMessageLength> templ;
				iEikonEnv->ReadResourceL(templ, R_IMPORT_FEED_SUCCESS);
				message.Format(templ, aNumFeedsImported);
				
				if(ShowQueryMessageL(message))
					{
					HandleCommandL(EPodcastUpdateAllFeeds);
					}
				}
				break;
			case EOpmlSearching:
				delete iWaitDialog;
				iWaitDialog = NULL;
				if (iPodcastModel.FeedEngine().GetSearchResults().Count() == 0)
					{
					TBuf<KMaxMessageLength> message;
					iEikonEnv->ReadResourceL(message, R_SEARCH_NORESULTS);
					ShowErrorMessageL(message);
					}
				else
					{
					AppUi()->ActivateLocalViewL(KUidPodcastSearchViewID,  TUid::Uid(0), KNullDesC8());
					}
				iOpmlState = EOpmlIdle;
				break;
			default:
				break;
			}
		}
	
	DP("CPodcastFeedView::OpmlParsingComplete END");
	}

void CPodcastFeedView::DialogDismissedL(TInt /*aButtonId*/)
	{
	iPodcastModel.FeedEngine().CancelUpdateAllFeeds();
	}

void CPodcastFeedView::GetFeedErrorText(TDes &aErrorMessage, TInt aErrorCode)
	{
	TRAP_IGNORE(((CPodcastAppUi*)AppUi())->GetErrorTextL(aErrorMessage,aErrorCode));
	}

void CPodcastFeedView::HandleLongTapEventL( const TPoint& aPenEventLocation, const TPoint& /* aPenEventScreenLocation */)
{
	DP("CPodcastListView::HandleLongTapEventL BEGIN");

	iListContainer->SetLongTapDetectedL(ETrue);

	const TInt KListboxDefaultHeight = 19; // for some reason it returns 19 for an empty listbox in S^1
	TInt lbHeight = iListContainer->Listbox()->CalcHeightBasedOnNumOfItems(
			iListContainer->Listbox()->Model()->NumberOfItems()) - KListboxDefaultHeight;

    if(iStylusPopupMenu && aPenEventLocation.iY < lbHeight)
    {
		iStylusPopupMenu->ShowMenu();
		iStylusPopupMenu->SetPosition(aPenEventLocation);
    }
	DP("CPodcastListView::HandleLongTapEventL END");
}

TBool CPodcastFeedView::ViewingShows()
	{
	return iViewingShows;
	}
