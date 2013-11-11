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
* Description: This file contains CStartCasesView class declaration.
*
*/

#ifndef STARTCASESVIEW_H
#define STARTCASESVIEW_H

// INCLUDES
#include "View.h"
#include <eiklbo.h> // MEikListBoxObserver
#include <aknnavide.h> 


// CONSTANTS

// FORWARD DECLARATIONS
class CStartCasesContainer;


// CLASS DECLARATION

/**
*  CTestCaseMenuView view class.
* 
*/
class CStartCasesView : public CView, public MEikListBoxObserver //CAknView
    {
    public: // Constructors and destructor

        /**
        * Symbian OS default constructor.
        */
        void ConstructL();

        /**
        * Destructor.
        */
        ~CStartCasesView();

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
        * @param aListBox A pointer to list box.
        * @param aEventType Type of list box event.
        */
        void HandleListBoxEventL(CEikListBox* aListBox, TListBoxEvent aEventType);

    public: // New functions
        
        /** 
        * Starts test case.
        */
        void StartTestCaseL();
        
        /**
        * Starts multiple test cases.
        * @param aSelectedÍndexes Indexes of selected test cases.
        */
        void StartTestCasesL( RArray<TInt> aSelectedIndexes );

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

        /**
        * Initializes menu pane.
        * @param aResourceId Menu pane resource ID.
        * @param aMenuPane Menu pane pointer.
        */
        void DynInitMenuPaneL(TInt aResourceId,CEikMenuPane* aMenuPane);
    
    private: // New functions
        
        /**
        * Checks listbox selections and launches 
        * query dialog to start test cases.
        * @param aListBox Pointer to listbox.
        */
        void CheckListBoxSelectionsL(CEikListBox* aListBox);

    private: // Data
        CAknNavigationDecorator*    iNaviDecorator;
        CStartCasesContainer*       iContainer;
        TInt                        iCurrentTestCase;
    };

#endif //STARTCASESVIEW_H

// End of File
