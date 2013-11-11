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

#ifndef PHONESERVICES_H
#define PHONESERVICES_H

#include <QObject>
#include "mpecallcontrolif.h"
#include "mpecallsettersif.h"

class DialService;
class DTMFService;
class UriSchemeHandlerService;

// Depricated APIs
class DialServiceDepricated;
class DTMFServiceDepricated;

#ifdef BUILD_PHONESERVICES
#define PHONESERVICES_EXPORT Q_DECL_EXPORT
#else
#define PHONESERVICES_EXPORT Q_DECL_IMPORT
#endif

class PHONESERVICES_EXPORT PhoneServices : public QObject
{
    Q_OBJECT
public:
    PhoneServices(
        MPECallControlIF &call, 
        MPECallSettersIF &parameters,
        QObject *parent = 0);
    ~PhoneServices();    
    
private:
    QScopedPointer<DialService> dialService;
    QScopedPointer<DTMFService> dtmfService;
    QScopedPointer<UriSchemeHandlerService> uriSchemeHandlerService;
    
    // Depricated
    QScopedPointer<DialServiceDepricated> dialServiceDepricated;
    QScopedPointer<DTMFServiceDepricated> dtmfServiceDepricated;
};

#endif // PHONESERVICES_H
