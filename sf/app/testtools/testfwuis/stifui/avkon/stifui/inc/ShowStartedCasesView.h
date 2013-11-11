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
* Description: This file contains CShowStartedCasesView class 
* declaration.
*
*/

#ifndef SHOWSTARTEDCASESVIEW_H
#define SHOWSTARTEDCASESVIEW_H

// INCLUDES
#include "View.h"
#include <eiklbo.h>     // MEikListBoxObserver
#include <aknnavide.h>  //CAknNavigationDecorator

// CONSTANTS

// FORWARD DECLARATIONS
class CShowStartedCasesContainer;

// CLASS DECLARATION

/**
*  CShowStartedCasesView view class.
* 
*/
class CShowStartedCasesView : public CView, public MEikListBoxObserver
    {
    public: // Constructors and destructor

        /**
        * Symbian OS default constructor.
        */
        void ConstructL();

        /**
        * Destructor.
        */
        ~CShowStartedCasesView();

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

        /**
        * Handles a list box event.
        * @param aListBox A pointer to list box (not used).
        * @param aEventType Type of list box event.
        */
        void HandleListBoxEventL(CEikListBox* /*aListBox*/, TListBoxEvent aEventType);

    public: // New functions
        
        /**
        * Handles status changes of test cases in view.
        */
        void TestCaseStateChangedL();
        
        /**
        * Handles call to view test case´s output data.
        */
        void ViewTestCaseOutputL();

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
        
        /**
        * Refreshes view name shown in the navi pane.
        */
        void RefreshNaviTitleL();

    private: // Data
        CShowStartedCasesContainer* iContainer;
        CAknNavigationDecorator*    iNaviDecorator;
        TName                       iFilterModule;
        TFileName                   iFilterTestCaseFile;
        TInt                        iSelectedTestCase; // Which testcase was 
                                                       // selected when view 
                                                       // was active earlier.
    };

#endif //SHOWSTARTEDCASESVIEW_H

// End of File
