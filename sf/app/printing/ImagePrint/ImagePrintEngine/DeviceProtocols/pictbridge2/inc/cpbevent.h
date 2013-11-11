/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Contains the CPBConnect class definition.
*
*/


#ifndef CPBEVENT_H
#define CPBEVENT_H

#include <pictbridge.h>
#include <dpsoperation.h>

class MPBObserver;


class CPBEvent : public CActive
	{
public:
    static CPBEvent* NewL( CDpsEngine& aEngine, MPBObserver& aObserver );
    CPBEvent( CDpsEngine& aEngine, MPBObserver& aObserver );
    ~CPBEvent();    
    void ConstructL();
    void Listen();
        
protected: // from CActive
    void RunL();
    void DoCancel();
    
private:        
    CDpsEngine& iDpsEngine;
    MPBObserver& iObserver;
    TDpsEvents iEvent;
	};

#endif // CPBEVENT_H

//  End of File
