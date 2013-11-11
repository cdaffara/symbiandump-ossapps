/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Unit tests
*
*/



#ifndef __C_GLXLISTTESTBASE_H__
#define __C_GLXLISTTESTBASE_H__

//  EXTERNAL INCLUDES
#include <CEUnitTestSuiteClass.h>
#include <EUnitDecorators.h>
#include <glxlistdefs.h>

//  INTERNAL INCLUDES
#include "mglxmediapool.h"
#include "mglxmediauser.h"
#include "_glxlisttesthelpers.h"

//  FORWARD DECLARATIONS
class CGlxItemListReconstruction;
class CMPXCollectionPath;
class TNotification;

template < class _ListClass, class _ReconstructionClass >
class CGlxListTestBase : public CEUnitTestSuiteClass, public MGlxMediaPool, 
        public MGlxMediaUser
    {
    protected:
        ~CGlxListTestBase();
        void DeleteMembers();
                
        // From MGlxMediaPool
        CGlxMedia* Media( const TGlxIdSpaceId& aIdSpaceId,
            const TGlxMediaId& aId) const;
    
        /** 
         * Create a collection path from a string, in which each char
         * is an id 
         */
        CMPXCollectionPath* CreatePathLC( const TDesC8& aString );
        
        /** 
         * Add a static item - can be only used for _ListClass
         * that support AddStaticItemL(...)
         */
        void AddStaticItemL( TChar aId, NGlxListDefs::TInsertionPosition aPosition );
        void AddStaticItemsL( NGlxListTestHelpers::TStaticItemInfo::TData* aInfos );
        
        /**
         * Test if counts equal - can only be used for _ListClass that supports 
         * Count( NGlxListDefs::TCountType ) and SetStaticItemsEnabled
         * Note: Will test with both static items enabled and disabled, so sends
         * notifications to list observer, which will affect notification list 
         * of the reconstruction. So don't use when notifications list is being tested.
         */
        TBool CountEqualsL( TInt aCountAll, TInt aCountNonStatic, 
            TInt aCountAllStatic, TInt aCountPreStatic, TInt aCountPostStatic );
         
        void CreateListL( const TDesC8& aString );
        void CreateListL( const TDesC8& aString, const TDesC8& aLinkedToMedia );
        void CreateListL( const TDesC8& aString, TBool aStaticItemsEnabled,
            NGlxListTestHelpers::TStaticItemInfo::TData* aStaticItemInfos );

        void CreateReconstructionL();
        TBool Equals( const TDesC8& aNewString );
        TBool Equals( const TDesC8& aNewString, const TNotification::TData* aNotificationList );
        TBool Equals( const TDesC8& aStringWithoutStaticItems, 
            const TDesC8& aStringWithStaticItems, TBool aStaticItemsEnabled, 
            const TNotification::TData* aNotificationList);
        
        /** 
         * Checks if items are linked to media objects
         * @param aAnswers expected linking, format "tttft", in which 
         *                 't' indicates "yes, is linked", 
         *                 and 'f' indicates "no, is not linked"
         */
        TBool IsLinkedToMedia( const TDesC8& aAnswers ) const;
        /** 
         * Creates a media object in pool, and offers to list.
         * @param aAnswers See IsLinkedToMedia
         * @param aShouldAccept ETrue if list should accept the offer
         */
        //TBool TryOfferMedia( TChar aId, const TDesC8& aAnswers, TBool aShouldAccept );
        
        /** 
         * Tries to remove reference of list from media
         * @param aAnswers See IsLinkedToMedia
         */
        TBool TryRemoveReference( TInt aIndex, const TDesC8& aAnswers );

        /** Set contents, compare result */
        TBool TrySetContentsL( const TDesC8& aOldString, const TDesC8& aNewString );
        /** Set contents, compare result and notifications list */
        TBool TrySetContentsL( const TDesC8& aOldString, const TDesC8& aNewString, 
            const TNotification::TData* aNotificationList );
        /** Set contents with and without static items, compare result and notifications list */
        TBool TrySetContentsL( const TDesC8& aOldString, const TDesC8& aNewString, 
            const TDesC8& aNewStringWithStaticItems, 
            const TNotification::TData* aNotificationListStaticItemsEnabled,
            const TNotification::TData* aNotificationListStaticItemsDisabled, 
            NGlxListTestHelpers::TStaticItemInfo::TData* aStaticItemInfos );
            
        TBool TryRemoveL( const TDesC8& aOldString, const TChar aItemToRemove, 
            const TDesC8& aNewString, const TNotification::TData* aNotificationList );
        TBool TryRemoveL( const TDesC8& aOldString, 
            const TChar aItemToRemove, TBool aIsStaticId,
            const TDesC8& aNewStringWithoutStaticItems, 
            const TDesC8& aNewStringWithStaticItems, 
            const TNotification::TData* aNotificationListStaticItemsDisabled,
            const TNotification::TData* aNotificationListStaticItemsEnabled, 
            NGlxListTestHelpers::TStaticItemInfo::TData* aStaticItemInfos);
        /**
         * Test setting static items enabled
         */
        TBool TrySetStaticItemsEnabledL( const TDesC8& aStringWithoutStaticItems, 
            const TDesC8& aStringWithStaticItems, 
            NGlxListTestHelpers::TStaticItemInfo::TData* aInfos,
            const TNotification::TData* aNotificationListForEnable,
            const TNotification::TData* aNotificationListForDisable );

        TBool TryAddStaticItemL( const TDesC8& aInitString, 
            TChar aStaticItemId, NGlxListDefs::TInsertionPosition aInsertPos,
            const TDesC8& aStringAfter, const TNotification::TData* aNotificationListForAdd,
            const TNotification::TData* aNotificationListForAllStatic,
            NGlxListTestHelpers::TStaticItemInfo::TData* aStaticItemInfos );

        /** Sets contents, does not create a list */
        TBool SetContentsL( const TDesC8& aString );

        /** Reorders contents, does not create a list */
        TBool ReorderContentsL( const TDesC8& aString );
    
        /** Run test for OfferMedia */
        void TestOfferMediaL();
        /** Run test for RemoveReference */
        void TestRemoveReferenceL();
        /** Run test for IdSpaceId */
        void TestIdSpaceIdL();
        /** Run test for Count, requires static items support in deriving class */
        void TestCountL();
        /** Run test for Index, requires static items support in deriving class */
        void TestIndexL();
        /** Run test for Item, requires static items support in deriving class */
        void TestItemL();

    private:
        TBool TrySetContentsL(  const TDesC8& aOldString, 
            const TDesC8& aNewString, const TDesC8& aNewStringWithStaticItems,
            const TNotification::TData* aNotificationList, 
            TBool aStaticItemsEnabled, 
            NGlxListTestHelpers::TStaticItemInfo::TData* aStaticItemInfos );
            
        TBool TryRemoveL( const TDesC8& aOldString, 
            const TChar aItemToRemove, TBool aIsStaticId,
            const TDesC8& aNewString, 
            const TNotification::TData* aNotificationList,
            TBool aStaticItemsEnabled,
            NGlxListTestHelpers::TStaticItemInfo::TData* aStaticItemInfos );
            
        TBool TryAddStaticItemL( const TDesC8& aInitString, TChar aStaticItemId, 
            NGlxListDefs::TInsertionPosition aInsertPos,
            const TDesC8& aStringAfter,
            const TNotification::TData* aNotificationList,
            TBool aStaticItemsEnabled, 
            NGlxListTestHelpers::TStaticItemInfo::TData* aStaticItemInfos );

        void GetRequiredAttributesL( TInt aIndex, RArray< TMPXAttribute >& aAttributes );
        void HandleAttributesAvailableL( TInt aIndex, const RArray< TMPXAttribute >& aAttributes );
        void RemoveReference( TInt aIndex );
        void HandleError( TInt aError );            
        
    protected:
        // class under test
        _ListClass* iList;
        // reconstruction of list based on notifications
        _ReconstructionClass* iReconstruction;
        // Pool of media items
        RPointerArray< CGlxMedia > iMediaPool;
    };
    
#include "glxlisttestbase.inl"

#endif      //  __C_GLXLISTTESTBASE_H__
