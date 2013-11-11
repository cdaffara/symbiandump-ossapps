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
* Description: Email services API to be used through Qt Highway.
*
*/

#ifndef EMAIL_SERVICES_API_H
#define EMAIL_SERVICES_API_H

// Qt
#include <QMetaType>
#include <QString>

// Platform
#include <xqaiwdeclplat.h>


/*!
    The flags which can be used with the service interfaces.
*/
enum EmailServiceInterfaceFlags {
    EmailNoFlags = 0x0,
    EmailBackReturnsToMessageList = 0x1
};

Q_DECLARE_METATYPE(EmailServiceInterfaceFlags)


/*!
    Keys for mail send service data.
*/
static const QString emailSendSubjectKey = "subject";
static const QString emailSendToKey = "to";
static const QString emailSendCcKey = "cc";
static const QString emailSendBccKey = "bcc";

// MIME type for body text, e.g. "text/html". If this key is not defined,
// emailSendBodyValueKey is treated as "text/plain".
static const QString emailSendBodyMimeTypeKey = "mimeType";

static const QString emailSendBodyTextKey = "body";
static const QString emailSendAttachmentKey = "attachment";

/*!
    Mail service name
*/
static const QString emailServiceName("nmail");

/*!
    Mail send service interface name.
*/
static const QString emailFullServiceNameSend = emailServiceName + "." + XQI_EMAIL_MESSAGE_SEND;

/*!
    Mailbox service full name.
*/
static const QString emailFullServiceNameMailbox = emailServiceName + "." + XQI_EMAIL_INBOX_VIEW;


/*!
    Message service full name.
*/
static const QString emailFullServiceNameMessage = emailServiceName + "." + XQI_EMAIL_MESSAGE_VIEW;


/*!
    \fn send(QVariant data)
    \param data The email message data e.g. recipients.
    \return 1 if success, 0 otherwise.

    The method opens the mail editor view and adds the given data into the view.

    The data is of type QMap<QString, QVariant> where the keys define the type
    of the data (e.g. recipient or mail subject). The values in the map for
    recipients (to, cc and bcc) may contain either strings (for a single
    recipient) or string lists (for multiple recipients).

    The method supports the following mail data:
        - recipients (to, cc and bcc) and
        - mail subject.

    This service is used via Qt Highway.

    Usage example:

        bool syncronous;

        XQServiceRequest request(XQI_EMAIL_MESSAGE_SEND,
                                 XQOP_EMAIL_MESSAGE_SEND,
                                 syncronous);

        QMap<QString, QVariant> map;

        QStringList recipients;
        QString subject;

        ...

        map.insert(emailSendToKey, QVariant::fromValue(recipients));
        map.insert(emailSendSubjectKey, QVariant(subject));

        QList<QVariant> data;
        data.append(map);
        request.setArguments(data);

        QVariant returnValue;
        bool retVal = request.send(returnValue);
*/


/*!
    \fn displayInboxByMailboxId(QVariant data)
    \param data The ID of the mailbox to open
    \return 1 if mailbox opened. 0 if mailbox ID was incorrect.

    This method opens a view for displaying the inbox of the given mailbox.
    It is intended to be used via Qt Highway.

    Usage example:

    XQServiceRequest request(
        XQI_EMAIL_INBOX_VIEW,
        XQOP_EMAIL_INBOX_VIEW,
        syncronous);

    QList<QVariant> list;
    list.append(QVariant(id));
    request.setArguments(list);

    QVariant returnValue;
    bool rval = request.send(returnValue);
*/


/*!
    \fn viewMessage(QVariant idList, QVariant flags)

    This method opens the mail viewer view containing the specific message.
    It is intended to be used via Qt Highway.


    \param idList A list containing the required IDs for locating the wanted
                  message. The ID are the following (and should be placed in
                  the following order): mailbox ID, folder ID and message ID.

    \param flags The flags which can contain any values defined by
                 EmailServiceInterfaceFlags enumeration. The only flag
                 acknowledged by this interface is EmailBackReturnsToMessageList,
                 which, if given, will add the message list (according to the
                 given mailbox and folder IDs) into the view stack. This means
                 that when the back button is pressed from the viewer view, the
                 message list view will be shown.

    \return 1 if message was opened. 0 if one of the IDs was incorrect.


    Usage example:

    XQServiceRequest request(
       XQI_EMAIL_MESSAGE_VIEW,
       XQOP_EMAIL_MESSAGE_VIEW,
       synchronous);

    QVariantList idList;
    idList.append(mailboxId);
    idList.append(folderId);
    idList.append(messageId);
    QVariant idListAsVariant = QVariant::fromValue(idList);

    // Add the message list view into the view stack. If the list view should
    // not be added into the stack, use "quint64 flags(EmailNoFlags);" instead.
    quint64 flags(EmailBackReturnsToMessageList);

    QList<QVariant> argumentList;
    argumentList.append(idListAsVariant);
    argumentList.append(flags);

    request.setArguments(argumentList);

    QVariant returnValue;

    bool rval = request.send(returnValue);
*/


#endif // EMAIL_SERVICES_API_H
