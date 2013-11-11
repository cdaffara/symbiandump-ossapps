/*
* Copyright (c) 2010 Anders Fridlund, EmbedDev AB
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

#ifndef PODCASTFEEDVIEWUPDATER_H_
#define PODCASTFEEDVIEWUPDATER_H_

#include <e32base.h>
class CPodcastFeedView;

class CPodcastFeedViewUpdater : public CAsyncOneShot 
    {
public:
	static CPodcastFeedViewUpdater* NewL(CPodcastFeedView& aPodcastFeedView);
	void StartUpdate(TInt aNbrItems);
	void StopUpdate();
	~CPodcastFeedViewUpdater();
        
private:
	void ConstructL();
	CPodcastFeedViewUpdater(CPodcastFeedView& aPodcastFeedView);

	// From CAsyncOneShot
	void RunL();
	TInt RunError(TInt aError);
	
private:
	CPodcastFeedView& iPodcastFeedView;
	TInt iNbrItems;
	TInt iNextItem;
};

#endif /* PODCASTFEEDVIEWUPDATER_H_ */
