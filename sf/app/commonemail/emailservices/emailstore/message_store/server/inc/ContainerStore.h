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
* Description:  Container store.
*
*/



#ifndef __CONTAINER_STORE_H__
#define __CONTAINER_STORE_H__

// ========
// INCLUDES
// ========

#include <e32base.h>
#include <f32file.h>
#include <e32cmn.h>
//<cmail>
#include "DebugLogMacros.h"
//</cmail>
#include "MessageStoreClientServer.h"
#include "EncryptableTable.h"

// ====================
// FORWARD DECLARATIONS
// ====================

class TDbBookmark;
class MCustomBehaviorProvider;
class MBackgroundOperationsObserver;
class MClient;
class CDeleteHandler; 
class CHandler;

// classes for internal use only
class CContainerStoreContainersTable;
class CContainerStoreContentManager;
class CContainerStoreEncryption;
class CContainerStoreGeneralTable;
class CContainerStoreUtils;
class CContainerStoreAccountTable;
class CContainerStoreSortingTable;
class CContainerStoreSearchResultTable;
class TMsgStoreSortableFields;
class CMsgStoreSortResultRowSet;
class CContainerStoreSearchHandler;
class MContainerStoreSearchClient;
class CMessageStoreEncryptHandler;
class CContainerStoreMruAddressTable;
class CMruAddress;

// =====
// TYPES
// =====

/** This type defines the base type for container IDs. */
typedef TUint TContainerId;

// =========
// CONSTANTS
// =========

/** This value defines the ID used throughout the container store to represent a nonexistent container. */
const TContainerId KContainerInvalidId = 0;

/** The type is encoded in the first 4 bits of the ID. */
const TContainerId KContainerTypeMask  = 0xF0000000;

/** This value means "any type" for operations that take type as a parameter. */
const TContainerId KContainerTypeAny   = 0xF0000000;

// ==================
// CLASS DECLARATIONS
// ==================

struct TSortSession
    {
    TContainerId                iSessionId;
    CMsgStoreSortResultRowSet*  iResultRowSet;
    };
    
class RMsgStoreSortableFields
    {
    public:
    
        void Close();
        
        TInt64   iReceivedDate;
        TUint    iFlags;
        TUint    iSizeOnServer;
        TUint    iSizeOnClient;
        RBuf     iSubject;
        RBuf     iFrom;
        RBuf     iTo;
    };
    
/** Defines the API for sorting observer.
*/
class MSortingTableObserver
    {
    public:
        virtual void MessageUpdate ( TContainerId       aMessageId, 
                                     TContainerId       aFolderId, 
                                     TMsgStoreOperation aOperation,               //EMsgStoreAdd, EMsgStoreDelete, EMsgStoreUpdateProperties
                                     TUint              aFieldsChanged = 0,       //For EMsgStoreUpdateProperties only, combinations of TSortableFieldsMasks
                                     const TDesC&       aFrom          = KNullDesC, 
                                     const TDesC&       aTo            = KNullDesC, 
                                     const TDesC&       aSubject       = KNullDesC,
                                     TInt64             aReceivedDate  = 0
                                   ) = 0;
        
        virtual void FolderDeleted ( TContainerId aFolderId ) = 0;
        virtual void MailBoxDeleted ( TContainerId aMailBoxId ) = 0;
    };

/** Defines the API that must be implemented in order for clients to receive notification of modifications
    of the contents of the container store.
*/
class MContainerStoreObserver
	{
	public:
	
	    // ==============
	    // PUBLIC METHODS
	    // ==============
	
	    /** Defines the types of operations that can be notified. */
		enum TOperation
			{
			EAdd,
			EMove,
			EDelete,
			EUpdateProperties,
			EUpdateContent,
			ERemoveContent,
			ERenameAccount
			}; // end TOperation
			

        /* This function notifies the observer of an add/change/delete/move of a message store object.
			
			\param aOtherId: this parameter will be the new parent ID for move operations,
			                 the container's parent ID for adds and updates,
			                 and KMsgStoreInvalidId otherwise			
		*/		
		virtual void ContainerModified( TOperation                  aOperation, 
		                                const RArray<TContainerId>& aHierarchy,
		                                TContainerId                aOtherId, 
		                                const TDesC8&               aQuickProperties ) = 0;
		                                
        virtual void AuthenticationChanged( TBool aAuthenticated ) = 0;
        
        virtual void AccountModified( TOperation aOperation, 
        							  TInt32 aOwnerId, 
        							  const TDesC8& aName, 
        							  const TDesC8& aNewAccName,
                                      TContainerId  aMailboxId ) = 0;
	
	}; // end class MContainerStoreObserver
    

/** The main class used to access and manipulate the contents of the container store. */	
class CContainerStore : public CBase, public MSortingTableObserver, public MEncryptableTable
	{
	public:
	
		static CContainerStore* NewL( const TDesC&                   aDbFilename,
									  TDriveNumber                   aDriveNumber,
									  MCustomBehaviorProvider&       aCustomBehaviorProvider,
									  MBackgroundOperationsObserver& aBackgroundOperationsObserver,
									  TInt                           aBasePriority );
		
		virtual ~CContainerStore();

        static TInt WipeEverything( const TDesC& aDbFilename,
								    TDriveNumber aDriveNumber );

		void SuspendCompactionLC();
		
		void ResumeCompaction();

		// AUTHENTICATION FUNCTIONS
		
		TBool Authenticated();
			
		TBool AuthenticateL( const TDesC& aPassword );
		
		TBool HasPasswordL();
		
		void ClearAuthentication();
		
		void SetPasswordL( const TDesC& aPassword );
	
		TBool ChangePasswordL( const TDesC& aOldPassword, const TDesC& aNewPassword );
        
        void EnableEncryptionL();
        
        void DisableEncryptionL();
	
		// OBSERVER FUNCTIONS
		
		void ObserveL( MContainerStoreObserver* aObserver );
		
		void StopObserving( MContainerStoreObserver* aObserver );
		
		//Account management
		TContainerId CreateAccountL( TInt32 aOwnerId, const TDesC& aName, MContainerStoreObserver* aDoNotNotify, const TDesC8& aProperties );
		
		TContainerId OpenAccountL( TInt32 aOwnerId, const TDesC& aName, RBuf8& aProperties );
		
		void RenameAccountL( TInt32 aOwnerId, const TDesC& aOldName, const TDesC& aNewName, MContainerStoreObserver* aDoNotNotify );
		
		void DeleteAccountL( TInt32 aOwnerId, const TDesC& aName, MContainerStoreObserver* aDoNotNotify );
		
		void ListAccountsL( TDes8& aBuf );
		
		// CONTAINER FUNCTIONS
		//
		// If an observer is passed in then that observer will not receive notification of the given
		// operation.
		
		void CreatePredefinedContainerIfNeededL( TContainerId  aId,
												 TContainerId  aType,
												 TContainerId  aParentId, 
												 const TDesC8& aProperties ); 

        // This function doesn't commit the container. CommitContainerL (or AbandonContainerL) must
        // subsequently called.  If neither commit or abandon is called, then the container will be
        // abandoned when the container store class is next constructed.
		TContainerId CreateContainerL( TContainerId  aType,
									   TContainerId  aParentId, 
									   TContainerId  aGrandparentId, 
									   const TDesC8& aProperties,
									   TContainerId  aId = KMsgStoreInvalidId ); 

        void CommitContainerL( TContainerId             aId,
                               TContainerId             aParentId,
                               TContainerId 			aMailBoxId,
                               MContainerStoreObserver* aObserver,
                               TContainerId             aCopiedFromOriginalMsgId = KContainerInvalidId );  //used by Copying messages only
        
        // Abandons an uncommitted container.
        void AbandonContainerL( TContainerId aId );

		void MoveContainerL( TContainerId             aId,
							 TContainerId             aOldParentId,
							 TContainerId             aNewParentId, 
							 MContainerStoreObserver* aObserver );
							 
        void DeleteContainerL( TContainerId             aId,
			                   TContainerId             aParentId,						
						       TContainerId             aGrandparentId, 
						       TContainerId             aMailBoxId,
		                       MContainerStoreObserver* aObserver ); 

		// Recursively copies the container.  CommitContainerL need not be called for the new container.
		TContainerId CopyContainerL( TContainerId             aId, 
				 			 		 TContainerId             aSourceId, 
				 			 		 TContainerId             aSourceParentId, 
				 			 		 TContainerId             aDestinationId, 
				 			 		 TContainerId             aDestinationParentId, 
				 			 		 TContainerId             aMailBoxId, 
							 		 MContainerStoreObserver* aObserver );
							 
		void ChildrenCountsL( TContainerId aId, TDes8& aCounts ); 
	
		void TotalCountsL( TContainerId aMailBoxId, TDes8& aCounts ); 
	
		void ListChildrenL( RArray<TContainerId>& aChildren,  // children of aId
                            TContainerId          aId, 
		                    TContainerId          aParentId = KContainerInvalidId,
   		                    TContainerId          aType = KContainerTypeAny,
   		                    TBool                 aRecursive = EFalse );
   		                    
		TContainerId FirstChildL( TContainerId aId );
        
        TContainerId FirstChildL( TContainerId aId, TBool& aIsChildEncrypted );
		
		// PROPERTIES FUNCTIONS
		
		void FetchPropertiesL( TContainerId  aId,
							   TContainerId& aParentId,
							   TContainerId  aGrandparentId,
				 			   RBuf8&        aProperties,
				 			   TContainerId  aMailboxId = KContainerInvalidId );
				 			   
        // This function is quicker than the FetchPropertiesL because it doesn't check the
        // hierarchy of the container.
		void FastFetchPropertiesL( TContainerId  aId,
		                           TBool         aQuickProperties,
		                           TContainerId& aParentId,
				 		           RBuf8&        aProperties );
				 		           
        TBool PropertyValueL( TContainerId  aId,
                              const TDesC8& aName,
                              RBuf8&        aValue );				 			   

		void UpdatePropertiesL( TContainerId             aId,
								TContainerId             aParentId, 
								TContainerId             aMailBoxId,
							    const TDesC8&            aProperties,                 
		                        MContainerStoreObserver* aObserver );
		
		void UpdatePropertyL( TContainerId             aId,
							  TContainerId             aParentId, 
							  TContainerId             aMailBoxId,
							  const TDesC8&            aName,
							  TUint8                   aType,
							  const TDesC8&            aValue,
		                      MContainerStoreObserver* aObserver );
		                      
		// CONTENT FUNCTIONS
		
		TUint ContentLengthL( TContainerId aId,
		                      TContainerId aParentId );
		
		void ReplaceContentL( TContainerId             aId, 
							  TContainerId             aParentId, 
							  const TDesC8&            aContent,
		                      MContainerStoreObserver* aObserver );

		void ReplaceContentL( TContainerId             aId, 
							  TContainerId             aParentId, 
							  RFile&                   aContentFile,
		                      MContainerStoreObserver* aObserver );

		void AppendContentL( TContainerId             aId, 
							 TContainerId             aParentId, 
							 const TDesC8&            aContent,
		                     MContainerStoreObserver* aObserver );

        void PrependContentL( TContainerId             aId, 
                             TContainerId             aParentId, 
                             const TDesC8&            aContent,
                             MContainerStoreObserver* aObserver );

        void RemoveContentL( TContainerId             aId,
		                     TContainerId             aParentId, 
							 MContainerStoreObserver* aObserver );

		// will truncate the content if the content buffer is too small							
		void FetchContentL( TContainerId aId,
		                    TContainerId aParentId,					
						    TDes8&       aContent,
						    TUint        aStartPosition = 0 );
						    
		void FetchContentL( TContainerId aId,
                            TContainerId aParentId,									
							RFile&       aDestinationFile );
        
        void OpenContentFileL( TContainerId aId,
                               TContainerId aParentId,         
                               RFs&         aFs,
                               RFile&       aDestinationFile );
							
		// SEARCH FUNCTIONS
		// Currently only the content is searched.
		
        CContainerStoreSearchHandler* SearchL( TContainerId                 aType,  
                                               TMsgStoreSearchCmdParams&    aCmdParam,
                                               RPointerArray<HBufC>&        aSearchStrings, 
                                               RArray<TContainerId>&        aFolderIds,
                                               RPointerArray<HBufC8>&       aPropertyNames,
                                               MContainerStoreSearchClient& aSearchClient );
        
        // SORTING FUNCTIONS
        TContainerId StartSortingL( TMsgStoreSortCriteria& aSortCriteria, 
                                    RPointerArray<HBufC8>& aPropertyNames,
                                    TBool                  aInMemorySort );
        
        void EndSortingL( TContainerId aSessionId );
        
        TBool GetSortedRowsL( TMsgStoreGetSortedRowsCmdParams aParams, RBuf8& aPropertiesBuf, const TDesC& aStartWith = KNullDesC );
        
        TInt IteratorGroupCountL( TContainerId aSessionId, RArray<TUint>& aItemsInGroup );
        
        void SortedIdsAndFlagsL( TContainerId aSessionId, RArray<TContainerId>& aIdArray, RArray<TUint32>& aFlagArray );

        TInt SortedIndexOfL( TContainerId aSessionId, TContainerId aMessageId );
        
        void SortedIdsL( TContainerId aSessionId, RArray<TContainerId>& aIdArray );
        
        void SortedIdsAndGroupCountL( TContainerId aSessionId, RArray<TContainerId>& aIdArray, RArray<TUint>& aItemsInGroup );
        
        //MRU address related methods
        void SetMaxMruCountL( TInt aCount );
        
        void SetMruAddressListL( TContainerId aMailboxId, RPointerArray<CMruAddress>& aMruAddressArray );
        
        const RPointerArray<CMruAddress>& MruAddressListL( TContainerId aMailboxId );

        //Methods inherited from MEncryptableTable
        TBool EncryptFirstL( TDbBookmark& aNextRow );
        TBool EncryptNextL( TDbBookmark& aNextRow );
        
        TBool DecryptFirstL( TDbBookmark& aNextRow );
        TBool DecryptNextL( TDbBookmark& aNextRow );
        
        /**
         * Allocates a block of container ids in a single transaction. Once
         * allocated these ids cannot be "freed".
         */
        void AllocateIdsBlockL(
            RArray<TContainerId>& aIds,
            TInt aBlockSize = 4 );
        

        CContainerStoreUtils& StoreUtils();
        
        CContainerStoreContentManager& ContentManager();
        
        TBool IsEncryptionEnabled();
        
        void BeginDatabaseTransactionLC();
        
        void CommitDatabaseTransactionL();
        
        const TDesC& PrivatePath();
        TBool DeleteFromSortingTable();
        
#ifdef _DEBUG                      
        
        void SimulateLowDiskSpace( TInt aLatency );                                 
        TInt GetEncryptionStateL();
        
#endif        
									
	private:

	    // ===============
	    // PRIVATE METHODS
	    // ===============
	    
		CContainerStore( MCustomBehaviorProvider& aCustomBehaviorProvider, 
						 TInt                     aBasePriority );
		
		void ConstructL( const TDesC&                   aDbFilename,
						 MBackgroundOperationsObserver& aBackgroundOperationsObserver,
                         TDriveNumber                   aDriveNumber );

        void CreateTablesL();		
		void OpenTablesL();	
		void CloseTables();
		
		void MarkUncommittedContainersForDeleteL();
	
		void NotifyObservers( MContainerStoreObserver::TOperation aOperation,
							  const RArray<TContainerId>&         aHierarchy,
							  const TDesC8&                       aQuickProperties,
							  MContainerStoreObserver* 			  aDoNotNotify,
  							  TContainerId                        aOtherId = KContainerInvalidId );

		void NotifyObservers( TBool aAuthenticated );
		
		void NotifyObservers( MContainerStoreObserver::TOperation aOperation,
							  TInt32 aOwnerId,
							  const TDesC8& aName,
							  const TDesC8& aNewName,
                              TContainerId  aMailboxId,
							  MContainerStoreObserver* aDoNotNotify);

		void ValidateParentL( TContainerId                aParentId, 
		                      const RArray<TContainerId>& aHierarchy, 
		                      TBool                       aStrictComparison = ETrue );
		                      
		void ValidateParentAndGrandparentL( TContainerId                aParentId, 
                                            TContainerId                aGrandparentId, 		                                    		    
		                                    const RArray<TContainerId>& aHierarchy,
		                                    TBool aStrictComparison = ETrue  );		
		                                    
		TContainerId RecursiveCopyL( TUint         aDepth,
                                     TContainerId  aId, 
				 			         TContainerId  aDestinationId );
							 

        void DoAbandonContainerL( TContainerId aId );		                                        

        // DELETE HANDLER SUPPORT FUNCTIONS
		
        friend class CDeleteHandler; 

		void FirstChildL( TContainerId  aId, 
		                  TContainerId& aChildId, 
		                  TDbBookmark&  aBookmark );
		
        TBool DeleteIndividualContainerL( TDbBookmark aBookmark );
        
        void GetSortableFieldsL( TContainerId aMessageId, RMsgStoreSortableFields& aSortableFields );
        
        void CreateSystemFoldersL( TContainerId aMailboxId );
        
        // =====================================
        // Inherited from MSortingTableObserver
        // =====================================
        void MessageUpdate ( TContainerId       aMessageId, 
                             TContainerId       aFolderId, 
                             TMsgStoreOperation aOperation,               //EMsgStoreAdd, EMsgStoreDelete, EMsgStoreUpdateProperties
                             TUint              aFieldsChanged = 0,       //For EMsgStoreUpdateProperties only, combinations of TSortableFieldsMasks
                             const TDesC&       aFromVal       = KNullDesC, 
                             const TDesC&       aToVal         = KNullDesC, 
                             const TDesC&       aSubjectVal    = KNullDesC,
                             TInt64             aReceivedDate  = 0 );

        void FolderDeleted ( TContainerId aFolderId );
        
        void MailBoxDeleted ( TContainerId aMailBoxId );
        
        TUint FindSortingSessionL( TContainerId aSessionId );
        
	    // ==================
	    // PRIVATE ATTRIBUTES
	    // ==================
	    		
		MCustomBehaviorProvider&               iCustomBehaviorProvider;
  	    const TInt                             iBasePriority;
		TUint                                  iCountsLength;
		CContainerStoreUtils*                  iUtils;
		CContainerStoreEncryption*             iEncryption;
		CContainerStoreGeneralTable*           iGeneralTable;
		CContainerStoreAccountTable*           iAccountTable;
		CContainerStoreContainersTable*        iContainersTable;
		CContainerStoreSortingTable*           iSortingTable;
        CContainerStoreSearchResultTable*      iSearchResultTable;
        CContainerStoreMruAddressTable*        iMruAddressTable;
		CContainerStoreContentManager*         iContentManager;
        CMessageStoreEncryptHandler*           iEncryptHandler;
		RPointerArray<MContainerStoreObserver> iObservers;
		CDeleteHandler*                        iDeleteHandler;
		RBuf8                                  iTotalCounts;
		
		// These variables are declared here because these types of data are very commonly used,
		// and by declaring them here once we can avoid quite a few heap allocations/deallocations.
		RArray<TContainerId>                   iHierarchy;
		RBuf8                                  iQuickProperties;
		RBuf8                                  iTotalCountsDelta;
		
        TContainerId                           iNextSortSessionId;   //running ID for sort sessions
        RArray<TSortSession>                   iSortSessions;
        
		__LOG_DECLARATION
		
	}; // end class CContainerStore

/** The interface to be implemented by container store clients, to provide domain-specific behaviors. */	
class MCustomBehaviorProvider
	{
	public:
	
        // ==============
        // PUBLIC METHODS
        // ==============
        
		virtual void QuickPropertiesAndCountsL( TContainerId  aType,
											    const TDesC8& aProperties,
												RBuf8&        aQuickProperties,
												TDes8&        aCounts ) const = 0;

        virtual void SortableFieldsL( const TDesC8& aQuickProperties, const TDesC8& aProperties, RMsgStoreSortableFields& aSortableFields ) const = 0;
        
        virtual void LengthsL( TUint& aCountsLength,
                               TUint& aQuickPropertiesMaxLength ) const = 0;
										
		virtual void InitializeCounts( TDes8& aCounts ) const = 0;
																									  
		virtual void IncrementParentCounts( TDes8& aParentCounts, const TDesC8& aChildCounts ) const = 0;
	
		virtual void DecrementParentCounts( TDes8& aParentCounts, const TDesC8& aChildCounts ) const = 0;
        
        virtual void LogCounts( const TDesC& aDescription, const TDesC8& aCounts ) const = 0;
		
	}; // end MCustomBehaviorProvider

/** The interface to be implemented observer's that need to be notified of background operations. */	
class MBackgroundOperationsObserver
    {
    public:
    
        // ==============
        // PUBLIC METHODS
        // ==============
        
		virtual void BackgroundOperationsInProgress() = 0;
		
		virtual void BackgroundOperationsCompleted() = 0;
		
    }; // end class MBackgroundOperationsObserver
	
#endif // __CONTAINER_STORE_H__

