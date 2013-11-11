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

#ifndef PODCASTQUEUEVIEWH
#define PODCASTQUEUEVIEWH 

#include <aknview.h>
#include <aknlists.h> 
#include <eiklbo.h>
#include <AknToolbarObserver.h>
#include <AknToolbar.h>
#include "ShowEngine.h"
#include "FeedEngine.h"
#include "PodcastModel.h"
#include "Podcast.hrh"
#include "PodcastShowsView.h"

class CPodcastQueueView : public CPodcastShowsView
	{
public: 
	static CPodcastQueueView* NewL(CPodcastModel& aPodcastModel);
	static CPodcastQueueView* NewLC(CPodcastModel& aPodcastModel);
	~CPodcastQueueView();
	void UpdateToolbar(TBool aVisible=ETrue);
protected:
	void ConstructL();
	CPodcastQueueView(CPodcastModel& aPodcastModel);

	/**
	 * Returns views id, intended for overriding by sub classes.
	 * @return id for this view.
	 */
	TUid Id() const;

	/** 
	 * Handles a view activation and passes the message of type 
	 * @c aCustomMessageId. This function is intended for overriding by 
	 * sub classes. This function is called by @c AknViewActivatedL().
	 * @param aPrevViewId Specifies the view previously active.
	 * @param aCustomMessageId Specifies the message type.
	 * @param aCustomMessage The activation message.
	 */
	void DoActivateL(const TVwsViewId& aPrevViewId,
			TUid aCustomMessageId,
			const TDesC8& aCustomMessage);

	/** 
	 * View deactivation function intended for overriding by sub classes. 
	 * This function is called by @c AknViewDeactivated().
	 */
	void DoDeactivate();

	void UpdateListboxItemsL();

	// From MEikListBoxObserverClass
	void HandleListBoxEventL(CEikListBox* aListBox, TListBoxEvent aEventType);

	/** 
	 * Command handling function intended for overriding by sub classes. 
	 * Default implementation is empty.  
	 * @param aCommand ID of the command to respond to. 
	 */
	void HandleCommandL(TInt aCommand);
	TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);
	void DynInitMenuPaneL(TInt aResourceId,CEikMenuPane* aMenuPane);
	
	virtual void HandleLongTapEventL( const TPoint& aPenEventLocation, const TPoint& aPenEventScreenLocation);
	
private:
	TBool iDontUpdateList;
	};

#endif


