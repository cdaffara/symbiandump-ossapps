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
* Description:  Container store containers table.
*
*/



#ifndef __CONTAINER_STORE_CONTAINERS_TABLE_H__
#define __CONTAINER_STORE_CONTAINERS_TABLE_H__

// ========
// INCLUDES
// ========

#include <e32base.h>
#include <d32dbms.h>  // database
//<cmail>
#include "DebugLogMacros.h"
//</cmail>

#include "ContainerStoreTable.h"
#include "ContainerStore.h"
#include "ContainerStoreDefs.h"

// =======
// CLASSES
// =======

// =====================================
// CLASS: CContainerStoreContainersTable
// =====================================
class CContainerStoreContainersTable : public CContainerStoreTable
	{
	public:
	
	    // ==============
	    // PUBLIC METHODS
	    // ==============
	    
		static CContainerStoreContainersTable* CreateL( CContainerStoreUtils&      aUtils,
		                                                MCustomBehaviorProvider&   aCustomBehaviorProvider, 
									  				    TUint                      aCountsLength,
									  				    CContainerStoreEncryption& aEncryption );
		
		static CContainerStoreContainersTable* OpenL( CContainerStoreUtils&      aUtils,
		                                              MCustomBehaviorProvider&   aCustomBehaviorProvider, 
									  				  TUint                      aCountsLength,
									  				  CContainerStoreEncryption& aEncryption );
		
		virtual ~CContainerStoreContainersTable();
		
		// Leaves with KErrNotFound if not found or if it is in the hierarchy of a container that is
		// to be deleted.
		void SeekL( TContainerId          aId, 
		            RArray<TContainerId>& aHierarchy );

		// Leaves with KErrNotFound if not found		
		void SeekL( TContainerId aId );
		
		// Leaves with KErrNotFound if not found		
		void SeekL( TContainerId  aId, 
		            TContainerId& aParentId );

		// Leaves with KErrNotFound if not found		
		TContainerId GotoL( TDbBookmark aBookmark );

		// Leaves with KErrNotFound if not found or if it is in the hierarchy of a container that is
		// to be deleted.  (This is different from SeekL in that GetL is not on the record.  Use this
		// version if the container whose hierarchy is being queried is not used immediately after the
		// call).
		void HierarchyL( TContainerId          aId, 
		                 RArray<TContainerId>& aHierarchy );
		
		TContainerId FindL( const TDesC& aSqlQuery );

		TDbBookmark Bookmark();
		
		TContainerId ParentId();

		void ListChildrenL( RArray<TContainerId>& aChildren,
   		                    TContainerId          aId,
		                    TContainerId          aType = KContainerTypeAny,
   		                    TBool                 aRecursive = EFalse ); 
		
		void ChildrenCountsL( TDes8& aCounts, TBool aRecursive = EFalse );

		void PropertiesL( RBuf8& aProperties );
        
        TUint PropertiesSizeL( TContainerId aId );

		void QuickPropertiesL( RBuf8& aQuickProperties );
        
		void CreateContainerL( TContainerId  aId,
	  				           TContainerId  aParentId,
	  				           const TDesC8& aProperties,
						       RBuf8&        aQuickProperties );
			
		// nonrecursive										   
		void DeleteL();
        
        /**
         * Only to be used when failed to delete a part because client has
         * the content file open, this method markes the part to be skipped in the current deletion
         * round, but the next round will change this to MarkForDeleteL(), so that it will get deleted.
         * 
         * This method does NOT update any message counts because those have already
         * been handled.
         */
        void MarkAsDeleteRetryL();
        
        /**
         * Only to be used when about to retry deleting a container.
         * This method markes the part as to be deleted so that it can be deleted in the
         * current round of deletion.
         * 
         * This method does NOT update any message counts because those have already
         * been handled.
         */
        void MarkForDeleteL();
		                            
		void MoveL( TContainerId aNewParentId, TDes8& aTotalCountsDelta ); 
		
		// nonrecursive, copies current row to the parent ID, using the given id
		void CopyL( TContainerId aNewId, TContainerId aParentId, RBuf8& aQuickProperties ); 
		
		void UpdatePropertiesL( const TDesC8& aProperties,
		                        RBuf8&        aQuickProperties,
		                        TDes8&        aTotalCountsDelta,
                                TContainerId&  aRealParentId );
				                  
		void UpdatePropertyL( const TDesC8& aName,
		                      TUint8        aType,
							  const TDesC8& aValue,
		                      RBuf8&        aQuickProperties,
		                      TDes8&        aTotalCountsDelta,
                              TContainerId& aRealParentId );
        
        TBool EncryptFirstL( TDbBookmark& aNextRow );
        
        TBool EncryptNextL ( TDbBookmark& aNextRow, TContainerId& aEncryptedContainerId );
        
        TBool DecryptFirstL( TDbBookmark& aNextRow );
        
        TBool DecryptNextL ( TDbBookmark& aNextRow, TContainerId& aDecryptedContainerId );
        
        TBool IsEncrypted();
        
        TContainerId FirstChildForDeleteL( TContainerId aId, TDbBookmark& aBookmark );
        
	private:
	
	    // ===============
	    // PRIVATE METHODS
	    // ===============
	    
		CContainerStoreContainersTable( CContainerStoreUtils&      aUtils,
										MCustomBehaviorProvider&   aCustomBehaviorProvider, 
									  	TUint                      aCountsLength,
									  	CContainerStoreEncryption& aEncryption );
		
		void CreateTableL();
		void OpenTableL();	

		void UpdateQuickPropertiesAndCountsL( TContainerId  aType,
										      const TDesC8& aProperties,
										      RBuf8&        aQuickProperties,
										      TDes8&        aCounts );
										   
		void IncrementParentCountsL( const TDesC8& aCountsDelta );							     				
		void DecrementParentCountsL( const TDesC8& aCountsDelta );

        void ReadPropertiesColumnL( RBuf8& aUnencryptedBuffer );
        void WritePropertiesColumnL( const TDesC8& aUnencryptedBuffer );
        
        void ReadAndDescrptPropertiesColumnL( RBuf8& aProperties );
        void EncryptAndWritePropertiesColumnL( const TDesC8& aProperties );
        
        void ReadUnencryptedPropertiesColumnL( RBuf8& aProperties );
        void WriteUnencryptedPropertiesColumnL( const TDesC8& aProperties );
        
        void SetEncryptedL( TBool aIsEncrypted );
        TContainerId GetFirstChildForDeleteL( TContainerId aId, TDbBookmark& aBookmark );
	    // ==================
	    // PRIVATE ATTRIBUTES
	    // ==================
	    
		MCustomBehaviorProvider&   iCustomBehaviorProvider;
		const TUint                iCountsLength;
		CContainerStoreEncryption& iEncryption;					     				
							     				
	    TInt iIdColNum;
	    TInt iParentIdColNum;
	    TInt iChildrenCountColNum;
		TInt iPropertiesColNum;
		TInt iQuickPropertiesColNum;
		TInt iCountsColNum;
	    TInt iChildrenCountsColNum;
        TInt iIsRowEncryptedColNum;
	    
	    __LOG_DECLARATION
    		
	}; // end class CContainerStoreContainersTable

#endif