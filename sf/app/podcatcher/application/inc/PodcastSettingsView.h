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

#ifndef PODCASTSETTINGSVIEWH
#define PODCASTSETTINGSVIEWH 

#include <aknview.h>
#include <aknsettingitemlist.h> 
#include "ShowEngineObserver.h"
#include "PodcastModel.h"

class CPodcastSettingsContainer;
class CPodcastSettingItemList;

class CPodcastSettingsView : public CAknView
    {
    public: 
        static CPodcastSettingsView* NewL(CPodcastModel& aPodcastModel);
        static CPodcastSettingsView* NewLC(CPodcastModel& aPodcastModel);
        ~CPodcastSettingsView();
	protected:
	    void ConstructL();
		CPodcastSettingsView(CPodcastModel& aPodcastModel);

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
		
		/** 
		 * Command handling function intended for overriding by sub classes. 
		 * Default implementation is empty.  
		 * @param aCommand ID of the command to respond to. 
		 */
		void HandleCommandL(TInt aCommand);
		void HandleStatusPaneSizeChange();

	private:
		//CPodcastSettingsContainer* iSettingsContainer;
		CPodcastModel& iPodcastModel;
		 /** Previous activated view */
		 TVwsViewId iPreviousView;
		 TInt iAutoDownloadValue;
		 CAknNavigationDecorator* iNaviDecorator;
		 CAknNavigationControlContainer* iNaviPane;
		 CPodcastSettingItemList  * iListbox;
};

#endif // PODCASTSETTINGSVIEWH


