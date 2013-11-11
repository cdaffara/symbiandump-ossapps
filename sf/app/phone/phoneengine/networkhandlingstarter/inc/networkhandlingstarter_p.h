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
#ifndef NETWORKHANDLINGSTARTERPRIVATE_H
#define NETWORKHANDLINGSTARTERPRIVATE_H

#include <QObject>
#include "mnetworklistenerobserver.h"

class CNetworkListener;
class HbDeviceMessageBox;

/*!
    \class NetworkHandlingStarterPrivate
 */
class NetworkHandlingStarterPrivate
    : public QObject,
      public MNetworkListenerObserver
{
    Q_OBJECT
    
public:
    
    NetworkHandlingStarterPrivate(QObject *parent = NULL);
    
    ~NetworkHandlingStarterPrivate();
    
public slots:

    void LaunchCpNetworkPluginView();
    
public: // From MNetworkListenerObserver
    
    void ShowNote();
    
    void RemoveNote();

private:
    
    CNetworkListener* m_networkListener;
    HbDeviceMessageBox *m_note;
    
};

#endif // NETWORKHANDLINGSTARTERPRIVATE_H
