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

#ifndef LOGSSTATEBASE_H
#define LOGSSTATEBASE_H

//  INCLUDES
#include <e32std.h>

// FORWARD DECLARATION
class LogsStateBaseContext;
class CLogViewEvent;
class CLogFilterList;
class CLogFilter;
class CLogEvent;

// CLASS DECLARATION

/**
 * Reader state base
 */
class LogsStateBase {

    friend class UT_LogsReaderStates;
    friend class UT_LogsReader;
    
    public:
        
        /**
         * Destructor
         */
        virtual ~LogsStateBase();
        
        /**
         * Set next state to be used once this state has completed.
         * @param nextState
         */
        void setNextState(LogsStateBase& nextState);

        /**
         * Enter to the state, may proceed immediately to next state.
         * @return true if entering started async operation,  false if not
         */
        virtual bool enterL();
        
        /**
         * Continue running in the state, may proceed to next state
         * @return true if continue started async operation,  false if not
         */
        virtual bool continueL();
    
    protected:
        
        /**
         * Constructor
         */
        LogsStateBase(LogsStateBaseContext& context);
        
        /**
         * Proceed to next state if such exists.
         * @return true if entering started async operation,  false if not 
         */
        virtual bool enterNextStateL();
        
        /**
         * Get number of events in view
         * @return view count
         */
        int viewCountL() const;
        
        /**
         * Get current event
         * @return event
         */
        CLogEvent& event() const;
        
        /**
         * Try to search duplicates for current event in the view
         * @param aFilter, filter to be used for searching duplicates
         * @return true, if duplicates are searched
         */
        bool duplicatesL(const CLogFilter* aFilter = 0);

    protected:   
        LogsStateBaseContext& mBaseContext;
        LogsStateBase* mNextState;
        int mStateIndex;
};



/**
 * Searching event state
 */
class LogsStateSearchingEvent : public LogsStateBase 
{
    friend class UT_LogsReaderStates;
    
    public:
    LogsStateSearchingEvent(LogsStateBaseContext& context);
        virtual ~LogsStateSearchingEvent(){}

    public: // From LogsReaderStateBase
        virtual bool enterL();
        virtual bool continueL();
};


#endif      // LOGSSTATEBASE_H


// End of File
      

        
       
