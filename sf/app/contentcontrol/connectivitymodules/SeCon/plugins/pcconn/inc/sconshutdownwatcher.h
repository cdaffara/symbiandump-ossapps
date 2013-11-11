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
* Description:  Power shutdown -watcher header file
*
*/


#ifndef _SCONSHUTDOWNWATCHER_H
#define _SCONSHUTDOWNWATCHER_H


#include <e32base.h>
#include <e32property.h>

//============================================================
// Class MShutdownObserver declaration
//============================================================
class MShutdownObserver
    {
public:
    virtual void NotifyShutdown()=0;
    };

//============================================================
// Class CShutdownWatcher declaration
//============================================================
NONSHARABLE_CLASS (  CShutdownWatcher ) : public CActive
    {
    public:
        static CShutdownWatcher* NewL( MShutdownObserver* aObserver );
        ~CShutdownWatcher();

        /**
         * Starts to watch shutdown status
         * @return none
         */
        void StartShutdownWatcher();

    private:
        //construct/destruct
        CShutdownWatcher( MShutdownObserver* aObserver );
        void ConstructL();

        // from CActive
        void DoCancel();
        void RunL();

    private:
        MShutdownObserver*   iObserver; 
        RProperty            iProperty;
    };

#endif //_SCONSHUTDOWNWATCHER_H

// End of file