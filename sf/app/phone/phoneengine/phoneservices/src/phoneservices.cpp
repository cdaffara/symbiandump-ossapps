/*!
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
* Description:  Phone Services manages all Phone Engine's Qt Highway services.
*
*/

#include "phoneservices.h"
#include "dialservice.h"
#include "dtmfservice.h"
#include "urischemehandlerservice.h"
#include "qtphonelog.h"

// Depricated APIs
#include "dialservicedepricated.h"
#include "dtmfservicedepricated.h"

PhoneServices::PhoneServices(
    MPECallControlIF &call, 
    MPECallSettersIF &parameters, 
    QObject* parent) 
    : 
    QObject(parent),
    dialService(0),
    dtmfService(0),
    uriSchemeHandlerService(0)
    // Depricated
    , dialServiceDepricated(0)
    , dtmfServiceDepricated(0)
{
    PHONE_DEBUG("PhoneServices::PhoneServices");
    dialService.reset(new DialService(call, parameters));
    dtmfService.reset(new DTMFService(call, parameters));
    uriSchemeHandlerService.reset(
        new UriSchemeHandlerService(call, parameters));
    
    // Depricated
    dialServiceDepricated.reset(new DialServiceDepricated(call, parameters));
    dtmfServiceDepricated.reset(new DTMFServiceDepricated(call, parameters));
}

PhoneServices::~PhoneServices()
{
}
