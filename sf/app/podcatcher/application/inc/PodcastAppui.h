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

#ifndef PODCASTAPPUI_H
#define PODCASTAPPUI_H

#include <aknviewappui.h>
#include <akntabobserver.h>
#include <akntabgrp.h>
#include <aknnavide.h> 

#include "ConnectionEngine.h"

class CPodcastMainView;
class CPodcastFeedView;
class CPodcastShowsView;
class CPodcastQueueView;
class CPodcastSettingsView;
class CPodcastSearchView;
class CPodcastModel;

const TUid KUidPodcastClientID  = {0xA00046AE};
const TUid KUidPodcastFeedViewID = {0x00000001};
const TUid KUidPodcastShowsViewID = {0x00000002};
const TUid KUidPodcastQueueViewID = {0x00000003};
const TUid KUidPodcastSearchViewID = {0x00000004};
const TUid KUidPodcastSettingsViewID = {0x00000005};

const TInt KTabIdFeeds = 0;
const TInt KTabIdQueue = 1;

const TInt KTimeoutPrio = CActive::EPriorityStandard;

enum TNaviStyle
	{
	ENaviEmpty,
	ENaviText,
	ENaviTabGroup
	};

class CTimeout;

class MTimeoutObserver
{
public:
    virtual void HandleTimeoutL(const CTimeout& aId, TInt aError)=0;
protected:
    virtual ~MTimeoutObserver() {}
};

class CTimeout : public CTimer
{
public:
	
     static CTimeout* NewLC(MTimeoutObserver& aObserver, TInt aPrio=KTimeoutPrio)
    	 {
         CTimeout* self=new(ELeave) CTimeout(aObserver, aPrio);
         CleanupStack::PushL(self);
         self->ConstructL();
         return self;
    	 }
     
     static CTimeout* NewL(MTimeoutObserver& aObserver, TInt aPrio=KTimeoutPrio)
		{
		CTimeout* self=NewLC(aObserver, aPrio);
		CleanupStack::Pop(self);
		return self;
		}
     
     ~CTimeout()
    	 {
    	 Cancel();
    	 }
protected:
    CTimeout(MTimeoutObserver& aObserver, TInt aPrio) : CTimer(aPrio), iObserver(aObserver)
    {
        CActiveScheduler::Add(this);
    }
    void ConstructL()
    	{
    	    CTimer::ConstructL();
    	}
    
    void RunL()
    	{
    	    TInt r=iStatus.Int();
    	    iObserver.HandleTimeoutL(*this, r);
    	}

protected:
    MTimeoutObserver& iObserver;
};

class CPodcastAppUi : public CAknViewAppUi, public MAknTabObserver, 
	public MTimeoutObserver, public MConnectionObserver
    {
    public: 
    	CPodcastAppUi(CPodcastModel* aPodcastModel);
        void ConstructL();
       ~CPodcastAppUi();

       void SetActiveTab(TInt aIndex);
       void UpdateQueueTabL(TInt aQueueLength);
       void TabLeftL();
       void TabRightL();
       void NaviSetTextL(TInt aResourceId);
       void NaviShowTabGroupL();
       void GetErrorTextL(TDes &aErrorMessage, TInt aErrorCode);

    private:
        // From MEikMenuObserver
        void DynInitMenuPaneL(TInt aResourceId,CEikMenuPane* aMenuPane);
        void TabChangedL (TInt aIndex);
        CArrayFix<TCoeHelpContext>* HelpContextL() const;
        void HandleCommandL(TInt aCommand);
    protected:
    	void HandleTimeoutL(const CTimeout& aId, TInt aError);

    protected:
    	// from MConnectionObserver
    	
    	void ConnectionSelectionStart();
    	void ConnectionSelectionEnd();    	
    	void ConnectCompleteL(TInt /*aErrorCode*/) {}
    	void Disconnected() {};
    	
    private:
    	CPodcastMainView *iMainView;
		CPodcastFeedView* iFeedView;
		CPodcastShowsView* iShowsView;
		CPodcastQueueView* iQueueView;
		CPodcastSearchView* iSearchView;
		CPodcastSettingsView* iSettingsView;
		
		CPodcastModel* iPodcastModel;

		CAknNavigationDecorator* iNaviTabGroup;
		CAknNavigationDecorator* iNaviText;
		CAknTabGroup* iTabGroup;
		CAknNavigationControlContainer* iNaviPane;
		TNaviStyle iNaviStyle;
		CTimeout* iStartTimer;
    };

#endif
