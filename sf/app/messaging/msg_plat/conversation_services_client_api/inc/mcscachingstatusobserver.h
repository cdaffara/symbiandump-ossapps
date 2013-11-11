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
* Description:  The observer interface which handles the cache 
 *                status event from the server.
 *
*/


#ifndef __M_CS_CACHING_STATUS_OBSERVER_H__
#define __M_CS_CACHING_STATUS_OBSERVER_H__

// INCLUDE FILES

// SYSTEM INCLUDES
#include <ccsdefs.h>

// USER INCLUDES

// FORWARD DECLARATION

// CLASS DECLARATION
/**
 *  MCsCachingStatusObserver
 *  Mixin class.
 *  Observer which handles cache status events from the conversation server.
 *
 *  @remarks Lib: CsServerClientAPI.lib
 *  @since S60 v5.0
 */
class MCsCachingStatusObserver
    {
public:
    /**  
     * CachingStarted
     * This is for notifying caching started event 
     * asynchronusly from the server 
     */
    virtual void CachingStarted() = 0;

    /**  
     * CachingCompleted
     * This is for notifying caching completed event 
     * asynchronusly from the server 
     */
    virtual void CachingCompleted() = 0;

    /**  
     * CachingError
     * This is for notifying caching error event 
     * asynchronusly from the server 
     * 
     * @param aError  Caching error
     */
    virtual void CachingError(const TInt aError) = 0;
    };

#endif // __M_CS_CACHING_STATUS_OBSERVER_H__

// End of File
