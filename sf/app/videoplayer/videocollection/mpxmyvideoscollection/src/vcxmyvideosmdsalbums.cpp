/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Provides albums support utilizing MDS.
*
*/


// INCLUDE FILES
#include <e32base.h>
#include <mpxlog.h>
#include <mdesession.h>
#include <mdeconstants.h>
#include <mdequery.h>
#include <mpxmedia.h>
#include <mpxmediaarray.h>
#include <mpxmediacontainerdefs.h>
#include <mpxmediageneraldefs.h>
#include "vcxmyvideosmdsalbums.h"
#include "vcxmyvideosmdsdb.h"
#include "vcxmyvideoscollectionutil.h"
#include "vcxmyvideosmdscmdqueue.h"

// ============================ MEMBER FUNCTIONS =============================

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CVcxMyVideosMdsAlbums::CVcxMyVideosMdsAlbums( CVcxMyVideosMdsDb& aMdsDb,
        MVcxMyVideosMdsAlbumsObserver* aObserver )
: CActive( EPriorityStandard ), iMdsDb( aMdsDb ), iObserver(aObserver) 
    {
    }

// ---------------------------------------------------------------------------
// 2nd-phase constructor
// ---------------------------------------------------------------------------
//
void CVcxMyVideosMdsAlbums::ConstructL()
    {
    GetSchemaDefinitionsL();
    SetObservingL();
    CActiveScheduler::Add( this );    
    }

// ---------------------------------------------------------------------------
// Two-Phase Constructor
// ---------------------------------------------------------------------------
//
CVcxMyVideosMdsAlbums* CVcxMyVideosMdsAlbums::NewL( CVcxMyVideosMdsDb& aMdsDb,
        MVcxMyVideosMdsAlbumsObserver* aObserver )
    {
    CVcxMyVideosMdsAlbums* self = new(ELeave) CVcxMyVideosMdsAlbums( aMdsDb, aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CVcxMyVideosMdsAlbums::~CVcxMyVideosMdsAlbums()
    {
    CancelQueries();
    delete iAlbumQuery;
    delete iVideoQuery;
    delete iRelationQuery;
    iItemArray.Close();
    iResultBuffer.Close();
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosMdsAlbums::CancelQueries
// ---------------------------------------------------------------------------
//
void CVcxMyVideosMdsAlbums::CancelQueries( CVcxMyVideosMdsDb::TRequestType aType )
    {
    MPX_DEBUG1("CVcxMyVideosMdsAlbums::CancelQueries() start");

    if ( aType == CVcxMyVideosMdsDb::EAll || aType == CVcxMyVideosMdsDb::EGetAlbums )
        {
        if ( iAlbumQuery )
            {
            iAlbumQuery->Cancel();
            }
        }

    if ( aType == CVcxMyVideosMdsDb::EAll || aType == CVcxMyVideosMdsDb::EGetAlbumContentIds )
        {
        if ( iRelationQuery )
            {
            iRelationQuery->Cancel();
            }
        }

    if ( aType == CVcxMyVideosMdsDb::EAll || aType == CVcxMyVideosMdsDb::EGetAlbumContentVideos )
        {
        if ( iVideoQuery )
            {
            iVideoQuery->Cancel();
            }
        }

    if ( aType == CVcxMyVideosMdsDb::EAll || aType == CVcxMyVideosMdsDb::EAddVideosToAlbum
            || aType == CVcxMyVideosMdsDb::ERemoveRelations
            || aType == CVcxMyVideosMdsDb::ERemoveAlbums )
        {
        
        // MDS does not offer cancel for these
        //Cancel();
        }

    MPX_DEBUG1("CVcxMyVideosMdsAlbums::CancelQueries() exit");
    }

// ---------------------------------------------------------------------------
// From CActive
// CVcxMyVideosMdsAlbums::DoCancel
// ---------------------------------------------------------------------------
//
void CVcxMyVideosMdsAlbums::DoCancel()
    {
    MPX_DEBUG1("CVcxMyVideosMdsAlbums::DoCancel() start");

    // MDS does not offer way to cancel these async requests
    
    MPX_DEBUG1("CVcxMyVideosMdsAlbums::DoCancel() exit");
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosMdsAlbums::SetObservingL
// ---------------------------------------------------------------------------
//
void CVcxMyVideosMdsAlbums::SetObservingL()
    {
    if ( iObserver )
        {
        //ENotifyAdd and ENotifyModify are not supported
        iMdsDb.MdsSessionL().AddRelationItemObserverL( *this, NULL,
                ENotifyRemove, iMdsDb.iNamespaceDef );

#if 0
        // We receive only IDs from here. We need to make query to get
        // relation objects-> slow to use. We use the response from
        // the add operation instead. This way we don't receive
        // add events if someone else adds videos to our albums
        // but the performance is the best possible.
        iMdsDb.MdsSessionL().AddRelationObserverL( *this, NULL,
                ENotifyAdd | ENotifyModify | ENotifyRemove );        
#endif    
        }
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosMdsAlbums::GetAlbumsL
// ---------------------------------------------------------------------------
//
void CVcxMyVideosMdsAlbums::GetAlbumsL( CMPXMedia* aAlbumList,
        MVcxMyVideosMdsAlbumsObserver& aClient )
    {
    CVcxMyVideosMdsCmdGetAlbums* cmd = new (ELeave) CVcxMyVideosMdsCmdGetAlbums;
    CleanupStack::PushL( cmd );
    cmd->iCmdType   = CVcxMyVideosMdsDb::EGetAlbums;
    cmd->iClient    = &aClient;
    cmd->iAlbumList = aAlbumList;
    iMdsDb.iCmdQueue->ExecuteCmdL( cmd ); //owneship moves
    CleanupStack::Pop( cmd );
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosMdsAlbums::GetAlbumL
// ---------------------------------------------------------------------------
//
CMPXMedia* CVcxMyVideosMdsAlbums::GetAlbumL( TUint32 aId )
    {
    CMdEObject* object = iMdsDb.ObjectL( aId, EFalse /* is not video, is album */);

    if ( !object )
        {
        MPX_DEBUG2("CVcxMyVideosMdsAlbums:: mds id %d not found from mds", aId);
        return NULL;
        }
        
    CleanupStack::PushL( object ); // 1->

    CMPXMedia* album = CMPXMedia::NewL( );
    CleanupStack::PushL( album ); // 2->

    Object2MediaL( *object, *album );

    CleanupStack::Pop( album );            // <-2
    CleanupStack::PopAndDestroy( object ); // <-1

    return album;
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosMdsAlbums::DoGetAlbumsL
// ---------------------------------------------------------------------------
//
void CVcxMyVideosMdsAlbums::DoGetAlbumsL( CMPXMedia* aAlbumList,
        MVcxMyVideosMdsAlbumsObserver& aClient )
    {
    MPX_DEBUG1("CVcxMyVideosMdsAlbums::DoGetAlbumsL() start");
    
    iAlbumList = aAlbumList; //store pointer to album list, we do not own this
    iClient    = &aClient;
    
    delete iAlbumQuery;
    iAlbumQuery = NULL;
    
    iAlbumQuery = iMdsDb.MdsSessionL().NewObjectQueryL( *iNamespaceDef, *iAlbumObjectDef, this );

    CMdELogicCondition& rootCondition = iAlbumQuery->Conditions();
    rootCondition.SetOperator( ELogicConditionOperatorOr );
    rootCondition.AddObjectConditionL( *iAlbumObjectDef );
    rootCondition.AddPropertyConditionL( *iTypePropertyDef,
            TMdEIntRange( MdeConstants::Album::EAlbumUser, MdeConstants::Album::EAlbumUser,
            EMdERangeTypeEqual ) );

    iAlbumQuery->FindL();

    MPX_DEBUG1("CVcxMyVideosMdsAlbums::DoGetAlbumsL() exit");
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosMdsAlbums::GetAlbumContentIdsL
// ---------------------------------------------------------------------------
//
void CVcxMyVideosMdsAlbums::GetAlbumContentIdsL( TUint32 aAlbumId,
        RArray<TVcxMyVideosAlbumVideo>& aContentArray,
        MVcxMyVideosMdsAlbumsObserver& aClient )
    {
    CVcxMyVideosMdsCmdGetAlbumContentIds* cmd = new (ELeave) CVcxMyVideosMdsCmdGetAlbumContentIds;
    CleanupStack::PushL( cmd ); // 1->
    cmd->iCmdType      = CVcxMyVideosMdsDb::EGetAlbumContentIds;
    cmd->iClient       = &aClient;
    cmd->iAlbumId      = aAlbumId;
    cmd->iAlbumContent = &aContentArray;
    iMdsDb.iCmdQueue->ExecuteCmdL( cmd ); //owneship moves
    CleanupStack::Pop( cmd ); // <-1
    }
    
// ---------------------------------------------------------------------------
// CVcxMyVideosMdsAlbums::DoGetAlbumContentIdsL
// ---------------------------------------------------------------------------
//
void CVcxMyVideosMdsAlbums::DoGetAlbumContentIdsL( TUint32 aAlbumId,
        RArray<TVcxMyVideosAlbumVideo>& aContentArray,
        MVcxMyVideosMdsAlbumsObserver& aClient )
    {
    MPX_DEBUG1("CVcxMyVideosMdsAlbums::DoGetAlbumContentIdsL() start");
        
    delete iRelationQuery;
    iRelationQuery = NULL;
    
    iClient       = &aClient;
    iAlbumId      = aAlbumId;
    iAlbumContent = &aContentArray;
    
    iRelationQuery = iMdsDb.MdsSessionL().NewRelationQueryL( *iNamespaceDef, this );

    CMdELogicCondition& rootCondition = iRelationQuery->Conditions();

    //relation left side contains and...
    CMdERelationCondition& relationCondition =
        rootCondition.AddRelationConditionL( *iContainsRelationDef, ERelationConditionSideLeft); // "AND"
    
    CMdELogicCondition& leftCondition = relationCondition.LeftL();
    CMdELogicCondition& rightCondition = relationCondition.RightL();
    
    //...left side is album...
    leftCondition.AddObjectConditionL( *iAlbumObjectDef );
    //...and left id is aAlbumId..
    leftCondition.AddObjectConditionL( aAlbumId );
    
    //Right side is video
    rightCondition.AddObjectConditionL( *iMdsDb.iVideoObjectDef ); 
    
    iRelationQuery->FindL();

    MPX_DEBUG1("CVcxMyVideosMdsAlbums::DoGetAlbumContentIdsL() exit");
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosMdsAlbums::GetAlbumContentVideosL
// ---------------------------------------------------------------------------
//
void CVcxMyVideosMdsAlbums::GetAlbumContentVideosL( TUint32 aAlbumId, CMPXMedia& aVideoList,
        MVcxMyVideosMdsAlbumsObserver& aClient )
    {
    MPX_DEBUG1("CVcxMyVideosMdsAlbums::GetAlbumContentVideosL() start");
    
    CVcxMyVideosMdsCmdGetAlbumContentVideos* cmd = new (ELeave) CVcxMyVideosMdsCmdGetAlbumContentVideos;
    CleanupStack::PushL( cmd ); // 1->
    cmd->iCmdType            = CVcxMyVideosMdsDb::EGetAlbumContentVideos;
    cmd->iClient             = &aClient;
    cmd->iAlbumId            = aAlbumId;
    cmd->iAlbumContentVideos = &aVideoList;
    iMdsDb.iCmdQueue->ExecuteCmdL( cmd ); //owneship moves
    CleanupStack::Pop( cmd ); // <-1
    MPX_DEBUG1("CVcxMyVideosMdsAlbums::GetAlbumContentVideosL() exit");
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosMdsAlbums::DoGetAlbumContentVideosL
// ---------------------------------------------------------------------------
//
void CVcxMyVideosMdsAlbums::DoGetAlbumContentVideosL( TUint32 aAlbumId, CMPXMedia& aVideoList,
        MVcxMyVideosMdsAlbumsObserver& aClient )
    {
    MPX_DEBUG1("CVcxMyVideosMdsAlbums::DoGetAlbumContentVideosL() start");
    
    delete iVideoQuery;
    iVideoQuery = NULL;

    iClient    = &aClient;
    iVideoList = &aVideoList;
    iAlbumId   = aAlbumId;
    
    iVideoQuery = iMdsDb.MdsSessionL().NewObjectQueryL( *iNamespaceDef, *iMdsDb.iVideoObjectDef, this );
    CMdELogicCondition& rootCondition = iVideoQuery->Conditions();
    CMdERelationCondition& relationCondition = 
            rootCondition.AddRelationConditionL( *iContainsRelationDef );

    CMdELogicCondition& leftCondition = relationCondition.LeftL();

    //...left side is an album...
    leftCondition.AddObjectConditionL( *iAlbumObjectDef );
    //...and left id is aAlbumId..
    leftCondition.AddObjectConditionL( aAlbumId );

    //and queried object is on the right side of the relation
    relationCondition.SetSide( ERelationConditionSideRight );
    
    const TInt maxItemsInQueryResult = 500;
    iVideoQuery->FindL( KMdEQueryDefaultMaxCount, maxItemsInQueryResult );
    
    MPX_DEBUG1("CVcxMyVideosMdsAlbums::DoGetAlbumContentVideosL() exit");
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosMdsAlbums::GetSchemaDefinitionsL
// ---------------------------------------------------------------------------
//
void CVcxMyVideosMdsAlbums::GetSchemaDefinitionsL()
    {
    iNamespaceDef = &(iMdsDb.MdsSessionL().GetDefaultNamespaceDefL());
    
    _LIT( KVcxAlbumObjectName, "Album" );
    iAlbumObjectDef = &(iNamespaceDef->GetObjectDefL( KVcxAlbumObjectName ));

    _LIT( KVcxTypePropertyName, "Type" );
    iTypePropertyDef = &(iAlbumObjectDef->GetPropertyDefL( KVcxTypePropertyName ));

    _LIT( KVcxContainsRelationName, "Contains" );
    iContainsRelationDef =
        &(iNamespaceDef->GetRelationDefL( KVcxContainsRelationName ));
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosMdsAlbums::Object2MediaL
// For album objects only.
// ---------------------------------------------------------------------------
//
void CVcxMyVideosMdsAlbums::Object2MediaL(
        CMdEObject& aObject,
        CMPXMedia& aAlbum )
    {    
    aAlbum.SetTObjectValueL<TMPXGeneralType>(KMPXMediaGeneralType, EMPXGroup );
   
    CMdEProperty* property = NULL;

    // ID
    TMPXItemId mpxId;
    mpxId.iId1 = aObject.Id();
    mpxId.iId2 = KVcxMvcMediaTypeAlbum;
    aAlbum.SetTObjectValueL<TMPXItemId>( KMPXMediaGeneralId, mpxId );
    
    // TITLE
    if ( aObject.Property( *iMdsDb.iTitlePropertyDef, property, 0 ) != KErrNotFound )
        {
        aAlbum.SetTextValueL( KMPXMediaGeneralTitle,
                static_cast<CMdETextProperty*>(property)->Value());
        }
       
    // CREATION DATE
    if ( aObject.Property( *iMdsDb.iCreationDatePropertyDef, property, 0 ) != KErrNotFound )
        {
        aAlbum.SetTObjectValueL<TInt64>( KMPXMediaGeneralDate,
                static_cast<CMdETimeProperty*>(property)->Value().Int64() );
        }
              
    // MODIFIED DATE
    if ( aObject.Property( *iMdsDb.iLastModifiedDatePropertyDef, property, 0 ) != KErrNotFound )
        {
        aAlbum.SetTObjectValueL<TInt64>( KVcxMediaMyVideosModifiedDate,
                static_cast<CMdETimeProperty*>(property)->Value().Int64() );
        }

#if 0 //TODO
    // ALBUM TYPE
    if ( aObject.Property( *iTypePropertyDef, property, 0 ) != KErrNotFound )
        {
        aAlbum.SetTObjectValueL<TUint16>( KVcxMediaMyVideosAlbumType,
                static_cast<CMdETUint16Property*>(property)->Value() );
        }
#endif

    //TODO: usage count
    
    aAlbum.SetTObjectValueL( KMPXMediaGeneralType, EMPXGroup );
    aAlbum.SetTObjectValueL( KMPXMediaGeneralCategory, EMPXNoCategory );
    aAlbum.SetTObjectValueL( KVcxMediaMyVideosCategoryItemCount, 0 );
    aAlbum.SetTObjectValueL( KVcxMediaMyVideosCategoryNewItemCount, 0 );

    }

// ---------------------------------------------------------------------------
// CVcxMyVideosMdsAlbums::Media2ObjectL
// Called by AddAlbumL()
// ---------------------------------------------------------------------------
//
void CVcxMyVideosMdsAlbums::Media2ObjectL(
        CMPXMedia& aAlbum,
        CMdEObject& aObject)
    {
    CMdEProperty* property;

    // MDS ID
    
    // TITLE (NAME)
    if ( aAlbum.IsSupported( KMPXMediaGeneralTitle ) )
        {
        if ( TVcxMyVideosCollectionUtil::Title( aAlbum ).Length() > KVcxMvcMaxTitleLength )
            {
            User::Leave( KErrArgument );
            }
        
        if ( aObject.Property( *iMdsDb.iTitlePropertyDef, property, 0 ) != KErrNotFound )
            {
            static_cast<CMdETextProperty*>(property)->SetValueL(
                    aAlbum.ValueText( KMPXMediaGeneralTitle ) );
            }
        else
            {
            aObject.AddTextPropertyL(
                    *iMdsDb.iTitlePropertyDef, aAlbum.ValueText( KMPXMediaGeneralTitle ) );
            }
        }
        
    // KMPXMediaGeneralDate ( creation date )
    if ( aAlbum.IsSupported( KMPXMediaGeneralDate ) )
        {
        TInt64 creationDateInt64 = 0;
        creationDateInt64 = aAlbum.ValueTObjectL<TInt64>( KMPXMediaGeneralDate );
        TTime creationDate( creationDateInt64 );
        if ( aObject.Property( *iMdsDb.iCreationDatePropertyDef, property, 0 ) != KErrNotFound )
            {
            static_cast<CMdETimeProperty*>(property)->SetValueL( creationDate );
            }
        else
            {
            aObject.AddTimePropertyL( *iMdsDb.iCreationDatePropertyDef, creationDate );
            }
        }
        
    // KVcxMediaMyVideosModifiedDate
    if ( aAlbum.IsSupported( KVcxMediaMyVideosModifiedDate ) )
        {
        TInt64 modifiedDateInt64 = aAlbum.ValueTObjectL<TInt64>( KVcxMediaMyVideosModifiedDate );
        TTime modifiedDate( modifiedDateInt64 );
        if ( aObject.Property( *iMdsDb.iLastModifiedDatePropertyDef, property, 0 ) != KErrNotFound )
            {
            static_cast<CMdETimeProperty*>(property)->SetValueL( modifiedDate );
            }
        else
            {
            aObject.AddTimePropertyL( *iMdsDb.iLastModifiedDatePropertyDef, modifiedDate );
            }
        }        
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosMdsAlbums::GetSchemaDefinitionsL
// ---------------------------------------------------------------------------
//
void CVcxMyVideosMdsAlbums::HandleQueryNewResults(CMdEQuery& aQuery,
            TInt aFirstNewItemIndex,
            TInt aNewItemCount )
    {
    if ( &aQuery == iVideoQuery )
        {
        TRAPD( err, HandleVideoQueryResultsL( aQuery, KErrNone,
                aFirstNewItemIndex, aNewItemCount, EFalse /* completed */ ) );
        if ( err != KErrNone )
            {
            MPX_DEBUG2("CVcxMyVideosMdsAlbums:: HandleVideoQueryResultsL() left: %d", err);
            }
        }
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosMdsAlbums::HandleQueryCompleted
// ---------------------------------------------------------------------------
//
void CVcxMyVideosMdsAlbums::HandleQueryCompleted( CMdEQuery& aQuery, TInt aError )
    {
    if ( &aQuery == iAlbumQuery )
        {
        TRAPD( err, HandleAlbumQueryCompletedL( aQuery, aError ) );
        if ( err != KErrNone )
            {
            MPX_DEBUG2("CVcxMyVideosMdsAlbums:: HandleAlbumQueryCompletedL() left: %d", err);
            }
        }
    else if ( &aQuery == iVideoQuery )
        {
        TRAPD( err, HandleVideoQueryResultsL( aQuery, aError, 0, 0, ETrue /* completed */ ) );
        if ( err != KErrNone )
            {
            MPX_DEBUG2("CVcxMyVideosMdsAlbums:: HandleVideoQueryResultsL() left: %d", err);
            }
        }
    else if ( &aQuery == iRelationQuery )
        {
        TRAPD( err, HandleRelationQueryCompletedL( aQuery, aError ) );
        if ( err != KErrNone )
            {
            MPX_DEBUG2("CVcxMyVideosMdsAlbums:: HandleRelationQueryCompletedL() left: %d", err);
            }
        }

    iMdsDb.iCmdQueue->CmdFinished();
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosMdsAlbums::HandleAlbumQueryCompletedL
// ---------------------------------------------------------------------------
//
void CVcxMyVideosMdsAlbums::HandleAlbumQueryCompletedL( CMdEQuery& /*aQuery*/, TInt aError )
    {
    MPX_DEBUG1("CVcxMyVideosMdsAlbums::HandleAlbumQueryCompletedL() start");
    
    if ( aError != KErrNone )
        {
        MPX_DEBUG2("CVcxMyVideosMdsAlbums:: object query failed with error: %d", aError);
        return;
        }
    
    CMPXMediaArray* array = iAlbumList->Value<CMPXMediaArray>( KMPXMediaArrayContents );
    
    CMPXMedia* media;    
    TInt count = iAlbumQuery->Count();
    
    iAlbumList->SetTObjectValueL<TInt>( KMPXMediaArrayCount, count );
    
    for ( TInt i = 0; i < count; i++ )
        {
        CMdEObject& object = iAlbumQuery->Result( i );

        media = CMPXMedia::NewL();
        CleanupStack::PushL( media ); // 1->

        Object2MediaL( object, *media );
        
#ifdef _DEBUG
        TBuf<KVcxMvcMaxTitleLength> title;
        title = TVcxMyVideosCollectionUtil::Title( *media );
        MPX_DEBUG2("CVcxMyVideosMdsAlbums:: object title: %S", &title);
#endif        

        array->AppendL( media );

        CleanupStack::Pop( media ); // <-1
        }
    
    iClient->HandleGetAlbumsResp( iAlbumList );
    iAlbumList = NULL;
    
    MPX_DEBUG1("CVcxMyVideosMdsAlbums::HandleAlbumQueryCompletedL() exit");
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosMdsAlbums::HandleVideoQueryCompletedL
// ---------------------------------------------------------------------------
//
void CVcxMyVideosMdsAlbums::HandleVideoQueryResultsL( CMdEQuery& /*aQuery*/, TInt aError,
    TInt aFirstNewItemIndex, TInt aNewItemCount, TBool aComplete  )
    {
    MPX_DEBUG1("CVcxMyVideosMdsAlbums::HandleVideoQueryResultsL() start");
    
    if ( aComplete )
        {
        // error code given only if completed
        if ( aError != KErrNone )
            {
            MPX_DEBUG2("CVcxMyVideosMdsAlbums:: object query failed with error: %d", aError);
            }

        iClient->HandleGetAlbumContentVideosResp( iAlbumId, *iVideoList, 
                aError, aFirstNewItemIndex, aNewItemCount, aComplete );
        return;
        }
    
    CMPXMediaArray* array = iVideoList->Value<CMPXMediaArray>( KMPXMediaArrayContents );
    
    CMPXMedia* media;    
    TInt count = iVideoQuery->Count();
    
    iVideoList->SetTObjectValueL<TInt>( KMPXMediaArrayCount, count );
    
    for ( TInt i = aFirstNewItemIndex; i < count; i++ )
        {
        CMdEObject& object = iVideoQuery->Result( i );

        media = CMPXMedia::NewL();
        CleanupStack::PushL( media ); // 1->

        iMdsDb.Object2MediaL( object, *media );
        
#ifdef _DEBUG
        TBuf<256> title;
        title = TVcxMyVideosCollectionUtil::Title( *media );
        MPX_DEBUG2("CVcxMyVideosMdsAlbums:: object title: %S", &title);
#endif        

        array->AppendL( media );

        CleanupStack::Pop( media ); // <-1
        }
    
    iClient->HandleGetAlbumContentVideosResp( iAlbumId, *iVideoList, 
            aError, aFirstNewItemIndex, aNewItemCount, aComplete );
    iVideoList = NULL;
    
    MPX_DEBUG1("CVcxMyVideosMdsAlbums::HandleVideoQueryResultsL() exit");
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosMdsAlbums::HandleRelationQueryCompletedL
// ---------------------------------------------------------------------------
//
void CVcxMyVideosMdsAlbums::HandleRelationQueryCompletedL( CMdEQuery& /*aQuery*/, TInt aError )
    {
    MPX_DEBUG1("CVcxMyVideosMdsAlbums::HandleRelationQueryCompletedL() start");
    
    if ( aError != KErrNone )
        {
        MPX_DEBUG2("CVcxMyVideosMdsAlbums:: relation query failed with error: %d", aError);
        return;
        }
            
    TInt count = iRelationQuery->Count();
    TVcxMyVideosAlbumVideo video;
    for ( TInt i = 0; i < count; i++ )
        {
        MPX_DEBUG4("Relation (id = %d): %d contains %d", iRelationQuery->Result( i ).Id(),
                iRelationQuery->Result( i ).LeftObjectId(),
                iRelationQuery->Result( i ).RightObjectId());
        video.iMdsId         = iRelationQuery->Result( i ).RightObjectId();
        video.iRelationMdsId = iRelationQuery->Result( i ).Id();
        iAlbumContent->AppendL( video );
        }
    
    iClient->HandleGetAlbumContentIdsResp( iAlbumId, *iAlbumContent );

    MPX_DEBUG1("CVcxMyVideosMdsAlbums::HandleRelationQueryCompletedL() exit");
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosMdsAlbums::AddVideosToAlbumL
// ---------------------------------------------------------------------------
//
void CVcxMyVideosMdsAlbums::AddVideosToAlbumL( CMPXMedia* aCmd,
        MVcxMyVideosMdsAlbumsObserver& aClient )
    {
    MPX_DEBUG1("CVcxMyVideosMdsAlbums::AddVideosToAlbumL() start");
    
    CVcxMyVideosMdsCmdAddVideosToAlbum* cmd = new (ELeave) CVcxMyVideosMdsCmdAddVideosToAlbum;
    CleanupStack::PushL( cmd ); // 1->
    cmd->iCmdType  = CVcxMyVideosMdsDb::EAddVideosToAlbum;
    cmd->iClient   = &aClient;
    cmd->iMpxCmd   = aCmd;
    iMdsDb.iCmdQueue->ExecuteCmdL( cmd ); //ownership moves
    CleanupStack::Pop( cmd ); // <-1
    MPX_DEBUG1("CVcxMyVideosMdsAlbums::AddVideosToAlbumL() exit");
    }
    
// ---------------------------------------------------------------------------
// CVcxMyVideosMdsAlbums::DoAddVideosToAlbumL
// Called by iMdsDb.iCmdQueue
// ---------------------------------------------------------------------------
//
void CVcxMyVideosMdsAlbums::DoAddVideosToAlbumL( CMPXMedia* aCmd,
        MVcxMyVideosMdsAlbumsObserver& aClient )
    {
    iItemArray.Reset();
    iResultBuffer.Close();
    CMdERelation* relation;
    CMPXMediaArray* videoArray = TVcxMyVideosCollectionUtil::MediaArrayL( *aCmd );
    
    TInt albumId = TVcxMyVideosCollectionUtil::Uint32ValueL( *aCmd );
    
    TInt count = videoArray->Count();
    iItemArray.Reserve( count );
    for ( TInt i = 0; i < count; i++ )
        {
        // Filter already failed items out from the request (KErrAlreadyExists)
        if ( TVcxMyVideosCollectionUtil::Int32ValueL( *videoArray->AtL( i ) ) 
                != KErrAlreadyExists )
            {
            relation = iMdsDb.MdsSessionL().NewRelationL(
                    *iContainsRelationDef, albumId,
                    TVcxMyVideosCollectionUtil::IdL( *videoArray->AtL( i ) ) );
            CleanupStack::PushL( relation );
            iItemArray.AppendL( relation );
            CleanupStack::Pop( relation );        
            }
        }
    
    iClient = &aClient;
    iMpxCmd = aCmd;

    iAsyncOperation = EVcxAddVideosToAlbum;

    iMdsDb.MdsSessionL().AddItemsAsyncL( iItemArray, iStatus, iResultBuffer );
    SetActive();
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosMdsAlbums::RemoveRelationsL
// ---------------------------------------------------------------------------
//
void CVcxMyVideosMdsAlbums::RemoveRelationsL( RArray<TUint32>& aRelationIds,
        RArray<TUint32>& aResults, MVcxMyVideosMdsAlbumsObserver& aClient )
    {
    MPX_DEBUG1("CVcxMyVideosMdsAlbums::RemoveRelationsL() start");
    
    CVcxMyVideosMdsCmdRemoveRelations* cmd = new (ELeave) CVcxMyVideosMdsCmdRemoveRelations;
    CleanupStack::PushL( cmd ); // 1->
    cmd->iCmdType     = CVcxMyVideosMdsDb::ERemoveRelations;
    cmd->iClient      = &aClient;
    cmd->iRelationIds = &aRelationIds;
    cmd->iResults     = &aResults;
    iMdsDb.iCmdQueue->ExecuteCmdL( cmd ); //owneship moves
    CleanupStack::Pop( cmd ); // <-1
    MPX_DEBUG1("CVcxMyVideosMdsAlbums::RemoveRelationsL() exit");    
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosMdsAlbums::DoRemoveRelationsL
// ---------------------------------------------------------------------------
//
void CVcxMyVideosMdsAlbums::DoRemoveRelationsL( RArray<TUint32>& aRelationIds,
        RArray<TUint32>& aResults, MVcxMyVideosMdsAlbumsObserver& aClient )
    {
    MPX_DEBUG1("CVcxMyVideosMdsAlbums::DoRemoveRelationsL() start");

    iResultBuffer.Close();
    
    iClient            = &aClient;
    iResultArrayUint32 = &aResults;
    iIdArray           = &aRelationIds;
    
    iAsyncOperation = EVcxRemoveRelations;
    
    iMdsDb.MdsSessionL().RemoveRelationsAsyncL( *iIdArray, iStatus, iResultBuffer, iNamespaceDef );
    SetActive();
    
    MPX_DEBUG1("CVcxMyVideosMdsAlbums::DoRemoveRelationsL() exit");
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosMdsAlbums::AddAlbumL
// ---------------------------------------------------------------------------
//
void CVcxMyVideosMdsAlbums::AddAlbumL( CMPXMedia& aAlbum )
    {
    MPX_DEBUG1("CVcxMyVideosMdsAlbums::AddAlbumL() start");
             
    CMdEObject* object = iMdsDb.MdsSessionL().NewObjectLC(
            *iAlbumObjectDef, KNullDesC ); // 1->

    Media2ObjectL( aAlbum, *object );
    iMdsDb.SetCreationAndModifiedDatesL( *object );

    TUint32 mdsId;

#ifdef _DEBUG
    TRAPD( err, mdsId = iMdsDb.MdsSessionL().AddObjectL( *object ) );

    if ( err != KErrNone )
        {
        MPX_DEBUG2( "CVcxMyVideosMdsAlbums:: iMdsDb.MdsSessionL().AddObjectL leaved with error: %d", err );
        User::Leave( err );
        }
#else

    mdsId = iMdsDb.MdsSessionL().AddObjectL( *object );

#endif
    
    if ( mdsId == KNoId )
        {
        MPX_DEBUG1( "CVcxMyVideosMdsAlbums::AddAlbumL could not add new album" );
        User::Leave( KErrGeneral );
        }
    
    MPX_DEBUG2( "CVcxMyVideosMdsAlbums::AddAlbumL album created, mds id: %d", mdsId );

    aAlbum.SetTObjectValueL<TMPXItemId>( KMPXMediaGeneralId, TMPXItemId( mdsId, KVcxMvcMediaTypeAlbum ) );
    
    CleanupStack::PopAndDestroy( object ); // <-1
    
    MPX_DEBUG1("CVcxMyVideosMdsAlbums::AddAlbumL() exit");
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosMdsAlbums::SetAlbumL
// ---------------------------------------------------------------------------
//
void CVcxMyVideosMdsAlbums::SetAlbumL( CMPXMedia& aVideo )
    {
    MPX_DEBUG1("CVcxMyVideosMdsDb::SetAlbumL() start");

    TMPXItemId mpxId = TVcxMyVideosCollectionUtil::IdL( aVideo );

    MPX_DEBUG2("CVcxMyVideosMdsAlbums::SetAlbumL updating object %d ", mpxId.iId1);
    
    CMdEObject* object =
            iMdsDb.MdsSessionL().OpenObjectL( mpxId.iId1, *iAlbumObjectDef );
    if ( !object )
        {
        // No object with this ID was found!
        MPX_DEBUG1("CVcxMyVideosMdsAlbums::SetAlbumL no object found");
        User::Leave( KErrNotFound );
        }
    else
        {
        MPX_DEBUG1("CVcxMyVideosMdsAlbums::SetAlbumL object found");
        
        if ( object->OpenForModifications() )
            {
            CleanupStack::PushL( object ); // 1->
            
            Media2ObjectL( aVideo, *object );
            
            iMdsDb.MdsSessionL().CommitObjectL( *object );

            CleanupStack::PopAndDestroy( object );
            }
        else
            {
            // Object is already locked!
            MPX_DEBUG1("CVcxMyVideosMdsAlbums::SetAlbumL object was locked!");
            delete object;
            User::Leave( KErrInUse );
            }
        }

    MPX_DEBUG1("CVcxMyVideosMdsDb::SetAlbumL() exit");
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosMdsAlbums::RemoveAlbumsL
// ---------------------------------------------------------------------------
//
void CVcxMyVideosMdsAlbums::RemoveAlbumsL( CMPXMedia* aMpxCmd, MVcxMyVideosMdsAlbumsObserver& aClient )
    {
    MPX_DEBUG1("CVcxMyVideosMdsAlbums::RemoveAlbumsL() start");
    
    CVcxMyVideosMdsCmdRemoveAlbums* cmd = new (ELeave) CVcxMyVideosMdsCmdRemoveAlbums;
    CleanupStack::PushL( cmd ); // 1->
    cmd->iCmdType  = CVcxMyVideosMdsDb::ERemoveAlbums;
    cmd->iClient   = &aClient;
    cmd->iMpxCmd   = aMpxCmd;
    iMdsDb.iCmdQueue->ExecuteCmdL( cmd ); //owneship moves
    CleanupStack::Pop( cmd ); // <-1
    MPX_DEBUG1("CVcxMyVideosMdsAlbums::RemoveAlbumsL() exit");    
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosMdsAlbums::DoRemoveAlbumsL
// ---------------------------------------------------------------------------
//
void CVcxMyVideosMdsAlbums::DoRemoveAlbumsL( CMPXMedia* aMpxCmd,
        MVcxMyVideosMdsAlbumsObserver& aClient )
    {
    MPX_DEBUG1("CVcxMyVideosMdsAlbums::DoRemoveAlbumsL() start");

    iResultBuffer.Close();

    iClient = &aClient;
    iMpxCmd = aMpxCmd;
    
    RArray<TUint32> idArray;
    CleanupClosePushL( idArray );

    CMPXMediaArray* mediaArray = TVcxMyVideosCollectionUtil::MediaArrayL( *aMpxCmd );

    TVcxMyVideosCollectionUtil::GetIdsFromMediaArrayL( *mediaArray, idArray );
    
    iAsyncOperation = EVcxRemoveAlbums;
    
    iMdsDb.MdsSessionL().RemoveObjectsAsyncL( idArray, iStatus, iResultBuffer, iNamespaceDef );
    
    CleanupStack::PopAndDestroy( &idArray );
    SetActive();

    MPX_DEBUG1("CVcxMyVideosMdsAlbums::DoRemoveAlbumsL() exit");    
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosMdsAlbums::RunL
// From CActive.
// ---------------------------------------------------------------------------
//
void CVcxMyVideosMdsAlbums::RunL()
    {
#ifdef _DEBUG
    TInt status = iStatus.Int();
    if ( status != KErrNone )
        {
        MPX_DEBUG2("CVcxMyVideosMdsAlbums::RunL() iStatus = %d", status);
        }
#endif
    
    switch ( iAsyncOperation )
        {
        case EVcxAddVideosToAlbum:
            {
            HandleAddVideosToAlbumCompletedL();
            }
            break;
            
        case EVcxRemoveRelations:
            HandleRemoveRelationsCompletedL();
            break;
            
        case EVcxRemoveAlbums:
            HandleRemoveAlbumsCompletedL();
            break;
        }

    iMdsDb.iCmdQueue->CmdFinished();
    }

//TODO: implement RunError

// ---------------------------------------------------------------------------
// CVcxMyVideosMdsAlbums::HandleAddVideosToAlbumCompletedL
// ---------------------------------------------------------------------------
//
void CVcxMyVideosMdsAlbums::HandleAddVideosToAlbumCompletedL()
    {
    MPX_DEBUG1("CVcxMyVideosMdsAlbums::HandleAddVideosToAlbumCompletedL() start");
    
    iMdsDb.MdsSessionL().DeserializeItemsL( iResultBuffer, iItemArray );
    
    iClient->HandleAddVideosToAlbumResp( iMpxCmd, iItemArray );
    iResultBuffer.Close();
    iItemArray.Close();    

    MPX_DEBUG1("CVcxMyVideosMdsAlbums::HandleAddVideosToAlbumCompletedL() exit");
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosMdsAlbums::HandleRemoveRelationsCompletedL
// ---------------------------------------------------------------------------
//
void CVcxMyVideosMdsAlbums::HandleRemoveRelationsCompletedL()
    {
    MPX_DEBUG1("CVcxMyVideosMdsAlbums::HandleRemoveRelationsCompletedL() start");
    
    iMdsDb.MdsSessionL().DeserializeIdsL( iResultBuffer, NULL, NULL, iResultArrayUint32 );

#ifdef _DEBUG
    TItemId result;
    TInt count = iResultArrayUint32->Count();
    for ( TInt i = 0; i < count; i++ )
        {
        result = (*iResultArrayUint32)[i];
        MPX_DEBUG3("relation remove result[%d] = %d", i, result);
        }
#endif
    
    iClient->HandleRemoveRelationsResp( *iIdArray, *iResultArrayUint32 );

    MPX_DEBUG1("CVcxMyVideosMdsAlbums::HandleRemoveRelationsCompletedL() exit");
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosMdsAlbums::HandleRemoveAlbumsCompletedL
// ---------------------------------------------------------------------------
//
void CVcxMyVideosMdsAlbums::HandleRemoveAlbumsCompletedL()
    {
    MPX_DEBUG1("CVcxMyVideosMdsAlbums::HandleRemoveAlbumsCompletedL() start");

    RArray<TUint32> resultArray;
    resultArray.Reset();
    CleanupClosePushL( resultArray );
    
    iMdsDb.MdsSessionL().DeserializeIdsL( iResultBuffer, &resultArray, NULL, NULL  );

#ifdef _DEBUG
    TItemId result;
    TInt count = resultArray.Count();

    MPX_DEBUG2("CVcxMyVideosMdsAlbums:: remove albums result count = %d", count);

    for ( TInt i = 0; i < count; i++ )
        {
        result = resultArray[i];
        MPX_DEBUG3("CVcxMyVideosMdsAlbums:: remove albums result[%d] = %d", i, result);
        }
#endif

    iClient->HandleRemoveAlbumsResp( iMpxCmd, resultArray );

    CleanupStack::PopAndDestroy( &resultArray );
    
    MPX_DEBUG1("CVcxMyVideosMdsAlbums::HandleRemoveRelationsCompletedL() exit");
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosMdsAlbums::HandleRelationItemNotification
// From MMdERelationItemObserver
// ----------------------------------------------------------------------------
//
void CVcxMyVideosMdsAlbums::HandleRelationItemNotification(CMdESession& /*aSession*/, 
        TObserverNotificationType aType,
        const RArray<TMdERelation>& aRelationArray)
    {
    if ( iObserver )
        {
        iObserver->HandleRelationEvent( aType, aRelationArray );
        }
    }

#if 0
// ----------------------------------------------------------------------------
// CVcxMyVideosMdsAlbums::HandleRelationNotification
// From MMdERelationObserver
// ----------------------------------------------------------------------------
//
void CVcxMyVideosMdsAlbums::HandleRelationNotification(CMdESession& /*aSession*/, 
        TObserverNotificationType aType,
        const RArray<TItemId>& aRelationIdArray)
    {    
    switch ( aType )
        {
        case ENotifyAdd:
            MPX_DEBUG1("CVcxMyVideosMdsAlbums:: relation ENotifyAdd");
            if ( iObserver )
                {
                iObserver->HandleRelationIdEvent( aType, aRelationIdArray );
                }
            break;
        case ENotifyModify:
            MPX_DEBUG1("CVcxMyVideosMdsAlbums:: ENotifyModify");
            break;
        case ENotifyRemove:
            //remove is handled at HandleRelationItemNotification
            break;
        }
    }
#endif

// END OF FILE
