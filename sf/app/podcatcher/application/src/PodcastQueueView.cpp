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

#include "PodcastQueueView.h"
#include "PodcastAppUi.h"
#include "ShowEngine.h"
#include "SettingsEngine.h"
#include "PodcastApp.h"
#include "Constants.h"
#include "imagehandler.h"
#include "PodcastShowsView.h"

#include <podcast.rsg>
#include <podcast.mbg>
#include <gulicon.h>
#include <barsread.h>
#include <aknnotedialog.h>
#include <aknmessagequerydialog.h>

#define KMaxMessageLength 200


CPodcastQueueView* CPodcastQueueView::NewL(CPodcastModel& aPodcastModel)
	{
	CPodcastQueueView* self = CPodcastQueueView::NewLC(aPodcastModel);
	CleanupStack::Pop(self);
	return self;
	}

CPodcastQueueView* CPodcastQueueView::NewLC(CPodcastModel& aPodcastModel)
	{
	CPodcastQueueView* self = new ( ELeave ) CPodcastQueueView(aPodcastModel);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CPodcastQueueView::CPodcastQueueView(CPodcastModel& aPodcastModel) :
	CPodcastShowsView(aPodcastModel)
	{
	}

void CPodcastQueueView::ConstructL()
	{
	BaseConstructL(R_PODCAST_QUEUEVIEW);
	CPodcastListView::ConstructL();

	CreateIconsL();
		
	iListContainer->Listbox()->SetListBoxObserver(this);
	
	iPodcastModel.FeedEngine().AddObserver(this);
	iPodcastModel.ShowEngine().AddObserver(this);
	
	iStylusPopupMenu = CAknStylusPopUpMenu::NewL( this , TPoint(0,0));
	TResourceReader reader;
	iCoeEnv->CreateResourceReaderLC(reader,R_QUEUEVIEW_POPUP_MENU);
	iStylusPopupMenu->ConstructFromResourceL(reader);

	CleanupStack::PopAndDestroy();

	SetEmptyTextL(R_PODCAST_EMPTY_QUEUE);
	}

TKeyResponse CPodcastQueueView::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType)
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
			case EKeyBackspace:
			case EKeyDelete:
				HandleCommandL(EPodcastRemoveDownload);
				break;
			default:
				break;
			}
			UpdateToolbar();
		}
	}
		return CPodcastListView::OfferKeyEventL(aKeyEvent, aType);
	}

CPodcastQueueView::~CPodcastQueueView()
	{
	iPodcastModel.ShowEngine().RemoveObserver(this);
	iPodcastModel.FeedEngine().RemoveObserver(this);
	
    if(iStylusPopupMenu)
        delete iStylusPopupMenu, iStylusPopupMenu = NULL;
	}


TUid CPodcastQueueView::Id() const
	{
	return KUidPodcastQueueViewID;
	}

void CPodcastQueueView::DoActivateL(const TVwsViewId& aPrevViewId,
		TUid aCustomMessageId, const TDesC8& aCustomMessage)
	{
	DP("CPodcastQueueView::DoActivateL BEGIN");
	
	CPodcastListView::DoActivateL(aPrevViewId, aCustomMessageId, aCustomMessage);
	iPreviousView = aPrevViewId;
	
	UpdateFeedUpdateStateL();
	UpdateToolbar();
	DP("CPodcastQueueView::DoActivateL END");
	}

void CPodcastQueueView::DoDeactivate()
	{
	CPodcastListView::DoDeactivate();
	}

void CPodcastQueueView::HandleListBoxEventL(CEikListBox* /*aListBox*/,
		TListBoxEvent aEventType)
	{
	switch (aEventType)
		{
		case EEventItemSingleClicked:
		case EEventEnterKeyPressed:		
		case EEventItemActioned:
		case EEventItemDoubleClicked:
			break;
		default:
			break;
		}
		UpdateToolbar();
	}

void CPodcastQueueView::UpdateListboxItemsL()
	{
	if (iListContainer->IsVisible() && !iDontUpdateList)
		{
		TListItemProperties itemProps;
		TInt len = 0;

		iPodcastModel.GetShowsDownloadingL();
	
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
					iListContainer->Listbox()->DrawItem(loop);
					}
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
void CPodcastQueueView::HandleCommandL(TInt aCommand)
	{
	switch (aCommand)
		{
		case EPodcastRemoveAllDownloads:
			{
			TBuf<KMaxMessageLength> msg;
			iEikonEnv->ReadResourceL(msg, R_CLEAR_QUERY);
												
			if(ShowQueryMessageL(msg))
				{
				iPodcastModel.ShowEngine().RemoveAllDownloadsL();
				UpdateListboxItemsL();
				}
			}
			break;
		case EPodcastRemoveDownload:
			{
			TInt index = iListContainer->Listbox()->CurrentItemIndex();
			if (index >= 0 && index < iPodcastModel.ActiveShowList().Count())
				{
				// this is an ugly hack to prevent UpdateListboxItemsL from being
				// triggered from the show engine, which causes focus to jump
				// around in an ugly fashion
				iDontUpdateList = ETrue;
				TRAP_IGNORE(iPodcastModel.ShowEngine().RemoveDownloadL(iPodcastModel.ActiveShowList()[index]->Uid()));
				iDontUpdateList = EFalse;
				}
			UpdateListboxItemsL();
			if (index > 0)
				{
				iListContainer->Listbox()->SetCurrentItemIndex(index - 1);
				}
			}
			break;
		case EPodcastMoveDownloadUp:
			{
			TInt index = iListContainer->Listbox()->CurrentItemIndex();
			TBool resumeAfterMove = EFalse;
			if (index == 1 && !iPodcastModel.SettingsEngine().DownloadSuspended())
				{
				iPodcastModel.ShowEngine().SuspendDownloads();
				resumeAfterMove = ETrue;
				}
			
			if (index >= 0 && index < iPodcastModel.ActiveShowList().Count())
				{
				TRAP_IGNORE(iPodcastModel.ShowEngine().MoveDownloadUpL(iPodcastModel.ActiveShowList()[index]->Uid()));
				}
			
			if(resumeAfterMove)
				{
				iPodcastModel.ShowEngine().ResumeDownloadsL();
				}
				
			iListContainer->Listbox()->SetCurrentItemIndex(index - 1);
			UpdateListboxItemsL();
			}
			break;
		case EPodcastMoveDownloadDown:
			{
			TInt index = iListContainer->Listbox()->CurrentItemIndex();
			TBool resumeAfterMove = EFalse;
			if (index == 0 && !iPodcastModel.SettingsEngine().DownloadSuspended())
				{
				iPodcastModel.ShowEngine().SuspendDownloads();
				resumeAfterMove = ETrue;
				}
			
			if (index >= 0 && index < iPodcastModel.ActiveShowList().Count())
				{
				TRAP_IGNORE(iPodcastModel.ShowEngine().MoveDownloadDownL(iPodcastModel.ActiveShowList()[index]->Uid()));
				}
			
			if(resumeAfterMove)
				{
				iPodcastModel.ShowEngine().ResumeDownloadsL();
				}
			
			iListContainer->Listbox()->SetCurrentItemIndex(index - 1);
			UpdateListboxItemsL();
			}
			break;
		case EPodcastSuspendDownloads:
			{
			iPodcastModel.ShowEngine().SuspendDownloads();
			UpdateListboxItemsL();
			}
			break;
		case EPodcastResumeDownloads:
			{
			iPodcastModel.ShowEngine().ResumeDownloadsL();
			UpdateListboxItemsL();
			}
			break;
		default:
			CPodcastListView::HandleCommandL(aCommand);
			break;
		}
	iListContainer->SetLongTapDetectedL(EFalse); // in case we got here by long tapping
	UpdateToolbar();
	}
	
void CPodcastQueueView::DynInitMenuPaneL(TInt aResourceId,CEikMenuPane* aMenuPane)
	{
	if(aResourceId == R_PODCAST_SHOWSVIEW_MENU)
		{
		aMenuPane->SetItemDimmed(EPodcastMarkAllPlayed, ETrue);
		}
	}

void CPodcastQueueView::UpdateToolbar(TBool aVisible)
	{
	CAknToolbar* toolbar = Toolbar();
	
	if (toolbar)
		{
		RShowInfoArray &fItems = iPodcastModel.ActiveShowList();
		TInt itemCnt = fItems.Count();
		if (iListContainer->IsVisible()) 
			{
			toolbar->SetToolbarVisibility(aVisible);
			}
		
		toolbar->HideItem(EPodcastRemoveAllDownloads, EFalse, ETrue);
		toolbar->SetItemDimmed(EPodcastRemoveAllDownloads, itemCnt == 0, ETrue);
		toolbar->HideItem(EPodcastSuspendDownloads,iPodcastModel.SettingsEngine().DownloadSuspended(), ETrue);
		toolbar->HideItem(EPodcastResumeDownloads,!iPodcastModel.SettingsEngine().DownloadSuspended(), ETrue);	
		toolbar->SetItemDimmed(EPodcastRemoveDownload, itemCnt == 0, ETrue);		
		}
	}

void CPodcastQueueView::HandleLongTapEventL( const TPoint& aPenEventLocation, const TPoint& /* aPenEventScreenLocation */)
{
	DP("CPodcastQueueView::HandleLongTapEventL BEGIN");
	iListContainer->SetLongTapDetectedL(ETrue);
	
	const TInt KListboxDefaultHeight = 19; // for some reason it returns 19 for an empty listbox in S^1
	TInt lbHeight = iListContainer->Listbox()->CalcHeightBasedOnNumOfItems(
			iListContainer->Listbox()->Model()->NumberOfItems()) - KListboxDefaultHeight;

    if(iStylusPopupMenu && aPenEventLocation.iY < lbHeight)
    {
		TBool dimDown = (iListContainer->Listbox()->CurrentItemIndex() >= iPodcastModel.ActiveShowList().Count() - 1 ?
				ETrue : EFalse);
		TBool dimUp = (iListContainer->Listbox()->CurrentItemIndex() <= 0 ?
				ETrue : EFalse);
		
		iStylusPopupMenu->SetItemDimmed(EPodcastMoveDownloadDown, dimDown);
		iStylusPopupMenu->SetItemDimmed(EPodcastMoveDownloadUp, dimUp);
		
		iStylusPopupMenu->ShowMenu();
		iStylusPopupMenu->SetPosition(aPenEventLocation);
    }
    
	DP("CPodcastQueueView::HandleLongTapEventL END");
}
