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
* Description: This file contains CStatisticsView class declaration.
*
*/

#ifndef STATISTICSVIEW_H
#define STATISTICSVIEW_H

// INCLUDES
#include "View.h"
#include <eiklbo.h> // MEikListBoxObserver

// CONSTANTS

// FORWARD DECLARATIONS
class CStatisticsContainer;

// CLASS DECLARATION

/**
*  CTestCaseMenuView view class.
* 
*/
class CStatisticsView : public CView, public MEikListBoxObserver //CAknView
    {
    public: // Constructors and destructor

        /**
        * Symbian OS default constructor.
        */
        void ConstructL();

        /**
        * Destructor.
        */
        ~CStatisticsView();

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
        void StartTestCasesL();

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
        CStatisticsContainer*       iContainer;
        TInt                        iCurrentTestCase;
    };

#endif //STATISTICSVIEW_H

// End of File
