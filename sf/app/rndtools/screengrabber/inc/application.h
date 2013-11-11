/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef SGAPPLICATION_H
#define SGAPPLICATION_H

#include <e32keys.h>
#include <w32std.h>

#include <hbapplication.h>
class EngineWrapper;

class SGApplication : public HbApplication
{
public:
    SGApplication(int argc, char *argv[]);//, EngineWrapper &engine);
    ~SGApplication();
#if defined(Q_WS_S60)

#if defined(HB_QT_S60_EVENT_FILTER)
    virtual bool s60EventFilter(TWsEvent *aEvent);
#else
    virtual bool symbianEventFilter(const QSymbianEvent *event);
#endif // (HB_QT_S60_EVENT_FILTER)
#endif//(Q_WS_S60)    
    inline void SetEngineWrapper(EngineWrapper *aEngine){iEngine = aEngine;}
private:
	RWsSession    		iWsSession_a; 
    RWindowGroup        iRootWin_a;  


    EngineWrapper *iEngine;
};

#endif // SGAPPLICATION_H
