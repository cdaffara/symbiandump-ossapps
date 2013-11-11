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
* Description:  AppUi class
*
*/


#ifndef __HTIADMIN_APPUI_H__
#define __HTIADMIN_APPUI_H__

// INCLUDES
#include <aknappui.h>

// FORWARD DECLARATIONS
class CHtiAdminAppView;
class CHtiCfg;

// CLASS DECLARATION

/**
* An instance of class CHtiAdminAppUi is the UserInterface part of the AVKON
* application framework for the HtiAdmin application
*/
class CHtiAdminAppUi : public CAknAppUi
    {
    public:
        /**
        * Perform the second phase construction of a CHtiAdminAppUi object.
        * This needs to be public due to the way the framework constructs the AppUi
        */
        void ConstructL();

        /**
        * Perform the first phase of two phase construction.
        * This needs to be public due to the way the framework constructs the AppUi
        */
        CHtiAdminAppUi();


        /**
        * Destroy the object and release all memory objects.
        */
        ~CHtiAdminAppUi();

        /**
        * Tries to find the HTIFramework.exe process and open a handle to it.
        * @param aPrs on return contains an open handle to the HTIFramework.exe
        * process if process found and opened successfully
        * @return ETrue if process found and opened successfully,
        * otherwise EFalse
        */
        TBool OpenHtiProcess( RProcess& aPrs );

        /**
        * Updates the version text in the display.
        */
        void UpdateVersion();

        /**
        * Updates the HTI Framework process status in the display.
        */
        void UpdateStatusL();

        /**
        * Updates the HTI auto start status in the display.
        */
        void UpdateAutoStartStatus();

        /**
        * Updates the selected communication in the display.
        */
        void UpdateSelectedComm();

        /**
        * Kills the HtiWatchDog process if it is running.
        */
        void KillHtiWatchDogL();

        /**
        * Stops the timer issuing the periodical HTI Framework process
        * status checks.
        */
        void KillTimer();

        /**
        * The method called by the periodic timer.
        * @param aPtr pointer to this CHtiAdminAppUi instance
        * @return zero if the callback function should not be called again,
        * otherwise non-zero. Current implementation returns always ETrue.
        */
        static TInt TimerCallBackL( TAny* aPtr );

    public: // from CAknAppUi
        /**
        * Handle user menu selections
        * @param aCommand the enumerated code for the option selected
        */
        void HandleCommandL( TInt aCommand );

        /**
        * Updates the HTI Framework status information and starts the
        * periodical status polling when application comes to foreground -
        * stops the polling when going to background.
        * @param aForeground ETrue if the application is in the foreground,
        * otherwise EFalse
        */
        void HandleForegroundEventL( TBool aForeground );

    public: // from MEikMenuObserver
        /**
        * Used to initialize user menu content dynamically
        * @param aResourceId the resource id of the menu pane being initialised
        * @param aMenuPane the in-memory representation of the menu pane
        */
        void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane *aMenuPane );
    
    private:
    	void ShowBTListQueryL();
    	void ShowIPListQueryL();
    	void ShowCommsListQueryL();
    	void ShowCommsDBListQueryL();
    	void HandleBtByAddress();
    	void HandleBtByName();
    	void HandleBtSearch();
    	void HandleIPConnect();
    	void HandleIPListen();
    	
    	void StartTimer();

    private:

        // The application view
        CHtiAdminAppView* iAppView;

        // Timer triggering periodic checking of HTIFramework process status
        CPeriodic*        iPeriodic;

        // For reading and writing HTI configuration files
        CHtiCfg*          iHtiCfg;
    };

#endif // __HTIADMIN_APPUI_H__

// End of File
