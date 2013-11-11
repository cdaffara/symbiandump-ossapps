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
* Description: This file contains CTestSetStartedCasesContainer
* class declaration.
*
*/

#ifndef TESTSETSTARTEDCASESCONTAINER_H
#define TESTSETSTARTEDCASESCONTAINER_H

// INCLUDES
#include <coecntrl.h>
#include <eiklbo.h>
#include <bamdesca.h> // MDesCArray

//#include "RRefArray.h" 
#include <stifinternal/RRefArray.h>


// Define *.mbm file name.
_LIT(KMbmFileName, "\\Resource\\Apps\\Stifui.mbm");

// FORWARD DECLARATIONS
class CEikTextListBox;
class CAknSingleGraphicStyleListBox;
class CDesC16ArrayFlat;
class CStartedTestCase;

class CUIStore;
class CStartedTestsListBoxModel;

// CLASS DECLARATION

/**
*  CTestSetStartedCasesContainer  container control class.
*  
*/
class CTestSetStartedCasesContainer : public CCoeControl /*, MEikListBoxObserver */ /* MCoeControlObserver */
    {
    public: // Constructors and destructor
        
        /**
        * Symbian OS default constructor.
        * @param aRect Frame rectangle for container.
        */
        void ConstructL(const TRect& aRect, MEikListBoxObserver* aListBoxObserver);

        /**
        * Destructor.
        */
        ~CTestSetStartedCasesContainer();

    public: // New functions
    
        /**
        * Returns reference to currently selected test case in view (listbox).
        * @return Currently selected test case.
        */
        CStartedTestCase* SelectedTestCase();
        
        /**
        * Constructs list box model.
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
        * Gets the index number of the current item in the view.
        * @return Index number of the current item.
        */
        TInt CurrentItemIndex();
        
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
        
        CEikTextListBox*            iListBox;
        CStartedTestsListBoxModel*  iListBoxModel;    
        RRefArray<CStartedTestCase> iStartedTestsPtrs;
        CUIStore*                   iUIStore;
    };

#endif // CTestSetStartedCasesCONTAINER_H

// End of File
