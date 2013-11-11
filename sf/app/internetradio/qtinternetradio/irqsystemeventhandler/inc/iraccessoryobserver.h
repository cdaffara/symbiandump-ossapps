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

#ifndef IRACCESSORYOBSERVER_H
#define IRACCESSORYOBSERVER_H

#include <AccessoryServer.h>
#include <AccessoryMode.h>
#include <AccPolAccessoryMode.h>

class MIRHeadsetObserverInterface;

class CIRAccessoryObserver : public CActive
{
public:

    static CIRAccessoryObserver* NewL(MIRHeadsetObserverInterface *aObserver);
    ~CIRAccessoryObserver();   
    TBool IsHeadsetConnected() const;    

private:
    CIRAccessoryObserver(MIRHeadsetObserverInterface *aObserver);
    void ConstructL();
    void  RunL();
    void  DoCancel();

private:
    MIRHeadsetObserverInterface* iObserver;
    RAccessoryServer iAccessoryServer;
    RAccessoryMode iAccessoryModeSession;
    TAccPolAccessoryMode iAccPolAccessoryMode;
};

#endif//IRACCESSORYOBSERVER_H
