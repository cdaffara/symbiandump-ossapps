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
* Description:
*
*/

#ifndef LOGSSTATEBASECONTEXT_H
#define LOGSSTATEBASECONTEXT_H

//  INCLUDES
#include <e32base.h>

// FORWARD DECLARATION
class LogsStateBase;
class CLogView;
class CLogViewDuplicate;
class CLogViewEvent;
class TRequestStatus;
class CLogClient;

// CLASS DECLARATION
        
/**
 * Context base for state machines
 */
class LogsStateBaseContext {
    public:
        
        /**
         * Change state
         * @param state, new state
         */
        virtual void setCurrentState(const LogsStateBase& state) = 0;
        
        /**
         * Get log view
         * @return current log view
         */
        virtual CLogView& logView() = 0;
        
        /**
         * Get duplicates view
         * @return current duplicates view
         */
        virtual CLogViewDuplicate& duplicatesView() = 0;
        
        /**
         * Get view index
         * @return ref to view index
         */
        virtual int& index() = 0;
       
        /**
         * Request status for async operations
         * inside states
         * @return reqstatus
         */
        virtual TRequestStatus& reqStatus() = 0;
        
        /**
         * Get ID of the event to be handled
         * @return ID of the event
         */
        virtual int currentEventId() = 0;
        
        /**
         * Get log client
         * @return log client
         */
        virtual CLogClient& logClient() = 0;
        
        /**
         * Check if current view is recent view.
         * @return true if recent view
         */
        virtual bool isRecentView() = 0;
   
};


#endif      // LOGSSTATEBASECONTEXT_H


// End of File
      

        
       
