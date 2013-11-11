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
* Description:   CGlxMediaList unit tests
*
*/



#ifndef __UT_CGLXMEDIALIST_H__
#define __UT_CGLXMEDIALIST_H__

//  EXTERNAL INCLUDES
#include <CEUnitTestSuiteClass.h>

//  INTERNAL INCLUDES
#include "mglxmedialistobserver.h"

//  FORWARD DECLARATIONS
class MGlxMediaList;
class CGlxMediaList;
class CGlxDefaultAttributeContext;
class CActiveSchedulerWait;

#include <e32def.h>
#ifndef NONSHARABLE_CLASS
    #define NONSHARABLE_CLASS(x) class x
#endif

//  CLASS DEFINITION
/**
 *
 * EUnitWizard generated test class. 
 *
 */
NONSHARABLE_CLASS( UT_CGlxMediaList )
     : public CEUnitTestSuiteClass, public MGlxMediaListObserver
    {
    public:     // Constructors and destructors

        /**
         * Two phase construction
         */
        static UT_CGlxMediaList* NewL();
        static UT_CGlxMediaList* NewLC();
        /**
         * Destructor
         */
        ~UT_CGlxMediaList();

    private:    // Constructors and destructors

        UT_CGlxMediaList();
        void ConstructL();

    public:     // From observer interface

	// From MGlxMediaListObserver
	virtual void HandleItemAddedL(TInt /*aStartIndex*/, TInt /*aEndIndex*/, MGlxMediaList* /*aList*/);
	virtual void HandleMediaL(TInt /*aListIndex*/, MGlxMediaList* /*aList*/) {};
	virtual void HandleItemRemovedL(TInt /*aStartIndex*/, TInt /*aEndIndex*/, MGlxMediaList* /*aList*/) {};
	virtual void HandleItemModifiedL(const RArray<TInt>& /*aItemIndexes*/, MGlxMediaList* /*aList*/) {};
	virtual void HandleAttributesAvailableL(TInt /*aItemIndex*/, const RArray<TMPXAttribute>& /*aAttributes*/, MGlxMediaList* /*aList*/);
	virtual void HandleFocusChangedL(NGlxListDefs::TFocusChangeType /*aType*/, TInt /*aNewIndex*/, TInt /*aOldIndex*/, MGlxMediaList* /*aList*/) {};
	virtual void HandleItemSelectedL(TInt /*aIndex*/, TBool /*aSelected*/, MGlxMediaList* /*aList*/) {};
	virtual void HandleMessageL(const CMPXMessage& /*aMessage*/, MGlxMediaList* /*aList*/) {};

    private:    // New methods

         void SetupL();
        
         void Teardown();
        
         void UT_CGlxMediaList_InstanceLL();
        
        
         void UT_CGlxMediaList_MediaListsL();
        
        
         void UT_CGlxMediaList_NewLCL();
        
        
         void UT_CGlxMediaList_OpenLL();
        
        
         void UT_CGlxMediaList_AddReferenceL();
        
        
         void UT_CGlxMediaList_RemoveReferenceL();
        
        
         void UT_CGlxMediaList_ReferenceCountL();
        
        
         void UT_CGlxMediaList_EqualsL();
        
        
         void UT_CGlxMediaList_OfferMediaL();
        
        
         void UT_CGlxMediaList_HandleAttributesAvailableLL();
        
        
         void UT_CGlxMediaList_AttributeRequestLL();
        
        
         void UT_CGlxMediaList_CloseL();
        
        
         void UT_CGlxMediaList_IdL();
        
        
         void UT_CGlxMediaList_CountL();
        
        
         void UT_CGlxMediaList_FocusIndexL();
        
        
         void UT_CGlxMediaList_SetFocusLL();
        
        
         void UT_CGlxMediaList_ItemL();
        
        
         void UT_CGlxMediaList_IndexL();
        
        
         void UT_CGlxMediaList_AddMediaListObserverLL();
        
        
         void UT_CGlxMediaList_RemoveMediaListObserverL();
        
        
         void UT_CGlxMediaList_AddContextLL();
        
        
         void UT_CGlxMediaList_RemoveContextL();
        
        
         void UT_CGlxMediaList_CollectionL();
        
        
         void UT_CGlxMediaList_PathLCL();
        
        
         void UT_CGlxMediaList_IsSelectedL();
        
        
         void UT_CGlxMediaList_SetSelectedLL();
        
        
         void UT_CGlxMediaList_SelectionCountL();
        
        
         void UT_CGlxMediaList_SelectedItemIndexL();
        
        
         void UT_CGlxMediaList_CommandLL();
        
        
         void UT_CGlxMediaList_CancelCommandL();
        
        
         void UT_CGlxMediaList_SetFilterLL();
        
        
         void UT_CGlxMediaList_FilterL();
        
        
         void UT_CGlxMediaList_HandleOpenLL();
        
        
         void UT_CGlxMediaList_HandleOpenL_1L();
        
        
         void UT_CGlxMediaList_HandleCollectionMediaLL();
        
        
         void UT_CGlxMediaList_HandleCollectionMessageL_1L();
        
	void SetupFocusTestL();
	void SetupPathTestL();
	void SetupSelectionTestL();
	void SetupFilterTestL();

	void MoveFocusAbsoluteGreaterThanCurrentFocusL();
	void MoveFocusAbsoluteLessThanCurrentFocusL();
	void MoveFocusRelativeForwardL();
	void MoveFocusRelativeBackwardL();
	void MoveFocusRelativeForwardGreaterThanListCountL();
	void MoveFocusRelativeBackwardLessThanZeroL();

	void ReceiveMessageItemChangedItemModifiedL();

    private:    // Data
	CGlxMediaList* iCGlxMediaList;
	CGlxDefaultAttributeContext* iContext;

	CActiveSchedulerWait* iSchedulerWait;

        EUNIT_DECLARE_TEST_TABLE; 

    private:

	// Test media list observer
	class CGlxMediaListObserverTest : public CBase, public MGlxMediaListObserver
		{
	public:
		static CGlxMediaListObserverTest* NewL();

		virtual ~CGlxMediaListObserverTest();

		// From MGlxMediaListObserver
		virtual void HandleItemAddedL(TInt /*aStartIndex*/, TInt /*aEndIndex*/, MGlxMediaList* /*aList*/) {};
		virtual void HandleMediaL(TInt /*aListIndex*/, MGlxMediaList* /*aList*/) {};
		virtual void HandleItemRemovedL(TInt /*aStartIndex*/, TInt /*aEndIndex*/, MGlxMediaList* /*aList*/) {};
		virtual void HandleItemModifiedL(const RArray<TInt>& /*aItemIndexes*/, MGlxMediaList* /*aList*/);
		virtual void HandleAttributesAvailableL(TInt /*aItemIndex*/, const RArray<TMPXAttribute>& /*aAttributes*/, MGlxMediaList* /*aList*/) {};
		virtual void HandleFocusChangedL(NGlxListDefs::TFocusChangeType aType, TInt aNewIndex, TInt aOldIndex, MGlxMediaList* aList);
		virtual void HandleItemSelectedL(TInt /*aIndex*/, TBool /*aSelected*/, MGlxMediaList* /*aList*/) {};
		virtual void HandleMessageL(const CMPXMessage& /*aMessage*/, MGlxMediaList* /*aList*/);

		const MGlxMediaList* List() { return iList; };

		// HandleFocusChangedL observations
		NGlxListDefs::TFocusChangeType FocusChangeType() { return iFocusChangeType; };
		TInt FocusNewIndex() { return iFocusNewIndex; };
		TInt FocusOldIndex() { return iFocusOldIndex; };

		// HandleItemModifiedL observations
		const RArray<TInt>& ModifiedItemIndices() { return iModifiedItemIndices; };

		// HandleMessageL observations
		const CMPXMessage* Message() { return iMessage; };
		TInt MessageCount() { return iMessageCount; };

	private:
		CGlxMediaListObserverTest();
		void ConstructL();

	private:
		MGlxMediaList* iList;

		// data from HandleFocusChangedL
		NGlxListDefs::TFocusChangeType iFocusChangeType;
		TInt iFocusNewIndex;
		TInt iFocusOldIndex;

		// data from HandleItemModifiedL
		RArray<TInt> iModifiedItemIndices;

		// data from HandleMessageL
		CMPXMessage* iMessage;
		TInt iMessageCount;
		};
    };

#endif      //  __UT_CGLXMEDIALIST_H__

// End of file
