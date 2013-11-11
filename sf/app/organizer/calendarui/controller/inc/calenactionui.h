/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Calendar controller
*
*/


#ifndef CALENACTIONUI_H
#define CALENACTIONUI_H

// INCLUDES
#include <e32base.h>
#include "hb_calencommands.hrh"
#include "calencommandhandler.h"

// FORWARD DECLARATIONS
class CCalenController;
class CalenDeleteUi;
class CCalenEditUi;
class MCalenCommandHandler;

// CLASS DEFINITIONS

/**
 * The action ui handles events passed from the controller and delegates
 * them to the appropriate place (i.e. the specific action ui classes).
 */
class CCalenActionUi : public CBase,
                       public MCalenCommandHandler
    {
    public:  // Construction and destruction
        /**
         * 1st phase constructor
         * @param aController A reference to CCalenController
         */
        static CCalenActionUi* NewL( CCalenController& aController );
        
        /**
         * Virtual destructor
         */
        virtual ~CCalenActionUi();

    public: // MCalenCommandHandler
        TBool HandleCommandL( const TCalenCommand& aCommand );

    public:  // New functions
        
        /**
         * Gets a command handler or NULL.
         */
        MCalenCommandHandler* GetCommandHandlerL( TInt aCommand );
        /**
        * This Function saves the entries
        * should only calls after edit or create function has called 
        */
        void saveAndCloseEditor();

    private:  // Construction and destruction
        /**
         * C++ constructor, non-leaving
         * @param aController A reference to the CCalenController
         */
        CCalenActionUi( CCalenController& aController );
        
        /**
         * 2nd phase constructor
         */
        void ConstructL();
        
        /**
         * Launches the settings view
         */
        void launchSettingsView();
   

    private:  // Data
        CalenDeleteUi*             iDeleteUi;     // pointer to the deleteui
        CCalenEditUi*                iEditUi;         // pointer to the seditui       
        CCalenController&           iController;    // reference to the CCalenController
    };

#endif // CALENACTIONUI_H

// End of file
