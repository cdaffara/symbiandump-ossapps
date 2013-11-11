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
#ifndef LOGSREMOVE_H
#define LOGSREMOVE_H

#include <QObject>
#include <e32base.h>
#include "logsworker.h"
#include "logsmodel.h"
#include "logsremovestatecontext.h"
#include "logsremoveobserver.h"

// FORWARDS DECLARATIONS
class CLogClient;
class RFs;
class CLogViewRecent;

// CLASS DECLARATION
class LogsRemoveObserver;
/**
 *	Clearing class.
 */
class LogsRemove : public LogsWorker, 
                   public LogsRemoveStateContext, 
                   public LogsRemoveObserver
    {
    public:
        friend class UT_LogsRemove;

        /**
         *  Destructor
         */
        ~LogsRemove();
        
        LogsRemove( LogsRemoveObserver& observer, bool readingAllEvents = false );
	

        /**
          * Initialize LogsRemove
          * @return 0 if initialized succesfully
          */
        int init();

        bool clearList(LogsModel::ClearType cleartype);
        
        int clearEvents(const QList<LogsEvent*>& events, bool& async);
        
    protected: // from CActive
        
        TInt RunError(TInt aError);

    private: // From LogsRemoveStateContext
        
        inline LogsRemoveObserver& observer();
        inline QList<LogsEvent>& removedEvents();
        inline int clearType();
        
    private: // From LogsRemoveObserver
        
        void removeCompleted();
        void logsRemoveErrorOccured(int err);
        
    private:
        
        void initL();
        bool clearListL(LogsModel::ClearType cleartype);
        void clearEventsL(const QList<LogsEvent*>& events, bool& async);
        void initializeClearAllL();
        void initializeIdBasedRemovalL();
        bool startClearingL();
        void removeAssociatedDuplicatesL();
        
    private: // data
        LogsRemoveObserver& mObserver;
        QList<LogsEvent> mRemovedEvents;
        QList<LogsEvent> mRemovedEventDuplicates;
     
        RFs* mFsSession;
        
        int mClearType;
        QList<LogsStateBase*> mRemoveStates;
        QList<LogsStateBase*> mClearAllStates;
        
    };

                  
#endif // LOGSREMOVE_H
