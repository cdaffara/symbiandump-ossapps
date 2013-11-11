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
 * Description: Message view service interface used for interfacing between
 *              QT highway and other applications.
 *
 */

#ifndef __MSG_VIEW_INTERFACE_H__
#define __MSG_VIEW_INTERFACE_H__

#include <QObject>
#include <xqserviceprovider.h>

class MsgServiceViewManager;

class MsgViewInterface : public XQServiceProvider
    {
    Q_OBJECT
    
public:
    /**
     * Constructor
     */
    MsgViewInterface(MsgServiceViewManager* viewManager);
    
    /**
     * Destructor
     */
    ~MsgViewInterface();
    
public slots:
    /**
     * Opens the appropriate viewer for a given message id.
     * @param msgId message Id
     */
    void view(int msgId);

private:
    /**
     * Pointer to view manager. 
     * Not owned.
     */
    MsgServiceViewManager* mViewManager;
    };

#endif /* __MSG_VIEW_INTERFACE_H__ */
