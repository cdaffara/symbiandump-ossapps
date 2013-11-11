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
* Description: This file contains CTestSetMenuView class declaration.
*
*/

#ifndef TESTSETMENUVIEW_H
#define TESTSETMENUVIEW_H

// INCLUDES
#include "View.h"
#include <eiklbo.h> // MEikListBoxObserver
#include <aknnavide.h> 

// CONSTANTS

// FORWARD DECLARATIONS
class CTestSetMenuContainer;

// CLASS DECLARATION
/**
*  CTestCaseMenuView view class.
*/
class CTestSetMenuView : public CView, public MEikListBoxObserver
    {
    public: // Constructors and destructor

        /**
        * Symbian OS default constructor.
        */
        void ConstructL();

        /**
        * Destructor.
        */
        ~CTestSetMenuView();

    public: // Functions from base classes
        
        /**
        * Returns view´s id.
        * @return View id.
        */
        TUid Id() const;

        /**
        * Handles a command.
        * @param aCommand A command ID.
        */
        void HandleCommandL( TInt aCommand );

        /**
        * Handles client rect changes.
        */
        void HandleClientRectChange();
    
    public: // From MEikListBoxObserver
        
        /**
        * Handles list box event.
        * @param aListBox Pointer to list box.
        * @param aEventType List box event.
        */
        void HandleListBoxEventL(CEikListBox* aListBox, 
            TListBoxEvent aEventType);
                
    public: // New functions
        
        /**
        * Starts all cases in active test set.
        */
        void StartTestSetL();
        
        /**
        * Removes selected test cases from active test set.
        */
        void RemoveSelectedTestCasesL();
        
        /**
        * Creates new test set.
        * @param aTestSetName Name of the test set.
        * @return Symbian OS error code.
.       */
        TInt CreateTestSetL( const TDesC& aTestSetName );
        
        /**
        * Loads saved test set.
        * @param aTestSetName Name of the test set.
        * @return Symbian OS error code.
        */
        TInt LoadTestSetL( const TDesC& aTestSetName );
        
        /**
        * Saves current test set.
        * @return Symbian OS error code.
        */
        TInt SaveCurrentTestSetL();
        
        /**
        * Removes current test set.
        * @return Symbian OS error code.
        */
        TInt RemoveActiveTestSet();
        
        /**
        * Get name of the current test set.
        * @return Name of the current test set.
        */
        inline const TDesC& CurrentTestSet()
            { return iCurrentTestSet; }
        
        /**
        * Set save needed flag.
        */
        inline void SetSaveNeeded( TBool aSave )
            { iSaveNeeded = aSave; }
                
    private: // From AknView

        /**
        * Initializes view when activated.
        * @param aPrevViewId Id of the previous view (not used).
        * @param aCustomMessageId Custom message id (not used).
        * @param aCustomMessage Custom message (not used).
        */
        void DoActivateL(const TVwsViewId& aPrevViewId,TUid aCustomMessageId,
            const TDesC8& aCustomMessage);

        /**
        * Deactivates view.
        */
        void DoDeactivate();

    private: // From MEikMenuObserver
        
        /**
        * Initializes menu pane.
        * @param aResourceId Menu pane resource ID.
        * @param aMenuPane Menu pane pointer.
        */
        void DynInitMenuPaneL(TInt aResourceId,CEikMenuPane* aMenuPane);

    private: // Data
        CAknNavigationDecorator*    iNaviDecorator;
        CTestSetMenuContainer*      iContainer;
        CUIStore*                   iUIStore;
        TInt                        iCurrentTestCase;
        TInt                        iTestSetIndex;
        TBuf<50>                    iCurrentTestSet;
        TBool                       iSaveNeeded;
        
    };

#endif // TESTSETMENUVIEW_H

// End of File
