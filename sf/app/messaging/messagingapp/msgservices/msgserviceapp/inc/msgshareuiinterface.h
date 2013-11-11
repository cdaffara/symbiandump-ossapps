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
 * Description: Message share UI service interface used for interfacing between
 *              QT highway and other applications.
 *
 */

#ifndef MSGSHAREUIINTERFACE_H_
#define MSGSHAREUIINTERFACE_H_

// INCLUDES
#include <xqserviceprovider.h>
#include <qvariant.h>

class MsgServiceViewManager;
class MsgShareUiInterface : public XQServiceProvider
    {
    Q_OBJECT
    
public:
    /*
     * Constructor
     */
    MsgShareUiInterface(MsgServiceViewManager* viewManager);
    
    /*
     * Destructor
     */
    ~MsgShareUiInterface();
    
public slots:
   /**
    * Send message.
    * @param data data to be sent.
    */
    void send(QVariant data);
    
private:
    /**
     * Pointer to view manager
     * Not owned.
     */
    MsgServiceViewManager* mViewManager;
    };


#endif /* MSGSHAREUIINTERFACE_H_ */
