/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  ?Description
*
*/


#include <bautils.h>

#include "irbrowsecatagoryitems.h"
#include "irbrowsechannelitems.h"
#include "ircachecleanup.h"
#include "ircachemgmt.h"
#include "ircacheobserver.h"
#include "irdebug.h"
#include "irhttprequestdata.h"
#include "irisdspreset.h"
#include "irotaupdate.h"
#include "irsettings.h"

//Default trust period value.24 hrs in secs with which the object is to be initialised
const TInt KDefaultInterval = 24*60*60;
//granualarity for the arrays
//the granualarity value is based on superficial observation of the amount of 
//data recieved for a particular request
const TInt KBrowseGranualarity = 32;
const TInt KChannelArrayGranualarity = 32;
const TInt KPresetGranualarity = 12;
//http length format length
// "Sun, 06 Nov 1994 08:49:37 GMT" format
const TInt KHttpDateLength = 25;

//Max size( critical size ) in Bytes( 90% of the max size )
//critical size is the size that triggers cleanup action
const TInt KCacheCriticalSizeLimit = KCacheLimitSize*9/10;

//No of rows deleted will be one fourth of the total number of rows
const TInt KPercentRowsDeleted = 4;  

//The indexing size for the column DataId is set to 100 
const TInt KDataIdIndexSize = 100;

//database file name
_LIT( KCacheDbFile,"cacheDb.db" );

//Table Name CacheTable
_LIT( KCacheTable,"CacheTable" );

//CacheTable columns
_LIT( KRowIndexCol,     "RowIndex" );  
_LIT( KDataTypeCol,     "DataType" );            
_LIT( KDataIdCol,       "DataId" );       
_LIT( KTrustPeriodCol,  "TrustPeriod" );       
_LIT( KLastModifiedCol, "LastModified" ); 
_LIT( KLastAccessedCol, "LastAccessed" ); 
_LIT( KTimeCreation,    "Created" );
_LIT( KItemCount,       "ItemCount" );
_LIT( KCachedDataCol,   "CachedData" );
_LIT( KETagHeader,      "ETagHeader" ); 

//Table Index Names
_LIT( KRowIndexColIndex,     "IndexRowIndex" );  
_LIT( KDataTypeColIndex,     "IndexDataType" );            
_LIT( KDataIdColIndex,       "IndexDataId" );  
_LIT( KLastAccessedColIndex, "IndexLastAccessed" ); 
_LIT( KTimeCreationIndex,    "IndexCreated" );
//Field Lengths

const TInt KIRObserverArrayGranularity( 2 );


// ---------------------------------------------------------------------------
// Function : OpenL
// Standard two phased construction
// calls ConstructL()
// ---------------------------------------------------------------------------
//
EXPORT_C CIRCacheMgmt* CIRCacheMgmt::OpenL( MIRCacheObserver &aObserver )
    {
    IRLOG_DEBUG( "IRCacheMgmt::OpenL - Entering." );
    CIRCacheMgmt* self = reinterpret_cast<CIRCacheMgmt*>( Dll::Tls() );
    
    if ( self )
        {
        User::LeaveIfError( self->Open() );
        }
    else
        {
        self = new ( ELeave ) CIRCacheMgmt( aObserver );
        CleanupClosePushL( *self );
        self->ConstructL();
        User::LeaveIfError( Dll::SetTls( self ) );
        CleanupStack::Pop( self );
        }
    IRLOG_DEBUG( "IRCacheMgmt::OpenL - Exiting." );
    return self;
    }


// ---------------------------------------------------------------------------
// Function : CIRCacheMgmt::~CIRCacheMgmt()
// Standard C++ destructor
// ---------------------------------------------------------------------------
//
CIRCacheMgmt::~CIRCacheMgmt() 
    { 
    IRLOG_DEBUG( "IRCacheMgmt::~CIRCacheMgmt - Entering." );
    //array destruction
    if ( iPtrPreset )
         {
         iPtrPreset->ResetAndDestroy();
             delete iPtrPreset;
             iPtrPreset = NULL;
         }
    if ( iPtrCategory )
        {
        iPtrCategory->ResetAndDestroy();
        }
    
    delete iPtrCategory;
    iPtrCategory = NULL;
    if ( iPtrChannel )
        {
        iPtrChannel->ResetAndDestroy();
        }
    
    delete iPtrChannel;
    iPtrChannel = NULL;
    if ( iSettings )
        {
        iSettings->Close();
        }
    if ( iOTA )
        {
        delete iOTA;
        iOTA = NULL;
        }
    iLogoData.Close();    
    CloseDb();
    iCacheDb.Close();
    delete iCleanup;
    iFsSession.Close();
    iCacheObservers.Close();
    Dll::FreeTls();
    IRLOG_DEBUG( "IRCacheMgmt::~CIRCacheMgmt - Exiting." );
    }

// ---------------------------------------------------------------------------
// Function : CIRCacheMgmt::CIRCacheMgmt()
// Standard C++ constructor
// sets the trust period to 24hrs(  default )
// ---------------------------------------------------------------------------
//
CIRCacheMgmt::CIRCacheMgmt ( MIRCacheObserver &aObserver )
    :iCacheObserver( aObserver ),
    iCacheObservers( KIRObserverArrayGranularity )
    {
    IRLOG_DEBUG( "IRCacheMgmt::CIRCacheMgmt - Entering." );
    //default trust period initially set to 24 hrs
    //to be fetched from central repository
    iTrustPeriod = TTimeIntervalSeconds( KDefaultInterval );  
    IRLOG_DEBUG( "IRCacheMgmt::CIRCacheMgmt - Exiting." );
    }

// ---------------------------------------------------------------------------
// Function : CIRCacheMgmt::ConstructL()
// Standard two phased construction
// ---------------------------------------------------------------------------
//
void CIRCacheMgmt::ConstructL() 
    {
    IRLOG_DEBUG( "IRCacheMgmt::ConstructL - Entering." );
    User::LeaveIfError( iFsSession.Connect() );
    //array construction
    iPtrPreset = new ( ELeave ) CArrayPtrFlat<CIRIsdsPreset>(
                              KPresetGranualarity );
    iPtrCategory = new ( ELeave ) CArrayPtrFlat<CIRBrowseCatagoryItems>(
                              KBrowseGranualarity );
    iPtrChannel = new ( ELeave ) CArrayPtrFlat<CIRBrowseChannelItems>(
                              KChannelArrayGranualarity );
    iOTA = CIROTAUpdate::NewL();
    iSettings = CIRSettings::OpenL();
    iDatabaseFileName = iSettings->PrivatePath();
    iDatabaseFileName.Append( KCacheDbFile );
    CreateDbConditionalL();
    iCleanup = CIRCacheCleanup::NewL( *this );
    RemoveOtaInfoL();
    IRLOG_DEBUG( "IRCacheMgmt::ConstructL - Exiting." );
    }
    
// ---------------------------------------------------------------------------
// Function : CIRCacheMgmt::CreateDbConditionalL(
// Creates dbms conditionally( only if not yet created )
// calls CreateDb()
// ---------------------------------------------------------------------------
//
void CIRCacheMgmt::CreateDbConditionalL()
    {
    IRLOG_DEBUG( "IRCacheMgmt::CreateDbConditionalL - Entering." );
    TInt err=KErrNone;
    if ( !BaflUtils::FileExists( iFsSession, iDatabaseFileName ) )
        {
        err=CreateDb();
        if ( KErrNone != err )
             {
             User::Leave( err ); 
             }
        }
    IRLOG_DEBUG( "IRCacheMgmt::CreateDbConditionalL - Exiting." );
    }

// ---------------------------------------------------------------------------
// Function : CIRCacheMgmt::CreateDb()
// Creates the tables
// ---------------------------------------------------------------------------
//
TInt CIRCacheMgmt::CreateDb()
    {
    IRLOG_DEBUG( "IRCacheMgmt::CreateDb - Entering." );
    CloseDb();
    TInt err = iCacheDb.Replace( iFsSession,iDatabaseFileName );
    if ( ( err != 0 ) )
        {
        return err; //unable to create file 
        }                                   
    TRAP( err,//trap start
        CreateCacheTableL();
        CreateCacheIndexL();
        )//trap end
    IRLOG_DEBUG( "IRCacheMgmt::CreateDb - Exiting." );
    return err;
    }

// ---------------------------------------------------------------------------
// Function : CIRCacheMgmt::OpenCacheDb()
// Opens both the databases 
// ---------------------------------------------------------------------------
//
TInt CIRCacheMgmt::OpenCacheDb()
    {
    IRLOG_DEBUG( "CIRCacheMgmt::OpenCacheDb - Entering" );
    CloseDb();
    TInt error = KErrNone;
    if ( !BaflUtils::FileExists( iFsSession, iDatabaseFileName ) )
        {
        //if file doesn't exist function leaves with error code
        //KErrNotFound
         return KErrNotFound;
        }
    //try and open the db 
    error = iCacheDb.Open( iFsSession,iDatabaseFileName );
    //return if error
    if ( KErrNone != error )
        {
        return error;
        }
    //check if damaged
    if ( iCacheDb.IsDamaged() )
        {
        //if data base is damaged then 
        //try to recover
        error = iCacheDb.Recover();
        return error;
        }
          
    iOpen = ETrue;
    
    IRLOG_DEBUG( "CIRCacheMgmt::OpenCacheDb - Exiting." );
    return KErrNone;
    }
  
// ---------------------------------------------------------------------------
// Function : CIRCacheMgmt::CloseDb()
// Closes the database 
// ---------------------------------------------------------------------------
//
void CIRCacheMgmt::CloseDb()
    {
    IRLOG_DEBUG( "IRCacheMgmt::CloseDb - Entering." );
    iCacheDb.Close();
    iOpen = EFalse;
    IRLOG_DEBUG( "IRCacheMgmt::CloseDb - Exiting." );
    }

// ---------------------------------------------------------------------------
// Function : CIRCacheMgmt::CheckCache()
// API Exposed to Isds Client to check and get the cached items. 
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRCacheMgmt::CheckCacheL( TInt aType,const TDesC& aName,
          TBool aForceGet,TInt& aReturn )   
    {
    IRLOG_DEBUG( "CIRCacheMgmt::CheckCacheL - Entering" );
    //opens the db if not open already
    if ( !iOpen )
        {
        TInt error = OpenCacheDb();
        if ( KErrNone != error )
            {
            aReturn = ENotCached;
            return; 
            }
        }
    
    //calls the function FetchCacheIfAvailable() passing the appropriate 
    //type and id
      
    switch( aType )
    {
    case ECatagory: 
    case EChannels:
    case EPresets:
    case EOtaInfo:
    case ELogo:
        {
        TRAP_IGNORE( FetchCacheIfAvailableL( aType,aName,aForceGet,aReturn ) )
        break;  
        }
      
    default:
        {
        for ( TInt i = 0; i < iCacheObservers.Count(); i++ )
            {
            iCacheObservers[i]->CacheFailed();
            }
        aReturn = ENotCached;
        break;  
        }
    }
    CloseDb();
    IRLOG_DEBUG( "CIRCacheMgmt::CheckCacheL - Exiting." );
    }
  
// ---------------------------------------------------------------------------
// Function : CIRCacheMgmt::FetchCacheIfAvailable()
// Fetches the cache if available 
// ---------------------------------------------------------------------------
//
//generic function to check the validity of the cache 
void CIRCacheMgmt::FetchCacheIfAvailableL( TInt aType,const TDesC& aId,
    TBool aForceGet,TInt& aReturnVal )
    {
    //Algo****
    //1.check the cache table for a request type
    //2.if the query returns a row,check for validity
    //3.if invalid( stale ) do a conditional get
    //4.if valid use cache
    IRLOG_DEBUG( "CIRCacheMgmt::FetchCacheIfAvailableL - Entering" );
    //SELECT * FROM KCacheTable WHERE KDataTypeCol = aType AND KDataIdCol=aId
    _LIT( query, "SELECT * FROM %S WHERE %S = %d AND %S = '%S'" );
    
    HBufC* sqlQuery = HBufC::NewLC( query().Length() + KCacheTable().Length() +
                      KDataTypeCol().Length() + KDataIdCol().Length() +
                      KDefaultRealWidth + aId.Length() );
                      
    sqlQuery->Des().Format( query,&KCacheTable,&KDataTypeCol,
                           aType,&KDataIdCol,&aId );
    
    IRLOG_DEBUG2( "CIRCacheMgmt::FetchCacheIfAvailableL - \
    Going to evaluate SQL query: %S", sqlQuery );
    
    RDbView cacheView;
    User::LeaveIfError( cacheView.Prepare( iCacheDb,*sqlQuery ) );
    CleanupStack::PopAndDestroy( sqlQuery );
    CleanupClosePushL( cacheView );
    User::LeaveIfError( cacheView.EvaluateAll() );
    
    //if no row match the query there is no cahche available for 
    //that particular 
    //request
    if ( cacheView.IsEmptyL() )
        {
        //Return ENotCached
        aReturnVal = ENotCached;
        }
    else
        {
        cacheView.FirstL();
        cacheView.GetL();
        CDbColSet* columns = cacheView.ColSetL();
        TTime creationTime = cacheView.ColTime(
            columns->ColNo( KTimeCreation ) );
            
        TInt cacheValidityTime = cacheView.ColInt(
                                 columns->ColNo( KTrustPeriodCol ) );
            
        TInt countItems=cacheView.ColInt( columns->ColNo( KItemCount ) );
        
        IRLOG_DEBUG( "CIRCacheMgmt::FetchCacheIfAvailableL - \
        can check validity." );
        
        //check if cache is valid
        //get it any way if it is "forcedget"
        if ( aForceGet || CheckValidity( creationTime,cacheValidityTime ) )
            {
            //Fetch the cached data
            //FetchCachedData( type,count,&view  )
            TRAPD( err,FetchCachedDataL( aType,countItems,cacheView ) );
            if ( err!=KErrNone )
                {
                //something goes wrong ,return not cached and fetch new data
                aReturnVal = ENotCached;
                }
            else
                {
                //cache usable
                aReturnVal = ECacheUseable;
                }
            }
        else
            {
            iLastModified = cacheView.ColTime(
                            columns->ColNo( KLastModifiedCol ) );
                
            iETag = cacheView.ColDes8( columns->ColNo( KETagHeader ) );     
            aReturnVal = ECacheNotValid;
            cacheView.Close();
            for ( TInt i = 0; i < iCacheObservers.Count(); i++ )
                {
                iCacheObservers[i]->CacheInvalid();
                }
            }
        delete columns; 
        }
    CleanupStack::PopAndDestroy( &cacheView );
    IRLOG_DEBUG( "CIRCacheMgmt::FetchCacheIfAvailableL - Exiting." );
    
    }
  
// ---------------------------------------------------------------------------
// Function : CIRCacheMgmt::FetchCachedDataL()
// fetches the cached data from the table into the cached structures
// ---------------------------------------------------------------------------
//
void CIRCacheMgmt::FetchCachedDataL( TInt aType,TInt aCountItems,
      RDbView& aCacheView )
    {
    //Algo****
    //fetch the data and internalize into appropriate structure
    
    IRLOG_DEBUG( "CIRCacheMgmt::FetchCachedDataL - Entering" );
    CDbColSet* columns = aCacheView.ColSetL();
    TInt columnNo = columns->ColNo( KCachedDataCol );
    delete columns;
    switch( aType )
        {
        case ECatagory:
            {
            //Internalize the category items into the 
            //CIRBrowseCatagoryItems array
            RDbColReadStream instream;
            instream.OpenLC( aCacheView, columnNo );
            CIRBrowseCatagoryItems* ptr;
            if ( iPtrCategory )
                {
                iPtrCategory->ResetAndDestroy();
                }
            else
                {
                break;
                }
            for ( TInt iter = 0; iter < aCountItems; iter++ )
                {
                ptr=CIRBrowseCatagoryItems::NewL();
                CleanupStack::PushL( ptr );
                instream>>*ptr;
                iPtrCategory->AppendL( ptr );
                CleanupStack::Pop( ptr );
                }
            CleanupStack::PopAndDestroy( &instream );
            //update the last accessed time
            UpdateLastAccessedTimeL( aCacheView );
            aCacheView.Close();
            for ( TInt i = 0; i < iCacheObservers.Count(); i++ )
                {
                iCacheObservers[i]->CachedStructureL( ECatagory );
                }
            break;
            }
        case EChannels:
            {
            //Internalize the channel items into 
            //the CIRBrowseChannelItems array
            RDbColReadStream instream;
            instream.OpenLC( aCacheView, columnNo );
            if ( iPtrChannel )
                {
                iPtrChannel->ResetAndDestroy();
                }
            CIRBrowseChannelItems *ptr;
            for ( TInt iter = 0; iter < aCountItems; iter++ )
                {
                ptr = CIRBrowseChannelItems::NewL();
                CleanupStack::PushL( ptr );
                instream>>*ptr;
                if ( iPtrChannel )
                    {                
                    iPtrChannel->AppendL( ptr );
                    }
                CleanupStack::Pop( ptr );
                }
            CleanupStack::PopAndDestroy( &instream );
            //update the last accessed time
            UpdateLastAccessedTimeL( aCacheView );
            aCacheView.Close();
            for ( TInt i = 0; i < iCacheObservers.Count(); i++ )
                {
                iCacheObservers[i]->CachedStructureL( EChannels );
                }
            break;
            }
        case EPresets:
            {
            //Internalize the preset  into the CIRIsdsPreset object
            RDbColReadStream instream;
            instream.OpenLC( aCacheView, columnNo );
            if ( iPtrPreset )
                {
                iPtrPreset->ResetAndDestroy();
                }
            CIRIsdsPreset *ptr;
            for ( TInt iter = 0; iter < aCountItems; iter++ )
                {
                ptr = CIRIsdsPreset::NewL();
                CleanupStack::PushL( ptr );
                instream>>*ptr;
                if ( iPtrPreset )
                    {                
                    iPtrPreset->AppendL( ptr );
                    }
                CleanupStack::Pop( ptr );
                }
            CleanupStack::PopAndDestroy( &instream );
            //update the last accessed time
            UpdateLastAccessedTimeL( aCacheView );
            aCacheView.Close();
            for ( TInt i = 0; i < iCacheObservers.Count(); i++ )
                {
                iCacheObservers[i]->CachedStructureL( EPresets );
                }
            break;
            }
        case EOtaInfo:
            {
            //Internalize the ota info into the CIROTAUpdate object
            RDbColReadStream instream;
            instream.OpenLC( aCacheView, columnNo );
            if ( iOTA )
                {
                delete iOTA;
                iOTA = NULL;
                iOTA = CIROTAUpdate::NewL();
                instream>>*iOTA;
                }
          
            CleanupStack::PopAndDestroy( &instream );
            //update the last accessed time
            UpdateLastAccessedTimeL( aCacheView );
            aCacheView.Close();
            for ( TInt i = 0; i < iCacheObservers.Count(); i++ )
                 {
                 iCacheObservers[i]->CachedStructureL( EOtaInfo );
                 }
            break;
            }
        case ELogo:
            {
            //get the logo data from the cache
            TInt logoSize = aCacheView.ColSize( columnNo );
            iLogoData.Close();
            iLogoData.Zero();
            iLogoData.CreateL( logoSize );
            
            RDbColReadStream instream;
            instream.OpenLC( aCacheView, columnNo );
            instream.ReadL( iLogoData,aCacheView.ColLength( columnNo ) );
            CleanupStack::PopAndDestroy( &instream );
            //update the last accessed time
            UpdateLastAccessedTimeL( aCacheView );
            aCacheView.Close();
            for ( TInt i = 0; i < iCacheObservers.Count(); i++ )
                {
                iCacheObservers[i]->CachedStructureL( ELogo );
                }
            break;
            }
        default:
            {
            aCacheView.Close();
            }
        }
    IRLOG_DEBUG( "CIRCacheMgmt::FetchCachedDataL - Exiting." );
    }
// ---------------------------------------------------------------------------
//updates the last accessed field to aid in deletion of old unused data
//
// ---------------------------------------------------------------------------
//
void CIRCacheMgmt::UpdateLastAccessedTimeL( RDbView &aCacheView )
    {
    IRLOG_DEBUG( "IRCacheMgmt::UpdateTrustPeriod - Entering." );
    //update the last accessed col
    CDbColSet* columns = aCacheView.ColSetL();
    TInt updateColNo = columns->ColNo( KLastAccessedCol );
    delete columns;
    TTime currentTime;
    currentTime.UniversalTime();
    //update the current time value
    aCacheView.GetL();
    aCacheView.UpdateL();
    aCacheView.SetColL( updateColNo,currentTime );
    aCacheView.PutL();
    IRLOG_DEBUG( "IRCacheMgmt::UpdateTrustPeriod - Exiting." );
    }

// ---------------------------------------------------------------------------
// Function : CIRCacheMgmt::CheckValidity()
// Checks the freshness of the cache
// ---------------------------------------------------------------------------
//
TBool CIRCacheMgmt::CheckValidity( const TTime& aCreateTime,
        TInt aTrustPeriod ) const
    {
    //Algo****
    //1.get the last creation time from the table for the request
    //2.calculate creation-time + trust-period < current-time *
    //3.if true the cache is valid
    //4.else do conditional-get
    IRLOG_DEBUG( "CIRCacheMgmt::CheckValidity - Entering" );
    TTimeIntervalSeconds trustPeriod( aTrustPeriod );
    TTimeIntervalSeconds diff;
    TTime currentDeviceTime;
    currentDeviceTime.UniversalTime();
    currentDeviceTime.SecondsFrom( aCreateTime,diff );
    //Has the cache expired
    if ( diff<trustPeriod )
        {
        IRLOG_DEBUG( "CIRCacheMgmt::CheckValidity - Exiting (1)." );
        return  ETrue;
        }
      //Else return false
     else
         {
         IRLOG_DEBUG( "CIRCacheMgmt::CheckValidity - Exiting (2)." );
         return  EFalse; 
         }
    }


// ---------------------------------------------------------------------------
// Function : CIRCacheMgmt::CacheCategoryItemsL()
// Caches the category array of data.By externalizing it into a file.
// ---------------------------------------------------------------------------
//

EXPORT_C void CIRCacheMgmt::CacheCategoryItemsL(
    CArrayPtrFlat<CIRBrowseCatagoryItems>& aPtrCategory,
    const TDesC& aName, const CIRHttpResponseData& aResponseHeaders )
    {
    //Algo****
    //1.check if similar request is cached
    //2.delete if similar request is already present
    //3.prepare a view for insertion
    //4.push in the requested data as serialized data
    
    IRLOG_DEBUG( "IRCacheMgmt::CacheCategoryItemsL - Entering" );
    TInt error = OpenCacheDb();
    if ( KErrNone != error )
        {
        //no problems if this function returns here.
        //normal flow won't get affected.
        //only a particular request wont get cached
        return; 
        }
    
    //clear similar data from the cache table before updating it with fresh data
    //DELETE FROM KCacheTable WHERE KDataTypeCol = TYPE AND KDataIdCol = DATAID 
    _LIT( query,"DELETE FROM %S WHERE %S = %d AND %S = '%S'" );
    HBufC* sqlQuery = HBufC::NewLC( query().Length() + KDataTypeCol().Length() +
                      KCacheTable().Length() + KDataIdCol().Length() +
                      KDefaultRealWidth + aName.Length() );
    sqlQuery->Des().Format( query,&KCacheTable,&KDataTypeCol,
                           ECatagory,&KDataIdCol,&aName );
    iCacheDb.Begin();
    //deletes a similar record if it existed.
    //no problem if it did not exist
    iCacheDb.Execute( *sqlQuery,EDbCompareNormal );
    CleanupStack::PopAndDestroy( sqlQuery );
    iCacheDb.Commit();
    
    //get the cached data view for preset column to update with fresh data.
    //SELECT * FROM KCacheTable WHERE KDataTypeCol = TYPE
    _LIT( query1,"SELECT * FROM %S WHERE %S = %d" );
    HBufC* sqlQuery1 = HBufC::NewLC( query1().Length() + 
                         KDataTypeCol().Length() +
                       KCacheTable().Length() + 
                       KDefaultRealWidth );
    sqlQuery1->Des().Format( query1,&KCacheTable,&KDataTypeCol,ECatagory );
    
    RDbView cacheTableView;
    User::LeaveIfError( cacheTableView.Prepare( iCacheDb,*sqlQuery1 ) );
    CleanupStack::PopAndDestroy( sqlQuery1 );
    CleanupClosePushL( cacheTableView );
    
    User::LeaveIfError( cacheTableView.EvaluateAll() );
    
    //prepare insertion values
    //will go into last accessed aswell as the creation time fields
    TTime currentTime;
    currentTime.UniversalTime();
    TInt countItems=aPtrCategory.Count();
    TInt maxAge;
    if ( aResponseHeaders.iMaxAge.Length() == 0 )
        {
        maxAge=iTrustPeriod.Int();
        }
    else
        {
        TLex8 conv( aResponseHeaders.iMaxAge );
        conv.Val( maxAge ); 
        maxAge = maxAge * KDefaultInterval;
        }
    CDbColSet* columns = cacheTableView.ColSetL();
    CleanupStack::PushL( columns );
    //write stream
    RDbColWriteStream writeStream;
    //start insertion into the view
    //insert a new row
    cacheTableView.InsertL();
    //set values
    cacheTableView.SetColL( columns->ColNo( KDataTypeCol ), ECatagory );
    cacheTableView.SetColL( columns->ColNo( KDataIdCol ), aName );
    cacheTableView.SetColL( columns->ColNo( KTrustPeriodCol ), maxAge );
    cacheTableView.SetColL( columns->ColNo( KItemCount ), countItems );
    //to be set only if available
    cacheTableView.SetColL( columns->ColNo( KLastModifiedCol ),
                           aResponseHeaders.iLastModified );
    cacheTableView.SetColL( columns->ColNo( KLastAccessedCol ), currentTime );
    cacheTableView.SetColL( columns->ColNo( KTimeCreation ), currentTime );
    //open stream
    writeStream.OpenLC( cacheTableView,columns->ColNo( KCachedDataCol ) );
    for ( TInt iter = 0; iter < countItems; iter++ )
        {
        aPtrCategory[iter]->ExternalizeL( writeStream );
        } 
    writeStream.CommitL();
    CleanupStack::PopAndDestroy( &writeStream );
    cacheTableView.PutL();
    CleanupStack::PopAndDestroy( columns );
    CleanupStack::PopAndDestroy( &cacheTableView );
    CloseDb();
    IRLOG_DEBUG( "IRCacheMgmt::CacheCategoryItemsL - Exiting." );
    
    }

// ---------------------------------------------------------------------------
// Function : CIRCacheMgmt::CacheChannelItemsL()
// Caches the channel objects array of data.By externalizing it into a file..
// ---------------------------------------------------------------------------
//

EXPORT_C void CIRCacheMgmt::CacheChannelItemsL(
    CArrayPtrFlat<CIRBrowseChannelItems>& aPtrChannel,
    const TDesC& aName,const CIRHttpResponseData& aResponseHeaders )
    {
    //Algo****
    //1.check if similar request is cached
    //2.delete if similar request is already present
    //3.prepare a view for insertion
    //4.push in the cached data request as serialized data
    IRLOG_DEBUG( "CIRCacheMgmt::CacheChannelItemsL - Entering" );
    
    TInt error = OpenCacheDb();
    if ( KErrNone != error )
        {
        //no problems if this function returns here.
        //normal flow won't get affected.
        //only a particular request wont get cached
        return; 
        }
    
    //convert the id information of descriptor to int value for comparision
    //clear data from the cache table before updating it with fresh data
    //DELETE FROM KCacheTable WHERE KDataTypeCol = TYPE AND KDataIdCol = DATAID 
    _LIT( query,"DELETE FROM %S WHERE %S = %d AND %S = '%S'" );
    HBufC* sqlQuery = HBufC::NewLC( query().Length() + KDataTypeCol().Length() +
                      KCacheTable().Length() + KDataIdCol().Length() +
                      KDefaultRealWidth + aName.Length() );
    sqlQuery->Des().Format( query,&KCacheTable,&KDataTypeCol,
    EChannels,&KDataIdCol,&aName );
    iCacheDb.Begin();
    //deletes a similar record if it existed.
    //no problem if it did not exist
    iCacheDb.Execute( *sqlQuery,EDbCompareNormal );
    CleanupStack::PopAndDestroy( sqlQuery );
    iCacheDb.Commit();
    
    //get the cached data view for preset column to update with fresh data.
    //SELECT * FROM KCacheTable WHERE KDataTypeCol = TYPE
    _LIT( query1,"SELECT * FROM %S WHERE %S = %d" );
    HBufC* sqlQuery1 = HBufC::NewLC( query1().Length() + 
                       KDataTypeCol().Length() +
                       KCacheTable().Length() + KDefaultRealWidth );
    RDbView cacheTableView;
    sqlQuery1->Des().Format( query1,&KCacheTable,&KDataTypeCol,EChannels ); 
    User::LeaveIfError( cacheTableView.Prepare( iCacheDb,*sqlQuery1 ) );
    CleanupStack::PopAndDestroy( sqlQuery1 );
    CleanupClosePushL( cacheTableView );
    
    User::LeaveIfError( cacheTableView.EvaluateAll() );
    
    //prepare insertion values
    //will go into last accessed aswell as the creation time fields
    TTime currentTime;
    currentTime.UniversalTime();
    TInt countItems=aPtrChannel.Count();
    TInt maxAge;
    if ( aResponseHeaders.iMaxAge.Length() == 0 )
        {
        maxAge=iTrustPeriod.Int();
        }
    else
        {
        TLex8 conv( aResponseHeaders.iMaxAge );
        conv.Val( maxAge ); 
        maxAge = maxAge * KDefaultInterval;
        }
    TBuf<KHttpDateLength>lastModified;
    
    CDbColSet* columns = cacheTableView.ColSetL();
    CleanupStack::PushL( columns );
    //write stream
    RDbColWriteStream writeStream;
    
    //start insertion into the view
    //insert a new row
    cacheTableView.InsertL();
    //set values
    cacheTableView.SetColL( columns->ColNo( KDataTypeCol ), EChannels );
    cacheTableView.SetColL( columns->ColNo( KDataIdCol ), aName );
    cacheTableView.SetColL( columns->ColNo( KTrustPeriodCol ), maxAge );
    cacheTableView.SetColL( columns->ColNo( KItemCount ), countItems );
    //to be set only if available
    cacheTableView.SetColL( columns->ColNo( KLastModifiedCol ),
                           aResponseHeaders.iLastModified );
    //
    cacheTableView.SetColL( columns->ColNo( KLastAccessedCol ), currentTime );
    cacheTableView.SetColL( columns->ColNo( KTimeCreation ), currentTime );
    //open stream
    writeStream.OpenLC( cacheTableView,columns->ColNo( KCachedDataCol ) );
    for ( TInt iter = 0; iter < countItems; iter++ )
        {
        aPtrChannel[iter]->ExternalizeL( writeStream );
        } 
    writeStream.CommitL();
    CleanupStack::PopAndDestroy( &writeStream );
    cacheTableView.PutL();
    CleanupStack::PopAndDestroy( columns );
    CleanupStack::PopAndDestroy( &cacheTableView );
    CloseDb();
    IRLOG_DEBUG( "CIRCacheMgmt::CacheChannelItemsL - Exiting." );
    
    }

// ---------------------------------------------------------------------------
// Function : CIRCacheMgmt::CachePresetItemL()
// Caches the preset object .By externalizing it into a file.
//multiple presets can be cached at one time
// ---------------------------------------------------------------------------
//

EXPORT_C void CIRCacheMgmt::CachePresetItemL(
    CArrayPtrFlat<CIRIsdsPreset>& aPtrPresets,
    const TDesC& aName,const CIRHttpResponseData& aResponseHeaders )
    {
    //Algo****
    //1.check if similar request is cached
    //2.delete if similar request is already present
    //3.prepare a view for insertion
    //4.push in the cached data request as serialized data
    IRLOG_DEBUG( "CIRCacheMgmt::CachePresetItemL - Entering" );
    
    TInt error = OpenCacheDb();
    if ( KErrNone != error )
        {
        //no problems if this function returns here.
        //normal flow won't get affected.
        //only a particular request wont get cached
        return; 
        }
    //convert the id information of descriptor to int value for comparision
    //clear data from the cache table before updating it with fresh data
    //DELETE FROM KCacheTable WHERE KDataTypeCol = TYPE AND KDataIdCol = DATAID 
    _LIT( query,"DELETE FROM %S WHERE %S = %d AND %S = '%S'" );
    HBufC* sqlQuery = HBufC::NewLC( query().Length() + 
                      KDataTypeCol().Length() +
                      KCacheTable().Length() + KDataIdCol().Length() +
                      KDefaultRealWidth + aName.Length() );
    sqlQuery->Des().Format( query,&KCacheTable,&KDataTypeCol,
                           EPresets,&KDataIdCol,&aName );
    iCacheDb.Begin();
    //deletes a similar record if it existed.
    //no problem if it did not exist
    iCacheDb.Execute( *sqlQuery,EDbCompareNormal );
    CleanupStack::PopAndDestroy( sqlQuery );
    iCacheDb.Commit();
    
    //get the cached data view for preset column to update with fresh data.
    //SELECT * FROM KCacheTable WHERE KDataTypeCol = TYPE
    _LIT( query1,"SELECT * FROM %S WHERE %S = %d" );
    HBufC* sqlQuery1 = HBufC::NewLC( query1().Length() + 
                       KDataTypeCol().Length() +
                       KCacheTable().Length() + KDefaultRealWidth );
    sqlQuery1->Des().Format( query1,&KCacheTable,&KDataTypeCol,EPresets );  
    RDbView cacheTableView;
    User::LeaveIfError( cacheTableView.Prepare( iCacheDb,*sqlQuery1 ) );
    CleanupStack::PopAndDestroy( sqlQuery1 );
    CleanupClosePushL( cacheTableView );
    
    User::LeaveIfError( cacheTableView.EvaluateAll() );
    
    //prepare insertion values
    //will go into last accessed aswell as the creation time fields
    TTime currentTime;
    currentTime.UniversalTime();
    
    TInt maxAge;
    if ( aResponseHeaders.iMaxAge.Length() == 0 )
        {
        maxAge=iTrustPeriod.Int();
        }
    else
        {
        TLex8 conv( aResponseHeaders.iMaxAge );
        conv.Val( maxAge ); 
        maxAge = maxAge * KDefaultInterval;
        }
    TBuf<KHttpDateLength>lastModified;
    //default time set to 24hrs
    //use iTrustPeriod
    //type set to EPresets
    //id set to dataId
    //last modified time kept null
    //item count set to 1
    CDbColSet* columns = cacheTableView.ColSetL();
    CleanupStack::PushL( columns );
    //write stream
    RDbColWriteStream writeStream;
    TInt countItems;
    countItems = aPtrPresets.Count();
    //start insertion into the view
    //insert a new row
    cacheTableView.InsertL();
    //set values
    cacheTableView.SetColL( columns->ColNo( KDataTypeCol ), EPresets );
    cacheTableView.SetColL( columns->ColNo( KDataIdCol ), aName );
    cacheTableView.SetColL( columns->ColNo( KTrustPeriodCol ), maxAge );
    cacheTableView.SetColL( columns->ColNo( KItemCount ), countItems );
    //to be set only if available
    cacheTableView.SetColL( columns->ColNo( KLastModifiedCol ),
    aResponseHeaders.iLastModified );
    //
    cacheTableView.SetColL( columns->ColNo( KLastAccessedCol ), currentTime );
    cacheTableView.SetColL( columns->ColNo( KTimeCreation ), currentTime );
    //open stream
    writeStream.OpenLC( cacheTableView,columns->ColNo( KCachedDataCol ) );
    for ( TInt iter = 0; iter < countItems; iter++ )
        {
        aPtrPresets[iter]->ExternalizeL( writeStream );
        }
    writeStream.CommitL();
    CleanupStack::PopAndDestroy( &writeStream );
    cacheTableView.PutL();
      
    CleanupStack::PopAndDestroy( columns );
    CleanupStack::PopAndDestroy( &cacheTableView );
    CloseDb();
    IRLOG_DEBUG( "CIRCacheMgmt::CachePresetItemL - Exiting." );
    }
// ---------------------------------------------------------------------------
// Function : CIRCacheMgmt::CacheOtaInfoL()
// Caches the OTA information  .By externalizing it into a db.
// ---------------------------------------------------------------------------
//

EXPORT_C void CIRCacheMgmt::CacheOtaInfoL( const CIROTAUpdate& aOta,
    const TDesC& aName,
    const CIRHttpResponseData& aResponseHeaders )
    {
    //Algo****
    //1.check if similar request is cached
    //2.delete if similar request is already present
    //3.prepare a view for insertion
    //4.push in the cached data request as serialized data
    IRLOG_DEBUG( "CIRCacheMgmt::CacheOtaInfoL - Entering" );
    
    TInt error = OpenCacheDb();
    if ( KErrNone != error )
        {
        //no problems if this function returns here.
        //normal flow won't get affected.
        //only a particular request wont get cached
        return; 
        }
    //convert the id information of descriptor to int value for comparision
    //clear data from the cache table before updating it with fresh data
    //DELETE FROM KCacheTable WHERE KDataTypeCol = TYPE AND KDataIdCol = DATAID 
    _LIT( query,"DELETE FROM %S WHERE %S = %d AND %S = '%S'" );
    HBufC* sqlQuery = HBufC::NewLC( query().Length() + 
                      KDataTypeCol().Length() +
                      KCacheTable().Length() + KDataIdCol().Length() +
                      KDefaultRealWidth + aName.Length() );
    sqlQuery->Des().Format( query,&KCacheTable,&KDataTypeCol,
        EOtaInfo,&KDataIdCol,&aName );
        
    User::LeaveIfError( iCacheDb.Begin() );
    //deletes a similar record if it existed.
    //no problem if it did not exist
    iCacheDb.Execute( *sqlQuery,EDbCompareNormal );
    CleanupStack::PopAndDestroy( sqlQuery );
    iCacheDb.Commit();
    
    //get the cached data view for preset column to update with fresh data.
    //SELECT * FROM KCacheTable WHERE KDataTypeCol = TYPE
    _LIT( query1,"SELECT * FROM %S WHERE %S = %d" );
    HBufC* sqlQuery1 = HBufC::NewLC( query1().Length() + 
                       KDataTypeCol().Length() +
                       KCacheTable().Length() + KDefaultRealWidth );
        
    sqlQuery1->Des().Format( query1,&KCacheTable,&KDataTypeCol,EOtaInfo );  
    RDbView cacheTableView;
    User::LeaveIfError( cacheTableView.Prepare( iCacheDb,*sqlQuery1 ) );
    CleanupStack::PopAndDestroy( sqlQuery1 );
    CleanupClosePushL( cacheTableView );
    
    User::LeaveIfError( cacheTableView.EvaluateAll() );
    
    //prepare insertion values
    //will go into last accessed aswell as the creation time fields
    TTime currentTime;
    currentTime.UniversalTime();
    
    TInt maxAge;
    if ( aResponseHeaders.iMaxAge.Length() == 0 )
        {
        maxAge=iTrustPeriod.Int();
        }
    else
        {
        TLex8 conv( aResponseHeaders.iMaxAge );
        conv.Val( maxAge ); 
        maxAge = maxAge * KDefaultInterval;
        }
    TBuf<KHttpDateLength>lastModified;
    //default time set to 24hrs
    //use iTrustPeriod
    //type set to EPresets
    //id set to dataId
    //last modified time kept null
    //item count set to 1
    CDbColSet* columns = cacheTableView.ColSetL();
    CleanupStack::PushL( columns );
    //write stream
    RDbColWriteStream writeStream;
    //start insertion into the view
    //insert a new row
    cacheTableView.InsertL();
    //set values
    cacheTableView.SetColL( columns->ColNo( KDataTypeCol ), EOtaInfo );
    cacheTableView.SetColL( columns->ColNo( KDataIdCol ), aName );
    cacheTableView.SetColL( columns->ColNo( KTrustPeriodCol ), maxAge );
    cacheTableView.SetColL( columns->ColNo( KItemCount ), 1 );
    //to be set only if available
    cacheTableView.SetColL( columns->ColNo( KLastModifiedCol ),
                           aResponseHeaders.iLastModified );
    //
    cacheTableView.SetColL( columns->ColNo( KLastAccessedCol ), currentTime );
    cacheTableView.SetColL( columns->ColNo( KTimeCreation ), currentTime );
    //open stream
    writeStream.OpenLC( cacheTableView,columns->ColNo( KCachedDataCol ) );
    aOta.ExternalizeL( writeStream );
    writeStream.CommitL();
    CleanupStack::PopAndDestroy( &writeStream );
    cacheTableView.PutL();
      
    CleanupStack::PopAndDestroy( columns );
    CleanupStack::PopAndDestroy( &cacheTableView );
    CloseDb();
    IRLOG_DEBUG( "CIRCacheMgmt::CacheOtaInfoL - Exiting." );
    }
  
// ---------------------------------------------------------------------------
// Function : CIRCacheMgmt::CacheLogoL()
// Caches the logo data  .By externalizing it into a db.
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRCacheMgmt::CacheLogoL( const TDesC8& aData, const TDesC& aUrl,
                           const CIRHttpResponseData& aResponseHeaders )
    {
    //Algo****
    //1.check if similar request is cached
    //2.delete if similar request is already present
    //3.prepare a view for insertion
    //4.push in the cached data request as serialized data
    IRLOG_DEBUG( "CIRCacheMgmt::CacheLogoL - Entering" );
    
    
    TInt error = OpenCacheDb();
    if ( KErrNone != error )
        {
        //no problems if this function returns here.
        //normal flow won't get affected.
        //only a particular request wont get cached
        return; 
        }
    //convert the id information of descriptor to int value for comparision
    //clear data from the cache table before updating it with fresh data
    //DELETE FROM KCacheTable WHERE KDataTypeCol = TYPE AND KDataIdCol = DATAID 
    _LIT( query,"DELETE FROM %S WHERE %S = %d AND %S = '%S'" );
    HBufC* sqlQuery = HBufC::NewLC( query().Length() + KDataTypeCol().Length() +
                      KCacheTable().Length() + KDataIdCol().Length() +
                      KDefaultRealWidth + aUrl.Length() );
    sqlQuery->Des().Format( query,&KCacheTable,&KDataTypeCol,
                           ELogo,&KDataIdCol,&aUrl );
    iCacheDb.Begin();
    //deletes a similar record if it existed.
    //no problem if it did not exist
    iCacheDb.Execute( *sqlQuery,EDbCompareNormal );
    CleanupStack::PopAndDestroy( sqlQuery );
    iCacheDb.Commit();  
    
    //get the cached data view for preset column to update with fresh data.
    //SELECT * FROM KCacheTable WHERE KDataTypeCol = TYPE
    _LIT( query1,"SELECT * FROM %S WHERE %S = %d" );
    HBufC* sqlQuery1 = HBufC::NewLC( query1().Length() + KDataTypeCol().Length() +
                       KCacheTable().Length() + KDefaultRealWidth );
    sqlQuery1->Des().Format( query1,&KCacheTable,&KDataTypeCol,ELogo ); 
    RDbView cacheTableView;
    User::LeaveIfError( cacheTableView.Prepare( iCacheDb,*sqlQuery1 ) );
    CleanupStack::PopAndDestroy( sqlQuery1 );
    CleanupClosePushL( cacheTableView );
    
    User::LeaveIfError( cacheTableView.EvaluateAll() );   
    
    //prepare insertion values
    //will go into last accessed aswell as the creation time fields
    TTime currentTime;
    currentTime.UniversalTime();
    
    TInt maxAge;
    if ( aResponseHeaders.iMaxAge.Length() == 0 )
        {
        maxAge=iTrustPeriod.Int();
        }
    else
        {
        TLex8 conv( aResponseHeaders.iMaxAge );
        conv.Val( maxAge ); 
        maxAge = maxAge * KDefaultInterval;
        }
    TBuf<KHttpDateLength>lastModified;
    //default time set to 24hrs
    //use iTrustPeriod
    //type set to ELogo
    //id set to dataId
    //last modified time kept null
    //item count set to 1
    CDbColSet* columns = cacheTableView.ColSetL();
    CleanupStack::PushL( columns );
    //write stream
    //start insertion into the view
    //insert a new row
    cacheTableView.InsertL();
    //set values
    cacheTableView.SetColL( columns->ColNo( KDataTypeCol ), ELogo );
    cacheTableView.SetColL( columns->ColNo( KDataIdCol ), aUrl );
    cacheTableView.SetColL( columns->ColNo( KTrustPeriodCol ), maxAge );
    cacheTableView.SetColL( columns->ColNo( KItemCount ), 1 );
    //to be set only if available
    cacheTableView.SetColL( columns->ColNo( KLastModifiedCol ),
    aResponseHeaders.iLastModified );
    //
    cacheTableView.SetColL( columns->ColNo( KLastAccessedCol ), currentTime );
    cacheTableView.SetColL( columns->ColNo( KTimeCreation ), currentTime );
    cacheTableView.SetColL( columns->ColNo( KCachedDataCol ), aData );
    
    cacheTableView.SetColL( columns->ColNo( KETagHeader ), 
                           aResponseHeaders.iETag );
    
    cacheTableView.PutL();
    
      
    CleanupStack::PopAndDestroy( columns );
    CleanupStack::PopAndDestroy( &cacheTableView );
    CloseDb();
    IRLOG_DEBUG( "CIRCacheMgmt::CacheLogoL - Exiting." );         
        
    }
  
  
  
  
// ---------------------------------------------------------------------------
// Function : CIRCacheMgmt::CreateCacheTableL()
// creates the cache table
// ---------------------------------------------------------------------------
//
//CacheTable
// -----------------------------------------------------------------------------------------------------
//| RowIndex|DataType|DataId|TrustPeriod|LastModified  |LastAccessed|Created|ItemCount|CachedData|
//------------------------------------------------------------------------------------------------
//| auto    |TInt    |DesC  |TInt       |TDesC/DateTime|TTime       |Time   |TInt     | Streamed |
//|increment|0,1,2,NotNull|NotNull|  NotNull  |              |            |       |         | Data     |
//------------------------------------------------------------------------------------------------------
void CIRCacheMgmt::CreateCacheTableL()
    {
    IRLOG_DEBUG( "IRCacheMgmt::CreateCacheTableL - Entering." );
    //the row index column which autoincrements
    TDbCol rowIndexCol( KRowIndexCol, EDbColInt32 );   
    rowIndexCol.iAttributes = TDbCol::EAutoIncrement;
    
    //the datatype column,to identify between   ECatagory = 0,EChannels=1,EPresets=2
    TDbCol dataTypeCol( KDataTypeCol, EDbColInt32 );
    dataTypeCol.iAttributes = TDbCol::ENotNull;
     
    //the dataID column,to uniquely identify the cached data
    TDbCol dataId( KDataIdCol, EDbColLongText16 );
    dataId.iAttributes = TDbCol::ENotNull; 
    
    //the trust period column to calculate cache validity
    TDbCol trustPeriodCol( KTrustPeriodCol, EDbColInt32 );
    
    //the lastmodified column to fetch data conditionally
    TDbCol lastModifiedCol( KLastModifiedCol, EDbColDateTime );
    
    
    //the last accessed column used for table clean-up
    TDbCol creationTimeCol( KTimeCreation, EDbColDateTime );
      
    //the time of creation saved to calculate the cache validity
    TDbCol lastAccessedCol( KLastAccessedCol, EDbColDateTime );
    
    //the number of items in the data field
    TDbCol itemCountCol( KItemCount, EDbColInt32 );
    
    //the data column to store cached data
    //The column stores a potentially large amount of Unicode text data.
    TDbCol cachedDataCol( KCachedDataCol, EDbColLongText8 );
    
    //the ETag header received along with the logo data
    TDbCol etagHeader( KETagHeader, EDbColText8 );
    
    //colset
    CDbColSet* cacheTablerColSet = CDbColSet::NewLC();
    //populate colset
    cacheTablerColSet->AddL( rowIndexCol );
    cacheTablerColSet->AddL( dataTypeCol );
    cacheTablerColSet->AddL( dataId );
    cacheTablerColSet->AddL( trustPeriodCol );
    cacheTablerColSet->AddL( lastModifiedCol );
    cacheTablerColSet->AddL( creationTimeCol );
    cacheTablerColSet->AddL( lastAccessedCol );
    cacheTablerColSet->AddL( itemCountCol );
    cacheTablerColSet->AddL( cachedDataCol );
    cacheTablerColSet->AddL( etagHeader );
    
    // Create the CacheTable table
    User::LeaveIfError( iCacheDb.CreateTable( KCacheTable, *cacheTablerColSet ) );
    
    CleanupStack::PopAndDestroy( cacheTablerColSet ); 
    IRLOG_DEBUG( "IRCacheMgmt::CreateCacheTableL - Exiting." );
    }
  
// ---------------------------------------------------------------------------
// Function : CIRCacheMgmt::CreateCacheIndexL()
// creates the indices for cache table
// ---------------------------------------------------------------------------
//
void CIRCacheMgmt::CreateCacheIndexL()
    {
    IRLOG_DEBUG( "IRCacheMgmt::CreateCacheIndexL - Entering." );
    TDbKeyCol rowIndexCol( KRowIndexCol );
    TDbKeyCol dataTypeCol( KDataTypeCol );
    TDbKeyCol dataIdCol( KDataIdCol,KDataIdIndexSize );
    TDbKeyCol timeCreationCol( KTimeCreation );
    TDbKeyCol lastAccessedCol( KLastAccessedCol );
     
    CDbKey* index;
    //index on the auto increment row id
    index = CDbKey::NewLC();   
    index->AddL( rowIndexCol );
    User::LeaveIfError( iCacheDb.CreateIndex( KRowIndexColIndex, KCacheTable,
       *index ) );
    CleanupStack::PopAndDestroy( index );
    
    //index on the datatype column
    index = CDbKey::NewLC();   
    index->AddL( dataTypeCol );
    User::LeaveIfError( iCacheDb.CreateIndex( KDataTypeColIndex, KCacheTable, 
      *index ) );
    CleanupStack::PopAndDestroy( index );
    
    //index on the data id column
    index = CDbKey::NewLC();   
    index->AddL( dataIdCol );
    User::LeaveIfError( iCacheDb.CreateIndex( KDataIdColIndex, KCacheTable, 
      *index ) );
    CleanupStack::PopAndDestroy( index );
    
    //index on the time of record creation column
    index = CDbKey::NewLC();   
    index->AddL( timeCreationCol );
    User::LeaveIfError( iCacheDb.CreateIndex( KTimeCreationIndex, KCacheTable, 
      *index ) );
    CleanupStack::PopAndDestroy( index );
    
    //index on the last accessed column
    index = CDbKey::NewLC();   
    index->AddL( lastAccessedCol );
    User::LeaveIfError( iCacheDb.CreateIndex( KLastAccessedColIndex, KCacheTable,
       *index ) );
    CleanupStack::PopAndDestroy( index );
    IRLOG_DEBUG( "IRCacheMgmt::CreateCacheIndexL - Exiting." );
    }
// ---------------------------------------------------------------------------
//void CIRCacheMgmt::UpdateTrustPeriod()
//updates the trust period for a not-modified responce
// ---------------------------------------------------------------------------

//
EXPORT_C void CIRCacheMgmt::UpdateTrustPeriodL( TInt aType, const TDesC& aName,
    CIRHttpResponseData& aResponseHeaders )
    { 
    IRLOG_DEBUG( "IRCacheMgmt::UpdateTrustPeriodL - Entering" );
    TInt error = OpenCacheDb();
    if ( KErrNone != error )
        {
          IRLOG_ERROR2( "IRCacheMgmt::UpdateTrustPeriodL - \
          Error while opening cache DB (%d).", error );      
        //no problems if this function returns here.
        //normal flow won't get affected.
        //only a particular request wont get cached
        return; 
        }
    
    //SELECT * FROM KCacheTable WHERE KDataTypeCol = TYPE AND KDataIdCol = DATAID 
    _LIT( query,"SELECT * FROM %S WHERE %S = %d AND %S = '%S'" );
    HBufC* sqlQuery = HBufC::NewLC( query().Length() + KCacheTable().Length() +
                      KDataIdCol().Length() + KDataTypeCol().Length() + aName.Length()+
                      KDefaultRealWidth );
       
    sqlQuery->Des().Format( query,&KCacheTable,&KDataTypeCol,
                           aType,&KDataIdCol,&aName );
        
    //create view
    RDbView cacheTableView;
    User::LeaveIfError( cacheTableView.Prepare( iCacheDb,*sqlQuery ) );
    CleanupStack::PopAndDestroy( sqlQuery );
    CleanupClosePushL( cacheTableView );
    
    User::LeaveIfError( cacheTableView.EvaluateAll() );
    
    //prepare the update data
    //will go into last accessed aswell as the creation time fields
    TTime currentTime;
    currentTime.UniversalTime();
    //get the new trust period from the headers
    TInt maxAge;
    if ( aResponseHeaders.iMaxAge.Length() == 0 )
        {
        maxAge=iTrustPeriod.Int();
        }
    else
        {
        TLex8 conv( aResponseHeaders.iMaxAge );
        conv.Val( maxAge ); 
        maxAge = maxAge * KDefaultInterval;
        }
    CDbColSet* columns = cacheTableView.ColSetL();
    CleanupStack::PushL( columns );
    cacheTableView.FirstL();
    if ( cacheTableView.AtRow() )
        {
        cacheTableView.UpdateL();
        //update values
        cacheTableView.SetColL( columns->ColNo( KTrustPeriodCol ), maxAge );
        cacheTableView.SetColL( columns->ColNo( KLastAccessedCol ), currentTime );
        cacheTableView.SetColL( columns->ColNo( KTimeCreation ), currentTime );
        cacheTableView.PutL();
        }
    
    CleanupStack::PopAndDestroy( columns );
    CleanupStack::PopAndDestroy( &cacheTableView );
    CloseDb();
    IRLOG_DEBUG( "IRCacheMgmt::UpdateTrustPeriod - Exiting." ); 
    }
  
// ---------------------------------------------------------------------------
//  Used to dcontrol the cache table size by compacting and clearing up unused items
// ---------------------------------------------------------------------------
//
void CIRCacheMgmt::CheckSizeL()
    {
    IRLOG_DEBUG( "IRCacheMgmt::CheckSizeL - Entering." );
    OpenCacheDb();
    RDbDatabase::TSize dbSize = iCacheDb.Size();
    iCacheDbSize = dbSize.iSize;
    TInt size = CacheSize();
    if ( size >= KCacheCriticalSizeLimit )
        {
        RemoveOldUnusedDataL();
        OpenCacheDb();
        iCacheDb.Compact();
        //refresh the size information
        dbSize = iCacheDb.Size();
        iCacheDbSize = dbSize.iSize;
        }
    CloseDb();
    IRLOG_DEBUG( "IRCacheMgmt::CheckSizeL - Exiting." );
    }

// ---------------------------------------------------------------------------
// Used to control the cache table size by compacting and clearing up unused items
// ---------------------------------------------------------------------------
//
void CIRCacheMgmt::RemoveOldUnusedDataL()
    {
    IRLOG_DEBUG( "IRCacheMgmt::RemoveOldUnusedDataL - Entering." );
    OpenCacheDb();
    RDbTable cacheTable;
    //open the table in updatable mode
    User::LeaveIfError( cacheTable.Open( iCacheDb, KCacheTable, 
        cacheTable.EUpdatable ) );
    CleanupClosePushL( cacheTable );
    cacheTable.Reset();
    //set index as the last accessed time
    cacheTable.SetIndex( KLastAccessedCol );
    //begin transaction
    iCacheDb.Begin();
    // no of rows deleted is one fourth of the total number of rows
    TInt deleteRows = cacheTable.CountL()/KPercentRowsDeleted;
    TInt inc = 0;
    //delete incrementally
    for ( cacheTable.FirstL(); ( ( inc < deleteRows ) && cacheTable.AtRow() ); inc++ )
        {
        cacheTable.GetL();
        cacheTable.DeleteL();
        cacheTable.NextL();
        }
    //commit transaction
    iCacheDb.Commit();
    CleanupStack::PopAndDestroy( &cacheTable );
    CloseDb();
    IRLOG_DEBUG( "IRCacheMgmt::RemoveOldUnusedDataL - Exiting." );
    }
  
// ---------------------------------------------------------------------------
// Used to get the cache table size
// ---------------------------------------------------------------------------
//
TInt CIRCacheMgmt::CacheSize()
    {
    IRLOG_DEBUG( "IRCacheMgmt::CacheSize - Entering." );
    OpenCacheDb();
    RDbDatabase::TSize dbSize = iCacheDb.Size();
    iCacheDbSize = dbSize.iSize;
    CloseDb();
    IRLOG_DEBUG( "IRCacheMgmt::CacheSize - Exiting." );
    return iCacheDbSize;
    }
// ---------------------------------------------------------------------------
//remove OTA info from cache if data is not valid any longer
// ---------------------------------------------------------------------------
EXPORT_C void CIRCacheMgmt::RemoveOtaInfoL()
    {
    IRLOG_DEBUG( "IRCacheMgmt::RemoveOtaInfoL - Entering." );
    //check if the silence period param is set.
    //if not set that means the cached ota info if present is not valid.
    if ( iSettings->GetSilencePeriodL() == 0 )
        {
        TInt error = OpenCacheDb();
        if ( KErrNone != error )
            {
            //no problems if this function returns here.
            //normal flow won't get affected.
            //only a particular request wont get cached
            return; 
            }
        //clear data from the cache table 
        //DELETE FROM KCacheTable WHERE KDataTypeCol = TYPE 
        _LIT( query,"DELETE FROM %S WHERE %S = %d" );
        HBufC* sqlQuery = HBufC::NewLC( query().Length() + 
                          KDataTypeCol().Length() +
                          KCacheTable().Length() + KDataIdCol().Length() );
        sqlQuery->Des().Format( query,&KCacheTable,&KDataTypeCol,EOtaInfo );
        User::LeaveIfError( iCacheDb.Begin() );
        //no problem if it did not exist
        iCacheDb.Execute( *sqlQuery,EDbCompareNormal );
        CleanupStack::PopAndDestroy( sqlQuery );
        iCacheDb.Commit();
            CloseDb();
        }
    IRLOG_DEBUG( "IRCacheMgmt::RemoveOtaInfoL - Exiting." );
    }



// ---------------------------------------------------------------------------
// Adds a cache observer
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRCacheMgmt::AddObserverL( MIRCacheObserver* aObserver )
    {
    IRLOG_DEBUG( "IRCacheMgmt::AddObserverL - Entering." );
    iCacheObservers.AppendL( aObserver );
    IRLOG_DEBUG( "IRCacheMgmt::AddObserverL - Exiting." );
    }

// ---------------------------------------------------------------------------
// Removes a cache observer
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRCacheMgmt::RemoveObserver( MIRCacheObserver* aObserver )
    {
    IRLOG_DEBUG( "IRCacheMgmt::RemoveObserver - Entering." );
    TInt objectIndex = iCacheObservers.Find( aObserver );
    
    if ( objectIndex != KErrNotFound )
        {
        iCacheObservers.Remove( objectIndex );
        }
    IRLOG_DEBUG( "IRCacheMgmt::RemoveObserver - Exiting." );
    }
