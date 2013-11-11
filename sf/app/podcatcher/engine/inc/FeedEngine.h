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

#ifndef FEEDENGINE_H_
#define FEEDENGINE_H_

#include "HttpClientObserver.h"
#include "HttpClient.h"
#include "FeedParser.h"
#include "FeedInfo.h"
#include "ShowInfo.h"
#include <e32cmn.h>
#include "Constants.h"
#include "FeedEngineObserver.h"
#include "FeedTimer.h"
#include <sqlite3.h>
#include <centralrepository.h>

class CPodcastModel;
class COpmlParser;

_LIT(KOpmlFeed, "    <outline title=\"%S\" description=\"%S\" xmlUrl=\"%S\" htmlUrl=\"%S\"/>");
_LIT(KOpmlHeader, "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n<opml version=\"1.1\" xmlns:podcastSearch=\"http://digitalpodcast.com/podcastsearchservice/output_specs.html\">\n<head>\n  <title>Podcast Feed List</title>\n</head>\n<body>\n  <outline>");
_LIT(KOpmlFooter, "  </outline>\n</body>\n</opml>");

_LIT(KSearchUrl, "http://www.digitalpodcast.com/podcastsearchservice/v2b/search/?appid=SymbianPodcatcher&keywords=%S&format=rssopml&sort=rel&searchsource=all&contentfilter=noadult&start=0&results=20");
_LIT(KSearchResultsFileName, "searchresults.opml");

const TInt KMaxUidBufLen = 20;
const TInt KMaxTitleLength = 512;
const TInt KMaxDescriptionLength = 1024;
const TInt KMaxURLLength = 512;
const TInt KMaxLineLength = 4096;
const TInt KMaxSearchString = 30;

enum TClientState {
	EIdle,
	EUpdatingFeed,
	EUpdatingImage,
	ESearching
};


class CFeedEngine : public CBase, public MHttpClientObserver, public MFeedParserObserver
{
public:
	static CFeedEngine* NewL(CPodcastModel& aPodcastModel);
	virtual ~CFeedEngine();
	
public:
	IMPORT_C TBool AddFeedL(const CFeedInfo& item);
	IMPORT_C void ImportFeedsL(const TDesC& aFile);
	IMPORT_C TBool ExportFeedsL(TFileName& aFile);
	IMPORT_C void RemoveFeedL(TUint aUid);
	IMPORT_C TBool UpdateFeedL(TUint aFeedUid);
	IMPORT_C void UpdateAllFeedsL(TBool aAutoUpdate = EFalse);
	IMPORT_C void CancelUpdateAllFeeds();
	IMPORT_C const RFeedInfoArray& GetSortedFeeds();
	IMPORT_C CFeedInfo* GetFeedInfoByUid(TUint aFeedUid);	
	IMPORT_C void GetStatsByFeedL(TUint aFeedUid, TUint &aNumShows, TUint &aNumUnplayed);

	IMPORT_C void AddObserver(MFeedEngineObserver *observer);
	IMPORT_C void RemoveObserver(MFeedEngineObserver *observer);

	void RunFeedTimer();
	
	IMPORT_C void UpdateFeedInfoL(CFeedInfo *aItem);
	/**
	 * Returns the current internal state of the feed engine4
	 */
	IMPORT_C TClientState ClientState();

	/**
	 * Returns the current updating client UID if clientstate is != ENotUpdateing
	 * @return TUint
	 */
	IMPORT_C TUint ActiveClientUid();
	
	IMPORT_C void SearchForFeedL(TDesC& aSearchString);
	IMPORT_C void AddSearchResultL(CFeedInfo *item);
	IMPORT_C const RFeedInfoArray& GetSearchResults();

	IMPORT_C void OpmlParsingCompleteL(TInt aError, TUint aNumFeedsAdded);
	void NotifyFeedUpdateComplete(TInt aFeedUid, TInt aError);
	
protected:
	
	static TInt CompareFeedsByTitle(const CFeedInfo &a, const CFeedInfo &b);

private:
	void ConstructL();
	CFeedEngine(CPodcastModel& aPodcastModel);

	// from HttpClientObserver
	void Connected(CHttpClient* aClient);
	void Disconnected(CHttpClient* aClient);
	void Progress(CHttpClient* aHttpClient, TInt aBytes, TInt aTotalBytes);
	void DownloadInfo(CHttpClient* aClient, TInt aSize);
	void CompleteL(CHttpClient* aClient, TInt aError);
	void FileError(TUint /*aError*/) { }
	// from FeedParser
	void NewShowL(CShowInfo& aItem);
	void ParsingCompleteL(CFeedInfo *aItem);

	void GetFeedImageL(CFeedInfo *aFeedInfo);
	
	void UpdateNextFeedL();
	void NotifyOpmlParsingCompleteL(TInt aError, TUint aNumFeedsAdded);

	
private:
	void DBLoadFeedsL();
	void DBRemoveFeedL(TUint aUid);
	void DBAddFeedL(const CFeedInfo& aTtem);
	CFeedInfo* DBGetFeedInfoByUidL(TUint aFeedUid);	
	TUint DBGetFeedCountL();
	void DBUpdateFeedL(const CFeedInfo& aItem);
	void DBGetStatsByFeedL(TUint aFeedUid, TUint &aNumShows, TUint &aNumUnplayed);

		
private:
	CHttpClient* iFeedClient;
	TClientState iClientState;
	CFeedTimer iFeedTimer;

	CPodcastModel& iPodcastModel;	
	
	// RSS parser
	CFeedParser* iParser;
	
	// the list of feeds
	RFeedInfoArray iSortedFeeds;

	CFeedInfo *iActiveFeed;
	TFileName iUpdatingFeedFileName;
	TFileName iSearchResultsFileName;
	RArray<TUint> iFeedsUpdating;
	
	// observers that will receive callbacks, not owning
    RArray<MFeedEngineObserver*> iObservers;
    
    // new feeds only add one show to download list when auto downloading
    TBool newFeed;
    TUint showsAdded;
    
    // offline mode detection
    CRepository* iRepository;
    
    sqlite3& iDB;
    
    TBuf<KDefaultSQLDataBufferLength> iSqlBuffer;
    
    COpmlParser* iOpmlParser;
	RFeedInfoArray iSearchResults;
	TBool iAutoUpdatedInitiator;
	TBool iCancelRequested;
};
#endif /*FEEDENGINE_H_*/

