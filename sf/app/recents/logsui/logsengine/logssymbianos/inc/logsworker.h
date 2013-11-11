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

#ifndef LOGSWORKER_H
#define LOGSWORKER_H

//  INCLUDES
#include <QList>
#include <e32base.h>
#include "logsstatebasecontext.h"

// FORWARD DECLARATION
class CLogViewRecent;
class CLogClient;
class CLogViewEvent;
class CLogViewDuplicate;
class LogsStateBase;

// CLASS DECLARATION



/**
 * LogsReader is used to read events from database
 */
class LogsWorker : public CActive,
                   public LogsStateBaseContext
    {

        friend class UT_LogsReader;
        friend class UT_LogsDbConnector;

    public:
        
        /**
         *  Constructor
         *  @param readAllEvents, true if all events in db should be read,
         *      otherwise only recent events are read
         */
        LogsWorker( bool readAllEvents = false );
  
       /**
        *   Destructor.
        */
        virtual ~LogsWorker();
        
        virtual int lock(bool locked);

    protected: // From CActive

        void RunL();
        void DoCancel();
        TInt RunError(TInt error);
    
    protected: // From LogsStateBaseContext
        
        inline void setCurrentState(const LogsStateBase& state);
        inline CLogView& logView();
        inline CLogViewDuplicate& duplicatesView();
        inline int& index();
        inline TRequestStatus& reqStatus();
        inline int currentEventId();
        inline CLogClient& logClient();
        inline bool isRecentView();
        
    protected:

        LogsStateBase& currentState();
        
    protected: // data
        
        bool mReadAllEvents;
        
        CLogClient* mLogClient;
        CLogViewRecent* mLogViewRecent;
        CLogViewEvent* mLogViewEvent;
        CLogViewDuplicate* mDuplicatesView;

        int mIndex;
        int mCurrentStateIndex;
        QList<LogsStateBase*>* mCurrentStateMachine;
        int mCurrentEventId;
        bool mLocked;
    };

#endif      // LOGSWORKER_H


// End of File
      

        
       
