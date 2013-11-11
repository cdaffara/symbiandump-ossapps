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
* Description: This file contains CMenuListBox class declaration.
*
*/

#ifndef MENULISTBOX_H_
#define MENULISTBOX_H_

// INCLUDES
#include <eiktxlbx.h>
#include "Stifui.hrh"
#include "AppUIAppUi.h"
#include <aknview.h>
#include <aknlists.h>

// CONSTANTS

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  CMenuListBox class. Has ability to store and retrieve previous focus position
*/
class CMenuListBox : public CAknSingleStyleListBox
    {
    public: // Constructors and destructor

        static CMenuListBox* NewL(TAppUIViewNumber aViewId);
        
        /**
        * Destructor.
        */
        virtual ~CMenuListBox();
        
        /**
         * Sets focus on last chosen position
         */
        void SetPreviousFocus();
        
        /**
         * Saves position of focus to CAppUiAppUi object
         */
        void SaveFocusPosition();
        
    private: // Constructors and destructor
    
    	CMenuListBox(TAppUIViewNumber aViewId);
    	
    	/**
        * Symbian OS default constructor.
        */
        void ConstructL();

    public: // Functions from base classes

    private:

    private: // Data
    	const TAppUIViewNumber	iViewId;
    };

#endif /*MENULISTBOX_H_*/

// End of File
