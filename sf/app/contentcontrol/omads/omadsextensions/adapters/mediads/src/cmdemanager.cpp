/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CMdEManager implementation
*
*/


#include "cmdemanager.h"
#include <mdeconstants.h>

#include <mmf\common\mmfcontrollerpluginresolver.h>

#include "logger.h"

// Warning:  #940-D: missing return statement at end of non-void function
#pragma  diag_remark 940

CMdEManager::CMdEManager( MMdEManagerObserver& aObserver ) :
    iObserver(aObserver)
    {
    }


CMdEManager* CMdEManager::NewL( MMdEManagerObserver& aObserver )
    {
    CMdEManager* self = new (ELeave) CMdEManager( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

void CMdEManager::ConstructL()
    {
    TRACE_FUNC;
    iMde = CMdESession::NewL( *this );
    }


CMdEManager::~CMdEManager()
    {
    TRACE_FUNC;
    Cancel(); // Cancel any request, if outstanding
    delete iAlbumQuery;
    delete iObjectQuery;
    delete iContainmentQuery;
    delete iMde;
    
    iAlbumsInProgress.ResetAndDestroy();
    iAlbums.ResetAndDestroy();
    }

void CMdEManager::GetAlbumsL()
    {
    TRACE_FUNC_ENTRY;
    iAlbumsInProgress.ResetAndDestroy();
    iAlbums.ResetAndDestroy();
    if ( !iMde )
        {
        LOGGER_WRITE("Session was not ready!");
        iState = EUninitialized;
        iMde = CMdESession::NewL( *this );
        }
    
    if ( iState == EUninitialized)
        {
        LOGGER_WRITE("Starting processing albums after session is ready");
        iState = EWaitingToEnumerateAlbums;
        }
    else if ( iState == EIdle )
        {
        StartProcessingAlbumsL();
        }
    else
        {
        LOGGER_WRITE_1("Wrong state: %d", iState);
        User::Leave( KErrGeneral );
        }
    
    TRACE_FUNC_EXIT;
    }

const CPlaylistItem& CMdEManager::AlbumL( TInt aAlbumId ) const
    {
    for ( TInt i=0; i<iAlbums.Count(); i++ )
        {
        if ( iAlbums[i]->Id() == aAlbumId )
            {
            LOGGER_WRITE("Album found");
            return *iAlbums[i];
            }
        }
    LOGGER_WRITE_1("CMdEManager::AlbumL - aAlbumId %d does not exist - Leaving KErrNotFound", aAlbumId);
    User::Leave( KErrNotFound );
    }

void CMdEManager::CreateAlbumL( CPlaylistItem& aAlbum )
    {
    TRACE_FUNC_ENTRY;
    if ( !iMde || iState != EIdle )
        {
        LOGGER_WRITE("Not ready!");
        User::Leave( KErrNotReady );
        }
    CMdENamespaceDef& defaultNamespaceDef = iMde->GetDefaultNamespaceDefL();
    CMdEObjectDef& albumObjDef = defaultNamespaceDef.GetObjectDefL( MdeConstants::Album::KAlbumObject );
    
    CMdEPropertyDef& titlePropDef = albumObjDef.GetPropertyDefL( MdeConstants::Object::KTitleProperty );   
    
    // Validate album name
    TInt albumLength = aAlbum.Title().Length();
    if ( albumLength < titlePropDef.MinTextLengthL() || 
         albumLength > titlePropDef.MaxTextLengthL() )
        {
        LOGGER_WRITE("Album length is not on valid range!");
        User::Leave( KErrBadName );
        }
    
    CMdEObject* albumObject = iMde->NewObjectLC( albumObjDef, KNullDesC );
    
    CMdEPropertyDef& sizePropDef = albumObjDef.GetPropertyDefL( MdeConstants::Object::KSizeProperty );
    albumObject->AddUint32PropertyL(sizePropDef, 0);
    CMdEPropertyDef& creationDatePropDef = albumObjDef.GetPropertyDefL( MdeConstants::Object::KCreationDateProperty );
    CMdEPropertyDef& lastModifiedDatePropDef = albumObjDef.GetPropertyDefL( MdeConstants::Object::KLastModifiedDateProperty );
    
    TTime timeDate;
    timeDate.UniversalTime();
    
    albumObject->AddTimePropertyL( creationDatePropDef, timeDate );
    albumObject->AddTimePropertyL( lastModifiedDatePropDef, timeDate );
    
    CMdEPropertyDef& typePropDef = albumObjDef.GetPropertyDefL( MdeConstants::Object::KItemTypeProperty );
    albumObject->AddTextPropertyL( typePropDef,MdeConstants::Album::KAlbumItemType );
    
    albumObject->AddTextPropertyL( titlePropDef, aAlbum.Title() );
    TItemId newAlbumId = iMde->AddObjectL( *albumObject );
    
    CleanupStack::PopAndDestroy( albumObject );
    
    if ( newAlbumId == KNoId )
        {
        LOGGER_WRITE("Adding album failed!");
        User::Leave( KErrGeneral );
        }
    LOGGER_WRITE_1("New almbum created, id: %d", newAlbumId);
    
    CMdERelationDef& relationDef = defaultNamespaceDef.GetRelationDefL( MdeConstants::Relations::KContains );
    TMdEObject mediaObject;
    RPointerArray<CMdEInstanceItem> relations;
    CleanupResetAndDestroyPushL( relations);
    for ( TInt i=0; i<aAlbum.ItemCount(); i++ )
        {
        TRAPD(err, iMde->CheckObjectL( mediaObject, aAlbum.ItemAt(i) ));
        LOGGER_WRITE_1("url: %S", &aAlbum.ItemAt(i));
        LOGGER_WRITE_1("CheckObjectL err: %d", err);
        if ( !err )
            {
            LOGGER_WRITE_1("object def: %S", &mediaObject.DefL().Name());
            if ( mediaObject.DefL().Name().Compare( MdeConstants::Image::KImageObject ) == 0 
                    || mediaObject.DefL().Name().Compare( MdeConstants::Video::KVideoObject ) == 0 )
                {
                CMdERelation* relation = iMde->NewRelationL( relationDef, newAlbumId, mediaObject.Id() );
                relations.AppendL( relation );
                }
            else
                {
                LOGGER_WRITE("type not supported");
                }
            }
        }
    
    if ( relations.Count() > 0 )
        {
        TInt err = iMde->AddItemsL( relations );
        LOGGER_WRITE_1("AddItemsL first err: %d", err);
        }
    
    CleanupStack::PopAndDestroy( &relations );
    
    aAlbum.SetId( newAlbumId );
    
    TRACE_FUNC_EXIT;
    }

void CMdEManager::ReplaceAlbumL( TInt aAlbumId, CPlaylistItem& aAlbum )
    {
    TRACE_FUNC_ENTRY;
    if ( !iMde || iState != EIdle )
        {
        LOGGER_WRITE("Not ready!");
        User::Leave( KErrNotReady );
        }
    // get old album from local cache and check do we need to change album title
    const CPlaylistItem& oldAlbum = AlbumL( aAlbumId );
    CMdENamespaceDef& defaultNamespaceDef = iMde->GetDefaultNamespaceDefL();
    if ( oldAlbum.Title().Compare( aAlbum.Title() ) != 0 )
        {
        // Title changed, open item from MdE and update it
        
        CMdEObjectDef& albumObjDef = defaultNamespaceDef.GetObjectDefL( MdeConstants::Album::KAlbumObject );
        
        CMdEPropertyDef& titlePropDef = albumObjDef.GetPropertyDefL( 
                MdeConstants::Object::KTitleProperty );
        
        // Validate album name
        TInt albumLength = aAlbum.Title().Length();
        if ( albumLength < titlePropDef.MinTextLengthL() || 
             albumLength > titlePropDef.MaxTextLengthL() )
            {
            LOGGER_WRITE("Album length is not on valid range!");
            User::Leave( KErrBadName );
            }
        
        CMdEObject* albumObject = iMde->OpenObjectL( aAlbumId, albumObjDef );
        
        CMdEProperty* titleProp = NULL;
        TInt index = albumObject->Property( titlePropDef, titleProp );
        if ( index == KErrNotFound )
            {
            LOGGER_WRITE("Cannot find title property");
            User::Leave( KErrCorrupt );
            }
        
        LOGGER_WRITE("Change title property");
        titleProp->SetTextValueL( aAlbum.Title() );
        
        CMdEPropertyDef& lastModDatePropDef = albumObjDef.GetPropertyDefL(
                MdeConstants::Object::KLastModifiedDateProperty );
        CMdEProperty* lastModDateProp = NULL;
        index = albumObject->Property( lastModDatePropDef, lastModDateProp );
        
        if ( index == KErrNotFound )
            {
            LOGGER_WRITE("Cannot find lastModDateProp property");
            User::Leave( KErrCorrupt );
            }
        
        TTime now;
        now.UniversalTime();
        lastModDateProp->SetTimeValueL( now );
        
        iMde->CommitObjectL( *albumObject );
        }
    
    // Update album and content relations
    // Search added relations
    CMdERelationDef& containsRelationDef = defaultNamespaceDef.GetRelationDefL( MdeConstants::Relations::KContains );
    TMdEObject mediaObject;
    RPointerArray<CMdEInstanceItem> addedRelations;
    CleanupResetAndDestroyPushL( addedRelations);
    for ( TInt i=0; i< aAlbum.ItemCount(); i++)
        {
        TInt index(KErrNotFound);
        TInt foundRes = oldAlbum.FindItem( aAlbum.ItemAt(i) ,index );
        if ( foundRes != 0 )
            {
            // Item not found for old album -> Added relation
            // Find object by uri
            TRAPD( err, iMde->CheckObjectL( mediaObject, aAlbum.ItemAt(i) ));
            LOGGER_WRITE_1("url: %S", &aAlbum.ItemAt(i));
            LOGGER_WRITE_1("CheckObjectL err: %d", err);
            if ( !err )
                {
                LOGGER_WRITE_1("object def: %S", &mediaObject.DefL().Name());
                if ( mediaObject.DefL().Name().Compare( MdeConstants::Image::KImageObject ) == 0 
                        || mediaObject.DefL().Name().Compare( MdeConstants::Video::KVideoObject ) == 0 )
                    {
                    CMdERelation* relation = iMde->NewRelationL( containsRelationDef, aAlbumId, mediaObject.Id() );
                    addedRelations.AppendL( relation );
                    }
                else
                    {
                    LOGGER_WRITE("type not supported");
                    }
                }
            
            }
        }
    if ( addedRelations.Count() > 0 )
        {
        TInt err = iMde->AddItemsL( addedRelations );
        LOGGER_WRITE_1("AddItemsL first err: %d", err);
        }
    
    CleanupStack::PopAndDestroy( &addedRelations );
    
    // search removed relations
    if ( iContainmentQuery )
        {
        delete iContainmentQuery;
        iContainmentQuery = NULL;
        }
    iContainmentQuery = iMde->NewRelationQueryL( defaultNamespaceDef, this );
    
    CMdELogicCondition& rootCondition = iContainmentQuery->Conditions();
    CMdERelationCondition& relationCondition =
        rootCondition.AddRelationConditionL(containsRelationDef,
                                            ERelationConditionSideLeft); // "AND"
    
    CMdELogicCondition& leftCondition = relationCondition.LeftL();
    CMdELogicCondition& rightCondition = relationCondition.RightL();
    
    leftCondition.AddObjectConditionL( aAlbumId );
    
    CMdELogicCondition& objectDefLogicCond = 
            rightCondition.AddLogicConditionL( ELogicConditionOperatorOr);
    
    TBool removingRelationsNeeded( EFalse );
    for ( TInt i=0; i< oldAlbum.ItemCount(); i++)
        {
        TInt index(KErrNotFound);
        TInt foundRes = aAlbum.FindItem( oldAlbum.ItemAt(i) ,index );
        if ( foundRes != 0 )
            {
            removingRelationsNeeded = ETrue;
            // Item not found from new album -> Removed relation (add uri to search condition)
            LOGGER_WRITE_1("relation to be removed, uri: %S", &oldAlbum.ItemAt(i) );
            objectDefLogicCond.AddObjectConditionL( EObjectConditionCompareUri, oldAlbum.ItemAt(i) );
            }
        }
    
    if ( removingRelationsNeeded )
        {
        // find all removed relation ID:s. HandleRelationQueryCompleted will be called when ready.
        iContainmentQuery->SetResultMode( EQueryResultModeId );
        iContainmentQuery->FindL();
        iState = EReplacingAlbum;
        }
    else
        {
        // All done
        iState = EIdle;
        iObserver.AlbumReplaced( KErrNone );
        }
    
    TRACE_FUNC_EXIT;
    }

void CMdEManager::DeleteAlbumL( TInt aAlbumId )
    {
    TRACE_FUNC_ENTRY;
    if ( !iMde || iState != EIdle )
        {
        LOGGER_WRITE("Not ready!");
        User::Leave( KErrNotReady );
        }
    
    CMdENamespaceDef& defaultNamespaceDef = iMde->GetDefaultNamespaceDefL();
    CMdEObjectDef& albumObjDef = defaultNamespaceDef.GetObjectDefL( MdeConstants::Album::KAlbumObject );
    
    CMdEObject* albumObject = iMde->GetObjectL( aAlbumId, albumObjDef );
    
    CMdEPropertyDef& typePropDef = albumObjDef.GetPropertyDefL( 
           MdeConstants::Album::KTypeProperty );
    
    CMdEProperty* typeProp = NULL;
    TInt index = albumObject->Property( typePropDef, typeProp );
    if ( index != KErrNotFound )
        {
        TUint16 typeVal = typeProp->Uint16ValueL();
        if ( typeVal == MdeConstants::Album::EAlbumSystemFavourite )
            {
            LOGGER_WRITE("Item type is EAlbumSystemFavourite, deletion not allowed!");
            User::Leave( KErrPermissionDenied );
            }
        }
    
    TItemId removedId(KNoId);
    removedId = iMde->RemoveObjectL( aAlbumId );
    if ( removedId == KNoId )
        {
        LOGGER_WRITE("Deletion failed!");
        User::Leave( KErrNotFound );
        }
    
    TRACE_FUNC_EXIT;
    }

void CMdEManager::StartProcessingAlbumsL()
    {
    TRACE_FUNC_ENTRY;
    CMdENamespaceDef& defaultNamespaceDef = iMde->GetDefaultNamespaceDefL();
    CMdEObjectDef& albumObjDef = defaultNamespaceDef.GetObjectDefL( MdeConstants::Album::KAlbumObject );
    if ( iAlbumQuery )
        {
        delete iAlbumQuery;
        iAlbumQuery = NULL;
        }
    // query objects with object definition "Album"
    iAlbumQuery = iMde->NewObjectQueryL( defaultNamespaceDef, albumObjDef, this );
    
    // Add order
    CMdEObjectDef& objdef = defaultNamespaceDef.GetObjectDefL( MdeConstants::Object::KBaseObject );
    CMdEPropertyDef& propDef = objdef.GetPropertyDefL( MdeConstants::Object::KTitleProperty );
    TMdEOrderRule rule( propDef, ETrue );
    iAlbumQuery->AppendOrderRuleL( rule );
    
    iAlbumQuery->FindL();
    
    iState = EEnumeratingAlbums;

    TRACE_FUNC_EXIT;
    }
    

void CMdEManager::FindItemsOnAlbumL( TItemId aAlbumObjectId )
    {
    TRACE_FUNC_ENTRY;
    if ( !iMde )
        {
        LOGGER_WRITE("Session was not ready!");
        User::Leave( KErrNotReady );
        }
    CMdENamespaceDef& defaultNamespaceDef = iMde->GetDefaultNamespaceDefL();
    CMdEObjectDef& objDef = defaultNamespaceDef.GetObjectDefL(  MdeConstants::Object::KBaseObject );
    
    CMdEPropertyDef& titlePropDef = objDef.GetPropertyDefL( MdeConstants::Object::KTitleProperty );
    
    if ( iObjectQuery )
        {
        delete iObjectQuery;
        iObjectQuery = NULL;
        }
    iObjectQuery = iMde->NewObjectQueryL( defaultNamespaceDef, objDef, this );
    
    // get only "Title" property
    iObjectQuery->AddPropertyFilterL( &titlePropDef );
    
    CMdEObjectDef& objdef = defaultNamespaceDef.GetObjectDefL( MdeConstants::Object::KBaseObject );
    CMdEPropertyDef& propDef = objdef.GetPropertyDefL( MdeConstants::Object::KTitleProperty );

    CMdELogicCondition& rootCond = iObjectQuery->Conditions();

    CMdERelationDef& containsRelDef = defaultNamespaceDef.GetRelationDefL( 
        MdeConstants::Relations::KContains );

    // query right side objects from relations
    CMdERelationCondition& relCond = rootCond.AddRelationConditionL( 
        containsRelDef, ERelationConditionSideRight );

    // left side object of relation must be defined album object
    CMdELogicCondition& leftRelCond = relCond.LeftL();
    leftRelCond.AddObjectConditionL( aAlbumObjectId );
    
    iObjectQuery->FindL();

    TRACE_FUNC_EXIT;
    }

/**
 * Called to notify the observer that opening the session has been 
 * completed and, if the opening succeeded, the session is ready for use.
 *
 * @param aSession session
 * @param aError   <code>KErrNone</code>, if opening the session succeeded;
 *                 or one of the system-wide error codes, if opening the 
 *                 session failed
 */
void CMdEManager::HandleSessionOpened(CMdESession& /*aSession*/, TInt aError)
    {
    TRACE_FUNC_ENTRY;
    if ( !aError )
        {
        LOGGER_WRITE("Session opened");
        if ( iState == EWaitingToEnumerateAlbums )
            {
            iState = EIdle;
            TRAPD(err, StartProcessingAlbumsL());
            if ( err )
                {
                iObserver.AlbumsReaded( err );
                }
            }
        else
            {
            iState = EIdle;
            }
        }
    else
        {
        LOGGER_WRITE_1("Error happened on opening session, aError: %d", aError);
        if ( iState == EWaitingToEnumerateAlbums )
            {
            iObserver.AlbumsReaded( aError );
            }
        iState = EUninitialized;
        delete iMde;
        iMde = NULL;
        }
    TRACE_FUNC_EXIT;
    }

/**
 * Called to notify the observer about errors, which are not a direct 
 * consequence of the operations initiated by the client but caused by 
 * some external source (e.g., other clients). The error cannot be 
 * recovered and all on-going operations initiated by the client have been 
 * aborted. Any attempts to continue using the session will cause a panic. 
 * The client should close the session immediately and try to open a new 
 * session, if it needs to continue using the metadata engine.
 *
 * @param aSession session
 * @param aError one of the system-wide error codes
 */
void CMdEManager::HandleSessionError(CMdESession& /*aSession*/, TInt aError)
    {
    // Something went wrong. Handle the error and delete the old session.
    LOGGER_WRITE_1("CMdEManager::HandleSessionError - aError: %d", aError)
    iState = EUninitialized;
    delete iMde;
    iMde = NULL;
    }

/**
 * Called to notify the observer that new results have been received 
 * in the query.
 *
 * @param aQuery              Query instance that received new results.
 * @param aFirstNewItemIndex  Index of the first new item that was added
 *                            to the result item array.
 * @param aNewItemCount       Number of items added to the result item 
 *                            array.
 */
void CMdEManager::HandleQueryNewResults(CMdEQuery& /*aQuery*/,
                                   TInt /*aFirstNewItemIndex*/,
                                   TInt /*aNewItemCount*/)
    {
    }

void CMdEManager::ProcessNextAlbumL()
    {
    TRACE_FUNC_ENTRY;
    if ( iAlbumsInProgress.Count() == 0 )
        {
        LOGGER_WRITE("All ready");
        // all ready
        return;
        }
    
    FindItemsOnAlbumL( iAlbumsInProgress[0]->Id() );
    
    TRACE_FUNC_EXIT;
    }

        
/**
 * Called to notify the observer that the query has been completed,
 * or that an error has occured.
 *
 * @param aQuery  Query instance.
 * @param aError  <code>KErrNone</code>, if the query was completed
 *                successfully. Otherwise one of the system-wide error 
 *                codes.
 */
void CMdEManager::HandleQueryCompleted(CMdEQuery& aQuery, TInt aError)
    {
    TRACE_FUNC_ENTRY;
    TInt err( aError );
    
    if ( iState == EEnumeratingAlbums )
        {
        CMdEObjectQuery& query = (CMdEObjectQuery&)aQuery;
        if ( !err && &aQuery == iAlbumQuery )
            {
            LOGGER_WRITE("Album query");
            TRAP( err, HandleAlbumQueryCompletedL( query ));
            }
        else if ( !err && &aQuery == iObjectQuery )
            {
            LOGGER_WRITE("Object query");
            TRAP( err, HandleObjectQueryCompletedL( query ));
            }
        else
            {
            LOGGER_WRITE("unknown query or error happened");
            }
        
        if ( err )
            {
            LOGGER_WRITE_1("Error happened: %d", err);
            iState = EIdle;
            iAlbumsInProgress.ResetAndDestroy();
            iAlbums.ResetAndDestroy();
            iObserver.AlbumsReaded( err );
            }
        }
    else if ( iState == EReplacingAlbum )
        {
        if ( !err && &aQuery == iContainmentQuery )
            {
            LOGGER_WRITE("relation query");
            CMdERelationQuery& query = (CMdERelationQuery&)aQuery;
            TRAP( err, HandleRelationQueryCompletedL( query ));
            }
        
        iState = EIdle;
        iObserver.AlbumReplaced( err );
        
        }
    TRACE_FUNC_EXIT;
    }


// -----------------------------------------------------------------------------
// CMdEManager::HandleAlbumQueryCompletedL
// Callback function for find all albums. Save albums to iAlbumsInProgress array 
// and start collecting objects on each album.
// -----------------------------------------------------------------------------
void CMdEManager::HandleAlbumQueryCompletedL( CMdEObjectQuery& aQuery )
    {
    TRACE_FUNC_ENTRY;
    // clear old items
    iAlbumsInProgress.ResetAndDestroy();
    iAlbums.ResetAndDestroy();
    
    for ( TInt i = 0; i < aQuery.Count(); i++ )
        {
        CPlaylistItem* albumItem = CPlaylistItem::NewLC();
        CMdEObject& object = aQuery.Result(i);
        LOGGER_WRITE_1("Id: %d", object.Id());
        albumItem->SetId( object.Id() );
        
        CMdEPropertyDef& titlePropDef = object.Def().GetPropertyDefL( MdeConstants::Object::KTitleProperty );
              
        CMdEProperty* titleProp = NULL;
        TInt index = object.Property( titlePropDef, titleProp );
        if ( index != KErrNotFound )
            {
            const TDesC& title = titleProp->TextValueL();
            LOGGER_WRITE_1("Title: %S", &title);
            albumItem->SetTitleL( title );
            }
        else
            {
            // not found, might be default album
            albumItem->SetTitleL( object.Uri() );
            }
        
        LOGGER_WRITE("");
        iAlbumsInProgress.AppendL( albumItem );
        CleanupStack::Pop( albumItem );
        }
    
    if ( aQuery.Count() > 0 )
        {
        // Start processing albums
        ProcessNextAlbumL();
        }
    else
        {
        // no albums
        iObserver.AlbumsReaded( KErrNone );
        }
    TRACE_FUNC_EXIT;
    }

void CMdEManager::HandleObjectQueryCompletedL( CMdEObjectQuery& aQuery )
    {
    if ( !iAlbumsInProgress.Count() )
        {
        LOGGER_WRITE("Error! No items on iAlbumsInProgress");
        User::Leave( KErrGeneral );
        }
    for ( TInt i = 0; i < aQuery.Count(); i++ )
        {
        CMdEObject& object = aQuery.Result(i);
        iAlbumsInProgress[0]->AddItemL( object.Uri() );
        }
    iAlbums.AppendL( iAlbumsInProgress[0] );
    iAlbumsInProgress.Remove( 0 );
    if ( iAlbumsInProgress.Count() > 0)
        {
        ProcessNextAlbumL();
        }
    else
        {
        // all albums processed
        iState = EIdle;
        iObserver.AlbumsReaded( KErrNone );
        }
    }

void CMdEManager::HandleRelationQueryCompletedL( CMdERelationQuery& aQuery )
    {
    TRACE_FUNC_ENTRY;
    const RArray<TItemId>& res = aQuery.ResultIds();
    LOGGER_WRITE_1("founded relations count: %d", res.Count())
    for (TInt i=0; i<res.Count(); i++ )
        {
        LOGGER_WRITE_1(" %d", res[i]);
        }
    LOGGER_WRITE_1("q count: %d", aQuery.Count() );
    RArray<TItemId> removed;
    CMdENamespaceDef& defaultNameSpace = iMde->GetDefaultNamespaceDefL();
    iMde->RemoveRelationsL( aQuery.ResultIds(),removed, &defaultNameSpace);
    LOGGER_WRITE_1("Removed relations count: %d", removed.Count())
    for (TInt i=0; i<removed.Count(); i++)
        {
        LOGGER_WRITE_1(" %d", removed[i]);
        }
    TRACE_FUNC_EXIT;
    }

void CMdEManager::Cancel()
    {
    TRACE_FUNC_ENTRY;
    if ( iState == EWaitingToEnumerateAlbums )
        {
        // Cancel enumeration before it even begins
        iState = EUninitialized;
        iObserver.AlbumsReaded( KErrCancel );
        }
    
    if ( iAlbumQuery )
        {
        LOGGER_WRITE("iAlbumQuery->Cancel()");
        iAlbumQuery->Cancel();
        }
    if ( iObjectQuery )
        {
        LOGGER_WRITE("iObjectQuery->Cancel()");
        iObjectQuery->Cancel();
        }
    if ( iContainmentQuery )
        {
        LOGGER_WRITE("iContainmentQuery->Cancel()");
        iContainmentQuery->Cancel();
        }
    TRACE_FUNC_EXIT;
    }
