/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  A helper object to open messagin entries (e.g. emails).
*
*/

#ifndef AIS_MSV_UI_H
#define AIS_MSV_UI_H


// INCLUDES

#include <e32base.h>
#include <msvapi.h>


// FORWARD DECLARATIONS

class CBaseMtm;
class CBaseMtmUi;
class CClientMtmRegistry;
class CMsvOperation;
class CMsvSession;
class CMtmUiRegistry;


// CLASS DECLARATIONS

/**
* Interface for opening messaging entries (e.g. emails).
*/
class MMsvUI
    {
    public:     // New functions
        
        /**
        * Opens the messaging entry.
        * 
        * @param   aMsvId   TMsvId of the entry to open, as TUid
        */
        virtual void OpenEntryL( const TUid& aMsvId ) = 0;
    };

/**
* A helper object to open messaging entries (e.g. emails).
* 
* MTM UI, which is used to open emails (at least), is asyncronous API.  This 
* is a helper class that is active while the UI is open.
*/
class CMsvUI : public CActive,
               public MMsvUI,
               public MMsvSessionObserver
    {
    public:     // Constructor and destructor
        
        /**
        * C++ constructor.
        */
        CMsvUI();
        
        /**
        * Destructor
        */
        virtual ~CMsvUI();
        
    public:     // Functions from base classes
        
        /**
        * From MMsvUI.
        * 
        * Opens the messaging entry.
        * 
        * @param   aMsvId   TMsvId of the entry to open, as TUid
        */
        void OpenEntryL( const TUid& aMsvId  );
        
    private:    // Functions from base classes
        
        /**
        * From CActive.
        * 
        * Handles an active object's request completion event.
        * 
        * The function is called by the active scheduler when a request 
        * completion event occurs, i.e. after the active scheduler's 
        * WaitForAnyRequest() function completes.
        * 
        * Before calling this active object's RunL() function, the active 
        * scheduler has:
        * 
        * 1. decided that this is the highest priority active object with a 
        *    completed request
        * 
        * 2. marked this active object's request as complete (i.e. the 
        *    request is no longer outstanding)
        * 
        * RunL() runs under a trap harness in the active scheduler. If it 
        * leaves, then the active scheduler calls RunError() to handle the 
        * leave.
        * 
        * Note that once the active scheduler's Start() function has been 
        * called, all user code is run under one of the program's active 
        * object's RunL() or RunError() functions.
        * 
        * @see CActiveScheduler::Start 
        * @see CActiveScheduler::Error 
        * @see CActiveScheduler::WaitForAnyRequest 
        * @see TRAPD
        */
        void RunL();
        
        /**
        * From CActive.
        * 
        * Implements cancellation of an outstanding request.
        * 
        * This function is called as part of the active object's Cancel().
        * 
        * It must call the appropriate cancel function offered by the active 
        * object's asynchronous service provider. The asynchronous service 
        * provider's cancel is expected to act immediately.
        * 
        * DoCancel() must not wait for event completion; this is handled by 
        * Cancel().
        * 
        * @see CActive::Cancel
        */
        void DoCancel();
        
        /**
        * From MMsvSessionObserver.
        * 
        * Indicates an event has occurred.
        * 
        * The type of event is indicated by the value of aEvent. The 
        * interpretation of the TAny arguments depends on this type.
        *
        * For most event types, the action that is taken, for example, 
        * updating the display, is client-specific. All clients though 
        * should respond to EMsvCloseSession and EMsvServerTerminated events.
        * 
        * @param   aEvent   Indicates the event type.
        * @param   aArg1    Event type-specific argument value.
        * @param   aArg2    Event type-specific argument value.
        * @param   aArg3    Event type-specific argument value.
        */
        void  HandleSessionEventL( TMsvSessionEvent aEvent,
                                   TAny* aArg1,
                                   TAny* aArg2,
                                   TAny* aArg3 );
        
    private:    // Data
        // Represents a channel of communication between message client 
        // application and the Message Server thread
        CMsvSession* iSession;
        // Accesses the Client-side MTM registry.
        CClientMtmRegistry* iMtmReg;
        // Accesses the User Interface MTM registry.
        CMtmUiRegistry* iUIReg;
        // Provides a high-level interface for accessing a  Message Server 
        // entry
        CBaseMtm* iMtm;
        // Provides MTM-specific user interaction such as viewing of message 
        // entries
        CBaseMtmUi* iUI;
        // Represents an ongoing action in the UI.
        CMsvOperation* iOperation;
    };

#endif // AIS_MSV_UI_H

// End of File
