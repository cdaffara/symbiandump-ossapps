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
* Description: This file contains CView class declaration.
*
*/

#ifndef VIEW_H
#define VIEW_H

// INCLUDES
#include <aknview.h>

//#include "UIStoreIf.h" 
#include <stifinternal/UIStoreIf.h>

// CONSTANTS

// FORWARD DECLARATIONS
//class CMainMenuContainer;

// CLASS DECLARATION

/**
*  CMainMenuView view class.
* 
*/
class CView : public CAknView
    {
    public: // Constructors and destructor

        /**
        * Symbian OS default constructor.
        */
        void ConstructL();

        /**
        * Destructor.
        */
        ~CView();

    public: // Functions from base classes
        
        /**
        * Returns view´s id.
        * @return View id.
        */
        virtual TUid Id() const;

        /**
        * Handles a command.
        * @param aCommand A command ID.
        */
        virtual void HandleCommandL( TInt aCommand );

        /**
        * Handles client rect changes.
        */
        virtual void HandleClientRectChange();
        
    private:

        /**
        * Initializes view when activated.
        * @param aPrevViewId Id of the previous view (not used).
        * @param aCustomMessageId Custom message id (not used).
        * @param aCustomMessage Custom message (not used).
        */
        virtual void DoActivateL(const TVwsViewId& aPrevViewId,TUid aCustomMessageId,
            const TDesC8& aCustomMessage);

        /**
        * Deactivates view.
        */
        virtual void DoDeactivate();

    protected: // Data
        
        /**
        * Pointer to UI Store.
        */
        CUIStore*     iUIStore;
		
		
    private: // Data

    };

#endif // VIEW_H

// End of File
