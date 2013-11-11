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
* Description: Timeout timer.
*
*/
#ifndef PROCESSTIMER_H_
#define PROCESSTIMER_H_

#include <e32base.h>
#include <DebugLogMacros.h>

/**
 * Timeout observer interface.
 */
class MObserverPreInstall
    {
public:
    virtual void Timeout() = 0;
    };

/**
 * Timeout timer implementation.
 */
class CProcessTimer : public CTimer 
    {
        
public :
    static CProcessTimer* NewL( MObserverPreInstall& aObserver );
    ~CProcessTimer();
    
    void StartTimeoutTimer( TInt aTimerValue );
    
protected :
    void RunL();
    
private :
    void ConstructL();
    CProcessTimer( MObserverPreInstall& aObserver );
    
private:    
    MObserverPreInstall& iObserver;
    __LOG_DECLARATION
    };

#endif /*PROCESSTIMER_H_*/
