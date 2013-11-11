/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: CProgressNotifier is used to notify about 
* the current state of the request
*
*/

#ifndef __PROGRESSNOTIFIER_H__
#define __PROGRESSNOTIFIER_H__

// INCLUDES
#include <in_sock.h>

// FORWARD DECLARATIONS
class MProgressNotifyHandler;

/**
* Class for receiving notifications from interfaces.
* Notifications are handled by the class, that implements
* MProgressNotifyHandler interface.
*/
class CProgressNotifier : public CActive
{
public: // new methods
    
    /**
    * Two-phased constructor.
    */
    static CProgressNotifier* NewL(RConnection& aConnection, MProgressNotifyHandler& aHandler);
    
    /**
    * Two-phased constructor.
    */
    static CProgressNotifier* NewLC(RConnection& aConnection, MProgressNotifyHandler& aHandler);
    
    /**
    * Destroy the object and release all memory objects
    */
    ~CProgressNotifier();
    
    /**
    * Start waiting for the progress notifications.
    */
    void StartNotify();

    /**
    * Reads the current stage of the interface when 
    * the progress notification is received, and issues
    * a new progress notification request.
    */
    void NotifyL();
    
protected: // from CActive

    /**
    * Called when operation complete
    */
    void RunL();

    /**
    * Cancel any outstanding operation
    */
    void DoCancel();
    
private: // New methods
    
    /**
    * Perform the first phase of two phase construction 
    */
    CProgressNotifier(RConnection& aConnection, MProgressNotifyHandler& aHandler);
    
    /**
    * Perform the second phase construction of two phase construction
    */
    void ConstructL();
    
private:
    // Member variables
    RConnection&                iConnection;
    MProgressNotifyHandler&     iHandler;
    TNifProgressBuf             iProgressBuf;
};

#endif // __PROGRESSNOTIFIER_H__

