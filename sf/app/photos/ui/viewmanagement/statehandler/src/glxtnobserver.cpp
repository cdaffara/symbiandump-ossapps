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
* Description:   ?Description
*
*/

#include <glxtnobserver.h>
#include <e32property.h>

#include "glxtracer.h"

const TUid KTAGDPSNotification = { 0x2001FD51 };
const TInt KForceBackgroundGeneration = 0x00000010;
const TInt KItemsleft = 0x00000008;

GlxTNObserver::GlxTNObserver() : mTNMonitor( NULL )
{
    TRACER("GlxTNObserver::GlxTNObserver() ");
    RProperty::Set( KTAGDPSNotification, KForceBackgroundGeneration, ETrue );
}

void GlxTNObserver::updateTNCount(int &count)
{
    TRACER("GlxTNObserver::updateTNCount() ");
    emit leftTNCount( count );
}

int GlxTNObserver::getTNLeftCount()
{
    TRACER("GlxTNObserver::getTNLeftCount() ");
    TInt leftVariable =  0;
    RProperty::Get( KTAGDPSNotification, KItemsleft, leftVariable );
    //To:Do error handling
    return leftVariable;
}

void GlxTNObserver::startTNObserving()
{
    TRACER("GlxTNObserver::startTNObserving() ");
    mTNMonitor = CGlxTNMonitor::NewL( this );
}

void GlxTNObserver::startTNMDaemon()
{
    TRACER("GlxTNObserver::startTNMDaemon() ");
    RProperty::Set( KTAGDPSNotification, KForceBackgroundGeneration, ETrue );
}

void GlxTNObserver::stopTNMDaemon()
{
    TRACER("GlxTNObserver::stopTNMDaemon() ");
    RProperty::Set( KTAGDPSNotification, KForceBackgroundGeneration, EFalse );
}

GlxTNObserver::~GlxTNObserver()
{
    TRACER("GlxTNObserver::~GlxTNObserver() ");
    RProperty::Set( KTAGDPSNotification, KForceBackgroundGeneration, EFalse );
    delete mTNMonitor;
}
