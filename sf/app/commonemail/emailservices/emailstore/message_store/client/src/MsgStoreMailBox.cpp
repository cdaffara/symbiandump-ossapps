/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Message store mailbox client implementation.
*
*/



//<cmail>
#include "MsgStoreMailBox.h"
#include "DebugLogMacros.h"
#include "RMessageStoreSession.h"
#include "MsgStoreSessionContext.h"
#include "MsgStoreFolder.h"
#include "MsgStoreMessage.h"
#include "MsgStorePropertyKeys.h"
#include "MsgStoreObserverHandler.h"
#include "MsgStorePropertyContainersArray.h"
#include "MsgStoreSortCriteria.h"
#include "MsgStoreSortResultIterator.h"
#include "MsgStoreSearchHandler.h"
#include "MsgStoreSearchCriteria.h"
//#include "IsMsgStorePropertyKeys.h"
//</cmail>

// ====================
// Internal CLASS: TFoldersArray
// ====================
class TFoldersArray : public MPropertiesArray
    {
    public:    

        // ==============
        // PUBLIC METHODS
        // ==============
        
        TFoldersArray( CMsgStoreSessionContext& aContext, TMsgStoreId aMailBoxId, RPointerArray<CMsgStoreFolder>& aFolders );
               
        // inherited from MPropertiesArray              
        virtual void AddElementL( TMsgStoreId aId, TMsgStoreId aParentId, const TDesC8& aProperties );
        virtual void Reset();        
    
    private:

        // ==================
        // PRIVATE ATTRIBUTES
        // ==================
    
        CMsgStoreSessionContext&        iContext;
        RPointerArray<CMsgStoreFolder>& iFolders;
        const TMsgStoreId 				iMailBoxId;
        
    }; // end class TFoldersArray
    
// -------------	
// TFoldersArray
// -------------	

// ==========================================================================
// FUNCTION: Constructor
// ==========================================================================
TFoldersArray::TFoldersArray( CMsgStoreSessionContext& aContext, TMsgStoreId aMailBoxId, RPointerArray<CMsgStoreFolder>& aFolders ) : 
iContext( aContext ), iFolders( aFolders ), iMailBoxId(aMailBoxId)
    {
    } // end 
       
// ==========================================================================
// FUNCTION: AddElementL 
// ==========================================================================
void TFoldersArray::AddElementL( TMsgStoreId aId, TMsgStoreId aParentId, const TDesC8& aProperties )
    {
	CMsgStoreFolder *newFolder = CMsgStoreFolder::NewL( iContext, aId, aParentId, iMailBoxId, aProperties );
	CleanupStack::PushL( newFolder );
	
	iFolders.AppendL( newFolder );    
	
	CleanupStack::Pop( newFolder );
    } // end AddElementL
    
// ==========================================================================
// FUNCTION: Reset
// ==========================================================================
void TFoldersArray::Reset()
    {
	iFolders.ResetAndDestroy();
    } // end Reset

// ==========================================================================
// FUNCTION: AddObserverL
// ==========================================================================
EXPORT_C void CMsgStoreMailBox::AddObserverL( MMsgStoreMailBoxObserver* aObserver )
    {
	if( !iContext.iObserverHandler )
	    {	 
    	iContext.iObserverHandler = CMsgStoreObserverHandler::NewL( iContext.iSession );
	    } // end if
		    
	iContext.iObserverHandler->AddObserverL( aObserver, Id() );
    }
		
// ==========================================================================
// FUNCTION: RemoveObserverL
// ==========================================================================
EXPORT_C void CMsgStoreMailBox::RemoveObserverL( MMsgStoreMailBoxObserver* aObserver )
    {
	if( !iContext.iObserverHandler )
	    {
    	User::Leave( KErrNotFound );
	    }
	else
	    {	    
    	iContext.iObserverHandler->RemoveObserverL( aObserver, Id() );
	    } // end if
    }

		
// ==========================================================================
// FUNCTION: FetchPropertiesL
// ==========================================================================
EXPORT_C void CMsgStoreMailBox::FetchPropertiesL( const RArray<TMsgStoreId>&                 aIds, 
                                                  const RPointerArray<TDesC8>&               aPropertyNames, 
                                                  RPointerArray<CMsgStorePropertyContainer>& aProperties )
    {
    if( aIds.Count() == 0 || aPropertyNames.Count() == 0 )
        {
    	__LOG_STATIC_ENTER( "MsgClient", "FetchPropertiesL" )
    	__LOG_WRITE_ERROR( "empty input array" )
   		__LOG_STATIC_EXIT
   		User::Leave( KErrArgument );
        } // end if
    
	TMsgStorePropertyContainersArray containersArray( aProperties );
	
    iContext.iSession.PropertiesL( aIds, aPropertyNames, containersArray );
    } // end FetchPropertiesL

// ==========================================================================
// FUNCTION: FetchPropertiesL
// ==========================================================================
EXPORT_C CMsgStoreWritablePropertyContainer* CMsgStoreMailBox::FetchPropertiesL( TMsgStoreId aId )
    {
    TMsgStoreId parentId = KMsgStoreInvalidId;
    
    RBuf8 propertiesBuf;
    CleanupClosePushL( propertiesBuf );
    
    iContext.iSession.ContainerPropertiesL( aId, parentId, KMsgStoreInvalidId, propertiesBuf );
    
    CMsgStoreWritablePropertyContainer* properties = CMsgStoreWritablePropertyContainer::NewL( iContext, aId, parentId, iId, propertiesBuf );
    
    CleanupStack::PopAndDestroy( &propertiesBuf );

    return properties;
    
    }
		
// ----------------
// FOLDER FUNCTIONS
// ----------------

// ==========================================================================
// FUNCTION: CreateFolderL
// ==========================================================================
EXPORT_C TMsgStoreId CMsgStoreMailBox::CreateFolderL( TMsgStoreId                 aParentId, 
													  CMsgStorePropertyContainer& aProperties,
													  TBool                       aIsLocal)
    {
	//the parentId needs to be either a MailBox or a Folder
	iContext.VerifyTypeL( aParentId, EMsgStoreMailBoxBits, EMsgStoreFolderBits );

	aProperties.AddOrUpdatePropertyL( KMsgStorePropertyLocal, (TBool)aIsLocal );
	
	RBuf8 serializedProperties;
	CleanupClosePushL( serializedProperties );
	aProperties.SerializeL( serializedProperties );
	
	TInt id = iContext.iSession.CreateContainerL( aParentId, 
												  KMsgStoreInvalidId, // do not check grandparent ID
												  iId,                // mail box id
												  EMsgStoreFolderBits,
												  serializedProperties );
	
	CleanupStack::PopAndDestroy( &serializedProperties );
	
	return id;
    }


// ==========================================================================
// FUNCTION: DeleteFolderL
// ==========================================================================
EXPORT_C void CMsgStoreMailBox::DeleteFolderL( TMsgStoreId aId )
    {
	iContext.VerifyTypeL( aId, EMsgStoreFolderBits );
	
	// Do not check location of folder.
	iContext.iSession.DeleteContainerL( aId, KMsgStoreInvalidId, KMsgStoreInvalidId, iId );
    }

		
// ==========================================================================
// FUNCTION: FoldersL
// ==========================================================================
EXPORT_C void CMsgStoreMailBox::FoldersL( RPointerArray<CMsgStoreFolder>& aFolders )
    {
	aFolders.ResetAndDestroy();
	
	TFoldersArray foldersArray( iContext, iId, aFolders );
	
	iContext.iSession.ChildrenPropertiesL( iId,                     // aId 
	                                       KMsgStoreInvalidId,     // aParentId (do not need to check parent ID)
	                                       EMsgStoreFolderBits,    // aContainerType
	                                       EFalse,                 // aQuickProperties
	                                       ETrue,                  // aRecursive
	                                       foldersArray );
	
    }

// ==========================================================================
// FUNCTION: FoldersL
// ==========================================================================
EXPORT_C void CMsgStoreMailBox::FoldersL( TMsgStoreId aFolderId, 
                                          RPointerArray<CMsgStoreFolder>& aFolders,
                                          TBool                           aRecursive )
    {
	iContext.VerifyTypeL( aFolderId, EMsgStoreMailBoxBits, EMsgStoreFolderBits );
	
	TFoldersArray foldersArray( iContext, iId, aFolders );
	
	iContext.iSession.ChildrenPropertiesL( aFolderId,           // aId
	                                       KMsgStoreInvalidId,  // aParentId (do not need to check parent ID)
	                                       EMsgStoreFolderBits, // aContainerType
	                                       EFalse,              // aQuickProperties
	                                       aRecursive,         
	                                       foldersArray );	
    } // end FoldersL
		
// ==========================================================================
// FUNCTION: FolderL
// ==========================================================================
EXPORT_C CMsgStoreFolder* CMsgStoreMailBox::FolderL( TMsgStoreId aId )
    {
	iContext.VerifyTypeL( aId, EMsgStoreFolderBits );
	
	TMsgStoreId parentId = KMsgStoreInvalidId;
	
	RBuf8 propertiesBuf;
	CleanupClosePushL( propertiesBuf );
	
	iContext.iSession.ContainerPropertiesL( aId, parentId, KMsgStoreInvalidId, propertiesBuf );
	
	CMsgStoreFolder* folder = CMsgStoreFolder::NewL( iContext, aId, parentId, iId, propertiesBuf );
	
	CleanupStack::PopAndDestroy( &propertiesBuf );

	return folder;
    }

// -----------------
// MESSAGE FUNCTIONS
// -----------------

// ==========================================================================
// FUNCTION: CreateMessageL
// ==========================================================================
EXPORT_C CMsgStoreMessage* CMsgStoreMailBox::CreateMessageL( TMsgStoreId aFolderId, const CMsgStorePropertyContainer& aProperties )
    {
	iContext.VerifyTypeL( aFolderId, EMsgStoreFolderBits );
	
	RBuf8 serializedProperties;
	CleanupClosePushL( serializedProperties );
	aProperties.SerializeL( serializedProperties );
	
	// Temporarily create the message in the scratch folder, until the commit function is called.
	TInt id = iContext.iSession.CreateContainerL( aFolderId, 
												  KMsgStoreInvalidId, // do not check grandparent ID
												  iId,                // mail box id
												  EMsgStoreMessageBits,
												  serializedProperties,
												  EFalse );  // do not commit
	
	CMsgStoreMessage* message = CMsgStoreMessage::NewL( iContext, id, KMsgStoreInvalidId, aFolderId, iId, serializedProperties );
	
	CleanupStack::PopAndDestroy( &serializedProperties );
	
	return message;
    }

// ==========================================================================
// FUNCTION: FetchMessageL
// ==========================================================================
EXPORT_C CMsgStoreMessage* CMsgStoreMailBox::FetchMessageL( TMsgStoreId aMessageId, TMsgStoreId aFolderId )
    {										  
	iContext.VerifyTypeL( aMessageId, EMsgStoreMessageBits );
	iContext.VerifyTypeL( aFolderId, EMsgStoreFolderBits );
	
	TMsgStoreId parentId = aFolderId;
	
	RBuf8 propertiesBuf;
	CleanupClosePushL( propertiesBuf );
	
	iContext.iSession.ContainerPropertiesL( aMessageId, parentId, KMsgStoreInvalidId, propertiesBuf, iId );
	
	CMsgStoreMessage* message = CMsgStoreMessage::NewL( iContext, aMessageId, parentId, parentId, iId, propertiesBuf );
	
	CleanupStack::PopAndDestroy( &propertiesBuf );
	
	return message;
    }
		
// ==========================================================================
// FUNCTION: DeleteMessageL
// ==========================================================================
EXPORT_C void CMsgStoreMailBox::DeleteMessageL( TMsgStoreId aMessageId, TMsgStoreId aFolderId )
    {
	iContext.VerifyTypeL( aMessageId, EMsgStoreMessageBits );
	iContext.VerifyTypeL( aFolderId, EMsgStoreFolderBits );

	iContext.iSession.DeleteContainerL( aMessageId, aFolderId, KMsgStoreInvalidId, iId );	
    } // end DeleteMessageL

		
// ==========================================================================
// FUNCTION: MoveMessageL
// ==========================================================================
EXPORT_C void CMsgStoreMailBox::MoveMessageL( TMsgStoreId aMessageId, 
											  TMsgStoreId aSourceFolderId, 
											  TMsgStoreId aDestinationFolderId )
    {
	iContext.VerifyTypeL( aMessageId, EMsgStoreMessageBits );
	iContext.VerifyTypeL( aSourceFolderId, EMsgStoreFolderBits );
	iContext.VerifyTypeL( aDestinationFolderId, EMsgStoreFolderBits );

	iContext.iSession.MoveContainerL( aMessageId, aSourceFolderId, aDestinationFolderId );	
    } // end MoveMessageL


EXPORT_C void CMsgStoreMailBox::MoveFolderL( TMsgStoreId aFolderId, 
											  TMsgStoreId aSourceFolderId, 
											  TMsgStoreId aDestinationFolderId )
    {
	iContext.VerifyTypeL( aFolderId, EMsgStoreFolderBits );
	iContext.VerifyTypeL( aSourceFolderId, EMsgStoreFolderBits, EMsgStoreMailBoxBits );
	iContext.VerifyTypeL( aDestinationFolderId, EMsgStoreFolderBits, EMsgStoreMailBoxBits );

	iContext.iSession.MoveContainerL( aFolderId, aSourceFolderId, aDestinationFolderId );	
    } // end MoveMessageL


// ==========================================================================
// FUNCTION: CopyMessageL
// ==========================================================================
EXPORT_C TMsgStoreId CMsgStoreMailBox::CopyMessageL( TMsgStoreId aMessageId, 
													 TMsgStoreId aSourceFolderId, 
													 TMsgStoreId aDestinationFolderId )
    {
	iContext.VerifyTypeL( aMessageId, EMsgStoreMessageBits );
	iContext.VerifyTypeL( aSourceFolderId, EMsgStoreFolderBits );
	iContext.VerifyTypeL( aDestinationFolderId, EMsgStoreFolderBits );
	
	return iContext.iSession.CopyContainerL( aMessageId, 
	                                          aSourceFolderId,
	                                          KMsgStoreInvalidId,   // don't care
	                                          aDestinationFolderId,
	                                          KMsgStoreInvalidId,   // don't care
	                                          iId);					// mailbox id
    }		
		
// ==========================================================================
// FUNCTION: MessageIdsL
// ==========================================================================
EXPORT_C void CMsgStoreMailBox::MessageIdsL( TMsgStoreId aFolderId, RArray<TMsgStoreId>& aMessageIdList )
    {
	iContext.VerifyTypeL( aFolderId, EMsgStoreFolderBits );
	
	aMessageIdList.Reset();
	iContext.iSession.ChildrenIdsL( aFolderId, EMsgStoreMessageBits, aMessageIdList );
    }
	
// ==========================================================================
// FUNCTION: MessageCountsL
// ==========================================================================
EXPORT_C void CMsgStoreMailBox::MessageCountsL( TMsgStoreId aFolderId, 
												TUint&      aTotalCount, 
												TUint&      aUnreadCount )
    {
	iContext.VerifyTypeL( aFolderId, EMsgStoreFolderBits );
	
	iContext.iSession.ChildrenCountsL( aFolderId, aTotalCount, aUnreadCount );
    } // end MessageCountsL


// ==========================================================================
// FUNCTION: TotalMessageCountsL
// ==========================================================================
EXPORT_C void CMsgStoreMailBox::TotalMessageCountsL( TUint& aTotalCount, 
													 TUint& aUnreadCount )
    {
	iContext.iSession.TotalCountsL( iId, aTotalCount, aUnreadCount );
    } // end TotalMessageCountsL

// ==========================================================================
// FUNCTION: SortL
// ==========================================================================
EXPORT_C CMsgStoreSortResultIterator* CMsgStoreMailBox::SortL( RMsgStoreSortCriteria& aSortCriteria, TBool aInMemorySort )
    {
    if( aSortCriteria.PropertyKeys().Count() == 0 )
        {
        __LOG_STATIC_ENTER( "MsgClient", "SortL" )
        __LOG_WRITE_ERROR( "empty property array" )
        __LOG_STATIC_EXIT
        User::Leave( KErrArgument );
        } // end if
    
    TMsgStoreId sortSessionId = iContext.iSession.SortL( aSortCriteria, iId, aInMemorySort );
    return CMsgStoreSortResultIterator::NewL( iContext, sortSessionId );
    }

// ==========================================================================
// FUNCTION: SearchL
// ==========================================================================
EXPORT_C void CMsgStoreMailBox::SearchL( RMsgStoreSearchCriteria&  aSearchCriteria,
                                         MMsgStoreSearchClient&    aSearchClient )
    {
    if( aSearchCriteria.SearchStrings().Count() == 0  )
        {
        __LOG_STATIC_ENTER( "MsgClient", "SearchL" )
        __LOG_WRITE_ERROR( "no search string" )
        __LOG_STATIC_EXIT
        User::Leave( KErrArgument );
        } // end if
    
    TBool isEmptyFolderIdArray = EFalse;
    RArray<TMsgStoreId>& folderIds = aSearchCriteria.FolderIds();
    if( folderIds.Count() == 0 )
        {
        //a "global" search is requested, fill in with all the folders ids
        RPointerArray<CMsgStoreFolder> folders;
        FoldersL( folders );
        for ( TInt i = 0 ; i < folders.Count() ; i++ )
            {
            aSearchCriteria.AddFolderId( folders[i]->Id() );
            }
        folders.ResetAndDestroy();
        isEmptyFolderIdArray = ETrue;
        } // end if
    else
        {
        for ( TInt i = 0 ; i < folderIds.Count() ; i++ )
            {
            iContext.VerifyTypeL( folderIds[i], EMsgStoreFolderBits );
            }
        }
    
    if( iContext.iSearchHandler )
        {
        __LOG_STATIC_ENTER( "MsgClient", "SearchL" )
        __LOG_WRITE_ERROR( "search already in progress" )
        __LOG_STATIC_EXIT
        User::Leave( KErrInUse );
        } // end if   
    
    //cleanup search handlers from previous search sessions
    iOldSearchHandlers.ResetAndDestroy();
    
    iContext.iSearchHandler = CMsgStoreSearchHandler::NewL( iContext.iSession, aSearchCriteria, *this );
    iContext.iSearchClient  = &aSearchClient;
    
    if ( isEmptyFolderIdArray )
        {
        //retrore the folder id to empty array as it was received by this method
        folderIds.Reset();
        }
    }
                           
// ==========================================================================
// FUNCTION: CancelSearchL
// ==========================================================================
EXPORT_C void CMsgStoreMailBox::CancelSearch()
    {
    
    if ( iContext.iSearchHandler )
        {
        iContext.iSearchHandler->CancelSearch();
        }
    
    delete iContext.iSearchHandler;
    iContext.iSearchHandler = NULL;
    iContext.iSearchClient  = NULL;
    }

// ==========================================================================
// FUNCTION: ClearSearchResultCache
// ==========================================================================
EXPORT_C void CMsgStoreMailBox::ClearSearchResultCache()
    {
    CancelSearch();
    iOldSearchHandlers.ResetAndDestroy();
    iContext.iSession.ClearSearchResultCache();
    }

// ==========================================================================
// FUNCTION: AddMruAddresses
// ==========================================================================
EXPORT_C void CMsgStoreMailBox::AddMruAddressesL( RPointerArray<CMsgStoreAddress>& aAddressArray )
    {
    iContext.iSession.AddMruAddressesL( iId, aAddressArray  );
    }

// ==========================================================================
// FUNCTION: MruAddresses
// ==========================================================================
EXPORT_C void CMsgStoreMailBox::MruAddressesL( RPointerArray<CMsgStoreAddress>& aAddressArray )
    {
    iContext.iSession.MruAddressesL( iId, aAddressArray  );
    }

// ==========================================================================
// FUNCTION: MruAddresses
// ==========================================================================
EXPORT_C TMsgStoreContainerType CMsgStoreMailBox::ContainerTypeById( TMsgStoreId aId )
    {
    return iContext.iSession.ContainerTypeById( aId );
    }

// ==========================================================================
// FUNCTION: IsCorporateMailbox
// Please see http://wiki.pumatech.com/display/EXPRESS/Multi+Mailbox+Support for
// how to determine if a mialbox is corporate
// ==========================================================================
EXPORT_C TBool CMsgStoreMailBox::IsCorporateMailbox()
    {
    TBool isCorporate = EFalse;
    TUint index = 0;
    if ( FindProperty( KIsPropertyEmailSourceId, index ) )
        {
        TUint32 sourceId = 0;
        TRAP_IGNORE( sourceId = PropertyValueUint32L( index ) );
        const TInt KShift = 16;
        const TInt KSourceIDNonCorporate = 8;
        const TInt KSourceIDCorporate = 12;
        
        sourceId = sourceId >> KShift;
        switch ( sourceId & 0x07ff )
            {
            case KSourceIDCorporate:
                isCorporate = ETrue;
                break;
                
            case KSourceIDNonCorporate:
                {
                index = 0;
                if ( FindProperty( KIsPropertyCapabilities, index ) )
                    {
                    TUint featureVal = 0;
                    TRAP_IGNORE( featureVal = PropertyValueUint32L( index ) );
                    if ( featureVal & 0x80000000 )
                        {
                        isCorporate = ETrue;
                        }
                    }
                }
                break;
            }
        }
    else
        {
        //no source id, let's check the "type"
        index = 0;
        if ( FindProperty( KIsPropertyServerType, index ) )
            {
            TUint serverType = 0;
            TRAP_IGNORE( serverType = PropertyValueUint32L( index ) );
            if ( serverType == ESrvExchange ||
                 serverType == ESrvDomino ||
                 serverType == ESrvGroupWise )
                {
                isCorporate = ETrue;
                }
            }
        }
            
    return isCorporate;
    }

/**
 * 
 */
/*public*/
EXPORT_C void CMsgStoreMailBox::BeginBatchInsertL()
    {
    iContext.iSession.BeginBatchInsertL();
    }

/**
 * 
 */
/*public*/
EXPORT_C void CMsgStoreMailBox::FinishBatchInsertL()
    {
    iContext.iSession.FinishBatchInsertL();
    }


// ==========================================================================
// FUNCTION: IsDefaultMailbox
// ==========================================================================
TBool CMsgStoreMailBox::IsDefaultMailbox()
    {
    TBool isDefault = EFalse;
    TUint index = 0;
    if ( FindProperty( KMsgStorePropertyIsDefault, index ) )
        {
        TRAP_IGNORE( isDefault = PropertyValueBoolL( index ) );
        }
            
    return isDefault;
    }

// ==========================================================================
// FUNCTION: SetDefaultMailboxL
// ==========================================================================
void CMsgStoreMailBox::SetDefaultMailboxL( TBool aIsDefault )
    {
    AddOrUpdatePropertyL( KMsgStorePropertyIsDefault, aIsDefault );
    StorePropertiesL();
    }

// ==========================================================================
// FUNCTION: MatchFound
// ==========================================================================
void CMsgStoreMailBox::MatchFound( CMsgStorePropertyContainer*& aMessageSummary )
    {
    iContext.iSearchClient->MatchFound( aMessageSummary );
    } // end MatchFound
    
// ==========================================================================
// FUNCTION: SearchCompleted
// ==========================================================================
void CMsgStoreMailBox::SearchCompleted()
    {
    //since we are in the the call back from the iContext.iSearchHandler
    //try not to delete it within its own call
    iOldSearchHandlers.Append( iContext.iSearchHandler );
    iContext.iSearchHandler = NULL;
    
    iContext.iSearchClient->SearchCompleted();
    iContext.iSearchClient = NULL;
    } // end SearchCompleted


// ==========================================================================
// FUNCTION: Destructor
// ==========================================================================
CMsgStoreMailBox::~CMsgStoreMailBox()
    {
    if ( iContext.iSearchHandler )
        {
        iContext.iSearchHandler->CancelSearch();
        delete iContext.iSearchHandler;
        }
    iOldSearchHandlers.ResetAndDestroy();
    }
		
// ==========================================================================
// FUNCTION: NewL
// ==========================================================================
CMsgStoreMailBox* CMsgStoreMailBox::NewL( CMsgStoreSessionContext& iContext, TMsgStoreId iId, const TDesC8& aProperties )
    {
	CMsgStoreMailBox* self = new( ELeave ) CMsgStoreMailBox( iContext, iId );
	CleanupStack::PushL( self );
	self->ConstructL( aProperties );
	CleanupStack::Pop( self );
	return self;
    }
										 
// ==========================================================================
// FUNCTION: Constructor
// ==========================================================================
CMsgStoreMailBox::CMsgStoreMailBox( CMsgStoreSessionContext& aContext, TMsgStoreId aId )
: CMsgStoreWritablePropertyContainer( aContext, aId, KMsgStoreRootMailBoxId, aId )
    {
    }
		
// ==========================================================================
// FUNCTION: ConstructL
// ==========================================================================
void CMsgStoreMailBox::ConstructL( const TDesC8& aProperties )
    {
	DeserializeL( aProperties );
    }
