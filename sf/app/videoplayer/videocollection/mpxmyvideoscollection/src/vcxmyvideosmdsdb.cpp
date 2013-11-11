/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:   MPX My Videos collection plugin's MDS database class*
*/



//#include <harvesterclient.h>
#include <mpxlog.h>
#include <mdeconstants.h>
#include <mdequery.h>
#include <bautils.h>
#include <mpxmedia.h>
#include <vcxmyvideosdefs.h>
#include <mpxmediacontainerdefs.h>
#include <mpxmediageneraldefs.h>
#include <mpxmediageneralextdefs.h>
#include <mpxmediaarray.h>
#include <mpxmediavideodefs.h>
#include "vcxmyvideosmdsdb.h"
#include "vcxmyvideoscollectionutil.h"
#include "vcxmyvideosmdsalbums.h"
#include "vcxmyvideoscollectionutil.h"
#include "vcxmyvideosmdscmdqueue.h"

_LIT( KVcxVideoObjectName, "Video" );
                                                              //1 (ID)
_LIT( KVcxTitlePropertyName, "Title" );                       //2
_LIT( KVcxDescriptionPropertyName, "Description" );           //3
                                                              //4 (file path = URI)
_LIT( KVcxSizePropertyName, "Size" );                         //11 (5 also for compatibility)

_LIT( KVcxCreationDatePropertyName, "CreationDate" );         //6 creation date is saved to 2 properties
_LIT( KVcxTimeOffsetPropertyName, "TimeOffset" );             //6

_LIT( KVcxFlagsPropertyName, "RecordingFlags" );              //7 flags are saved
_LIT( KVcxDrmPropertyName, "DRM");                            //7 to both of these

_LIT( KVcxCopyrightPropertyName, "Copyright" );               //8
_LIT( KVcxItemTypePropertyName, "ItemType" );                 //9 (mime type)
_LIT( KVcxLastModifiedDatePropertyName, "LastModifiedDate" ); //12
_LIT( KVcxAgeProfilePropertyName, "AgeProfile" );             //13
_LIT( KVcxAudioLanguagePropertyName, "AudioLanguage" );       //14
_LIT( KVcxAuthorPropertyName, "Author" );                     //15
_LIT( KVcxOriginPropertyName, "Origin" );                     //16
_LIT( KVcxDurationPropertyName, "Duration" );                 //17
_LIT( KVcxLastPlayPositionPropertyName, "LastPlayPosition" ); //10
_LIT( KVcxRatingPropertyName, "Rating" );                     //19
_LIT( KVcxBitratePropertyName, "Bitrate" );                   //20
_LIT( KVcxAudioFourCcPropertyName, "AudioFourCC" );           //21
_LIT( KVcxWidthPropertyName, "Width" );                       //22
_LIT( KVcxHeightPropertyName, "Height" );                     //23
_LIT( KVcxArtistPropertyName, "Artist" );                     //24


// ---------------------------------------------------------------------------
// CVcxMdsShutdownMonitor::NewL()
// ---------------------------------------------------------------------------
//
CVcxMdsShutdownMonitor* CVcxMdsShutdownMonitor::NewL( MVcxMdsShutdownMonitorObserver& aObserver,
                                                const TUid& aKeyCategory,
                                                const TInt aPropertyKey,
                                                TBool aDefineKey)
    {
    CVcxMdsShutdownMonitor* self = new( ELeave )CVcxMdsShutdownMonitor( aObserver, 
                                                                  aKeyCategory,
                                                                  aPropertyKey,
                                                                  aDefineKey);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CVcxMdsShutdownMonitor::CVcxMdsShutdownMonitor()
// ---------------------------------------------------------------------------
//
CVcxMdsShutdownMonitor::CVcxMdsShutdownMonitor( MVcxMdsShutdownMonitorObserver& aObserver,
                                          const TUid& aKeyCategory,
                                          const TInt aPropertyKey,
                                          TBool aDefineKey)
    : CActive( CActive::EPriorityStandard ), iObserver( aObserver ),
      iKeyCategory( aKeyCategory ), iPropertyKey(aPropertyKey), iDefineKey( aDefineKey )
    {   
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// CVcxMdsShutdownMonitor::ConstructL()
// ---------------------------------------------------------------------------
//
void CVcxMdsShutdownMonitor::ConstructL()
    { 
    // define P&S property types
    if ( iDefineKey )
        {
        RProperty::Define( iKeyCategory, iPropertyKey,
                          RProperty::EInt, KAllowAllPolicy, KPowerMgmtPolicy );
        }
    
    // attach to the property
    TInt err = iProperty.Attach( iKeyCategory, iPropertyKey,EOwnerThread );
    User::LeaveIfError( err );
    
    // wait for the previously attached property to be updated
    iProperty.Subscribe( iStatus );
    SetActive();
    }

// ---------------------------------------------------------------------------
// CVcxMdsShutdownMonitor::~CVcxMdsShutdownMonitor()
// ---------------------------------------------------------------------------
//
CVcxMdsShutdownMonitor::~CVcxMdsShutdownMonitor()
    {
    Cancel();
    iProperty.Close();
    }

// ---------------------------------------------------------------------------
// CVcxMdsShutdownMonitor::RunL()
// ---------------------------------------------------------------------------
//
void CVcxMdsShutdownMonitor::RunL()
    {
    // resubscribe before processing new value to prevent missing updates
    iProperty.Subscribe( iStatus );
    SetActive();
    
    // retrieve the value
    TInt value = 0;
    TInt err = iProperty.Get( value );
    MPX_DEBUG2("CVcxMyVideosMdsDb::RunL(): iProperty.Get(value); returns %d", err);
    
    User::LeaveIfError( err );

    iObserver.ShutdownNotification( value );    
    }

// ---------------------------------------------------------------------------
// CVcxMdsShutdownMonitor::DoCancel()
// ---------------------------------------------------------------------------
//
void CVcxMdsShutdownMonitor::DoCancel()
    {
    iProperty.Cancel();
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosMdsDb::CVcxMyVideosMdsDb
// ---------------------------------------------------------------------------
//
CVcxMyVideosMdsDb::CVcxMyVideosMdsDb( MVcxMyVideosMdsDbObserver* aObserver,
        RFs& aFs, MVcxMyVideosMdsAlbumsObserver* aAlbumsObserver )
: iFs( aFs ), iMdsDbObserver(aObserver), iAlbumsObserver(aAlbumsObserver) 
    {
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosMdsDb::ConstructL
// ---------------------------------------------------------------------------
//
void CVcxMyVideosMdsDb::ConstructL()
    {
    MPX_DEBUG1( "CVcxMyVideosMdsDb::ConstructL start" );

    iCmdQueue = CVcxMyVideosMdsCmdQueue::NewL( *this );

    iActiveSchedulerWait = new (ELeave) CActiveSchedulerWait;
    
    OpenMdsSessionL();

        
    iAlbums = CVcxMyVideosMdsAlbums::NewL( *this, iAlbumsObserver );
    
    TCallBack callBack( AsyncHandleQueryCompleted, this );    
    iAsyncHandleQueryCompleteCaller = new (ELeave) CAsyncCallBack( callBack,
            CActive::EPriorityStandard );

    iMdsShutdownMonitor = CVcxMdsShutdownMonitor::NewL(
            *this, KHarvesterPSShutdown, KMdSShutdown, EFalse );
    
    iEvents.Reset();

    MPX_DEBUG1( "CVcxMyVideosMdsDb::ConstructL exit" );
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosMdsDb::OpenMdsSessionL
// ---------------------------------------------------------------------------
//
void CVcxMyVideosMdsDb::OpenMdsSessionL()
    {
    MPX_DEBUG1( "CVcxMyVideosMdsDb::OpenMdsSessionL() start" );

    iMdsSessionError = KErrNone;

    delete iMdsSession;
    iMdsSession = NULL;
    iMdsSession = CMdESession::NewL( *this );
    if ( !iMdsSession )
        {
        User::Leave( KErrGeneral );
        }
    
    //  Wait until session opened
    iActiveSchedulerWait->Start();    
    MPX_DEBUG1( "CVcxMyVideosMdsDb:: iActiveSchedulerWait->Start done" );

    if ( iMdsSessionError == KErrNone )
        {
        MPX_DEBUG1( "CVcxMyVideosMdsDb:: session opened ok, adding observers" );

        // We order all object notifications. If we set video condition, then we wont
        // receive remove notifications at all (mds feature). Extra notifications
        // do not bother us much since we try to fetch the item from the db
        // after the add notification anyways, and then we use video condition.
        // Eventually extra events are ignored.    
        iMdsSession->AddObjectObserverL( *this, NULL );
    
        iMdsSession->AddObjectPresentObserverL( *this );
        
        GetSchemaDefinitionsL();
        
        if ( iAlbums )
            {
            iAlbums->GetSchemaDefinitionsL();
            iAlbums->SetObservingL();
            }
        }
    else
        {
        MPX_DEBUG2( "CVcxMyVideosMdsDb:: session opening failed: %d", iMdsSessionError );
        iMdsSessionError = KErrGeneral; // this ensures that next time when mds is tried to use, it tries to open session again
        delete iMdsSession;
        iMdsSession = NULL;        
        }

    MPX_DEBUG1( "CVcxMyVideosMdsDb::OpenMdsSessionL() exit" );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CVcxMyVideosMdsDb::HandleObjectPresentNotification( CMdESession& /*aSession*/,
        TBool aPresent, const RArray<TItemId>& aObjectIdArray)
    {
    MPX_DEBUG1( "CVcxMyVideosMdsDb::--------------------------------------------------------------." );
    MPX_DEBUG3( "CVcxMyVideosMdsDb::HandleObjectPresentNotification( aPresent = %1d, count = %4d) |", aPresent, aObjectIdArray.Count() );
    MPX_DEBUG1( "CVcxMyVideosMdsDb::--------------------------------------------------------------'" );

    TObserverNotificationType type;
    if ( aPresent )
        {
        type = ENotifyAdd;
        }
    else
        {
        type = ENotifyRemove;
        }
    
    TRAP_IGNORE( DoHandleObjectNotificationL( type, aObjectIdArray ) );    
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CVcxMyVideosMdsDb* CVcxMyVideosMdsDb::NewL( MVcxMyVideosMdsDbObserver* aObserver,
        RFs& aFs, MVcxMyVideosMdsAlbumsObserver* aAlbumsObserver )
    {
    CVcxMyVideosMdsDb* self =
            CVcxMyVideosMdsDb::NewLC( aObserver, aFs, aAlbumsObserver );
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CVcxMyVideosMdsDb* CVcxMyVideosMdsDb::NewLC( MVcxMyVideosMdsDbObserver* aObserver,
        RFs& aFs, MVcxMyVideosMdsAlbumsObserver* aAlbumsObserver )
    {
    CVcxMyVideosMdsDb* self = new( ELeave ) CVcxMyVideosMdsDb( aObserver, aFs, aAlbumsObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CVcxMyVideosMdsDb::~CVcxMyVideosMdsDb()
    {
    MPX_DEBUG1( "CVcxMyVideosMdsDb::~CVcxMyVideosMdsDb() start" );

    delete iEventProcessor;
    iEvents.Close();

    Cancel( EGetVideoList );

    delete iAlbums;
    delete iCmdQueue;
    delete iVideoQuery;
    delete iMdsSession;
    delete iActiveSchedulerWait;
    delete iAsyncHandleQueryCompleteCaller;
    delete iMdsShutdownMonitor;
    MPX_DEBUG1( "CVcxMyVideosMdsDb::~CVcxMyVideosMdsDb() exit" );
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosMdsDb::Cancel
// ---------------------------------------------------------------------------
//
void CVcxMyVideosMdsDb::Cancel( TRequestType aType )
    {
    MPX_DEBUG1("CVcxMyVideosMdsDb::Cancel() start");
    
    iCmdQueue->Cancel( aType );
    
    if ( aType == EAll || aType == EGetVideoList )
        {
        if ( iVideoQuery )
            {
            iVideoQuery->Cancel();
            }
        iVideoListFetchingIsOngoing = EFalse;
        }

    if ( aType != EGetVideoList )
        {
        iAlbums->CancelQueries( aType );
        }
    
    MPX_DEBUG1("CVcxMyVideosMdsDb::Cancel() exit");
    }
    
// ---------------------------------------------------------------------------
// CVcxMyVideosMdsDb::AddVideoL
// ---------------------------------------------------------------------------
//
void CVcxMyVideosMdsDb::AddVideoL(
        CMPXMedia& aVideo, 
        TUint32& aMdsId )
    {
    MPX_DEBUG1( "CVcxMyVideosMdsDb::AddVideoL() start" );
         
    CMdEObject* object = MdsSessionL().NewObjectLC(
            *iVideoObjectDef, aVideo.ValueText( KMPXMediaGeneralUri ) ); // 1->

    // Value from aVideo is taken in use in Media2ObjectL if aVideo contains creation date
    SetCreationAndModifiedDatesL( *object ); // use current time
    
    object->AddUint8PropertyL( *iOriginPropertyDef,
            aVideo.ValueTObjectL<TUint8>( KVcxMediaMyVideosOrigin ) );
    
    CMdEProperty* property;

    //  Type can not be modified normally, so set it here
    if ( aVideo.IsSupported( KMPXMediaGeneralMimeType ) )
        {
        if (KErrNotFound != object->Property( *iItemTypePropertyDef, property, 0 ))
            {
            static_cast<CMdETextProperty*>(property)->SetValueL(
                    aVideo.ValueText( KMPXMediaGeneralMimeType ) );
            }
        else
            {
            object->AddTextPropertyL(
                *iItemTypePropertyDef,
                aVideo.ValueText( KMPXMediaGeneralMimeType ) );
            }
        }

    Media2ObjectL( aVideo, *object );
    TRAPD( err, aMdsId = MdsSessionL().AddObjectL( *object ) );

    if ( err != KErrNone )
        {
        MPX_DEBUG2( "CVcxMyVideosMdsDb:: MdsSessionL().AddObjectL leaved with error: %d", err );
        User::Leave( err );
        }
        
    if ( aMdsId == KNoId )
        {
        MPX_DEBUG1( "CVcxMyVideosMdsDb::AddVideoL could not add new video" );
        User::Leave( KErrGeneral );
        }
    else
        {
        MPX_DEBUG2( "CVcxMyVideosMdsDb::AddVideoL video created, mds id: %d", aMdsId );
        }        

    CleanupStack::PopAndDestroy( object ); // <-1

    MPX_DEBUG1( "CVcxMyVideosMdsDb::AddVideoL() exit" );
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosMdsDb::RemoveVideo
// ---------------------------------------------------------------------------
//
TInt CVcxMyVideosMdsDb::RemoveVideo( TUint32 aMdsId )
    {
    MPX_DEBUG1( "CVcxMyVideosMdsDb::RemoveVideoL() start" );

    TInt retValue( KErrNone );
    TItemId id( 0 ); //init to avoid warning

    MPX_DEBUG2( "CVcxMyVideosMdsDb:: removing object %d", aMdsId );
    
    TRAPD( err,  id = MdsSessionL().RemoveObjectL( aMdsId ) );

    if ( err == KErrNone )
        {
        if ( id == KNoId )
            {
            MPX_DEBUG1( "CVcxMyVideosMdsDb:: video not found" );
            retValue = KErrNotFound;
            }
        else
            {
            MPX_DEBUG1( "CVcxMyVideosMdsDb:: remove successful" );
            }
        }
    else
        {
        MPX_DEBUG2( "CVcxMyVideosMdsDb:: MdsSessionL().RemoveObjectL left: %d", err );
        retValue = err;        
        }
        
    MPX_DEBUG1( "CVcxMyVideosMdsDb::RemoveVideoL() exit" );        
    return retValue;
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosMdsDb::UpdateVideoL
// ---------------------------------------------------------------------------
//
void CVcxMyVideosMdsDb::UpdateVideoL( CMPXMedia& aVideo )
    {
    MPX_DEBUG1( "CVcxMyVideosMdsDb::UpdateVideoL() start" );

    TMPXItemId mpxId = aVideo.ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId );

    MPX_DEBUG2("CVcxMyVideosMdsDb::UpdateVideoL updating object %d ", mpxId.iId1);
    
    CMdEObject* object =
            MdsSessionL().OpenObjectL( mpxId.iId1, *iVideoObjectDef );
    if ( !object )
        {
        // No object with this ID was found!
        MPX_DEBUG1("CVcxMyVideosMdsDb::UpdateVideoL no object found");
        User::Leave( KErrNotFound );
        }
    else
        {
        MPX_DEBUG1("CVcxMyVideosMdsDb::UpdateVideoL object found");
        
        if ( object->OpenForModifications() )
            {
            CleanupStack::PushL( object ); // 1->
            
            Media2ObjectL( aVideo, *object );
            
            MdsSessionL().CommitObjectL(*object);

            CleanupStack::PopAndDestroy(object);
            }
        else
            {
            // Object is already locked!
            MPX_DEBUG1("CVcxMyVideosMdsDb::UpdateVideoL object was locked!");
            delete object;
            User::Leave( KErrInUse );
            }
        }
    MPX_DEBUG1( "CVcxMyVideosMdsDb::UpdateVideoL() exit" );
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosMdsDb::CreateVideoListL
// ---------------------------------------------------------------------------
//
void CVcxMyVideosMdsDb::CreateVideoListL( TVcxMyVideosSortingOrder aSortingOrder,
        TBool aAscending, TBool aFullDetails, CMPXMedia*& aVideoList )
    {
    MPX_DEBUG1( "CVcxMyVideosMdsDb::CreateVideoListL() start" );
    
    CVcxMyVideosMdsCmdGetVideoList* cmd = new (ELeave) CVcxMyVideosMdsCmdGetVideoList;
    CleanupStack::PushL( cmd ); // 1->
    cmd->iCmdType      = CVcxMyVideosMdsDb::EGetVideoList;
    cmd->iSortingOrder = aSortingOrder;
    cmd->iAscending    = aAscending;
    cmd->iFullDetails  = aFullDetails;
    cmd->iVideoList    = &aVideoList;

    iCmdQueue->ExecuteCmdL( cmd ); //ownership moves
    CleanupStack::Pop( cmd ); // <-1
    }
    
// ---------------------------------------------------------------------------
// CVcxMyVideosMdsDb::DoCreateVideoListL
// ---------------------------------------------------------------------------
//
void CVcxMyVideosMdsDb::DoCreateVideoListL( TVcxMyVideosSortingOrder aSortingOrder,
        TBool aAscending, TBool aFullDetails, CMPXMedia*& aVideoList )
    {
    MPX_FUNC( "CVcxMyVideosMdsDb::DoCreateVideoListL" );
        
    if ( iVideoListFetchingIsOngoing )
        {
        MPX_DEBUG1("CVcxMyVideosMdsDb:: video list fetching already going on, returning current list");
        aVideoList = iVideoList;
        }
    
    if ( !aVideoList )
        {
        aVideoList = TVcxMyVideosCollectionUtil::CreateEmptyMediaListL();
        }
                    
    CMPXMediaArray* mediaArray = aVideoList->Value<CMPXMediaArray>( KMPXMediaArrayContents );
    
    delete iVideoQuery;
    iVideoQuery = NULL;
    
    iVideoQuery = MdsSessionL().NewObjectQueryL(
            *iNamespaceDef,
            *iVideoObjectDef,
            this);

    iVideoQuery->SetResultMode( EQueryResultModeItem );

    TMdESortOrder direction;
    
    if ( aAscending )
        {
        direction = ESortAscending;
        }
    else
        {
        direction = ESortDescending;
        }
        
    switch ( aSortingOrder )
        {
        case EVcxMyVideosSortingId:
            {
            MPX_DEBUG1("CVcxMyVideosMdsDb:: EVcxMyVideosSortingId"); 
            iVideoQuery->AppendOrderRuleL(
                TMdEOrderRule( EOrderRuleTypeItemID, direction ) );
            }
            break;
            
        case EVcxMyVideosSortingCreationDate:
            {
            MPX_DEBUG1("CVcxMyVideosMdsDb:: EVcxMyVideosSortingCreationDate"); 
            iVideoQuery->AppendOrderRuleL(
                TMdEOrderRule( *iCreationDatePropertyDef/*EOrderRuleTypeCreationTime*/, direction ) );
            }
            break;

        case EVcxMyVideosSortingModified:
            {
            MPX_DEBUG1("CVcxMyVideosMdsDb:: EVcxMyVideosSortingModified"); 
            iVideoQuery->AppendOrderRuleL(
                TMdEOrderRule( *iLastModifiedDatePropertyDef/*EOrderRuleTypeLastModifiedDate*/, direction ) );
            }
            break;

        case EVcxMyVideosSortingName:
            {
            MPX_DEBUG1("CVcxMyVideosMdsDb:: EVcxMyVideosSortingName"); 
        
            TMdEOrderRule orderRule( *iTitlePropertyDef, direction );
            orderRule.SetCaseSensitive( EFalse /* insensitive */);
            iVideoQuery->AppendOrderRuleL( orderRule );
            }
            break;

        case EVcxMyVideosSortingSize:
            {
            MPX_DEBUG1("CVcxMyVideosMdsDb:: EVcxMyVideosSortingSize"); 
            iVideoQuery->AppendOrderRuleL(
                TMdEOrderRule( *iSizePropertyDef, direction ) );
            }
            break;
        }

    iVideoListFetchingIsOngoing = ETrue;

    iFullDetails = aFullDetails;
    
    const TInt maxItemsInQueryResult = 500;
    iVideoQuery->FindL( KMdEQueryDefaultMaxCount, maxItemsInQueryResult );

    aVideoList->SetTObjectValueL<TMPXGeneralType>( KMPXMediaGeneralType, EMPXGroup );            
    aVideoList->SetTObjectValueL( KMPXMediaArrayCount, mediaArray->Count() );

    iVideoList = aVideoList; // lets store the pointer, ownership stays at observer
    }

// ---------------------------------------------------------------------------
// From class MMdEQueryObserver.
// ---------------------------------------------------------------------------
//
void CVcxMyVideosMdsDb::HandleQueryNewResults(
        CMdEQuery& aQuery,
        TInt aFirstNewItemIndex,
        TInt aNewItemCount )
    {
    TRAPD( err, DoHandleQueryNewResultsL( aQuery, aFirstNewItemIndex, aNewItemCount ));
    if ( err != KErrNone )
        {
        MPX_DEBUG2("CVcxMyVideosMdsDb:: DoHandleQueryNewResultsL() left with error code: %d", err);
        }
    }
    
// ---------------------------------------------------------------------------
// Leaving version of HandleQueryNewResults.
// ---------------------------------------------------------------------------
//
void CVcxMyVideosMdsDb::DoHandleQueryNewResultsL(
        CMdEQuery& /*aQuery*/,
        TInt aFirstNewItemIndex,
#ifdef _DEBUG
        TInt aNewItemCount )
#else
        TInt /*aNewItemCount*/ )
#endif
    {
    MPX_DEBUG1("CVcxMyVideosMdsDb::HandleQueryNewResults() start");
    
    if ( !iVideoList )
        {
        MPX_DEBUG1("CVcxMyVideosMdsDb:: iVideoList is NULL, should never happen");
        return;
        }
    
    MPX_DEBUG2("CVcxMyVideosMdsDb:: aFirstNewItemIndex = %d", aFirstNewItemIndex);
    MPX_DEBUG2("CVcxMyVideosMdsDb:: aNewItemCount      = %d", aNewItemCount);
        
    CMPXMediaArray* array = iVideoList->Value<CMPXMediaArray>( KMPXMediaArrayContents );
    CMPXMedia* media;    
    TInt count = iVideoQuery->Count();
    
    iVideoList->SetTObjectValueL<TInt>( KMPXMediaArrayCount, count );
    
    for ( TInt i = aFirstNewItemIndex; i < count; i++ )
        {
        CMdEObject& object = iVideoQuery->Result( i );

        media = CMPXMedia::NewL();
        CleanupStack::PushL( media ); // 1->
        
        Object2MediaL( object, *media, iFullDetails );
        array->AppendL( media );

        CleanupStack::Pop( media ); // <-1
        }
    
    iMdsDbObserver->HandleCreateVideoListResp( iVideoList, aFirstNewItemIndex,
            EFalse /* not complete yet */);
    MPX_DEBUG1("CVcxMyVideosMdsDb::HandleQueryNewResults() exit");
    }


            
// ---------------------------------------------------------------------------
// From class MMdEQueryObserver.
// ---------------------------------------------------------------------------
//
void CVcxMyVideosMdsDb::HandleQueryCompleted(
        CMdEQuery& /*aQuery*/,
        TInt /*aError*/)
    {
    iAsyncHandleQueryCompleteCaller->CallBack();
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosMdsDb::AsyncHandleQueryCompleted
// ---------------------------------------------------------------------------
//
TInt CVcxMyVideosMdsDb::AsyncHandleQueryCompleted( TAny* aThis )
    {
    MPX_FUNC( "CVcxMyVideosMdsDb::AsyncHandleQueryCompleted" );

    CVcxMyVideosMdsDb* thisObj = static_cast<CVcxMyVideosMdsDb*>( aThis );
    
    if ( !thisObj->iVideoList )
        {
        MPX_DEBUG1("CVcxMyVideosMdsDb:: iVideoList is NULL, should never happen");
        return KErrNone;
        }

#ifdef _DEBUG
    CMPXMediaArray* array = thisObj->iVideoList->Value<CMPXMediaArray>( KMPXMediaArrayContents );
#endif

    MPX_DEBUG2("CVcxMyVideosMdsDb:: query contains %d items", thisObj->iVideoQuery->Count());
    MPX_DEBUG2("CVcxMyVideosMdsDb:: iVideoList contains %d items", array->Count());
    
    if ( thisObj->iActiveSchedulerWait->IsStarted() )
        {
        thisObj->iActiveSchedulerWait->AsyncStop();
        }
    
    thisObj->iVideoListFetchingIsOngoing = EFalse;
    thisObj->iMdsDbObserver->HandleCreateVideoListResp( thisObj->iVideoList, KErrNotFound /* KErrNotFound = no new items */,
            ETrue /* complete */);
    
    thisObj->iVideoList = NULL; // not owned by us -> just clear
    
    thisObj->iCmdQueue->CmdFinished();
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosMdsDb::CreateVideoL
// ---------------------------------------------------------------------------
//
CMPXMedia* CVcxMyVideosMdsDb::CreateVideoL( TUint32 aId, TBool aFullDetails )
    {
    MPX_DEBUG1( "CVcxMyVideosMdsDb::CreateVideoL() start" );

    CMdEObject* object = ObjectL( aId );

    if ( !object )
        {
        MPX_DEBUG2("CVcxMyVideosMdsDb:: mds id %d not found from mds", aId);
        return NULL;
        }
        
    CleanupStack::PushL( object ); // 1->

    CMPXMedia* video = CMPXMedia::NewL( );
    CleanupStack::PushL( video ); // 2->

    Object2MediaL( *object, *video, aFullDetails );

    CleanupStack::Pop( video );            // <-2
    CleanupStack::PopAndDestroy( object ); // <-1
    MPX_DEBUG1( "CVcxMyVideosMdsDb::CreateVideoL() exit" );

    return video;
    }

// ---------------------------------------------------------------------------
// From class MMdESessionObserver.
// ---------------------------------------------------------------------------
//
void CVcxMyVideosMdsDb::HandleSessionOpened(
        CMdESession& /*aSession*/,
        TInt aError)
    {
    MPX_DEBUG1("CVcxMyVideosMdsDb::HandleSessionOpened() start" );

    iActiveSchedulerWait->AsyncStop();

    if ( aError != KErrNone )
        {
        MPX_DEBUG2( "CVcxMyVideosMdsDb::HandleSessionOpened: %d", aError );

        iMdsSessionError = aError;
        }
    MPX_DEBUG1("CVcxMyVideosMdsDb::HandleSessionOpened() exit" );
    }

// ---------------------------------------------------------------------------
// From class MMdESessionObserver.
// ---------------------------------------------------------------------------
//
void CVcxMyVideosMdsDb::HandleSessionError(
        CMdESession& /*aSession*/,
        TInt aError)
    {
    MPX_DEBUG1("CVcxMyVideosMdsDb::HandleSessionError() start" );

    if (iActiveSchedulerWait->IsStarted())
        {
        MPX_DEBUG1( "CVcxMyVideosMdsDb:: Session opening was going on");
        iActiveSchedulerWait->AsyncStop();
        }

    if ( iVideoListFetchingIsOngoing )
        {
        MPX_DEBUG1( "CVcxMyVideosMdsDb:: Videolist fetching was going on");
        iVideoListFetchingIsOngoing = EFalse;
        iMdsDbObserver->HandleCreateVideoListResp( iVideoList, -1 /* -1 = no new items */,
                ETrue /* complete */);
        }
        
    MPX_DEBUG2( "CVcxMyVideosMdsDb::HandleSessionError: %d", aError );

    iMdsSessionError = aError;

    MPX_DEBUG1("CVcxMyVideosMdsDb::HandleSessionError() exit" );
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosMdsDb::HandleObjectNotification
// ---------------------------------------------------------------------------
//
void CVcxMyVideosMdsDb::HandleObjectNotification(
        CMdESession& /*aSession*/,
        TObserverNotificationType aType,
        const RArray<TItemId>& aObjectIdArray)
    {
    TRAP_IGNORE( DoHandleObjectNotificationL( aType, aObjectIdArray ));
    }
    
// ---------------------------------------------------------------------------
// CVcxMyVideosMdsDb::ObjectL
// ---------------------------------------------------------------------------
//
CMdEObject* CVcxMyVideosMdsDb::ObjectL( const TItemId aId, TBool aIsVideo )
    {
    MPX_DEBUG1("CVcxMyVideosMdsDb::ObjectL start");
    //  If the id is not valid, just return NULL, because
    //  MdsSessionL().GetObjectL leaves in that case
    if ( aId == KNoId )
        {
        return NULL;
        }

    CMdEObject* object;
    if ( aIsVideo )
        {
        object = MdsSessionL().GetObjectL( aId, *iVideoObjectDef );
        }
    else
        {
        object = MdsSessionL().GetObjectL( aId, *iAlbums->iAlbumObjectDef );
        }
    
    if ( object )
        {
        MPX_DEBUG2( "CVcxMyVideosMdsDb::ObjectL found, id: %d", aId );
        }
    else
        {
        MPX_DEBUG2( "CVcxMyVideosMdsDb::ObjectL not found, id: %d", aId );
        }

    MPX_DEBUG1("CVcxMyVideosMdsDb::ObjectL exit");
    return object;
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosMdsDb::Object2MediaL
// ---------------------------------------------------------------------------
//
void CVcxMyVideosMdsDb::Object2MediaL(
        CMdEObject& aObject,
        CMPXMedia& aVideo,
        TBool aFullDetails )
    {    
    aVideo.SetTObjectValueL<TMPXGeneralType>(KMPXMediaGeneralType, EMPXItem );
   
    CMdEProperty* property = NULL;

    //1. ID, KMPXMediaGeneralId (BRIEF)
    TMPXItemId mpxId;
    mpxId.iId1 = aObject.Id();
    aVideo.SetTObjectValueL<TMPXItemId>( KMPXMediaGeneralId, mpxId );
    
    //2. TITLE (BRIEF)
    if ( aObject.Property( *iTitlePropertyDef, property, 0 ) != KErrNotFound )
        {
        aVideo.SetTextValueL( KMPXMediaGeneralTitle,
                static_cast<CMdETextProperty*>(property)->Value());
        }

    //  This is a fix for setting the title always
    if ( !property || static_cast<CMdETextProperty*>(property)->Value().Length() == 0 )
        {
        TParse parse;
        parse.Set( aObject.Uri(), NULL, NULL );
        aVideo.SetTextValueL( KMPXMediaGeneralTitle, parse.Name() );
        }

    //3. DESCRIPTION
    if ( aObject.Property( *iDescriptionPropertyDef, property, 0 ) != KErrNotFound
         && aFullDetails )
        {
        aVideo.SetTextValueL( KMPXMediaGeneralComment,
                static_cast<CMdETextProperty*>(property)->Value() );
        }

    //4. URI (= FILE PATH) (BRIEF)
    aVideo.SetTextValueL( KMPXMediaGeneralUri, aObject.Uri() );
   
    //5. FILE SIZE, KMPXMediaGeneralSize (BRIEF)    
    // With 64bit support the value will be copied after reading KMPXMediaGeneralExtSizeInt64
#ifndef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
    if (KErrNotFound !=
            aObject.Property( *iSizePropertyDef, property, 0 ))
        {
        aVideo.SetTObjectValueL<TInt>( KMPXMediaGeneralSize,
                static_cast<CMdEUint32Property*>(property)->Value() );
        }
        
    //  If the size was not found or it is 0, get it from the file
    if ( !(aVideo.Value<TInt>( KMPXMediaGeneralSize )) )
        {
        TEntry entry;
        TInt err = iFs.Entry( aObject.Uri(), entry );
        if ( err == KErrNone )
            {
            aVideo.SetTObjectValueL<TInt>( KMPXMediaGeneralSize, entry.iSize );
            }
        }
#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
    
    //6. CREATION DATE, KMPXMediaGeneralDate
    if ( aObject.Property( *iCreationDatePropertyDef, property, 0 ) != KErrNotFound )
        {
        aVideo.SetTObjectValueL<TInt64>( KMPXMediaGeneralDate,
                static_cast<CMdETimeProperty*>(property)->Value().Int64() );
        }
    
    //7. FLAGS, KMPXMediaGeneralFlags (BRIEF)
    TUint32 flags = 0;
    if ( aObject.Property( *iFlagsPropertyDef, property, 0 ) != KErrNotFound )
        {
        flags = static_cast<CMdEUint32Property*>(property)->Value();
        }
    else
        {
        // MDS didn't have flags variable set at all, so this is harvested video.
        // Set new flag on.
        flags |= EVcxMyVideosVideoNew;
        }
    if ( aObject.Property( *iDrmPropertyDef, property, 0 ) != KErrNotFound )
        {
        TBool drmProtected = static_cast<CMdEBoolProperty*>(property)->Value();
        if ( drmProtected )
            {
            flags |= EVcxMyVideosVideoDrmProtected;
            }
        }
    aVideo.SetTObjectValueL<TUint32>( KMPXMediaGeneralFlags, flags );
  
    //8. COPYRIGHT, KMPXMediaGeneralCopyright
    if ( aObject.Property( *iCopyrightPropertyDef, property, 0 ) != KErrNotFound 
         && aFullDetails )
        {
        aVideo.SetTextValueL( KMPXMediaGeneralCopyright,
                static_cast<CMdETextProperty*>(property)->Value() );
        }
    
    //9. KMPXMediaGeneralMimeType
    if ( aObject.Property( *iItemTypePropertyDef, property, 0 ) != KErrNotFound 
         && aFullDetails )
        {
        aVideo.SetTextValueL( KMPXMediaGeneralMimeType,
                static_cast<CMdETextProperty*>(property)->Value() );
        }
    
    //11. FILE SIZE, KMPXMediaGeneralExtSizeInt64 (BRIEF)    
#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
    if (KErrNotFound !=
            aObject.Property( *iSizePropertyDef, property, 0 ))
        {
        TUint32 val = static_cast<CMdEUint32Property*>(property)->Value(); 
        aVideo.SetTObjectValueL<TInt64>( KMPXMediaGeneralExtSizeInt64, val );
        // Maintains compatibility
        aVideo.SetTObjectValueL<TInt>( KMPXMediaGeneralSize, val );
        }
        
    //  If the size was not found or it is 0, get it from the file
    if ( !(aVideo.Value<TInt64>( KMPXMediaGeneralExtSizeInt64 )) )
        {
        TEntry entry;
        TInt err = iFs.Entry( aObject.Uri(), entry );
        if ( err == KErrNone )
            {
            aVideo.SetTObjectValueL<TInt64>( KMPXMediaGeneralExtSizeInt64, entry.FileSize() );
            // Maintains compatibility
            aVideo.SetTObjectValueL<TInt>( KMPXMediaGeneralSize, entry.iSize );
            }
        }
#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API

    //12. KVcxMediaMyVideosModifiedDate
    if ( aObject.Property( *iLastModifiedDatePropertyDef, property, 0 ) != KErrNotFound 
        && aFullDetails )
        {
        aVideo.SetTObjectValueL<TInt64>( KVcxMediaMyVideosModifiedDate,
                static_cast<CMdETimeProperty*>(property)->Value().Int64() );
        }
    
    //13. KVcxMediaMyVideosAgeProfile (BRIEF)
    if ( aObject.Property( *iAgeProfilePropertyDef, property, 0 ) != KErrNotFound )
        {
        aVideo.SetTObjectValueL<TInt32>( KVcxMediaMyVideosAgeProfile,
            static_cast<CMdEInt32Property*>(property)->Value() );
        }
        
    //14. KVcxMediaMyVideosAudioLanguage
    if ( aObject.Property( *iAudioLanguagePropertyDef, property, 0 ) != KErrNotFound
         && aFullDetails )
        {
        aVideo.SetTextValueL( KVcxMediaMyVideosAudioLanguage,
                static_cast<CMdETextProperty*>(property)->Value() );
        }
    
    //15. AUTHOR, KVcxMediaMyVideosAuthor
    if ( aObject.Property( *iAuthorPropertyDef, property, 0 ) != KErrNotFound
         && aFullDetails )
        {
        aVideo.SetTextValueL( KVcxMediaMyVideosAuthor,
                static_cast<CMdETextProperty*>(property)->Value() );
        }

    //16. ORIGIN, KVcxMediaMyVideosOrigin
    if ( aObject.Property( *iOriginPropertyDef, property, 0 ) != KErrNotFound )
        {
        TUint8 origin = static_cast<CMdEUint8Property*>(property)->Value();
#ifndef VCX_DOWNLOADS_CATEGORY
        if( origin != EVcxMyVideosOriginCapturedWithCamera )
            {
            origin = EVcxMyVideosOriginOther;
            }           
#else
        if ( origin != EVcxMyVideosOriginCapturedWithCamera &&
				 origin != EVcxMyVideosOriginDownloaded
				)
            {
            origin = EVcxMyVideosOriginOther;
            }

#endif
        aVideo.SetTObjectValueL<TUint8>( KVcxMediaMyVideosOrigin, origin );
        }

    //17. DURATION, (KMPXMediaGeneralDuration can't be used since it is TInt
    //   and we need TReal32 ) (BRIEF)
    if ( aObject.Property( *iDurationPropertyDef, property, 0 ) != KErrNotFound )
        {        
        aVideo.SetTObjectValueL<TReal32>( KVcxMediaMyVideosDuration,
                static_cast<CMdEReal32Property*>(property)->Value() );
        }

    //10. LAST PLAYBACK POSITION
    // KMPXMediaGeneralLastPlaybackPosition
    if ( aObject.Property( *iLastPlayPositionPropertyDef, property, 0 ) != KErrNotFound
         && aFullDetails )
        {        
        TInt pos = static_cast<CMdEReal32Property*>(property)->Value();
        
        aVideo.SetTObjectValueL<TInt>( KMPXMediaGeneralLastPlaybackPosition, pos );
        }
    
    //19. RATING (FULL)
    if ( aObject.Property( *iRatingPropertyDef, property, 0 ) != KErrNotFound
            && aFullDetails )
        {
        aVideo.SetTObjectValueL<TUint8>( KVcxMediaMyVideosRating,
                static_cast<CMdEUint8Property*>(property)->Value() );
        }
    else
        {
        aVideo.SetTObjectValueL<TUint8>( KVcxMediaMyVideosRating, 0 );
        }
    
    //20. BITRATE (FULL)
    if ( aObject.Property( *iBitratePropertyDef, property, 0 ) != KErrNotFound
            && aFullDetails )
        {
        TUint16 bitrate = static_cast<CMdEUint16Property*>(property)->Value();        
        aVideo.SetTObjectValueL<TUint16>( KMPXMediaVideoBitRate, bitrate );
        }    

    //21. AUDIOFOURCC (BRIEF)
    if ( aObject.Property( *iAudioFourCcPropertyDef, property, 0 ) != KErrNotFound )
        {
        aVideo.SetTObjectValueL<TUint32>( KVcxMediaMyVideosAudioFourCc,
                static_cast<CMdEUint32Property*>(property)->Value() );
        }
    else
        {
        aVideo.SetTObjectValueL<TUint32>( KVcxMediaMyVideosAudioFourCc, 0 );
        }
    
    //22. WIDTH (FULL)
    if ( aObject.Property( *iWidthPropertyDef, property, 0 ) != KErrNotFound
            && aFullDetails )
        {    
        aVideo.SetTObjectValueL<TUint16>( KMPXMediaVideoWidth,
                static_cast<CMdEUint16Property*>(property)->Value() );
        }    
    
    //23. Height (FULL)
    if ( aObject.Property( *iHeightPropertyDef, property, 0 ) != KErrNotFound
            && aFullDetails )
        {    
        aVideo.SetTObjectValueL<TUint16>( KMPXMediaVideoHeight,
                static_cast<CMdEUint16Property*>(property)->Value() );
        }    
    
    //24. ARTIST (FULL)
   if ( aObject.Property( *iArtistPropertyDef, property, 0 ) != KErrNotFound
           && aFullDetails )
       {
       aVideo.SetTextValueL( KMPXMediaVideoArtist,
               static_cast<CMdETextProperty*>(property)->Value());
       }

    }

// ---------------------------------------------------------------------------
// CVcxMyVideosMdsDb::Media2ObjectL
// Called by AddVideoL()
// ---------------------------------------------------------------------------
//
void CVcxMyVideosMdsDb::Media2ObjectL(
        CMPXMedia& aVideo,
        CMdEObject& aObject)
    {
    CMdEProperty* property;

    // 1. MDS ID
    
    // 2. TITLE (NAME)
    if ( aVideo.IsSupported( KMPXMediaGeneralTitle ) )
        {
        if ( aObject.Property( *iTitlePropertyDef, property, 0 ) != KErrNotFound )
            {
            static_cast<CMdETextProperty*>(property)->SetValueL(
                    aVideo.ValueText( KMPXMediaGeneralTitle ) );
            }
        else
            {
            aObject.AddTextPropertyL(
                    *iTitlePropertyDef, aVideo.ValueText( KMPXMediaGeneralTitle ) );
            }
        }
        
    // 3. DESC
    if ( aVideo.IsSupported( KMPXMediaGeneralComment ) )
        {
        if ( aObject.Property( *iDescriptionPropertyDef, property, 0 ) != KErrNotFound )
            {
            static_cast<CMdETextProperty*>(property)->SetValueL(
                    aVideo.ValueText( KMPXMediaGeneralComment ) );
            }
        else
            {
            aObject.AddTextPropertyL(
                    *iDescriptionPropertyDef, aVideo.ValueText( KMPXMediaGeneralComment ) );
            }
        }

    // 4. URI ( = file path )
    if ( aVideo.IsSupported( KMPXMediaGeneralUri ) )
        {
        aObject.SetUriL( aVideo.ValueText( KMPXMediaGeneralUri ) );
        TVolumeInfo volInfo;
        // 4.1 MEDIA ID ( = drive number )
        if ( iFs.Volume( volInfo, TVcxMyVideosCollectionUtil::DriveLetter2DriveNumber(
                aObject.Uri() ) ) == KErrNone)
            {
            aObject.SetMediaId( volInfo.iUniqueID );
            }
        }

    // 5. With 64bit support KMPXMediaGeneralSize is not used, see KMPXMediaGeneralExtSizeInt64
#ifndef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
    if ( aVideo.IsSupported( KMPXMediaGeneralSize ) )
        {
        TUint32 size = 0;
        TInt sizeInMediaObject = aVideo.ValueTObjectL<TInt>( KMPXMediaGeneralSize );
        size = static_cast<TUint32>( sizeInMediaObject );

        if ( aObject.Property( *iSizePropertyDef, property, 0 ) != KErrNotFound )
            {
            static_cast<CMdEUint32Property*>( property )->SetValueL( size );
            }
        else
            {
            aObject.AddUint32PropertyL( *iSizePropertyDef, size );
            }
        }
#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API

    // 6. KMPXMediaGeneralDate ( creation date )
    SetCreationDateToObjectL( aVideo, aObject );
    
    // 7. KMPXMediaGeneralFlags (including DRM flag)
    if ( aVideo.IsSupported( KMPXMediaGeneralFlags ) )
        {
        TUint32 flags = aVideo.ValueTObjectL<TUint32>( KMPXMediaGeneralFlags );
        if ( aObject.Property( *iFlagsPropertyDef, property, 0 ) != KErrNotFound )
            {
            static_cast<CMdEUint32Property*>(property)->SetValueL( flags );
            }
        else
            {
            aObject.AddUint32PropertyL( *iFlagsPropertyDef, flags );
            }

        TBool drmProtected;
        if ( flags & EVcxMyVideosVideoDrmProtected )
            {
            drmProtected = ETrue;
            }
        else
            {
            drmProtected = EFalse;
            }
            
        if ( aObject.Property( *iDrmPropertyDef, property, 0 ) != KErrNotFound )
            {
            static_cast<CMdEBoolProperty*>(property)->SetValueL( drmProtected );
            }
        else
            {
            aObject.AddBoolPropertyL( *iDrmPropertyDef, drmProtected );
            }
        }

    // 8. KMPXMediaGeneralCopyright
    if ( aVideo.IsSupported( KMPXMediaGeneralCopyright ))
        {
        if ( aObject.Property( *iCopyrightPropertyDef, property, 0 ) != KErrNotFound )
            {
            static_cast<CMdETextProperty*>(property)->SetValueL(
                    aVideo.ValueText( KMPXMediaGeneralCopyright ) );
            }
        else
            {
            if ( aVideo.ValueText( KMPXMediaGeneralCopyright ).Length() )
                {
                aObject.AddTextPropertyL(
                        *iCopyrightPropertyDef,
                        aVideo.ValueText( KMPXMediaGeneralCopyright ) );
                }
            }
        }

#if 0
    // 9. KMPXMediaGeneralMimeType
    if ( aVideo.IsSupported( KMPXMediaGeneralMimeType ))
        {
        if ( aObject.Property( *iItemTypePropertyDef, property, 0 ) != KErrNotFound )
            {
            static_cast<CMdETextProperty*>(property)->SetValueL(
                    aVideo.ValueText( KMPXMediaGeneralMimeType ) );
            }
        else
            {
            aObject.AddTextPropertyL(
                    *iItemTypePropertyDef,
                    aVideo.ValueText( KMPXMediaGeneralMimeType ) );
            }
        }
#endif

    // 11. KMPXMediaGeneralExtSizeInt64
#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
    if ( aVideo.IsSupported( KMPXMediaGeneralExtSizeInt64 ) )
        {
        TUint32 size = 0;
        TInt64 sizeInMediaObject = aVideo.ValueTObjectL<TInt64>( KMPXMediaGeneralExtSizeInt64 );
        size = static_cast<TUint32>( sizeInMediaObject );

        if ( aObject.Property( *iSizePropertyDef, property, 0 ) != KErrNotFound )
            {
            static_cast<CMdEUint32Property*>( property )->SetValueL( size );
            }
        else
            {
            aObject.AddUint32PropertyL( *iSizePropertyDef, size );
            }
        }
#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
    
    // 12. KVcxMediaMyVideosModifiedDate
    if ( aVideo.IsSupported( KVcxMediaMyVideosModifiedDate ) )
        {
        TInt64 modifiedDateInt64 = aVideo.ValueTObjectL<TInt64>( KVcxMediaMyVideosModifiedDate );
        TTime modifiedDate( modifiedDateInt64 );
        if ( aObject.Property( *iLastModifiedDatePropertyDef, property, 0 ) != KErrNotFound )
            {
            static_cast<CMdETimeProperty*>(property)->SetValueL( modifiedDate );
            }
        else
            {
            aObject.AddTimePropertyL( *iLastModifiedDatePropertyDef, modifiedDate );
            }
        }
            
    // 13. KVcxMediaMyVideosAgeProfile
    if ( aVideo.IsSupported( KVcxMediaMyVideosAgeProfile ) )
        {
        TInt32 ageProfile = aVideo.ValueTObjectL<TInt32>(KVcxMediaMyVideosAgeProfile );
        if ( aObject.Property( *iAgeProfilePropertyDef, property, 0 ) != KErrNotFound )
            {
            static_cast<CMdEInt32Property*>(property)->SetValueL( ageProfile );
            }
        else
            {
            aObject.AddInt32PropertyL( *iAgeProfilePropertyDef, ageProfile );
            }
        }
            
    // 14. KVcxMediaMyVideosAudioLanguage
    if ( aVideo.IsSupported( KVcxMediaMyVideosAudioLanguage ) )
        {
        if ( aObject.Property( *iAudioLanguagePropertyDef, property, 0 ) != KErrNotFound )
            {
            static_cast<CMdETextProperty*>(property)->SetValueL(
                    aVideo.ValueText( KVcxMediaMyVideosAudioLanguage ) );
            }
        else
            {
            aObject.AddTextPropertyL(
                    *iAudioLanguagePropertyDef,
                    aVideo.ValueText( KVcxMediaMyVideosAudioLanguage ) );
            }
        }
        
    // 15. KVcxMediaMyVideosAuthor
    if ( aVideo.IsSupported( KVcxMediaMyVideosAuthor ) )
        {
        if ( aObject.Property( *iAuthorPropertyDef, property, 0 ) != KErrNotFound )
            {
            static_cast<CMdETextProperty*>(property)->SetValueL(
                    aVideo.ValueText( KVcxMediaMyVideosAuthor ) );
            }
        else
            {
            aObject.AddTextPropertyL(
                    *iAuthorPropertyDef,
                    aVideo.ValueText( KVcxMediaMyVideosAuthor ) );
            }
        }
        
    // 16. KVcxMediaMyVideosOrigin
    if ( aVideo.IsSupported( KVcxMediaMyVideosOrigin ) )
        {
        TUint8 origin = aVideo.ValueTObjectL<TUint8>( KVcxMediaMyVideosOrigin );
        if ( aObject.Property( *iOriginPropertyDef, property, 0 ) != KErrNotFound )
            {
            static_cast<CMdEUint8Property*>(property)->SetValueL( origin );
            }
        else
            {
            aObject.AddUint8PropertyL( *iOriginPropertyDef, origin );
            }
        }
    
    // 17. KVcxMediaMyVideosDuration
    if ( aVideo.IsSupported( KVcxMediaMyVideosDuration ))
        {        
        TReal32 duration = aVideo.ValueTObjectL<TReal32>( KVcxMediaMyVideosDuration );

        if ( aObject.Property( *iDurationPropertyDef, property, 0 ) != KErrNotFound )
            {
            static_cast<CMdEReal32Property*>(property)->SetValueL( duration );
            }
        else
            {
            aObject.AddReal32PropertyL( *iDurationPropertyDef, duration );
            }
        }

    // 10. KMPXMediaGeneralLastPlaybackPosition
    if ( aVideo.IsSupported( KMPXMediaGeneralLastPlaybackPosition ))
        {        
        TReal32 lastPlaybackPos = aVideo.ValueTObjectL<TInt>(
                        KMPXMediaGeneralLastPlaybackPosition );
        
        // Play pos is written only after playback. Reset new video flag
        // now, so there's no need for changing the flag explitically
        if ( aObject.Property( *iFlagsPropertyDef, property, 0 ) != KErrNotFound )
            {
            TUint32 flags = static_cast<CMdEUint32Property*>(property)->Value();
            
            flags &= ~EVcxMyVideosVideoNew;
            
            static_cast<CMdEUint32Property*>(property)->SetValueL( flags );
            }
        
        if ( aObject.Property( *iLastPlayPositionPropertyDef, property, 0 ) != KErrNotFound )
            {
            static_cast<CMdEReal32Property*>(property)->SetValueL( lastPlaybackPos );
            }
        else
            {
            aObject.AddReal32PropertyL( *iLastPlayPositionPropertyDef, lastPlaybackPos );
            }
        }
    
    // 19. RATING
    if ( aVideo.IsSupported( KVcxMediaMyVideosRating ) )
        {
        TUint8 rating = aVideo.ValueTObjectL<TUint8>( KVcxMediaMyVideosRating );
        
        if ( aObject.Property( *iRatingPropertyDef, property, 0 ) != KErrNotFound )
            {
            static_cast<CMdEUint8Property*>(property)->SetValueL( rating );
            }
        else
            {
            aObject.AddUint8PropertyL( *iRatingPropertyDef, rating );
            }
        }
    // 20. BITRATE
    if ( aVideo.IsSupported( KMPXMediaVideoBitRate ) )
        {           
        TUint16 bitrate = aVideo.ValueTObjectL<TUint16>( KMPXMediaVideoBitRate );
        
        if ( aObject.Property( *iBitratePropertyDef, property, 0 ) != KErrNotFound )
            {
            static_cast<CMdEUint16Property*>(property)->SetValueL( bitrate );
            }
        else
            {
            aObject.AddUint16PropertyL( *iBitratePropertyDef, bitrate );
            }
        }

    // 21. AUDIOFOURCC
    if ( aVideo.IsSupported( KVcxMediaMyVideosAudioFourCc ) )
        {
        TUint32 audioFourCc = aVideo.ValueTObjectL<TUint32>( KVcxMediaMyVideosAudioFourCc );
        
        if ( aObject.Property( *iAudioFourCcPropertyDef, property, 0 ) != KErrNotFound )
            {
            static_cast<CMdEUint32Property*>(property)->SetValueL( audioFourCc );
            }
        else
            {
            aObject.AddUint32PropertyL( *iAudioFourCcPropertyDef, audioFourCc );
            }
        }
    
    //22. WIDTH (FULL)
    if ( aVideo.IsSupported( KMPXMediaVideoWidth ) )
       {
       TUint16 width = aVideo.ValueTObjectL<TUint16>( KMPXMediaVideoWidth );
       
       if ( aObject.Property( *iWidthPropertyDef, property, 0 ) != KErrNotFound )
           {
           static_cast<CMdEUint16Property*>(property)->SetValueL( width );
           }
       else
           {
           aObject.AddUint16PropertyL( *iWidthPropertyDef, width );
           }
       }
        
    //23. Height (FULL)
    if ( aVideo.IsSupported( KMPXMediaVideoHeight ) )
       {
       TUint16 width = aVideo.ValueTObjectL<TUint16>( KMPXMediaVideoHeight );
       
       if ( aObject.Property( *iHeightPropertyDef, property, 0 ) != KErrNotFound )
           {
           static_cast<CMdEUint16Property*>(property)->SetValueL( width );
           }
       else
           {
           aObject.AddUint16PropertyL( *iHeightPropertyDef, width );
           }
       }
    
    // 24. Artist
    if ( aVideo.IsSupported( KMPXMediaVideoArtist ) )
        {
        if ( aObject.Property( *iArtistPropertyDef, property, 0 ) != KErrNotFound )
            {
            static_cast<CMdETextProperty*>(property)->SetValueL(
                    aVideo.ValueText( KMPXMediaVideoArtist ) );
            }
        else
            {
            aObject.AddTextPropertyL(
                    *iArtistPropertyDef, aVideo.ValueText( KMPXMediaVideoArtist ) );
            }
        }
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosMdsDb::SetCreationDateToObjectL
// ---------------------------------------------------------------------------
//
void CVcxMyVideosMdsDb::SetCreationDateToObjectL( const CMPXMedia& aVideo, CMdEObject& aObject )
    {
    CMdEProperty* property;
    
    if ( aVideo.IsSupported( KMPXMediaGeneralDate ) )
        {
        TInt64 creationDateInt64 = 0;
        creationDateInt64 = aVideo.ValueTObjectL<TInt64>( KMPXMediaGeneralDate );
        TTime creationDate( creationDateInt64 );
        if ( aObject.Property( *iCreationDatePropertyDef, property, 0 ) != KErrNotFound )
            {
            static_cast<CMdETimeProperty*>(property)->SetValueL( creationDate );
            }
        else
            {
            aObject.AddTimePropertyL( *iCreationDatePropertyDef, creationDate );
            }
#ifdef _DEBUG
        TDateTime dT = creationDate.DateTime();
        TBuf<200> buf;
        buf.Format(_L("date from aVideo to aObject: %2d.%2d.%4d %2d:%2d:%2d"),
                dT.Day()+1, dT.Month()+1, dT.Year(), dT.Hour(), dT.Minute(), dT.Second() ); 
        MPX_DEBUG2("%S", &buf ); 
#endif
        }
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosMdsDb::GetSchemaDefinitionsL
// ---------------------------------------------------------------------------
//
void CVcxMyVideosMdsDb::GetSchemaDefinitionsL()
    {
    MPX_FUNC( "CVcxMyVideosMdsDb::GetSchemaDefinitionsL" );

    //  Namespace
    iNamespaceDef = &(MdsSessionL().GetDefaultNamespaceDefL());
    
    //  Default object definitions
    iVideoObjectDef = &(iNamespaceDef->GetObjectDefL( KVcxVideoObjectName ));

    //  Base object property definitions
    iTitlePropertyDef = &(iVideoObjectDef->GetPropertyDefL(
            KVcxTitlePropertyName )); //2
    iSizePropertyDef = &(iVideoObjectDef->GetPropertyDefL(
            KVcxSizePropertyName )); //11 (5 also)
    iCreationDatePropertyDef = &(iVideoObjectDef->GetPropertyDefL(
            KVcxCreationDatePropertyName )); //6
    iTimeOffsetPropertyDef = &(iVideoObjectDef->GetPropertyDefL(
            KVcxTimeOffsetPropertyName )); //6
    iItemTypePropertyDef = &(iVideoObjectDef->GetPropertyDefL(
            KVcxItemTypePropertyName )); //9
    iLastModifiedDatePropertyDef = &(iVideoObjectDef->GetPropertyDefL(
            KVcxLastModifiedDatePropertyName )); //12

    //  Media object property definitions
    iDescriptionPropertyDef = &(iVideoObjectDef->GetPropertyDefL(
            KVcxDescriptionPropertyName)); //3
    iCopyrightPropertyDef = &(iVideoObjectDef->GetPropertyDefL(
            KVcxCopyrightPropertyName )); //8
    iAuthorPropertyDef = &(iVideoObjectDef->GetPropertyDefL(
            KVcxAuthorPropertyName )); //15
    iOriginPropertyDef = &(iVideoObjectDef->GetPropertyDefL(
            KVcxOriginPropertyName )); //16
    iDurationPropertyDef = &(iVideoObjectDef->GetPropertyDefL(
            KVcxDurationPropertyName )); //17
    iDrmPropertyDef = &(iVideoObjectDef->GetPropertyDefL(
            KVcxDrmPropertyName )); //part of flags (7)
    iRatingPropertyDef = &(iVideoObjectDef->GetPropertyDefL(
            KVcxRatingPropertyName )); //19
    iBitratePropertyDef = &(iVideoObjectDef->GetPropertyDefL(
                KVcxBitratePropertyName )); //20
    iAudioFourCcPropertyDef = &(iVideoObjectDef->GetPropertyDefL(
                KVcxAudioFourCcPropertyName )); //21
    iWidthPropertyDef = &(iVideoObjectDef->GetPropertyDefL(
            KVcxWidthPropertyName )); //22
    iHeightPropertyDef = &(iVideoObjectDef->GetPropertyDefL(
            KVcxHeightPropertyName )); //23
    iArtistPropertyDef = &(iVideoObjectDef->GetPropertyDefL(
                KVcxArtistPropertyName )); //24

    //  Video object property definitions
    iLastPlayPositionPropertyDef = &(iVideoObjectDef->GetPropertyDefL(
            KVcxLastPlayPositionPropertyName )); //10

    iFlagsPropertyDef = &(iVideoObjectDef->GetPropertyDefL(
            KVcxFlagsPropertyName )); //7

    iAgeProfilePropertyDef = &(iVideoObjectDef->GetPropertyDefL(
            KVcxAgeProfilePropertyName )); //13

    iAudioLanguagePropertyDef = &(iVideoObjectDef->GetPropertyDefL(
            KVcxAudioLanguagePropertyName )); //14
    
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosMdsDb::DoHandleObjectNotificationL
// ---------------------------------------------------------------------------
//
void CVcxMyVideosMdsDb::DoHandleObjectNotificationL(
        TObserverNotificationType aType,
        const RArray<TItemId>& aObjectIdArray )
    {
    MPX_FUNC( "CVcxMyVideosMdsDb::DoHandleObjectNotificationL" );

    if ( iMdsDbObserver )
        {
        // Let delete events bypass buffer only if it empty.
        // This assures that event arriving order stays the same.
        if ( aType & ENotifyRemove && iEvents.Count() == 0 )
            {
            // Delete handling is so fast that we dont buffer them at all.
            // Modify and add require fetch from mds -> they are slow.
            RArray<TUint32> idArray;
            CleanupClosePushL( idArray ); // 1->
    
            for ( TInt i = 0; i < aObjectIdArray.Count(); i++ )
                {
                idArray.Append( aObjectIdArray[i] );
                }
                 
            iMdsDbObserver->HandleMyVideosDbEvent( EMPXItemDeleted, idArray, iEvents.Count() );
    
            CleanupStack::PopAndDestroy( &idArray ); // <-1
            }
        else
            {
            // Buffer modify & add events, since their handling is slow.
            // Process them on background.
            TInt count = aObjectIdArray.Count();
            
            if ( iEvents.Count() == 0 )
                {
                iEvents.ReserveL( count );
                }
            
            TEvent event;
            for ( TInt i = 0; i < count; i++ )
                {
                event.iMdsId     = aObjectIdArray[i];
                event.iEventType = aType;
                iEvents.AppendL( event );
                }
        
            if ( !iEventProcessor )
                {
                iEventProcessor = CIdle::NewL( CActive::EPriorityIdle );
                }
        
            if ( !iEventProcessor->IsActive() )
                {
                iEventProcessor->Start( TCallBack( ProcessEvents, this ));
                }
            }
        }    
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosMdsDb::ProcessEvents
// ---------------------------------------------------------------------------
//
TInt CVcxMyVideosMdsDb::ProcessEvents( TAny* aPtr )
    {
    return static_cast<CVcxMyVideosMdsDb*>(aPtr)->DoProcessEvents();
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosMdsDb::DoProcessEvents
// ---------------------------------------------------------------------------
//
TInt CVcxMyVideosMdsDb::DoProcessEvents()
    {
    TInt sent             = 0;
    TInt currentEventType = -1;
    const TInt sendAtOnce = 10;

    RArray<TUint32> idArray;
    CleanupClosePushL( idArray ); // 1->

    idArray.Reserve( sendAtOnce ); // may fail, it's ok
    
    while ( sent < sendAtOnce )
        {
        if ( iEvents.Count() == 0 )
            {
            break;
            }
        
        if ( iEvents[0].iEventType != currentEventType &&
                currentEventType != -1 )
            {
            break;
            }
        currentEventType = iEvents[0].iEventType;
        idArray.Append( iEvents[0].iMdsId ); // this may fail, it is ok, we can't do anything for it
        iEvents.Remove( 0 );
        sent++;           
        }

    iEvents.Compress();
    
    if ( currentEventType & ENotifyAdd )
        {
        iMdsDbObserver->HandleMyVideosDbEvent( EMPXItemInserted, idArray, iEvents.Count() );
        }        
    else if ( currentEventType & ENotifyModify )
        {
        iMdsDbObserver->HandleMyVideosDbEvent( EMPXItemModified, idArray, iEvents.Count() );
        }       
    else if ( currentEventType & ENotifyRemove )
        {
        iMdsDbObserver->HandleMyVideosDbEvent( EMPXItemDeleted, idArray, iEvents.Count() );
        }

    CleanupStack::PopAndDestroy( &idArray ); // <-1
    
    return iEvents.Count();
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosMdsDb::SetCreationAndModifiedDatesL
// ---------------------------------------------------------------------------
//
void CVcxMyVideosMdsDb::SetCreationAndModifiedDatesL( CMdEObject& aObject )
    {
    TTime time;
    time.UniversalTime();
    TTimeIntervalSeconds timeOffset = User::UTCOffset();
    TTime localTime = time + timeOffset;

    const TInt secondsInMinute( 60 );    
    aObject.AddTimePropertyL( *iCreationDatePropertyDef, localTime ); 
    aObject.AddInt16PropertyL( *iTimeOffsetPropertyDef, timeOffset.Int() / secondsInMinute );
    aObject.AddTimePropertyL( *iLastModifiedDatePropertyDef, localTime );
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosMdsDb::MdsSessionL
// ---------------------------------------------------------------------------
//
CMdESession& CVcxMyVideosMdsDb::MdsSessionL()
    {
    if ( iMdsSessionError == KErrServerTerminated )
        {
        // This ensures that the iMdsSessionError KErrServerTerminated
        // value does not change, since the user can't access iMdsSession.
        // Recovery happens in ShutDownNotification func.
        User::Leave( KErrServerTerminated );
        }

    if ( !iMdsSession )
        {
        OpenMdsSessionL();
        
        if ( !iMdsSession )
            {
            User::Leave( KErrGeneral );
            }
        }

    return *iMdsSession;
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosMdsDb::ShutdownNotification
// From MVcxMdsShutDownMonitorObserver
// ---------------------------------------------------------------------------
//    
void CVcxMyVideosMdsDb::ShutdownNotification( TInt aShutdownState )
    {
    if ( aShutdownState == 0 )
        {
        // Server has restarted, recreate session.
        MPX_DEBUG1("CVcxMyVideosMdsDb:: MDS Server restarted!");
        OpenMdsSessionL();
        return;
        }
        
    if ( aShutdownState == 1 )
        {
        MPX_DEBUG1("CVcxMyVideosMdsDb:: MDS Server went down!");
        iMdsSessionError = KErrServerTerminated;
        }
    }
