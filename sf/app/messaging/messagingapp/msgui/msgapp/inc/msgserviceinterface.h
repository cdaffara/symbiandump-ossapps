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

#ifndef MSGSERVICEINTERFACE_H_
#define MSGSERVICEINTERFACE_H_

// INCLUDES
#include <xqserviceprovider.h>
#include <qvariant.h>

//TODO: to be deprecated
#include <QStringList>
class ConvergedMessageAddress;

struct ContactDetail
{
public:
    /**
     * Constructor
     */
    ContactDetail():contactId(-1),displayName(QString()){};
    
public:
    int contactId;
    QString displayName;
    QStringList addressList;
};

class MsgViewManager;
class MsgServiceInterface : public XQServiceProvider
    {
    Q_OBJECT
    
public:
    /*
     * Constructor
     */
    MsgServiceInterface(QObject* parent=0, MsgViewManager* manager =0);
    
    /*
     * Destructor
     */
    ~MsgServiceInterface();
    
public slots:
    
    /**
     * send called from phone book.
     */
    void send(const QString phoneNumber, const qint32 contactId, const QString displayName);
    
    /**
     * called from indications.
     */
    void open(qint64 conversationId);
    
    /**
     * called from send UI.
     */
    void send(QVariant data);
    
     /**
     * Opens the viewer to view the message.
     * @param msgId message id of the message.
     */
    void view(int msgId);
    
    /**
     * depricated api. will be removed in future.
     */
    void openConversationView(QString number, QString name);

    /**
     * send called with prepopulated message body.
     */
    void send(const QString phoneNumber, const QString alias, const QString bodyText);

signals:

private:
    //TODO: to be deprecated
    bool resolveContact(const ConvergedMessageAddress &address,
            ContactDetail &contactDetail);
 
private:
MsgViewManager* mViewManager;
    
    };


#endif /* MsgServiceInterface_H_ */
