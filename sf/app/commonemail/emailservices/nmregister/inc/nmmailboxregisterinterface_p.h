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
*
*/


#ifndef NMMAILBOXREGISTERINTERFACEPRIVATE_H_
#define NMMAILBOXREGISTERINTERFACEPRIVATE_H_

//  INCLUDES
#include <QObject>
#include <qmobilityglobal.h>
#include <xqserviceprovider.h>

//  FORWARD DECLARATIONS
class QString;
class QTranslator;

QTM_BEGIN_NAMESPACE
class QServiceManager;
QTM_END_NAMESPACE

QTM_USE_NAMESPACE

class NmMailboxRegisterInterfacePrivate : public XQServiceProvider
{
    Q_OBJECT

public:

    explicit NmMailboxRegisterInterfacePrivate(QObject *parent = 0);
    ~NmMailboxRegisterInterfacePrivate();


public slots:

    bool registerNewMailbox(quint64 accountId, QString accountName, QString accountIconName);

    bool updateMailboxName(quint64 accountId, QString newName);
    
    bool unregisterMailbox(quint64 accountId);

private:
   
    bool pushWidgetToHomescreen(quint64 a, QString accountIconName);
    
    QString formLaunchUri(quint64 accountId) const;
    
    bool loadMenuClientService();

private: 
    QServiceManager* mManager;
    QObject* mService;
    QTranslator *mTranslator;
};

#endif /* NMMAILBOXREGISTERINTERFACE_H_ */
