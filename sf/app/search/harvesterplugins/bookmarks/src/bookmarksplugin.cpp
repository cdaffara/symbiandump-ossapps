/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Bookmarks harvester plugin implementation
*
*/

#include "bookmarksplugin.h"

#include <FavouritesItemList.h>
#include <ActiveFavouritesDbNotifier.h>
#include <e32base.h> 
#include <uri8.h> //For parsing URL names.

#include "harvesterserverlogger.h"
#include "common.h"
#include "csearchdocument.h"
#include "ccpixindexer.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "bookmarkspluginTraces.h"
#endif


/** The milliseconds delay between harvesting chunks. */
const TInt KHarvestingDelay = 1000;
/** Number of bookmarks to process in one active scheduler cycle */
const TInt KBookmarksPerRunL = 1;
//_LIT(KExcerptDelimiter, " ");
/** Bookmark list size for dynamic array*/
const TInt KBookmarkListSize = 100;

// -----------------------------------------------------------------------------
CBookmarksPlugin* CBookmarksPlugin::NewL()
	{
	CBookmarksPlugin* instance = CBookmarksPlugin::NewLC();
    CleanupStack::Pop(instance);
    return instance;
	}

// -----------------------------------------------------------------------------
CBookmarksPlugin* CBookmarksPlugin::NewLC()
	{
	CBookmarksPlugin* instance = new (ELeave) CBookmarksPlugin();
    CleanupStack::PushL(instance);
    instance->ConstructL();
    return instance;
	}

// -----------------------------------------------------------------------------
CBookmarksPlugin::CBookmarksPlugin()
	{
	//No implementation required. 
	//Necessary things done in ConstructL()
	}

// -----------------------------------------------------------------------------
CBookmarksPlugin::~CBookmarksPlugin()
	{
	if (iAsynchronizer)
        iAsynchronizer->CancelCallback();
	if ( iFavoritesNotifier )
	    iFavoritesNotifier->Cancel();
	delete iFavoritesNotifier;
	delete iAsynchronizer;
	delete iIndexer;
	delete iArrUidsCurrentBookmarkList;
	iFavouritesDb.Close();
	iFavouritesSession.Close();
	}
	
// -----------------------------------------------------------------------------
void CBookmarksPlugin::ConstructL()
	{
	iArrUidsCurrentBookmarkList = new(ELeave) CArrayFixFlat<TInt>( KBookmarkListSize );
	User::LeaveIfError( iFavouritesSession.Connect() );
	User::LeaveIfError( iFavouritesDb.Open( iFavouritesSession, KBrowserBookmarks ) );
	iFavoritesNotifier = new ( ELeave ) CActiveFavouritesDbNotifier( iFavouritesDb, *this );
    iAsynchronizer = CDelayedCallback::NewL( CActive::EPriorityIdle );
	}

// -----------------------------------------------------------------------------
void CBookmarksPlugin::StartPluginL()
	{
    iFavoritesNotifier->Start(); //Start the notifier. Guarenteed to be the first call to Start. So no check for IsActive().
	// Define this base application class, use default location
	User::LeaveIfError(iSearchSession.DefineVolume( _L(BOOKMARK_QBASEAPPCLASS), KNullDesC ));
	// Open database
	iIndexer = CCPixIndexer::NewL( iSearchSession );
	iIndexer->OpenDatabaseL( _L(BOOKMARK_QBASEAPPCLASS) ); 
	// Start harvester for this plugin
	iObserver->AddHarvestingQueue(this, iIndexer->GetBaseAppClass() );
	}

// -----------------------------------------------------------------------------
void CBookmarksPlugin::StartHarvestingL( const TDesC& /*aMedia*/ )
    {
    OstTrace0( TRACE_NORMAL, CBOOKMARKSPLUGIN_STARTHARVESTINGL, "StartHarvestingL: resetting database" );
    CPIXLOGSTRING("StartHarvestingL: resetting database");
    iIndexer->ResetL();//reset any indexes if exist already
    iCurrentIndex = 0; //Initialize to zero as it is started
    iFavouritesDb.Count( iCurrentCount );
    OstTrace1( TRACE_NORMAL, DUP1_CBOOKMARKSPLUGIN_STARTHARVESTINGL, "StartHarvestingL::Current count=%d", iCurrentCount );
    CPIXLOGSTRING2("StartHarvestingL::Current count = %d.", iCurrentCount);
    //Get all the UID list from current database
    //GetUids() appends items to the list. So, reset it first.
    //From the documentation of GetUids(): "Existing items remain (new ones appended)"
    iArrUidsCurrentBookmarkList->Reset();
    iFavouritesDb.GetUids( *iArrUidsCurrentBookmarkList );
#ifdef __PERFORMANCE_DATA
    iStartTime.UniversalTime();
#endif
  	iAsynchronizer->Start( 0, this, KHarvestingDelay ); //Guarenteed to be the first call to Start. So no checking for IsActive().
    }

// -----------------------------------------------------------------------------
void CBookmarksPlugin::DelayedCallbackL( TInt /*aCode*/ )
    {
    // Harvest items on each call

    // Read the next set of bookmark.
    for( TInt i = 0; i < KBookmarksPerRunL; i++ )
        {
        // Exit the loop if no more bookmarks
        if (iCurrentIndex >= iCurrentCount)
            break;
        
        //Create index item
        OstTraceExt2( TRACE_NORMAL, CBOOKMARKSPLUGIN_DELAYEDCALLBACKL, "CBookmarksPlugin::DelayedCallbackL;Harvesting id=%d;BookmarkUid=%d", iCurrentIndex, iArrUidsCurrentBookmarkList->At(iCurrentIndex) );
        CPIXLOGSTRING3("CBookmarksPlugin::DelayedCallbackL(): Harvesting id=%d, BookmarkUid = %d.", iCurrentIndex, iArrUidsCurrentBookmarkList->At(iCurrentIndex) );
        //Create new bookmark document and add
        CreateBookmarksIndexItemL(iArrUidsCurrentBookmarkList->At(iCurrentIndex),ECPixAddAction);
        iCurrentIndex++;
        }

    if( iAsynchronizer && (iCurrentIndex < iCurrentCount) )
        {
        // Launch the next RunL
        OstTrace1( TRACE_NORMAL, DUP1_CBOOKMARKSPLUGIN_DELAYEDCALLBACKL, "CBookmarksPlugin::DelayedCallbackL;scheduling item count=%d", iCurrentIndex );
        CPIXLOGSTRING2("CBookmarksPlugin::DelayedCallbackL(): scheduling item count: %d.", iCurrentIndex );
        iAsynchronizer->Start(0, this, KHarvestingDelay);
        OstTrace1( TRACE_NORMAL, DUP2_CBOOKMARKSPLUGIN_DELAYEDCALLBACKL, "CBookmarksPlugin::DelayedCallbackL;DONE scheduling item count=%d", iCurrentIndex );
        CPIXLOGSTRING2("CBookmarksPlugin::DelayedCallbackL(): DONE scheduling item count: %d.", iCurrentIndex );
        }
    else
        {
        // Harvesting was successfully completed
        Flush(*iIndexer);
#ifdef __PERFORMANCE_DATA
    UpdatePerformaceDataL();
#endif
        iObserver->HarvestingCompleted(this, iIndexer->GetBaseAppClass(), KErrNone);
        OstTrace0( TRACE_NORMAL, DUP3_CBOOKMARKSPLUGIN_DELAYEDCALLBACKL, "CBookmarksPlugin::DelayedCallbackL(): Harvesting complete" );
        CPIXLOGSTRING("CBookmarksPlugin::DelayedCallbackL(): Harvesting complete");
        }
	}

// -----------------------------------------------------------------------------
void CBookmarksPlugin::DelayedError( TInt aCode )
    {
    Flush(*iIndexer);
    iObserver->HarvestingCompleted(this, iIndexer->GetBaseAppClass(), aCode);
	}

// -----------------------------------------------------------------------------
/**
 * If input is www2.google.com, this return google.com.
 * If input is www2.google.com/someting, this returns google.com.
 *
 * @param aUrl URL from which to extract domain name.
 * @param aDomain descriptor pointer in which domain will be returned.
 *
 * @note Leaves in case of error with KErrArgument
 * @example
 * TPtrC domain;
 * GetDomainNameLC( myUrl, domain );
 */
void GetDomainNameL( const TDesC& aUrl, TPtrC& aDomain )
    {
    OstTraceExt1( TRACE_NORMAL, CBOOKMARKSPLUGIN_GETDOMAINNAMEL, "CBookmarksPlugin::GetDomainNameL();URL=%S", aUrl );
    CPIXLOGSTRING2("CBookmarksPlugin::GetDomainNameL(): URL = %S", &aUrl );
    //Convert to 8-bit descriptors.
    HBufC8* url8 = HBufC8::NewLC( aUrl.Length() );
    url8->Des().Copy( aUrl );
    
    TUriParser8  uriParser;
    User::LeaveIfError( uriParser.Parse( *url8 ) );

    //Get the host name
    //This returns www.google.com if URL is http://www.google.com/something...
    const TDesC8& host = uriParser.Extract( EUriHost );
    if( host.Length() == 0  )
        {
        aDomain.Set( aUrl );
        CleanupStack::PopAndDestroy( url8 );
        return;
        }
    TLex8 lex( host );
    //Check if starts with www.
    TChar currentChar = 0;
    int wwwCount = 0;
    do{
        if( ( currentChar = lex.Get() ) != 0 ) wwwCount++;
    }while( currentChar == TChar('w') || currentChar == TChar('W') );
    
    if( currentChar != 0 )
        {
        lex.UnGet();
        wwwCount--;
        }

    HBufC* domain16 = NULL; 
    //if www34.google.com, skip anything after www, till '.'
    if( 3 == wwwCount )
        {
        while( lex.Peek() != '.' ) lex.Inc();
        //Now stopped at .google.com. So remove '.'
        lex.Inc();
        //The rest is the domain!
        const TDesC8& domain = lex.Remainder();
        if ( domain.Length() == 0 )
            {
            User::Leave( KErrArgument );
            }
        //Convert to 16-bit descriptors.
        domain16 = HBufC::NewLC( domain.Length() );
        domain16->Des().Copy( domain );
        }
    else 
        {
        domain16 = HBufC::NewLC( host.Length() );
        domain16->Des().Copy( host );
        }
    aDomain.Set( *domain16 );
    CleanupStack::Pop( domain16 );
    CleanupStack::PopAndDestroy( url8 );
    OstTraceExt1( TRACE_NORMAL, DUP1_CBOOKMARKSPLUGIN_GETDOMAINNAMEL, "CBookmarksPlugin::GetDomainNameL();domain=%S", aDomain );
    CPIXLOGSTRING2("CBookmarksPlugin::GetDomainNameL(): domain = %S", &aDomain );
    }

// -----------------------------------------------------------------------------
void CBookmarksPlugin::DoIndexingL(CFavouritesItem*& aItem, const TDesC& aDocidStr, TCPixActionType& aActionType)
    {
    OstTraceFunctionEntry0( CBOOKMARKSPLUGIN_DOINDEXINGL_ENTRY );
    CPIXLOGSTRING("CBookmarksPlugin::DoIndexingL(): Entering");
    if(aItem->Type() == CFavouritesItem::EItem ) //Store only Items not folders
        {
        OstTraceExt2( TRACE_NORMAL, CBOOKMARKSPLUGIN_DOINDEXINGL, "CBookmarksPlugin::DoIndexingL();url=%S;name=%S", (aItem->Url()), (aItem->Name()) );
        CPIXLOGSTRING3("CBookmarksPlugin::DoIndexingL(): url = %S ,name = %S", &(aItem->Url()), &(aItem->Name()));
        CSearchDocument* index_item = CSearchDocument::NewLC(aDocidStr, _L(BOOKMARKAPPCLASS));
        index_item->AddFieldL(KMimeTypeField, KMimeTypeBookmark, CDocumentField::EStoreYes | CDocumentField::EIndexUnTokenized);
        if(aItem->Name() != KNullDesC)
            index_item->AddFieldL(KBookMarkFieldName,aItem->Name(),CDocumentField::EStoreYes | CDocumentField::EIndexTokenized);

        TPtrC domain;
        if(aItem->Url() != KNullDesC)
            {
            index_item->AddFieldL(KBookMarkUrl, aItem->Url(), CDocumentField::EStoreYes | CDocumentField::EIndexTokenized);
            GetDomainNameL( aItem->Url(), domain );
            index_item->AddFieldL(KBookMarkDomain, domain , CDocumentField::EStoreYes | CDocumentField::EIndexTokenized);
            OstTraceExt1( TRACE_NORMAL, DUP1_CBOOKMARKSPLUGIN_DOINDEXINGL, "CBookmarksPlugin::DoIndexingL();domain=%S", domain );
            CPIXLOGSTRING2("CBookmarksPlugin::DoIndexingL(): domain = %S", &domain );
            }
        
        //For bookmarks, only URL should go into exceprt field.
        //For more info, check the appclass-hierarchy.txt

        //Add Excerpt as it is must have field. What should be excerpt in bookmarks ?
        HBufC* excerpt = HBufC::NewLC( aItem->Url().Length() + 1 );
        TPtr ptr = excerpt->Des();
        ptr.Append( aItem->Url() );
        index_item->AddExcerptL( *excerpt );
        CleanupStack::PopAndDestroy( excerpt );

        // Send for indexing
        TRAPD(err, iIndexer->AddL(*index_item));
        if (err == KErrNone)
            {
                OstTrace0( TRACE_NORMAL, DUP2_CBOOKMARKSPLUGIN_DOINDEXINGL, "CBookmarksPlugin::DoIndexingL(): Added." );
                CPIXLOGSTRING("CBookmarksPlugin::DoIndexingL(): Added.");
                }
            else
                {
                OstTrace1( TRACE_NORMAL, DUP3_CBOOKMARKSPLUGIN_DOINDEXINGL, "CBookmarksPlugin::DoIndexingL();Error %d in adding", err );
                CPIXLOGSTRING2("CBookmarksPlugin::DoIndexingL(): Error %d in adding.", err);
                }           
        OstTrace0( TRACE_NORMAL, DUP4_CBOOKMARKSPLUGIN_DOINDEXINGL, "CBookmarksPlugin::DoIndexingL(): Logic complete" );
        CPIXLOGSTRING("CBookmarksPlugin::DoIndexingL(): Logic complete");
        CleanupStack::PopAndDestroy( index_item );
        OstTrace0( TRACE_NORMAL, DUP5_CBOOKMARKSPLUGIN_DOINDEXINGL, "CBookmarksPlugin::DoIndexingL(): Pop complete complete" );
        CPIXLOGSTRING("CBookmarksPlugin::DoIndexingL(): Pop complete complete");
        }
    CPIXLOGSTRING("CBookmarksPlugin::DoIndexingL(): Returning");
    OstTraceFunctionExit0( CBOOKMARKSPLUGIN_DOINDEXINGL_EXIT );
    }

// -----------------------------------------------------------------------------
void CBookmarksPlugin::CreateBookmarksIndexItemL(TInt aBookMarkUid, TCPixActionType aActionType)
    {
    //@sai: CTC: Will never be null in normal usecase. Needs to be present for UTs.
    if( !iIndexer )
        return;
    OstTrace1( TRACE_NORMAL, CBOOKMARKSPLUGIN_CREATEBOOKMARKSINDEXITEML, "CBookmarksPlugin::CreateBookmarkIndexItemL();aBookMarkUid=%d", aBookMarkUid );
    CPIXLOGSTRING2("CBookmarksPlugin::CreateBookmarkIndexItemL(): aBookMarkUid = %d ", aBookMarkUid );
    // creating CSearchDocument object with unique ID for this application
    TBuf<20> docid_str;
    docid_str.AppendNum(aBookMarkUid);
    //Add or update actions.
    CFavouritesItem* item = CFavouritesItem::NewLC();
    TInt err = iFavouritesDb.Get(aBookMarkUid,*item);
    OstTrace1( TRACE_NORMAL, DUP1_CBOOKMARKSPLUGIN_CREATEBOOKMARKSINDEXITEML, "CBookmarksPlugin::CreateBookmarkIndexItemL();DB Get error=%d", err );
    CPIXLOGSTRING2("CBookmarksPlugin::CreateBookmarkIndexItemL(): DB Get error = %d ", err );
    //@sai: CTC: did not ever get 'false' for this check. Since this is a private function,
    // we cannot UT this with an invalid bookmarkUid.
    if(KErrNone != err)
        {
        CleanupStack::PopAndDestroy(item);
        return;
        }
    OstTrace1( TRACE_NORMAL, DUP2_CBOOKMARKSPLUGIN_CREATEBOOKMARKSINDEXITEML, "CBookmarksPlugin::CreateBookmarkIndexItemL();item->Type()=%d", item->Type() );
    CPIXLOGSTRING2("CBookmarksPlugin::CreateBookmarkIndexItemL(): item->Type() = %d ", item->Type() );
    DoIndexingL(item, docid_str, aActionType); //add to / update index
    CleanupStack::PopAndDestroy(item);
    }

// -----------------------------------------------------------------------------
void CBookmarksPlugin::HandleFavouritesDbEventL( RDbNotifier::TEvent aEvent )
    {
    //Since we cannot efficiently identify updated item, it makes most
    //sense to simply re-index the bookmarks - their numbers are not 
    //prohibitively large.
    //@sai:CTC: the DB does not seem to be sending any other event.
    if( RDbNotifier::ECommit == aEvent )
        {
        if( iAsynchronizer->CallbackPending() )
            {
            OstTrace0( TRACE_NORMAL, CBOOKMARKSPLUGIN_HANDLEFAVOURITESDBEVENTL, "HandleFavouritesDbEventL: Cancelling callback" );
            CPIXLOGSTRING("HandleFavouritesDbEventL: Cancelling callback");
            iAsynchronizer->CancelCallback(); //first cancel any ongoing harvesting.
            }
        OstTrace0( TRACE_NORMAL, DUP1_CBOOKMARKSPLUGIN_HANDLEFAVOURITESDBEVENTL, "HandleFavouritesDbEventL: calling StartHarvestingL" );
        CPIXLOGSTRING("HandleFavouritesDbEventL: calling StartHarvestingL");
        StartHarvestingL( KNullDesC );
        }
    }

void CBookmarksPlugin::PausePluginL()
    {
    
    }

void CBookmarksPlugin::ResumePluginL()
    {
    
    }	
// -----------------------------------------------------------------------------
#ifdef __PERFORMANCE_DATA
void  CBookmarksPlugin::UpdatePerformaceDataL()
    {
    TTime now;
   
    
    iCompleteTime.UniversalTime();
    TTimeIntervalMicroSeconds timeDiff = iCompleteTime.MicroSecondsFrom(iStartTime);
    
    RFs fileSession;
    RFile perfFile;
    User::LeaveIfError( fileSession.Connect () );
    
    
    /* Open file if it exists, otherwise create it and write content in it */
    
        if(perfFile.Open(fileSession, _L("c:\\data\\BookmarksPerf.txt"), EFileWrite))
                   User::LeaveIfError(perfFile.Create (fileSession, _L("c:\\data\\BookmarksPerf.txt"), EFileWrite));
    
    HBufC8 *heap = HBufC8::NewL(100);
    TPtr8 ptr = heap->Des();
    now.HomeTime();
    TBuf<50> timeString;             
                
    _LIT(KOwnTimeFormat,"%:0%H%:1%T%:2%S");
    now.FormatL(timeString,KOwnTimeFormat);
    ptr.AppendNum(now.DateTime().Day());
    ptr.Append(_L("/"));
    ptr.AppendNum(now.DateTime().Month());
    ptr.Append(_L("/"));
    ptr.AppendNum(now.DateTime().Year());
    ptr.Append(_L(":"));
    ptr.Append(timeString);
    ptr.Append( _L(":Ani: Time took for Harvesting Bookmarks is : "));
    ptr.AppendNum(timeDiff.Int64()/1000) ;
    ptr.Append(_L(" MilliSeonds \n"));
    TInt myInt = 0;
    perfFile.Seek(ESeekEnd,myInt);
    perfFile.Write (ptr);
    perfFile.Close ();
    fileSession.Close ();
    delete heap;
    }
#endif
// End of file
