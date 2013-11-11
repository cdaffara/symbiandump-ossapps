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
* Description: This file contains CTestCaseOutputView class 
* declaration.
*
*/

#ifndef TESTCASEOUTPUTVIEW_H
#define TESTCASEOUTPUTVIEW_H

// INCLUDES
#include "View.h"
#include <aknnavide.h> 


// CONSTANTS

// FORWARD DECLARATIONS
class CTestCaseOutputContainer;

// CLASS DECLARATION

/**
*  CTestCaseOutputView view class.
* 
*/
class CTestCaseOutputView : public CView
    {
    public: // Constructors and destructor

        /**
        * Symbian OS default constructor.
        */
        void ConstructL();

        /**
        * Destructor.
        */
        ~CTestCaseOutputView();

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

    public: // New functions
        
        /**
        * Receives output update notification from AppUI
        * @param aTestCase A pointer to started test case.
        */
        void OutputUpdateL( CStartedTestCase* aTestCase );

        /**
        * Prints test case state to title pane
        */
        void PrintTestCaseStateL();

        /**
        * Handles test case state changed notification
        */
        void TestCaseStateChangedL();

    private:

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

        /** From MEikMenuObserver
        * Initializes menu pane.
        * @param aResourceId Menu pane resource ID
        * @param aMenuPane Menu pane pointer
        */
        void DynInitMenuPaneL(TInt aResourceId,CEikMenuPane* aMenuPane);


    private: // Data
        CAknNavigationDecorator*    iNaviDecorator;
        CTestCaseOutputContainer*   iContainer;
        CStartedTestCase* 			iCurrentTestCase;

    };

#endif //SHOWSTARTEDCASESVIEW_H

// End of File
