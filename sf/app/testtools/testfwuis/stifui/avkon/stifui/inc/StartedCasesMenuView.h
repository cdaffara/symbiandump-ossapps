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
* Description: This file contains CStartedCasesMenuView class
* declaration.
*
*/

#ifndef STARTEDCASESMENUVIEW_H
#define STARTEDCASESMENUVIEW_H

// INCLUDES
#include <eiklbo.h> // MEikListBoxObserver
#include <aknnavide.h> 
#include "View.h"


// CONSTANTS

// FORWARD DECLARATIONS
class CStartedCasesMenuContainer;

// CLASS DECLARATION

/**
*  CStartedCasesMenuView view class.
* 
*/
class CStartedCasesMenuView : public CView
    {
    public: // Constructors and destructor

        /**
        * Symbian OS default constructor.
        */
        void ConstructL();

        /**
        * Destructor.
        */
        ~CStartedCasesMenuView();

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

    private: // Data
        CAknNavigationDecorator*    iNaviDecorator;
        CStartedCasesMenuContainer* iContainer;
    };

#endif

// End of File
