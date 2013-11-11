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
#ifndef NETWORKHANDLINGSTARTER_H
#define NETWORKHANDLINGSTARTER_H

#include <xqserviceprovider.h>

#ifdef BUILD_NETWORKHANDLINGSTARTER
#define NETWORKHANDLINGSTARTER_EXPORT Q_DECL_EXPORT
#else
#define NETWORKHANDLINGSTARTER_EXPORT Q_DECL_IMPORT
#endif

class NetworkHandlingStarterPrivate;

/*!
    \class NetworkStatusObserver
 */
class NETWORKHANDLINGSTARTER_EXPORT NetworkHandlingStarter
    : public XQServiceProvider
{
    Q_OBJECT
    
public:
    
    NetworkHandlingStarter(QObject *parent = 0);
    
    ~NetworkHandlingStarter();
    
public slots:
    /*!
        \fn start()
        
        This method start networkhandling. It is intended to be used via Qt Highway.
        
        Usage example:
        XQServiceRequest snd("com.nokia.services.networkhandling","start()");
    */
    void start();
    
private:
    
    NetworkHandlingStarterPrivate* m_privateImpl;
    
};

#endif // NETWORKHANDLINGSTARTER_H
