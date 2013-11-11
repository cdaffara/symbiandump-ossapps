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
* Description: NMail application mailbox registration.
*              Adds mailbox to application library and to KQTI HS (as a widget)
*
*/

#ifndef NMMAILBOXREGISTERINTERFACE_H_
#define NMMAILBOXREGISTERINTERFACE_H_

//  INCLUDES
#include <QObject>
#include <qmobilityglobal.h>
#include <QVariant>

class NmMailboxRegisterInterfacePrivate;

class NmMailboxRegisterInterface : public QObject
{
    Q_OBJECT

public:
    explicit NmMailboxRegisterInterface(QObject *parent = 0);
    ~NmMailboxRegisterInterface();
    
public slots:
    /*!
    \fn bool registerNewMailbox(quint64 accountId, QString accountName, QString accountIconName)
    \param accountId The ID of the mailbox to register
    \param accountName The name of the mailbox to register
    \param accountIconName The icon name of the mailbox to register 
    \return true if mailbox registration succeed. false if failed.

    This method registers mailbox to application library and to homescreen as an email widget.

    Usage example:

    QServiceManager manager;    
    QServiceFilter filter("com.nokia.symbian.IEmailRegisterAccount");
    QList<QServiceInterfaceDescriptor> interfaces = manager.findInterfaces(filter);  
    QObject *widgetObject = manager.loadInterface(interfaces.first());
    
    QMetaObject::invokeMethod(widgetObject,
            "registerNewMailbox",
            Q_ARG(quint64, accountId),
            Q_ARG(QString, accountName),
            Q_ARG(QString, "qtg_large_email"));
     */
    bool registerNewMailbox(quint64 accountId, QString accountName, QString accountIconName);
    
    /*!
    \fn bool updateMailboxName (quint64 accountId, QString newName)
    \param accountId The ID of the mailbox to register
    \param accountName The name of the mailbox to register
    \return true if mailbox name updated succesfully. false if failed.

    This method updates mailbox name to application library.

    Usage example:

    QServiceManager manager;   
    QServiceFilter filter("com.nokia.symbian.IEmailRegisterAccount");
    QList<QServiceInterfaceDescriptor> interfaces = manager.findInterfaces(filter);  
    QObject *widgetObject = manager.loadInterface(interfaces.first());
     
    QMetaObject::invoke(widgetObject,
            "updateMailboxName",
            Q_ARG(quint64, accountId),
            Q_ARG(QString, accountName));
     */
    bool updateMailboxName (quint64 accountId, QString newName);
        
    /*!
    \fn bool unregisterMailbox (quint64 accountId)
    \param accountId The ID of the mailbox to unregister
    \return true if mailbox unregistration succeed. false if failed.

    This method unregisters mailbox from application library.

    Usage example:

    QServiceManager manager;    
    QServiceFilter filter("com.nokia.symbian.IEmailRegisterAccount");
    QList<QServiceInterfaceDescriptor> interfaces = manager.findInterfaces(filter);  
    QObject *widgetObject = manager.loadInterface(interfaces.first());

    QMetaObject::invoke(widgetObject,
            "unregisterMailbox",
            Q_ARG(quint64, accountId));
     */
    bool unregisterMailbox (quint64 accountId);

private: 
    /**
    * Pointer to a private implementation.
    */
    NmMailboxRegisterInterfacePrivate* const m_d;

};

#endif /* NMMAILBOXREGISTERINTERFACE_H_ */



