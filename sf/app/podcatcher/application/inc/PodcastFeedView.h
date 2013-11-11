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

#ifndef PODCASTFEEDVIEWH
#define PODCASTFEEDVIEWH 

#include "FeedEngine.h"
#include "PodcastListView.h"
#include "ImageHandler.h"

class CPodcastFeedViewUpdater;

enum TOpmlState {
	EOpmlIdle,
	EOpmlImporting,
	EOpmlSearching
};

class CPodcastFeedView : public CPodcastListView, public MEikListBoxObserver, 
	public MFeedEngineObserver, public MImageHandlerCallback
    {
    public: 
        static CPodcastFeedView* NewL(CPodcastModel& aPodcastModel);
        static CPodcastFeedView* NewLC(CPodcastModel& aPodcastModel);
        ~CPodcastFeedView();
        void UpdateItemL(TInt aIndex);
		void CheckResumeDownloadL();
		void UpdateToolbar(TBool aVisible=ETrue);
		TBool ViewingShows();
		
	protected:
	    void ConstructL();
		CPodcastFeedView(CPodcastModel& aPodcastModel);

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
		

		// From // MEikListBoxObserverClass
		void HandleListBoxEventL(CEikListBox* aListBox, TListBoxEvent aEventType);
		void UpdateListboxItemsL();
	
		// from MFeedEngineObserver
		void FeedDownloadStartedL(TFeedState aState,TUint aFeedUid);		
		void FeedDownloadFinishedL(TFeedState aState,TUint aFeeidUid, TInt aError);
		void FeedUpdateAllCompleteL(TFeedState aState);
		void OpmlParsingComplete(TInt aError, TUint aNumFeedsImported);

		/** 
		 * Command handling function intended for overriding by sub classes. 
		 * Default implementation is empty.  
		 * @param aCommand ID of the command to respond to. 
		 */
		void HandleCommandL(TInt aCommand);
		void FeedInfoUpdatedL(TUint aFeedUid);
		void UpdateFeedInfoDataL(CFeedInfo* aFeedInfo, TInt aIndex, TBool aIsUpdating = EFalse);
		void UpdateFeedInfoStatusL(TUint aFeedUid, TBool aIsUpdating = EFalse);

		// from MProgressDialogCallback
		void DialogDismissedL(TInt aButtonId); 

		// from MImageHandlerCallback
		void ImageOperationCompleteL(TInt aError, TUint aHandle, CPodcastModel& aPodcastModel);
		
		void HandleLongTapEventL( const TPoint& aPenEventLocation, const TPoint& aPenEventScreenLocation);

	private:
		void FormatFeedInfoListBoxItemL(CFeedInfo& aFeedInfo, TBool aIsUpdating = EFalse);
		void HandleAddFeedL();
		void HandleEditFeedL();
		void HandleRemoveFeedL();
		void HandleImportFeedsL();
		void HandleExportFeedsL();
		void HandleUpdateFeedL();
		void GetFeedErrorText(TDes &aErrorMessage, TInt aErrorCode);
		void OpmlParsingCompleteL(TInt aError, TUint aNumFeedsImported);

	private:
		CPodcastModel& iPodcastModel;		
		TBool iUpdatingRunning;
		HBufC* iFeedsFormat;
		HBufC* iNeverUpdated;
		CPodcastFeedViewUpdater* iUpdater;
		TBool iFirstActivateAfterLaunch;
		TOpmlState iOpmlState;
		TBool iViewingShows;
		RArray<TInt> iFeedIdForIconArray;
};

#endif // PODCASTFEEDVIEWH
