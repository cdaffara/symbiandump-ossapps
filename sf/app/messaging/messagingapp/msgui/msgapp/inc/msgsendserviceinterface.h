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
 * Description: Message Application service interface used for interfacing between
 *              QT highway and other applications
 *
 */

#ifndef MSGSENDSERVICEINTERFACE_H_
#define MSGSENDSERVICEINTERFACE_H_

// INCLUDES
#include <xqserviceprovider.h>
#include <qvariant.h>

class MsgViewManager;
class MsgSendServiceInterface : public XQServiceProvider
    {
    Q_OBJECT
    
public:
    /*
     * Constructor
     */
    MsgSendServiceInterface(QObject* parent=0, MsgViewManager* manager =0);
    
    /*
     * Destructor
     */
    ~MsgSendServiceInterface();
    
public slots:
   
    void send(QVariant data);
    
signals:
 
private:
MsgViewManager* mViewManager;
    
    };


#endif /* MSGSENDSERVICEINTERFACE_H_ */
