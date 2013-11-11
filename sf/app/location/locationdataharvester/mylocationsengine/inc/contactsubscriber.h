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
 * Description: Subscribe notification .
 *
 */
#ifndef CONTACTSUBSCRIBER_H
#define CONTACTSUBSCRIBER_H

// INCLUDES

#include <e32base.h>
#include <e32property.h>
#include "notification.h" 
class CContactSubscriber : public CActive
{
public:
    /** 
     * CContactSubscriber::NewL()
     */
    static CContactSubscriber* NewL(MNotifyChange* aNotifyChange);

    /** 
     * CContactSubscriber::NewL()
     */
    static CContactSubscriber* NewLC(MNotifyChange* aNotifyChange);
    /**
     * ~CContactSubscriber.
     * Virtual Destructor.
     */
    virtual ~ CContactSubscriber();

private:
    /**
     * ConstructL.
     * 2nd phase constructor.
     */
    void ConstructL();
    /**
     * CMyLocationsEngine.
     * C++ default constructor. 
     */
    CContactSubscriber(MNotifyChange* aNotifyChange);
#ifdef MYLOCATIONENGINE_UNIT_TEST
public:
#else
private:
#endif
    
    /**
     start subscribe for contact entry
     */
    void SubscribeChangeNotiFication();

    /**
     *  Handles active object's request completion event. 
     */
    void RunL();
    /** 
     * Implements cancellation of an outstanding request. 
     */
    void DoCancel();
private:   
    //observer to provide notification on completion of event
    MNotifyChange& iNotifyChange;
    RProperty iProperty;
};
#endif // CONTACTSUBSCRIBER_H
