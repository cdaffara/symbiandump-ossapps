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
* Description:  Timer class for event handling delays
*
*/

#ifndef IPSPLGEVENTTIMER_H
#define IPSPLGEVENTTIMER_H

#include <msvapi.h>

struct TIpsPlgEvent
    {
    TFSMailMsgId    iAccountId;
    TFSMailEvent    iEvent;
    TAny*           iArg1;
    TAny*           iArg2;
    TAny*           iArg3;
    };

/**
* Event timer. When timer expires, it passes it's event to FS framework
*/
NONSHARABLE_CLASS( CIpsPlgEventTimer ) : public CMsvOperation
    {
    public:
        
        /**
        *
        */
        static CIpsPlgEventTimer* NewL( 
            CMsvSession& aMsvSession, 
            TInt aPriority, 
            TRequestStatus& aObserverRequestStatus,
            TIpsPlgEvent aEvent, 
            MFSMailEventObserver& aObserver );
    
        /**
        *
        */
        virtual ~CIpsPlgEventTimer();

        /**
        *
        */
        void ConstructL();

        /**
        *
        */
        virtual const TDesC8& ProgressL();

        
        /**
        *
        */
        void After(TTimeIntervalMicroSeconds32 aInterval);

        
    protected:

        /**
        * From CActive
        */
        virtual void DoCancel();

        /**
        * From CActive
        */
        virtual void RunL();

        /**
        *
        */
        CIpsPlgEventTimer(
            CMsvSession& aMsvSession,
            TInt aPriority,
            TRequestStatus& aObserverRequestStatus,
            TIpsPlgEvent aEvent,
            MFSMailEventObserver& aObserver );
            
    protected: // Data
    
        RTimer                  iTimer;
        TBuf8<1>                iProgress;
        TIpsPlgEvent            iEvent;                
        MFSMailEventObserver&   iObserver;
    };



#endif//IPSPLGEVENTTIMER_H
