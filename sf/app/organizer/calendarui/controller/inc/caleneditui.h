/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Controller class for editing related UI behavior
 *
*/


#ifndef __CALENEDITUI_H__
#define __CALENEDITUI_H__

#include <QObject>

// INCLUDES
#include "calennotificationhandler.h"   // MCalenNotificationHandler
#include "calencommandhandler.h"

// FORWARD DECLARATIONS
class CCalInstance;
class CCalenController;
class CalenEditor;


// CLASS DECLARATION

/**
 * CCalenEditUi encapsulates editing related behavior in Calendar application.
 *
 */
class CCalenEditUi : public QObject,
									public MCalenNotificationHandler,
                                    public MCalenCommandHandler
    {
    Q_OBJECT
    
    public: // public API
        /**
         * Symbian 1st phase construction
         * @param aController Reference of the controller that owns the editui
         */
        static CCalenEditUi* NewL( CCalenController& aController );
        
        /**
         * Destructor
         */
        virtual ~CCalenEditUi();

    public:  // MCalenCommandHandler
        /**
         * Handles action ui commands
         * @param aCommand Command to be handled
         */
        TBool HandleCommandL( const TCalenCommand& aCommand );
    
    public:  // from MCalenNotificationHandler
        /** 
         * Handle notifications
         * @param aNotification Nofication to be handled
         */
        void HandleNotification(const TCalenNotification aNotification );
        
        /**
        * This Function saves the entries
        * should only calls after edit or create function has called 
        */
        void saveAndCloseEditor();

    private: // own methods
        /**
         * C++ constructor
         * @param aController Reference to the controller that owns the caleneditui
         */
        CCalenEditUi( CCalenController& aController);
        
        /**
         * Symbian 2nd phase constructor
         */
        void ConstructL();
        
    private slots:
		
		void handleEntrySaved();
		void handleDialogClosed();
        
    private: // data
        
        CCalenController&       iController;
        CalenEditor*			iEditor;
    };

#endif // __CALENEDITUI_H__

// End of File
