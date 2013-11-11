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
* Description:   CGlxAttributeContext unit tests
*
*/



#ifndef __UT_CGLXATTRIBUTECONTEXT_H__
#define __UT_CGLXATTRIBUTECONTEXT_H__

//  EXTERNAL INCLUDES
#include <CEUnitTestSuiteClass.h>

//  INTERNAL INCLUDES
#include "mglxmedialist.h"

//  FORWARD DECLARATIONS
class CGlxDefaultAttributeContext;

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
NONSHARABLE_CLASS( UT_CGlxAttributeContext )
     : public CEUnitTestSuiteClass
    {
    public:     // Constructors and destructors

        /**
         * Two phase construction
         */
        static UT_CGlxAttributeContext* NewL();
        static UT_CGlxAttributeContext* NewLC();
        /**
         * Destructor
         */
        ~UT_CGlxAttributeContext();

    private:    // Constructors and destructors

        UT_CGlxAttributeContext();
        void ConstructL();

    public:     // From observer interface


    private:    // New methods

         void SetupL();
        
         void Teardown();
        
         void UT_CGlxAttributeContext_CGlxAttributeContextL();
        
        
         void UT_CGlxAttributeContext_AddAttributeLL();
        
        
         void UT_CGlxAttributeContext_RemoveAttributeL();
        
        
         void UT_CGlxAttributeContext_AllAttributesLL();
        
        
         void UT_CGlxAttributeContext_AttributeRequestLL();
        
	void SetupAttributeRequestTestL();
	void FetchItems_NoMediaL();
	void FetchOneItem_IndexZeroL();
	void FetchOneItem_IndexOneL();
	void FetchOneItem_IndexTwoL();
	void FetchOneItem_IndexThreeL();
	void FetchOneItem_IndexFourL();
	void FetchItems_GranularityLessThanRange_IndexZeroL();
	void FetchItems_GranularityLessThanRange_IndexOneL();
	void FetchItems_GranularityLessThanRange_IndexTwoL();
	void FetchItems_GranularityLessThanRange_IndexThreeL();
	void FetchItems_GranularityLessThanRange_IndexFourL();
	void FetchItems_GranularityLessThanRange_IndexFiveL();
	void FetchItems_GranularityGreaterThanRangeL();
	TBool FindAttribute(RArray<TMPXAttribute>& aAttributes, const TMPXAttribute& aAttribute);

    private:    // Data

	CGlxDefaultAttributeContext* iAttributeContext;

        EUNIT_DECLARE_TEST_TABLE; 

    private:

	// Test media list
	class CGlxMediaListTest : public CBase, public MGlxMediaList
		{
	public:
		static CGlxMediaListTest* NewL(TUint aNumberOfMedia);

		virtual ~CGlxMediaListTest();

		// From MGlxMediaList
		virtual void Close() {};
		virtual TGlxMediaListId Id() const { return TGlxMediaListId(1); };
		virtual TInt Count(NGlxListDefs::TCountType /*aType*/) const { return iItems.Count(); };
		virtual TInt FocusIndex() const { return iFocusIndex; };
		virtual void SetFocusL(NGlxListDefs::TFocusSetType /*aType*/, TInt aValue) { iFocusIndex = aValue; };
		virtual const TGlxMedia& Item(TInt aIndex) const { return iItems[aIndex]; };
		virtual TInt Index(const TGlxIdSpaceId& /* aIdSpaceId */, const TGlxMediaId& /*aId*/) const { return 0; };
		virtual void AddMediaListObserverL(MGlxMediaListObserver* /*aObserver*/) {};
		virtual void RemoveMediaListObserver(MGlxMediaListObserver* /*aObserver*/) {};
		virtual void AddContextL(const MGlxFetchContext* /*aContext*/, TInt /*aPriority*/) {};
		virtual void RemoveContext(const MGlxFetchContext* /*aContext*/) {};
		virtual MMPXCollection& Collection() const { MMPXCollection* c = NULL; return *c; }; // Don't call
		virtual CMPXCollectionPath* PathLC(NGlxListDefs::TPathType /*aType*/) const { CMPXCollectionPath* p = NULL; return p; }; // Don't call
		virtual TBool IsSelected(TInt /*aIndex*/) const { return EFalse; };
		virtual void SetSelectedL(TInt /*aIndex*/, TBool /*aSelected*/) {};
		virtual TInt SelectionCount() const { return 0; };
		virtual TInt SelectedItemIndex(TInt /*aSelectionIndex*/) const { return KErrNotFound; };
		virtual void CommandL(CMPXCommand& /*aCommand*/) {};
		virtual void CancelCommand() {};
		virtual void SetFilterL(CMPXFilter* /*aFilter*/) {};
		virtual CMPXFilter* Filter() const { return NULL; };
		virtual TGlxIdSpaceId IdSpaceId(TInt /*aIndex*/) const { return KGlxIdNone; };
    virtual TBool IsPopulated() const { return ETrue; }; // Don't call
    virtual void AddStaticItemL( CGlxMedia* /*aStaticItem*/,
        NGlxListDefs::TInsertionPosition /*aTargetPosition*/ ) {};
    virtual void RemoveStaticItem(const TGlxMediaId& /*aItemId*/) {};
    virtual void SetStaticItemsEnabled( TBool aEnabled ) { iStaticItemsEnabled = aEnabled; };
    virtual TBool IsStaticItemsEnabled() const { return iStaticItemsEnabled; };
	private:
		CGlxMediaListTest();
		void ConstructL(TUint aNumberOfMedia);

                void CreateMediaL(TUint aMediaId);

		TInt iFocusIndex;
		RPointerArray<CGlxMedia> iMedias;
		RArray<TGlxMedia> iItems;
		RArray<TInt> iSelectedItems;
		TBool iStaticItemsEnabled;
		};
    };

#endif      //  __UT_CGLXATTRIBUTECONTEXT_H__

// End of file
