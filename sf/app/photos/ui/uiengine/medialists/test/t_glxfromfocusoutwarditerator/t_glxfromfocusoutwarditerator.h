/*
* Copyright (c) 2006-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   TGlxFromFocusOutwardIterator unit tests
*
*/



#ifndef __T_GLXFROMFOCUSOUTWARDITERATOR_H__
#define __T_GLXFROMFOCUSOUTWARDITERATOR_H__

//  EXTERNAL INCLUDES
#include <CEUnitTestSuiteClass.h>

//  INTERNAL INCLUDES
#include "mglxmedialist.h"
#include "glxmedialistiterator.h"

//  FORWARD DECLARATIONS

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
NONSHARABLE_CLASS( T_CGlxFromFocusOutwardIterator )
     : public CEUnitTestSuiteClass
    {
    public:     // Constructors and destructors
        static T_CGlxFromFocusOutwardIterator* NewL();
        static T_CGlxFromFocusOutwardIterator* NewLC();
        ~T_CGlxFromFocusOutwardIterator();

    private:    // Constructors and destructors
        T_CGlxFromFocusOutwardIterator();
        void ConstructL();
 
    private:    // New methods
        void SetupL();
        void Teardown();
         
        void UT_TestEmptyL();
        void UT_TestSymmetricRangeL();
        void UT_TestAsymmetricRangeL();
        void UT_TestOneSideRangeL();
        void UT_TestInRangeL();
       
        /**
         * Run a single test
         * @param aCount Number of items in list
         * @param aFocusIndex index of focus
         * @param aFrontOffset
         * @param aRearOffset
         * @param aResult expected results in format "iiiii*e",
         *				  where i is an index, and e means KErrNotFound.
         *				  E.g., "14523ee" means iOrder++ should return 1 first,
         *				  4 then, etc. and finally KErrNotFound two times
         */
		void TestSingleRange(TInt aCount, TInt aFocusIndex, TInt aRearOffset,
			TInt aFrontOffset, char* aResult);
       
        /**
         * Run a single "InRange()" test
         * @param aCount Number of items in list
         * @param aFocusIndex index of focus
         * @param aFrontOffset
         * @param aRearOffset
         * @param aResult expected results in format "xxx  xx",
         *				  where x means the n-th item is in range, and space means it is not
         */
		void TestSingleInRange(TInt aCount, TInt aFocusIndex, TInt aRearOffset,
			TInt aFrontOffset, char* aResult);

    private:
		// Dummy media list class         
	    struct TGlxMediaListTester : public MGlxMediaList
	    	{
	    	TGlxMediaListTester() : iItem(TGlxMediaId(1)), iSelectedItems()
	    		{
	    		iCount = 0;
	    		iFocusIndex = KErrNotFound;
	    		};
			virtual void Close() {};
			virtual TGlxMediaListId Id() const { return TGlxMediaListId(1); };
			virtual TInt Count(NGlxListDefs::TCountType /*aType*/) const { return iCount; };
			virtual TInt FocusIndex() const { return iFocusIndex; };
			virtual void SetFocusL(NGlxListDefs::TFocusSetType /*aType*/, TInt aValue) { iFocusIndex = aValue; };
			virtual const TGlxMedia& Item(TInt /*aIndex*/) const { return iItem; };
			virtual TInt Index(const TGlxIdSpaceId& /* aIdSpaceId */, const TGlxMediaId& /*aId*/) const { return 0; };
			virtual void AddMediaListObserverL(MGlxMediaListObserver* /*aObserver*/) {};
			virtual void RemoveMediaListObserver(MGlxMediaListObserver* /*aObserver*/) {};
			virtual void AddContextL(const MGlxFetchContext* /*aContext*/, TInt /*aPriority*/) {};
			virtual void RemoveContext(const MGlxFetchContext* /*aContext*/) {};
			virtual MMPXCollection& Collection() const { MMPXCollection* c = NULL; return *c; }; // Don't call
			virtual CMPXCollectionPath* PathLC(NGlxListDefs::TPathType /*aType*/) const { CMPXCollectionPath* p = NULL; return p; }; // Don't call
			virtual TBool IsSelected(TInt /*aIndex*/) const { return EFalse; };
			virtual void SetSelectedL(TInt /*aIndex*/, TBool /*aSelected*/) {};
			virtual TInt SelectionCount() const { return iSelectedItems.Count(); };
			virtual TInt SelectedItemIndex(TInt /*aSelectionIndex*/) const { return KErrNotFound; };
			virtual void CommandL(CMPXCommand& /*aCommand*/) {};
			virtual void CancelCommand() {};
			virtual void SetFilterL(CMPXFilter* /*aFilter*/) {};
			virtual CMPXFilter* Filter() const { return NULL; };
			virtual TGlxIdSpaceId IdSpaceId(TInt /*aIndex*/) const { return TGlxIdSpaceId(1); };
      virtual TBool IsPopulated() const { return ETrue; }; // Don't call
      virtual void AddStaticItemL( CGlxMedia* /*aStaticItem*/,
        NGlxListDefs::TInsertionPosition /*aTargetPosition*/ ) {};
      virtual void RemoveStaticItem(const TGlxMediaId& /*aItemId*/) {};
      virtual void SetStaticItemsEnabled( TBool aEnabled ) { iStaticItemsEnabled = aEnabled; };
      virtual TBool IsStaticItemsEnabled() const { return iStaticItemsEnabled; };
			TGlxMedia iItem;
			TInt iCount;
			TInt iFocusIndex;
			RArray<TInt> iSelectedItems;
			TBool iStaticItemsEnabled;
			};

    private:    // Data
		MGlxMediaListIterator* iOrder; // To access virtual functions only
		TGlxMediaListTester iList;
		TGlxFromFocusOutwardIterator iFocusOrder;
		
        EUNIT_DECLARE_TEST_TABLE; 
    };

#endif      //  __T_GLXFROMFOCUSOUTWARDITERATOR_H__
