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
* Description: This file contains CStartedTestsListBoxModel class
* declaration.
*
*/

#ifndef CSHOWSTARTEDCASESCONTAINER_H
#define CSHOWSTARTEDCASESCONTAINER_H

// INCLUDES
#include "Container.h"

#include <coecntrl.h>
#include <eiklbo.h>
#include <bamdesca.h> // MDesCArray

//#include "RRefArray.h" 
#include <stifinternal/RRefArray.h>

// CONSTANTS

// FORWARD DECLARATIONS
class CEikTextListBox;
class CAknSingleGraphicStyleListBox;
class CDesC16ArrayFlat;
class CStartedTestCase;
class CUIStore;

// CLASS DECLARATION

class CStartedTestsListBoxModel : public CTestCaseListBoxModel //public CBase, public MDesCArray
    {
    public:
        /**
        * Destructor.
        */
        ~CStartedTestsListBoxModel();
        
        /**
        * Two-phased constructor.
        * @param aTestsInView Array of test cases in view.
        * @return Pointer to created list box model.
        */
        static CStartedTestsListBoxModel* NewL(
                            RRefArray<CStartedTestCase>* aTestsInView);

    public: // Functions from base classes
        
        /**
        * Returns the number of descriptor elements in the array.
        * @return The number of descriptor elements.
        */
        TInt MdcaCount() const;
        
        /**
        * Indexes into a descriptor array.
        * @param aIndex The position of the descriptor element within the
        *               array. The position is relative to zero; i.e. zero
        *               implies the first descriptor element in the array. 
        *               This value must be non-negative and less than the 
        *               number of descriptors currently within the array 
        *               otherwise the operator panics with 
        *               EArrayIndexOutOfRange. 
        * @return A non-modifiable pointer descriptor representing the 
        *         descriptor element located at position aIndex within 
        *         the array.
        */
        TPtrC MdcaPoint(TInt aIndex) const;

    private:

        /**
        * Symbian OS two phased constructor.
        * Completes the construction of the object.
        */
        void ConstructL();

    private: //data
        RRefArray<CStartedTestCase>*        iTestCasesInView;
    };

/**
*  CShowStartedCasesContainer  container control class.
*  
*/
class CShowStartedCasesContainer : public CContainer //public CCoeControl /*, MEikListBoxObserver */ /* MCoeControlObserver */
    {
    public: // Constructors and destructor
        
        /**
        * Symbian OS default constructor.
        * @param aRect Frame rectangle for container.
        * @return Observer for list box.
        */
        void ConstructL(const TRect& aRect, MEikListBoxObserver* aListBoxObserver);

        /**
        * Destructor.
        */
        ~CShowStartedCasesContainer();

    public: // New functions
        
        /**
        * Show only testcases which are defined is specified module.
        * @param aModuleName Module which test cases are shown.
        */
        void FilterCasesByModuleL(TName aModuleName);
        
        /**
        * Show only testcases which are defined in specified test case file.
        * @param aTestCaseFileName Test cases file name.
        */
        void FilterCasesByTCFileNameL(TFileName aTestCaseFileName);
        
        /**
        * Remove possible filtering of test cases -> show all test cases.
        */
        void NoFilteringL();
        
        /**
        * Returns reference to currently selected test case in view (listbox).
        * @return Currently selected test case.
        */
        CStartedTestCase* SelectedTestCase();
        
        /**
        * Constructs list box model without any filtering.
        */
        void ConstructListBoxModelL();
        
        /**
        * Draws the list box.
        */
        void DrawListBox();
        
        /**
        * Returns count of test cases in view.
        * @return Count of test cases in view.
        */
        TInt TestsInViewCount();
        
        /**
        * Handles the addition of an item to the model.
        */
        void HandleItemAdditionL();
        
        /**
        * Handles the removal of an item from the model.
        */
        void HandleItemRemovalL();
        
        /**
        * Returns pointer to currently selected test case.
        * @param Current test case.
        */
        CStartedTestCase* CurrentTestCase();
        
        /**
        * Sets the current item.
        * @param aIndex Index of the item to make current.
        */
        void SetCurrentItemIndex(TInt aIndex);
        
        /**
        * Resets the selection indices, top and current item indices,
        * the selection, and the horizontal scroll offset of this list box.
        * This function does not redraw the list box.
        */
        void ResetListBox();
        
        /**
        * Removes items from list box.
        */
        void RemoveSelectedExecutionsL();
        
        /**
        * Removes all started test cases from list box.
        */
        void RemoveAllExecutionsInViewL();
        
        /**
         * Stores current position of focus of iListBox to the CAppUiAppUi object
         */
        void SaveActiveLine();

    private: // New functions

    public: // Functions from base classes

    private: // Functions from base classes

        /**
        * From CoeControl,SizeChanged.
        * Called by framework when the view size is changed.
        */
        void SizeChanged();

        /**
        * From CoeControl,CountComponentControls.
        * Gets a count of the component controls of this list box control.
        * This information is used for DrawNow().
        */
        TInt CountComponentControls() const;

        /**
        * From CCoeControl,ComponentControl.
        * Gets a pointer to the specified component control.
        * @param aIndex Index of the component control to look up.
        * @return The control at aIndex.
        */
        CCoeControl* ComponentControl(TInt aIndex) const;

        /**
        * From CCoeControl,Draw.
        * Draw a control, called by window server.
        * @param aRect The region of the control to be redrawn. Co-ordinates
        *              are relative to the control's origin (top left corner).
        */
        void Draw(const TRect& aRect) const;

        /**
        * From CCoeControl,OfferKeyEventL
        * Handles key events.
        * @param aKeyEvent The key event.
        * @param aType The type of key event.
        * @return Indicates whether or not the key event was used 
        *         by this control.
        */
		TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);
	
    private: //data
        
        CStartedTestsListBoxModel*  iListBoxModel;    
        RRefArray<CStartedTestCase> iStartedTestsPtrs;
    };

#endif // CSHOWSTARTEDCASESCONTAINER_H

// End of File
