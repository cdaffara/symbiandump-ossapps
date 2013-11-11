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

#include "PodcastAppui.h"
#include <Podcast.rsg>
#include "Podcast.hrh"
#include "PodcastFeedView.h"
#include "PodcastShowsView.h"
#include "PodcastQueueView.h"
#include "PodcastSettingsView.h"
#include "PodcastSearchView.h"
#include "ShowEngine.h"
#include "PodcastModel.h"
#include "debug.h"
#include "..\help\podcatcher.hlp.hrh"
#include "PodcastApp.h"
#include <APGWGNAM.H>
#include <HLPLCH.H>
#include <avkon.hrh>

const TUint KDelayLaunch = 1000;

CPodcastAppUi::CPodcastAppUi(CPodcastModel* aPodcastModel):iPodcastModel(aPodcastModel)
	{
	
	}

void CPodcastAppUi::ConstructL()
    {
    DP("CPodcastAppUi::ConstructL() BEGIN");
    BaseConstructL(CAknAppUi::EAknEnableSkin | CAknAppUi::EAknEnableMSK | CAknAppUi::EAknSingleClickCompatible); 

    iPodcastModel->ConnectionEngine().AddObserver(this);
    
    DP("Constructing FeedView");
	iFeedView = CPodcastFeedView::NewL(*iPodcastModel);
	this->AddViewL(iFeedView);

    DP("Constructing ShowsView");
	iShowsView = CPodcastShowsView::NewL(*iPodcastModel);
	this->AddViewL(iShowsView);

    DP("Constructing QueueView");
    iQueueView = CPodcastQueueView::NewL(*iPodcastModel);
	this->AddViewL(iQueueView);

    DP("Constructing SearchView");
	iSearchView = CPodcastSearchView::NewL(*iPodcastModel);
	this->AddViewL(iSearchView);
	
    DP("Constructing SettingsView");
	iSettingsView = CPodcastSettingsView::NewL(*iPodcastModel);
	this->AddViewL(iSettingsView);
	
	iNaviPane =( CAknNavigationControlContainer * ) StatusPane()->ControlL( TUid::Uid( EEikStatusPaneUidNavi ) );
	NaviShowTabGroupL();
	
	// start a timer to let ConstructL finish before we display any dialogs or start downloading
	// this provides another entry point in HandleTimeout below
	iStartTimer = CTimeout::NewL(*this);
	iStartTimer->After(KDelayLaunch);
    DP("CPodcastAppUi::ConstructL() END");
    }

CPodcastAppUi::~CPodcastAppUi()
    {
    if (iNaviStyle != ENaviEmpty)
    	{
    	iNaviPane->Pop();
    	}

    if(iNaviTabGroup)
    	{
    	delete iNaviTabGroup;
    	}
    
    if (iNaviText)
    	{
    	delete iNaviText;
    	}
    
	delete iStartTimer;
    }

// -----------------------------------------------------------------------------
// CPodcastAppUi::DynInitMenuPaneL(TInt aResourceId,CEikMenuPane* aMenuPane)
//  This function is called by the EIKON framework just before it displays
//  a menu pane. Its default implementation is empty, and by overriding it,
//  the application can set the state of menu items dynamically according
//  to the state of application data.
// ------------------------------------------------------------------------------
//
void CPodcastAppUi::DynInitMenuPaneL(
    TInt /*aResourceId*/,CEikMenuPane* /*aMenuPane*/)
    {
    // no implementation required 
    }

// -----------------------------------------------------------------------------
// CPodcastAppUi::HandleCommandL(TInt aCommand)
// takes care of command handling
// -----------------------------------------------------------------------------
//
void CPodcastAppUi::HandleCommandL( TInt aCommand )
    {
    switch ( aCommand )
        {
        case EAknSoftkeyExit:
            {
            Exit();
            break;
            }
        case EEikCmdExit:
        	{
        	// we want to prevent red button from closing podcatcher, and
        	// instead we send it to background
        	// however, we want to respect the task manager (fast swap) close
        	// command, so we check if task manager is the focussed window group
        	
        	RWsSession& ws = iEikonEnv->WsSession();
			TInt wgid = ws.GetFocusWindowGroup();
			CApaWindowGroupName* gn = CApaWindowGroupName::NewLC(ws, wgid);
			TUid activeAppUid = gn->AppUid();
			CleanupStack::PopAndDestroy(gn);
        	
			const TUid KUidFastSwap = { 0x10207218 };
			if (activeAppUid == KUidFastSwap)
				{
				// closed by task manager
				Exit();
				}
			else
        		{
        		// red button pressed
				TApaTask task(iEikonEnv->WsSession());
				task.SetWgId(iEikonEnv->RootWin().Identifier());
				task.SendToBackground(); 
        		}   		
			break;
        	}
	case EPodcastHelp:
        	{	
        	HlpLauncher::LaunchHelpApplicationL(iEikonEnv->WsSession(), HelpContextL());
        	}
        	break;      	
        default:
            break;      
        }
    }

CArrayFix<TCoeHelpContext>* CPodcastAppUi::HelpContextL() const
   { 
    CArrayFixFlat<TCoeHelpContext>* array = 
                new(ELeave)CArrayFixFlat<TCoeHelpContext>(1);
    CleanupStack::PushL(array);
    
    if (iFeedView->IsVisible()) {
		array->AppendL(TCoeHelpContext(KUidPodcast,KContextFeedsView));
    } else if (iShowsView->IsVisible()) {
		array->AppendL(TCoeHelpContext(KUidPodcast,KContextShowsView));
    } else if (iSearchView->IsVisible()) {
		array->AppendL(TCoeHelpContext(KUidPodcast,KContextSearch));
    } else if (iQueueView->IsVisible()) {
		array->AppendL(TCoeHelpContext(KUidPodcast,KContextDownloadQueue));
    } else {
		array->AppendL(TCoeHelpContext(KUidPodcast,KContextSettings));
    }
	
    CleanupStack::Pop(array);
    return array;
	}

void CPodcastAppUi::NaviSetTextL(TInt aResourceId)
	{
	
	if (iNaviStyle != ENaviEmpty) {
		iNaviPane->Pop();
	}

	HBufC* naviText =  iEikonEnv->AllocReadResourceLC(aResourceId);
	iNaviText  = iNaviPane->CreateNavigationLabelL(*naviText);
	
	iNaviPane->PushL(*iNaviText);

	CleanupStack::PopAndDestroy(naviText);	
	
	iNaviStyle = ENaviText;
	}

void CPodcastAppUi::NaviShowTabGroupL()
	{
	if (iNaviStyle != ENaviEmpty) {
		iNaviPane->Pop();
	}
	
	iNaviTabGroup = iNaviPane->CreateTabGroupL();
	
	iTabGroup = STATIC_CAST(CAknTabGroup*, iNaviTabGroup->DecoratedControl());
	iTabGroup->SetTabFixedWidthL(EAknTabWidthWithTwoTabs);

	HBufC *label1 = iEikonEnv->AllocReadResourceLC(R_TABGROUP_FEEDS);
	iTabGroup->AddTabL(KTabIdFeeds,*label1);
		
	HBufC *label3 = iEikonEnv->AllocReadResourceLC(R_TABGROUP_QUEUE);
	iTabGroup->AddTabL(KTabIdQueue,*label3);

	CleanupStack::PopAndDestroy(label3);
	CleanupStack::PopAndDestroy(label1);
	
	iTabGroup->SetActiveTabByIndex(0);
	iTabGroup->SetObserver(this);

	iNaviPane->PushL(*iNaviTabGroup);
	iNaviStyle = ENaviTabGroup;

	UpdateQueueTabL(iPodcastModel->ShowEngine().GetNumDownloadingShows());
	}

void CPodcastAppUi::TabChangedL (TInt aIndex)
	{
	DP("CPodcastListView::TabChangedL ");
	
	if (iNaviStyle == ENaviTabGroup)
		{
		TUid newview = TUid::Uid(0);
		TUid messageUid = TUid::Uid(0);
		if (aIndex == KTabIdFeeds) 
			{
			if (iFeedView->ViewingShows())
				{
				newview = KUidPodcastShowsViewID;
				}
			else
				{
				newview = KUidPodcastFeedViewID;
				}
			} 
		else if (aIndex == KTabIdQueue)
			{
			newview = KUidPodcastQueueViewID;
			} 
		else 
			{
			User::Leave(KErrTooBig);
			}
		
		if(newview.iUid != 0)
			{			
			ActivateLocalViewL(newview,  messageUid, KNullDesC8());
			}
		}
	}

void CPodcastAppUi::SetActiveTab(TInt aIndex) {
	if (iNaviStyle == ENaviTabGroup)
		{
		iTabGroup->SetActiveTabByIndex(aIndex);
		}
}

void CPodcastAppUi::HandleTimeoutL(const CTimeout& /*aId*/, TInt /*aError*/)
	{
	iFeedView->CheckResumeDownloadL();
	}

void CPodcastAppUi::UpdateQueueTabL(TInt aQueueLength)
	{
	if (iNaviStyle == ENaviTabGroup)
		{
		if (aQueueLength == 0)
			{
			HBufC *queue = iEikonEnv->AllocReadResourceLC(R_TABGROUP_QUEUE);
			iTabGroup->ReplaceTabL(KTabIdQueue, *queue);
			CleanupStack::PopAndDestroy(queue);
			}
		else
			{
			HBufC *queueTemplate = iEikonEnv->AllocReadResourceLC(R_TABGROUP_QUEUE_COUNTER);
			HBufC *queueCounter = HBufC::NewLC(queueTemplate->Length()+2);
			queueCounter->Des().Format(*queueTemplate, aQueueLength);
			
			iTabGroup->ReplaceTabL(KTabIdQueue, *queueCounter);
			CleanupStack::PopAndDestroy(queueCounter);
			CleanupStack::PopAndDestroy(queueTemplate);	
			}
		}
	}

void CPodcastAppUi::TabLeftL()
	{
	if (iNaviStyle == ENaviTabGroup) 
		{
		TInt ati = iTabGroup->ActiveTabIndex();
		if(ati > 0)
			{
			SetActiveTab(ati-1);
			TabChangedL(ati-1);
			}
		}
	}

void CPodcastAppUi::TabRightL()
	{
	if (iNaviStyle == ENaviTabGroup) 
		{
		TInt ati = iTabGroup->ActiveTabIndex();
		if(ati < iTabGroup->TabCount()-1)
			{
			SetActiveTab(ati+1);
			TabChangedL(ati+1);
			}
		}
	}

void CPodcastAppUi::ConnectionSelectionStart()
	{
	DP("CPodcastAppUi::ConnectionSelectionStart() BEGIN");
	iFeedView->UpdateToolbar(EFalse);
	iShowsView->UpdateToolbar(EFalse);
	iQueueView->UpdateToolbar(EFalse);
	iSearchView->UpdateToolbar(EFalse);
	DP("CPodcastAppUi::ConnectionSelectionStart() END");
	}

void CPodcastAppUi::ConnectionSelectionEnd()
	{
	DP("CPodcastAppUi::ConnectionSelectionEnd() BEGIN");
	iFeedView->UpdateToolbar(ETrue);
	iShowsView->UpdateToolbar(ETrue);
	iQueueView->UpdateToolbar(ETrue);
	iSearchView->UpdateToolbar(ETrue);
	DP("CPodcastAppUi::ConnectionSelectionEnd() END");
	}

void CPodcastAppUi::GetErrorTextL(TDes &aErrorMessage, TInt aErrorCode)
	{
	switch (aErrorCode)
		{
		case KErrNotFound:
			{
			HBufC* error = iCoeEnv->AllocReadResourceLC(R_ERROR_INVALID_ADDRESS);
			aErrorMessage.Copy(*error);
			CleanupStack::PopAndDestroy(error);
			}
			break;
		case KErrDiskFull:
			{
			HBufC* error = iCoeEnv->AllocReadResourceLC(R_ERROR_DISK_FULL);
			aErrorMessage.Copy(*error);
			CleanupStack::PopAndDestroy(error);
			}
			break;
		case 404:
			{
			HBufC* error = iCoeEnv->AllocReadResourceLC(R_ERROR_NOTFOUND);
			aErrorMessage.Copy(*error);
			CleanupStack::PopAndDestroy(error);
			}
			break;
		default:
			{
			if (aErrorCode > 200)
				{
				HBufC* error = iCoeEnv->AllocReadResourceLC(R_ERROR_HTTP);
				aErrorMessage.Format(*error, aErrorCode);
				CleanupStack::PopAndDestroy(error);
				}
			else
				{
				HBufC* error = iCoeEnv->AllocReadResourceLC(R_ERROR_GENERAL);
				aErrorMessage.Format(*error, aErrorCode);
				CleanupStack::PopAndDestroy(error);
				}
			}
			break;
		}
	}
