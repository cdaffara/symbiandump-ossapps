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
* Description:  Message store client interface.
*
*/



#ifndef __RMESSAGE_STORE_SESSION_H__
#define __RMESSAGE_STORE_SESSION_H__

// ========
// INCLUDES
// ========

#include <e32base.h>
#include <f32file.h>
//<cmail>
#include "MsgStoreTypes.h"
#include "DebugLogMacros.h"
#include "MessageStoreClientServer.h"
#include "MsgStoreSortResultIterator.h"
//</cmail>
// ====================
// FORWARD DECLARATIONS
// ====================

class RFs;
class CMsgStoreAccount;
class CMsgStoreSortResultIterator;
class RMsgStoreSortCriteria;
class RMsgStoreSearchCriteria;
class CMsgStoreAddress;

// =========
// CONSTANTS
// =========

_LIT( KNullDes, "" );

// =========
// Templates
// =========
template <class T> class CleanupResetAndDestroyClose
    {
public:
    inline static void PushL(T& aRef);
private:
    static void Close(TAny *aPtr);
    };

template <class T> inline void CleanupResetAndDestroyClosePushL(T& aRef);

template <class T> inline void CleanupResetAndDestroyClose<T>::PushL(T& aRef)
    {
    CleanupStack::PushL(TCleanupItem(&Close, &aRef));
    }

template <class T> void CleanupResetAndDestroyClose<T>::Close(TAny *aPtr)
    {
    static_cast<T*>(aPtr)->ResetAndDestroy();
    static_cast<T*>(aPtr)->Close();
    }

template <class T> inline void CleanupResetAndDestroyClosePushL(T& aRef)
    {
    CleanupResetAndDestroyClose<T>::PushL(aRef);
    }




// =======
// CLASSES
// =======

// =======================
// CLASS: MPropertiesArray
// =======================
class MPropertiesArray
    {
    public:
    
 		// ==============
		// PUBLIC METHODS
		// ==============

       virtual void AddElementL( TMsgStoreId aId, TMsgStoreId aParentId, const TDesC8& aProperties ) = 0;
        
       virtual void Reset() = 0;
        
    }; // end class MPropertiesArray

// =======================
// CLASS: RMessageStoreSession
// =======================
class RMessageStoreSession : public RSessionBase  
	{
	public:

		// ==============
		// PUBLIC METHODS
		// ==============
		
		RMessageStoreSession();
		
		~RMessageStoreSession();

		TInt Connect( TBool aLaunchServerIfNeeded, TBool aRetryIndefinitely );

		void Close();
				
		void WipeEverythingL();
        
        void EnableEncryptionL();
        
        void DisableEncryptionL();
		
		TBool AuthenticatedL();
		
        TBool AuthenticateL( const TDesC& aPassword );
        
        void ClearAuthenticationL();

		TBool HasPasswordL();
		
        void SetPasswordL( const TDesC& aPassword );
        
        TBool ChangePasswordL( const TDesC& aOldPassword, const TDesC& aNewPassword ); 
        
        //account management
        TMsgStoreId CreateAccountL( const CMsgStoreAccount& aAccount, const TDesC8& aProperties );
        
        TMsgStoreId OpenAccountL( const CMsgStoreAccount& aAccount, RBuf8& aBuffer  );
        
        void DeleteAccountL( const CMsgStoreAccount& aAccount );
        
        void RenameAccountL( TInt32 aOwnerId, const TDesC& aOldName, const TDesC& aNewName );
        
        void AccountsL( RPointerArray<CMsgStoreAccount>& aAccounts );
        
		// pass in KMsgStoreInvalidID for grandparent ID if hierarchy checking is not desired
		TMsgStoreId CreateContainerL( TMsgStoreId             aParentId, 
									  TMsgStoreId             aGrandparentId,
									  TMsgStoreId             aMailBoxId,
									  TMsgStoreContainerMasks aType,
		                          	  const TDesC8&           aProperties,
		                          	  TBool                   aCommitContainer = ETrue,
		                          	  const TDesC&            aContentFilename = KNullDes );
		
		// pass in KMsgStoreInvalidID for grandparent ID if hierarchy checking is not desired
		TMsgStoreId CreateContainerL( TMsgStoreId             aParentId, 
									  TMsgStoreId             aGrandparentId,
									  TMsgStoreId             aMailBoxId,
									  TMsgStoreContainerMasks aType,
		                          	  const TDesC8&           aProperties,
		                          	  TBool                   aCommitContainer,
		                          	  RFile&                  aFile );
		 
		void CommitContainerL( TMsgStoreId aId, 
			  			       TMsgStoreId aParentId,
			  			       TMsgStoreId aMailBoxId);

		void AbandonContainerL( TMsgStoreId aId );
		 
		// pass in KMsgStoreInvalidID for aSourceId if hierarchy checking is not desired
		void MoveContainerL( TMsgStoreId aId, 
							 TMsgStoreId aSourceId,
			 				 TMsgStoreId aDestinationId );
		
		// pass in KMsgStoreInvalidID for aSourceId if hierarchy checking is not desired
		TMsgStoreId CopyContainerL( TMsgStoreId aId, 
									TMsgStoreId aSourceId,
									TMsgStoreId aSourceParentId,
									TMsgStoreId aDestinationId,
									TMsgStoreId aDestinationParentId,
									TMsgStoreId aMailBoxId);
		
		// pass in KMsgStoreInvalidID for parent or grandparent ID if hierarchy checking is not desired
		void DeleteContainerL( TMsgStoreId aId, 
							   TMsgStoreId aParentId,
							   TMsgStoreId aGrandparentId,
							   TMsgStoreId aMailBoxId);

		void TotalCountsL( TMsgStoreId aMailBoxId,
						   TUint&      aMessageCounts, 
		                   TUint&      aUnreadCounts );

		void ChildrenCountsL( TMsgStoreId aId,
		                      TUint&      aMessageCounts, 
		                      TUint&      aUnreadCounts );

		void ChildrenIdsL( TMsgStoreId             aId, 
		                   TMsgStoreContainerMasks aType,
		                   RArray<TMsgStoreId>&    aChildrenIds );
		
		// pass in KMsgStoreInvalidID for parent or grandparent ID if hierarchy checking is not desired
		void ContainerPropertiesL( TMsgStoreId  aId, 
		                           TMsgStoreId& aParentId,
		                           TMsgStoreId  aGrandparentId,
		                           RBuf8&       aBuffer,
		                           TMsgStoreId  aMailboxId = KMsgStoreInvalidId );
		
		// pass in KMsgStoreInvalidID for parent ID if hierarchy checking is not desired
		void UpdatePropertiesL( TMsgStoreId   aId,
								TMsgStoreId   aParentId, 
								TMsgStoreId   aMailBoxId,
		                        const TDesC8& aProperties );		
		
		// pass in KMsgStoreInvalidID for parent ID if hierarchy checking is not desired
		void UpdatePropertyL( TMsgStoreId                aId, 
							  TMsgStoreId                aParentid, 
							  TMsgStoreId                aMailBoxId,
		                      const TDesC8&              aName,
		                      TMsgStorePropertyValueType aType,
		                      const TDesC8&              aValue );		
		
		// pass in KMsgStoreInvalidID for parent ID if hierarchy checking is not desired
		TUint ContentLengthL( TMsgStoreId aId, TMsgStoreId aParentId );

		// pass in KMsgStoreInvalidID for parent ID if hierarchy checking is not desired
		void FetchContentL( TMsgStoreId aId,
		                    TMsgStoreId aParentId,
		                    TDes8&      aBuffer, 
		                    TUint       aStartOffset );
		
		// pass in KMsgStoreInvalidID for parent ID if hierarchy checking is not desired
		void FetchContentL( TMsgStoreId  aId, 
		                    TMsgStoreId aParentId,		
		                    const TDesC& aFilename );
		
		// pass in KMsgStoreInvalidID for parent ID if hierarchy checking is not desired
		void ReplaceContentL( TMsgStoreId   aId, 
							  TMsgStoreId   aParentId, 
		                      const TDesC8& aContent );	
		
		// pass in KMsgStoreInvalidID for parent ID if hierarchy checking is not desired
		void ReplaceContentWithFileL( TMsgStoreId  aId,  
							          TMsgStoreId  aParentId, 
		                              const TDesC& aFilename );	
		
		// pass in KMsgStoreInvalidID for parent ID if hierarchy checking is not desired
		void AppendToContentL( TMsgStoreId   aId, 
							   TMsgStoreId   aParentId, 
		                       const TDesC8& aContent );	
		
        // pass in KMsgStoreInvalidID for parent ID if hierarchy checking is not desired
        void PrependToContentL( TMsgStoreId   aId, 
                               TMsgStoreId   aParentId, 
                               const TDesC8& aContent );    

        // pass in KMsgStoreInvalidID for parent ID if hierarchy checking is not desired
		void RemoveContentL( TMsgStoreId aId,
 							 TMsgStoreId aParentId );
        
        // Open the original content file to be used on the client side
        void OpenOriginalContentFileL( TMsgStoreId aId,
                                       TMsgStoreId aParentId, 
                                       RFile&      aFile );
		
		// pass in KMsgStoreInvalidID for parent ID if hierarchy checking is not desired
		void ChildrenPropertiesL( TMsgStoreId             aId,
		                          TMsgStoreId             aParentId,
		                          TMsgStoreContainerMasks aType,
		                          TBool                   aQuickProperties,
		                          TBool                   aRecursive,
		                          MPropertiesArray&       aPropertiesArray );
		                          
        void PropertiesL( const RArray<TMsgStoreId>&   aIds, 
                          const RPointerArray<TDesC8>& aPropertyNames, 
                          MPropertiesArray&            aPropertiesArray );	          
        
        void BeginBatchInsertL();
        
        void FinishBatchInsertL();
		
		// OBSERVER FUNCTIONS
		
		void StartObservingL();
		
		// Returns a ID of KMsgStoreInvalidID when the search has completed.
		void GetEvents( TRequestStatus& aStatus, 
		                 TDes8&         aBuffer );
		
		void StopObserving();
        
        // SORT FUNCTIONS
        
        TMsgStoreId SortL( RMsgStoreSortCriteria& aSortCriteria, TMsgStoreId aMailBoxId, TBool aInMemorySort );
        
        TBool GetSortedRowsL( TMsgStoreId                aSortSessionId,
                              TMsgStoreId                aCurrentMessageId, 
                              TMsgStoreIteratorDirection aDirection,
                              TUint                      aCount, 
                              MPropertiesArray&          aPropertiesArray,
                              const TDesC&               aStartWith = KNullDesC,
                              TBool                      aSkipCurrentGroup = EFalse );
        
        TInt SortIteratorGroupCountL( TMsgStoreId aSortSessionId, RArray<TUint>& aItemsInGroup );
        
        void SortedIdsAndFlagsL( TMsgStoreId aSortSessionId, RArray<TMsgStoreIdAndFlag>& aIdsAndFlags );
        
        TInt SortedIndexOfL( TMsgStoreId aSortSessionId, TMsgStoreId aMessageId );
        
        void SortedIdsL( TMsgStoreId aSortSessionId, RArray<TMsgStoreId>& aMessageIds );
        
        void IdsAndGroupCountL( TMsgStoreId aSortSessionId, RArray<TMsgStoreId>& aMessageIds, RArray<TUint>& aItemsInGroup );
        
        void EndSortL( TMsgStoreId aSortSessionId );
        
		
		// SEARCH FUNCTIONS
		
		void PrepareSearchL( RMsgStoreSearchCriteria& aSearchCriteria );
		
		// returns empty buffer when search has completed.
		void GetMatchesL( TRequestStatus& aStatus, TDes8& aBuffer );
		
		void CancelSearch();
        
        void ClearSearchResultCache();
        
        void SetMaxMruAddressCountL( TInt aMaxCount );
        
        void AddMruAddressesL( TMsgStoreId aMailBoxId, RPointerArray<CMsgStoreAddress>& aAddressArray );
        
        void MruAddressesL( TMsgStoreId aMailBoxId, RPointerArray<CMsgStoreAddress>& aAddressArray );
        
        TMsgStoreContainerType ContainerTypeById( TMsgStoreId aId );
		
    private:
    
        void DoPropertiesListL( TMsgStorePropertiesListCmdParams& aParams,
                                const TDesC8&                     aSerializedIds, 
                                const TDesC8&                     aSerializedPropertyNames, 
                                MPropertiesArray&                 aPropertiesArray );
                                
        TInt StartServer();
        
        __LOG_DECLARATION                              
		        
	}; // end client RMessageStoreSession
	
#endif // __RMESSAGE_STORE_SESSION_H__
