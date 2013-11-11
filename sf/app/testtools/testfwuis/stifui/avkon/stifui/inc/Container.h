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
* Description: This file contains CTestCaseListBoxModel class
* declaration
*
*/

#ifndef CCONTAINER_H
#define CCONTAINER_H

#include <e32std.h>
// Define *.mbm file name.
_LIT(KListMbmFileName, "\\Resource\\Apps\\Stifui.mbm");

// INCLUDES
#include "View.h"

#include <coecntrl.h>
#include <eiklbo.h>     //  MEikListBoxObserver
#include <bamdesca.h>   //  MDesCArray
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

//class CTestSetInsertMenuView;
class CEikColumnListBox;
class CStartedTestCase;

class CUIStoreHandler;
class CUIStore;
class CUIStoreIf;
class CUIEngineContainer;
class CUIEngine;

// CLASS DECLARATION

class CTestCaseListBoxModel : public CBase, public MDesCArray
    {
    public:
        /**
        * Destructor.
        */
        ~CTestCaseListBoxModel();
        
        /**
        * Two-phased constructor.
        * @param aTestCasesInView Array of test cases in view.
        * @return Pointer to created list box model.
        */
        static CTestCaseListBoxModel* NewL(
            RPointerArray<CTestInfo>* aTestCasesInView);

    public: // Functions from base classes
    
        /**
        * Returns the number of descriptor elements in the array.
        * @return The number of descriptor elements.
        */
        virtual TInt MdcaCount() const;
        
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
        virtual TPtrC MdcaPoint( TInt aIndex ) const;
    protected:
        /**
        * Symbian OS two phased constructor.
        * Completes the construction of the object.
        */
        void ConstructL();
    
    protected: //data
        RPointerArray<CTestInfo>*   iTestCasesInView;
        HBufC*                      iBuffer;
        
    private: //data
        
    };
    
/**
*  CContainer  container control class.
*  
*/
class CContainer : public CCoeControl
    {
    public: // Constructors and destructor
        
        /**
        * Symbian OS default constructor.
        * @param aRect Frame rectangle for container.
        */
        void ConstructL();

        /**
        * Destructor.
        */
        ~CContainer();

    public: // New functions
    
        /**
        * Show only testcases which are defined is specified module.
        * @param aModuleName Module which test cases are shown.
        */
        virtual void FilterCasesByModuleL(TName aModuleName);
        
        /**
        * Show only testcases which are defined in specified test case file.
        * @param aTestCaseFileName Test cases file name.
        */
        virtual void FilterCasesByTCFileNameL(TFileName aTestCaseFileName);
        
        /**
        * Remove possible filtering of test cases -> show all test cases.
        */
        virtual void NoFilteringL();
        
        /**
        * Returns pointers to selected test cases.
        * @param aSelectedTestCases Currently selected test case.
        */
        virtual void SelectedTestCases(RPointerArray<CTestInfo>& aSelectedTestCases);

        /**
        * Returns pointers to currently (in list box) selected test cases info.
        * @return Currently selected test case info.
        */
        virtual CTestInfo* SelectedTestCaseInfo();
        
        /**
        * Returns pointers to selected test cases info.
        * @param aIndex test case index
        * @return Currently selected test case info.
        */
        virtual CTestInfo* TestCaseInfo( TInt aIndex );
        
        /**
        * Sets graphic icon using listbox as CEikColumnListBox.
        * @param aListBox Pointer to list box.
        */
        virtual void SetGraphicIconL( CEikColumnListBox* aListBox );

        /**
        * Appends graphics data.
        * @param aIcons Pointer array of icons.
        */
        virtual void GraphicIconL( CArrayPtr<CGulIcon>* aIcons );
        
        /**
        * Gets the index number of the current item in the view.
        * @return Index number of the current item.
        */
        virtual TInt CurrentItemIndex();
        
        /**
        * Sets the current item.
        * @param aIndex Index of the item to make current.
        */
        virtual void SetCurrentItemIndex(TInt aCurrentTestCase);
		
		/**
		* Inline function for getting pointer to list box.
		* @return Pointer to list box.
		*/
		virtual inline CEikListBox* ListBox() { return (CEikListBox*)iListBox; };
		
		
		
		/**
		* Processes user commands.
		* @param aCommand ID of the command to respond to.
		*/
		virtual void ProcessCommandL( TInt  aCommand );
		
		/**
		* Processes user commands.
		* @param aCommand ID of the command to respond to.
		*/
		virtual void SelectionListProcessCommandL( TInt  aCommand );
		
		/**
		* Handles mark commands.
		* @param aCommand ID of the command to respond to.
		*/
		virtual void HandleMarkCommandL( TInt aCommand );
		
		
		
    public: // Functions from base classes

    private: // Functions from base classes

        /**
        * From CoeControl,SizeChanged.
        * Called by framework when the view size is changed.
        */
        virtual void SizeChanged();




        /**
        * From CoeControl,CountComponentControls.
        * Gets a count of the component controls of this list box control.
        * This information is used for DrawNow().
        */
        virtual TInt CountComponentControls() const;

        /**
        * From CCoeControl,ComponentControl.
        * Gets a pointer to the specified component control.
        * @param aIndex Index of the component control to look up.
        * @return The control at aIndex.
        */
        virtual CCoeControl* ComponentControl(TInt aIndex) const;



        /**
        * From CCoeControl,Draw.
        * Draw a control, called by window server.
        * @param aRect The region of the control to be redrawn. Co-ordinates
        *              are relative to the control's origin (top left corner).
        */
        virtual void Draw(const TRect& aRect) const;
        
    protected: //data
        
        CView*                          iParentView;
        CEikColumnListBox*              iListBox;
        //CEikTextListBox*                iListBox;
        CTestCaseListBoxModel*          iListBoxModel;
        RPointerArray<CTestInfo>        iTestCasesInView;
        CUIStore*                       iUIStore;
                        
    private: //data
        
    };

#endif // CTestSetInsertMenuContainer_H

// End of File
