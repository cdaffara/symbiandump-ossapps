/*
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
 * Description: Message Application service interface used for interfacing between
 *              QT highway and other applications
 *
 */

#ifndef __MSG_URIHANDLERINTERFACE_H__
#define __MSG_URIHANDLERINTERFACE_H__

#include <xqserviceprovider.h>
#include <QVariant>
#include <convergedmessage.h>

class MsgServiceViewManager;

class MsgUriHandlerInterface : public XQServiceProvider
    {
    Q_OBJECT
    
public:
    /*
     * Constructor
     */
    MsgUriHandlerInterface(MsgServiceViewManager* viewManager, QObject* parent=0);
    
    /*
     * Destructor
     */
    ~MsgUriHandlerInterface();
    
public slots:

    bool view(const QString& uri);

private:
    
    void normalizeQueryItems(QUrl &url);
    
    void addRecipients(ConvergedMessage &message, const QString &recipientsList);

private:
    
    MsgServiceViewManager* viewManager;
    
    };

#endif /* __MSG_URIHANDLERINTERFACE_H__ */
