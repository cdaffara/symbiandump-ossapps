/*!
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
* Description:  Network handling.
*
*/

#ifndef PENETWORKHANDLER_H
#define PENETWORKHANDLER_H

#include <QObject>
#include <qsysteminfo.h>
#include <networkhandlingdomainpskeys.h>
#include "mpephonemodelinternal.h"

QTM_USE_NAMESPACE

class PeNetworkHandler : public QObject
{
    Q_OBJECT
    
public:

    /*!
       Constructor
     */
    PeNetworkHandler(MPEPhoneModelInternal& aModel);
    
    /*!
       Destructor
     */
    virtual ~PeNetworkHandler ();
    
    
public slots:

    /*!
        \fn void networkStatusChanged()
    
        Slot is called when network status is changed.
    */
    void networkStatusChanged(
            QSystemNetworkInfo::NetworkMode mode, 
            QSystemNetworkInfo::NetworkStatus status);
    
private:
    
    /*!
        \fn void sendStatus()
    
        Sends new status.
    */
    void sendStatus(QSystemNetworkInfo::NetworkStatus status);
        
    /*!
        \fn void convertStatus ()
    
        Converts QSystemNetworkInfo::NetworkStatus to 
        TNWNetworkRegistrationStatus.
    */
    TNWNetworkRegistrationStatus convertStatus(
            QSystemNetworkInfo::NetworkStatus status);
    

private:

    // Reference to phone model object
    MPEPhoneModelInternal& iModel;
        
    QSystemNetworkInfo* m_networkInfo;
    
};

#endif // PENETWORKHANDLER_H
