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

#ifndef FEEDENGINEOBSERVER_H_
#define FEEDENGINEOBSERVER_H_
class MFeedEngineObserver {
public:
	enum TFeedState
		{
		EFeedAutoUpdate,
		EFeedManualUpdate, 
		EFeedSearch
		};
	
	virtual void FeedDownloadStartedL(TFeedState aState, TUint aFeedUid) = 0;
	virtual void FeedDownloadFinishedL(TFeedState aState, TUint aFeedUid, TInt aError) = 0;
	virtual void FeedUpdateAllCompleteL(TFeedState aState) = 0;
	virtual void OpmlParsingComplete(TInt aError, TUint aNumFeedsImported) = 0;
};
#endif /*FEEDENGINEOBSERVER_H_*/
