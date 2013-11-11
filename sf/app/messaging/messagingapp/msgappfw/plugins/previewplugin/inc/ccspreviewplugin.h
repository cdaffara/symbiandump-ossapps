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
* Description:  Preview Plugin main class
 *
*/


#ifndef _C_CS_PREVIEW_PLUGIN_H_
#define _C_CS_PREVIEW_PLUGIN_H_

// INCLUDE FILES

// SYSTEM INCLUDES

// USER INCLUDES
#include "ccsplugin.h"
#include "ccsdebug.h"
#include "ccsdefs.h"

// FORWARD DECLARATION
class MCsPluginEventObserver;
class CCsConversationEntry;
class CCsPreviewPluginHandler;

/**
 *  Preview plugin class
 *
 */
class CCsPreviewPlugin : public CCsPlugin
    {
public: 

    /**
     * Two phase construction
     */
    static CCsPreviewPlugin* NewL( MCsPluginEventObserver* aMCsPluginEventObserver);

    /**
     * Destructor
     */
    virtual ~CCsPreviewPlugin();


public: // From base class CCsPlugin
    /**
     *  GetConversationsL
     *  This function starts the state machine to fetch msg data from msvserver
     */        
    void GetConversationsL();

public: 
    
    /**
     *  HandleCachingCompleted
     *  Sends the cache completed status to server
     */
    void HandleCachingCompleted();

    /**
     *  HandleCachingError
     *  Sends the error occured during the caching to server
     */
    void HandleCachingError(const TInt aError);
    
private:

    /**
     * Constructor
     */
    CCsPreviewPlugin(MCsPluginEventObserver* aObserver);

    /**
     * 2nd phase construtor
     */
    void ConstructL();

private: //Data

    /**
     * MMS Preview Handler
     * Own
     */
    CCsPreviewPluginHandler* iPreviewPluginHandler;  

    /**
     * iPluginEventObserver
     * Plugin event observer
     * Not Own.
     */
    MCsPluginEventObserver* iPluginEventObserver;
    };

#endif // _C_CS_PREVIEW_PLUGIN_H_
