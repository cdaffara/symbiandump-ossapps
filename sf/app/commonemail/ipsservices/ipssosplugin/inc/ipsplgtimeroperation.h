/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Timer operation class
*
*/

#ifndef IPSPLGTIMEROPERATION_H
#define IPSPLGTIMEROPERATION_H

class MIpsPlgTimerOperationCallBack
    {
public:
    virtual void HandleTimerFiredL( const TFSMailMsgId& aMailboxId ) = 0;
    };

/**
* Simple Timer Operation. Completes observer when time expires.
*/
NONSHARABLE_CLASS ( CIpsPlgTimerOperation ) : public CActive
    {
    public:
        /**
        *
        */
        static CIpsPlgTimerOperation* NewL(
            TFSMailMsgId aMailboxId,
            MIpsPlgTimerOperationCallBack& aObserver );

        /**
        *
        */
        virtual ~CIpsPlgTimerOperation();


        /**
        *
        */
        void After(TTimeIntervalMicroSeconds32 aInterval);

        /**
        *
        */
        void ResetTimerOperation();
        
        /**
        *
        */
        TFSMailMsgId FSMailboxId() const;
        
    protected:


        CIpsPlgTimerOperation(
            TFSMailMsgId aMailboxId,
            MIpsPlgTimerOperationCallBack& aObserver );

        /**
        *
        */
        void ConstructL();

        /**
        * From CActive
        */
        virtual void DoCancel();

        /**
        * From CActive
        */
        virtual void RunL();

    protected: // Data
        RTimer iTimer;
        TFSMailMsgId iMailboxId;
        MIpsPlgTimerOperationCallBack& iObserver;
        // true if timer have to reactivated
        TBool iStateReActivate;
    };



#endif
