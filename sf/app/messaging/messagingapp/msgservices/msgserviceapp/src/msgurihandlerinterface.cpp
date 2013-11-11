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
 * Description:
 * class to manage differnt messaging views.
 *
 */

#include "msgurihandlerinterface.h"

#include <QtGlobal>

#include <QString>
#include <QStringList>
#include <QPair>
#include <QUrl>
#include <convergedmessage.h>
#include <convergedmessageaddress.h>

#include "msgserviceviewmanager.h"
#include "debugtraces.h"


MsgUriHandlerInterface::MsgUriHandlerInterface(MsgServiceViewManager* viewManager, QObject* parent)
:
XQServiceProvider(QLatin1String("messaging.com.nokia.symbian.IUriView"), parent),   
viewManager(viewManager)
{
    publishAll();
}
    
MsgUriHandlerInterface::~MsgUriHandlerInterface()
{
    
}
    
bool MsgUriHandlerInterface::view(const QString& uri)
{
    QDEBUG_WRITE_FORMAT("service request for URI:", uri.toLatin1().data());
    QUrl parsedUri;
    
    parsedUri.setUrl(uri);
    normalizeQueryItems(parsedUri);

    QDEBUG_WRITE_FORMAT("scheme: ", parsedUri.scheme().toLatin1().data());
    QDEBUG_WRITE_FORMAT("path: ", parsedUri.path().toLatin1().data());
    QDEBUG_WRITE_FORMAT("items count: ", parsedUri.queryItems().size());
    QDEBUG_WRITE_FORMAT("error: ", parsedUri.errorString());
#ifdef _DEBUG_TRACES_
    QPair<QString,QString> item;
    foreach (item, parsedUri.queryItems()) 
    {
        int i = 0;
        QDEBUG_WRITE_FORMAT("MsgUriHandlerInterface::view(): query item # i :", i++);
        QDEBUG_WRITE_FORMAT("MsgUriHandlerInterface::view(): query item # First Item :", item.first.toLatin1().data());
        QDEBUG_WRITE_FORMAT("MsgUriHandlerInterface::view(): query item # Second Item :", item.second.toLatin1().data());
    }
#endif    
    
    ConvergedMessage message;
    
    if (parsedUri.scheme().compare("sms", Qt::CaseInsensitive)) 
    {
        message.setMessageType(ConvergedMessage::Sms);
    } 
    else if (parsedUri.scheme().compare("mmsto", Qt::CaseInsensitive)) 
    {
        message.setMessageType(ConvergedMessage::Mms);
    } 
    else 
    {
       QDEBUG_WRITE_FORMAT("Unsupported scheme:", parsedUri.scheme().toLatin1().data());
       return false;
    }

    QString addressLine = parsedUri.path();
    addRecipients(message, addressLine);
    
    QString body = parsedUri.queryItemValue("body");
    message.setBodyText(body);
    
    /* mms specific query values */

    QString subject = parsedUri.queryItemValue("subject");
    message.setSubject(subject);

    QString priority = parsedUri.queryItemValue("priority");
    if (priority.compare("Low", Qt::CaseInsensitive)) 
    {
        message.setPriority(ConvergedMessage::Low);
    } 
    else if (priority.compare("Normal", Qt::CaseInsensitive)) 
    {
        message.setPriority(ConvergedMessage::Normal);
    } 
    else if (priority.compare("High", Qt::CaseInsensitive)) 
    {
        message.setPriority(ConvergedMessage::High);
    } 
    else 
    {
       QDEBUG_WRITE_FORMAT("MsgUriHandlerInterface::view(): Unknown priority value: ", priority.toLatin1().data());
       return false;
    }
    
    QStringList tos = parsedUri.allQueryItemValues("to");
    QString to;
    foreach (to, tos) 
    {
        addRecipients(message, to);
    }
    
    QStringList ccs = parsedUri.allQueryItemValues("cc");
    QString cc;
    foreach (cc, ccs) 
    {
        addRecipients(message, cc);
    }
    
    viewManager->send(message);
    
    return true;
}

void MsgUriHandlerInterface::normalizeQueryItems(QUrl &url)
{
    QList<QPair<QString, QString> > items = url.queryItems();
    QPair<QString, QString> item;
    int i = 0;
    foreach (item, items) 
    {
        items.replace(i++, QPair<QString,QString >(item.first.toLower(), item.second));
    }
    url.setQueryItems(items);
}


void MsgUriHandlerInterface::addRecipients(ConvergedMessage &message, const QString &recipientsList)
{
    QStringList addressList = recipientsList.split(",");
    QString addressEntry;
    
    foreach (addressEntry, addressList) 
    {
        addressEntry.trimmed();
        if (! addressEntry.isEmpty()) 
        {
            ConvergedMessageAddress address;
            address.setAddress(addressEntry);
            message.addToRecipient(address);
            QDEBUG_WRITE_FORMAT("MsgUriHandlerInterface::addRecipients() recipient added: ", addressEntry.toLatin1().data());
         }
     }
}
