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

#ifndef LOGSREMOVESTATECONTEXT_H
#define LOGSREMOVESTATECONTEXT_H

//  INCLUDES
#include <QList>

// FORWARD DECLARATION
class LogsRemoveObserver;
class LogsEvent;

// CLASS DECLARATION

/**
 * Context for remove state machine
 */
class LogsRemoveStateContext {
    public:

        /**
         * Get observer
         * @return observer
         */
        virtual LogsRemoveObserver& observer() = 0;
        
        /**
         * Get ids of removed events
         * @return ids of removed events
         */
        virtual QList<LogsEvent>& removedEvents() = 0;
        
        virtual int clearType() = 0;
        
};


#endif      // LOGSREADERSTATECONTEXT_H


// End of File
      

        
       
