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

#ifndef LOGSREMOVESTATES_H
#define LOGSREMOVESTATES_H

//  INCLUDES
#include <QList>
#include "logsstatebase.h"

class LogsRemoveStateContext;

/**
 * Base for remove states
 */
class LogsRemoveStateBase : public LogsStateBase {

    friend class UT_LogsRemoveStates;
    
    public:
        LogsRemoveStateBase(LogsStateBaseContext& context, LogsRemoveStateContext& removeContext);
        virtual ~LogsRemoveStateBase(){}

    protected:
        LogsRemoveStateContext& mContext;
};

/**
 * State for clearing all events or specific event types from main db view
 */
class LogsRemoveStateClearAll : public LogsRemoveStateBase {

    friend class UT_LogsRemoveStates;
    
    public:
        LogsRemoveStateClearAll(
            LogsStateBaseContext& context, LogsRemoveStateContext& removeContext) : 
                LogsRemoveStateBase(context, removeContext) {}
        virtual ~LogsRemoveStateClearAll(){}
    
    public: // From LogsReaderStateBase
        virtual bool enterL();
        virtual bool continueL();
};

/**
 * State for initializing removal from recent db view
 */
class LogsRemoveStateInit : public LogsRemoveStateBase {

    friend class UT_LogsRemoveStates;
    
    public:
        LogsRemoveStateInit(
            LogsStateBaseContext& context, LogsRemoveStateContext& removeContext) : 
                LogsRemoveStateBase(context, removeContext) {}
        virtual ~LogsRemoveStateInit(){}
    
    public: // From LogsReaderStateBase
        virtual bool enterL();
        virtual bool continueL();
};

/**
 * State for event id based deletion of events from main db view
 */
class LogsRemoveStateDelete : public LogsRemoveStateBase {

    friend class UT_LogsRemoveStates;
    
    public:
        LogsRemoveStateDelete(
            LogsStateBaseContext& context, LogsRemoveStateContext& removeContext) : 
                LogsRemoveStateBase(context, removeContext), mRemoveIndex(0) {}
        virtual ~LogsRemoveStateDelete(){}
    
    public: // From LogsReaderStateBase
        virtual bool enterL();
        virtual bool continueL();
    protected:
        bool deleteNextEvent();
    protected:
        int mRemoveIndex;
};

/**
 * State for deleting duplicate events from recent db view
 */
class LogsRemoveStateDeleteDuplicates : public LogsRemoveStateBase {

    friend class UT_LogsRemoveStates;
    
    public:
        LogsRemoveStateDeleteDuplicates(
            LogsStateBaseContext& context, LogsRemoveStateContext& removeContext) : 
                LogsRemoveStateBase(context, removeContext) {}
        virtual ~LogsRemoveStateDeleteDuplicates(){}
    
    public: // From LogsReaderStateBase
        virtual bool enterL();
        virtual bool continueL();
    protected:
            bool deleteNextEvent();   
    protected:
        bool mDeleting;
        QList<int> mDeleteDuplicateIds;
};

/**
 * Completion state
 */
class LogsRemoveStateDone : public LogsRemoveStateBase {

    friend class UT_LogsRemoveStates;
    
    public:
        LogsRemoveStateDone(
            LogsStateBaseContext& context, LogsRemoveStateContext& removeContext) : 
                LogsRemoveStateBase(context, removeContext) {}
        virtual ~LogsRemoveStateDone(){}
    
    public: // From LogsReaderStateBase
        virtual bool enterL();
};



#endif      // LOGSREMOVESTATES_H


// End of File
      

        
       
