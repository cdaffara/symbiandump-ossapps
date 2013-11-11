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
*               THIS IS DEPRICATED API - DO NOT USE!
*               (use DTMFService instead)
*
*/

#ifndef DTMFSERVICEDEPRICATED_H
#define DTMFSERVICEDEPRICATED_H

#include <QObject>
#include <QString>
#include <xqserviceprovider.h>
#include "mpecallcontrolif.h"
#include "mpecallsettersif.h"

class DTMFServiceDepricated : public XQServiceProvider
{
    Q_OBJECT
public:
    DTMFServiceDepricated(
        MPECallControlIF &call, 
        MPECallSettersIF &parameters, 
        QObject *parent = 0);
    ~DTMFServiceDepricated();
    
public slots:
    /*!
    deprecated
    */
    void playDTMFTone(const QChar& keyToPlay);

    /*!
    deprecated
    */
    void stopDTMFPlay();
    
    
private:
    MPECallControlIF &m_call;
    MPECallSettersIF &m_parameters;
};

#endif // DTMFSERVICE_H
