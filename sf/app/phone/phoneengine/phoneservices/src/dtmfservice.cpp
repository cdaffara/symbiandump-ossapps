/*!
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  DTMF API to be used through Qt Highway.
*
*/

#include <xqserviceutil.h>
#include <e32base.h>
#include "dtmfservice.h"
#include "qtphonelog.h"

DTMFService::DTMFService(
    MPECallControlIF &call, 
    MPECallSettersIF &parameters, 
    QObject* parent) 
    : 
    XQServiceProvider(
        QLatin1String("phoneui.com.nokia.symbian.IDtmfPlay"), parent), 
    m_call(call), 
    m_parameters(parameters)
{
    publishAll();
}

DTMFService::~DTMFService()
{
}

int DTMFService::playDTMFTone(const QChar& keyToPlay)
{
    PHONE_TRACE2("keyToPlay:", keyToPlay);
    if (!hasCapability()) {
        return KErrPermissionDenied;
    }
    m_parameters.SetKeyCode(keyToPlay.unicode());
    TRAPD(ret, m_call.HandlePlayDTMFL() );
    return ret;
}

int DTMFService::stopDTMFPlay()
{
    PHONE_TRACE;
    if (!hasCapability()) {
        return KErrPermissionDenied;
    }
    return m_call.HandleEndDTMF(); 
}

bool DTMFService::hasCapability()
{
    XQRequestInfo req = requestInfo();
    QSet<int> caps = req.clientCapabilities();
    if (!caps.contains(ECapabilityNetworkServices)) {
        PHONE_TRACE1("NetworkServices capability missing");
        return false;
	}
	return true;
}

