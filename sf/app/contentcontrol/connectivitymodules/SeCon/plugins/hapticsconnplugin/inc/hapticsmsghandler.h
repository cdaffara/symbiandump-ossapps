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
* Description:  Handles messaging between file system and haptics 
*                client interface.
*
*/


#ifndef C_HAPTICSMSGHANDLER_H
#define C_HAPTICSMSGHANDLER_H

#include <e32base.h>
#include <e32debug.h>
#include <f32file.h>

#include "hapticsbridgeclient.h"

class CHapticsConnTimer;

/**
 * Interface to notify about connection timeout.
 */
class MHapticsConnectionShutdown
    {
    public:
        /**
         * Called to notify about connection timeout.
         */
        virtual void NotifyShutdown() = 0;
    
    };

/**
 * Helper class for Bridge functionality for handling RFs (File Session)
 * change notifications for VibeTonz request file.
 *
 * @since S60 5.1
 */
class CHapticsMsgHandler : public CActive,
                           public MHapticsConnectionShutdown
    {        
public:    
    /**
     * 2-phased constructor.
     */
    static CHapticsMsgHandler* NewL();
    
    /**
     * Destructor
     */
    virtual ~CHapticsMsgHandler();

    /**
     * Method for starting the listening for change notifications    
     */
    void StartNotifier();
        
    /**
     * From MHapticsConnectionShutdown.
     * Called to notify about connection timeout.
     */        
    void NotifyShutdown();   
    
protected: // From CActive.    
    /**
     * @see CActive
     */
    void RunL();
    /**
     * @see CActive
     */    
    TInt RunError(TInt aError);
    /**
     * @see CActive
     */    
    void DoCancel();
        
private:

    /**
     * Constructor
     */
    CHapticsMsgHandler();

    /**
     * Two-phase construction ConstructL
     */
    void ConstructL();
    
    /**
     * Method for connecting to file system. Also sets the paths and
     * creates directories (if not already exists).
     */
    void ConnectToFilesystemL();            	
 
    /**
     * Helper for clearing the possible left-over request file from
     * previous runs.
     */
    void ClearRequestFile();

private: // data
    
    /**
     * File server session through which all file server related 
     * services are requested.
     */
    RFs iFs;
    
    /**
     * Buffer for the request file data
     */
    HBufC8* iReqBuf;    

    /**
     * The RHapticsBridgeClient object that is responsible for further
     * handling of received Bridge commands
     */
    RHapticsBridgeClient iClient;
    
    CHapticsConnTimer* iConnectionTimeout;
        
  };

#endif // C_HAPTICSMSGHANDLER_H
