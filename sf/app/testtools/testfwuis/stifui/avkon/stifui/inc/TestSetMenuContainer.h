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
* Description: This file contains CTestSetMenuContainer class 
* declaration.
*
*/

#ifndef TestSetMenuContainer_H
#define TestSetMenuContainer_H

// INCLUDES
#include <coecntrl.h>
#include <eiklbo.h>     // MEikListBoxObserver
#include <bamdesca.h>   // MDesCArray
#include <aknsfld.h>

//#include "RRefArray.h"        
//#include "UIEngine.h"         
//#include "UIStoreIf.h"        
//#include "UIStoreContainer.h" 

#include <stifinternal/RRefArray.h>
#include <stifinternal/UIEngine.h>
#include <stifinternal/UIStoreIf.h>
#include <stifinternal/UIStoreContainer.h>
   
// FORWARD DECLARATIONS
class CEikTextListBox;
class CDesC16Array;

class CTestSetMenuView;
class CEikColumnListBox;
class CTestCaseListBoxModel;

class CUIStoreHandler;
class CUIStore;
class CUIStoreIf;
class CUIEngineContainer;
class CUIEngine;

// CLASS DECLARATION
/**
*  CTestSetMenuContainer  container control class.
*  
*/
class CTestSetMenuContainer : public CCoeControl /*, MEikListBoxObserver */ /* MCoeControlObserver */
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
        ~CTestSetMenuContainer();

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
        * Returns pointers to selected test cases.
        * @param aSelectedTestCases Currently selected test case.
        */
        void SelectedTestCases(RPointerArray<CTestInfo>& aSelectedTestCases);

        /**
        * Sets graphic icon using listbox as CEikColumnListBox.
        * @param aListBox Pointer to list box.
        */
        void SetGraphicIconL( CEikColumnListBox* aListBox );

        /**
        * Appends graphics data.
        * @param aIcons Pointer array of icons.
        */
        void GraphicIconL( CArrayPtr<CGulIcon>* aIcons );
        
        /**
        * Gets the index number of the current item in the view.
        * @return Index number of the current item.
        */
        TInt CurrentItemIndex();
        
        /**
        * Sets the current item.
        * @param aIndex Index of the item to make current.
        */
        void SetCurrentItemIndex(TInt aCurrentTestCase);
		
		/**
		* Inline function for getting pointer to list box.
		* @return Pointer to list box.
		*/
		inline CEikListBox* ListBox() { return (CEikListBox*)iListBox; };
		
		/**
		* Processes user commands.
		* @param aCommand ID of the command to respond to.
		*/
		void ProcessCommandL( TInt  aCommand );
		
		/**
		* Processes user commands.
		* @param aCommand ID of the command to respond to.
		*/
		void SelectionListProcessCommandL( TInt  aCommand );
		
		/**
		* Handles mark commands.
		* @param aCommand ID of the command to respond to.
		*/
		void HandleMarkCommandL( TInt aCommand );
        
        /**
		* Removes items from list box.
		* @param aSelected Indexes of removed items.
		*/
		void RemoveListBoxItemL( TInt aSelected );
		
        /**
         * Stores current position of focus of iListBox to the CAppUiAppUi object
         */
		void SaveActiveLine();
		
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
        
        CTestSetMenuView*           iParentView;
        CEikColumnListBox*          iListBox;
        CTestCaseListBoxModel*      iListBoxModel;
        RPointerArray<CTestInfo>    iTestCasesInView;
        CUIStore*                   iUIStore;
    };

#endif // CTestSetMenuContainer_H

// End of File
