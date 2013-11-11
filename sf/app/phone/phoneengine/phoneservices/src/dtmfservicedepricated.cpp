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
#include "dtmfservicedepricated.h"
#include "qtphonelog.h"

DTMFServiceDepricated::DTMFServiceDepricated(
    MPECallControlIF &call, 
    MPECallSettersIF &parameters, 
    QObject* parent) 
    : 
    XQServiceProvider(
        QLatin1String("com.nokia.symbian.IDtmfPlay"), parent), 
    m_call(call), 
    m_parameters(parameters)
{
    publishAll();
}

DTMFServiceDepricated::~DTMFServiceDepricated()
{
}

void DTMFServiceDepricated::playDTMFTone(const QChar& keyToPlay)
{
    PHONE_DEBUG2("DTMFServiceDepricated::playDTMFTone keyToPlay:", keyToPlay);
    m_parameters.SetKeyCode(keyToPlay.unicode());
    TRAP_IGNORE( m_call.HandlePlayDTMFL() );
}

void DTMFServiceDepricated::stopDTMFPlay()
{
    PHONE_DEBUG("DTMFServiceDepricated::stopDTMFPlay");
    m_call.HandleEndDTMF(); 
}
