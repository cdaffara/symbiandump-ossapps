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

#ifndef CNTACTIONLAUNCHER_H_
#define CNTACTIONLAUNCHER_H_

#include <QObject>
#include <qtcontactsglobal.h>
#include <qtcontacts.h>

QTM_USE_NAMESPACE

class CntActionLauncher : public QObject
    {
    Q_OBJECT
    
public:
    CntActionLauncher( QContactManager& aContactManager, QString aAction = QString() );
    ~CntActionLauncher();
    
public:
    bool execute( QContact aContact, QContactDetail aDetail );
    bool execute( QContact aGrpContact, QContactDetail aDetail, QVariantMap aParameters );
    bool execute( QContact aContact, QContactDetail aDetail, QContactActionDescriptor aActionDescriptor );
    
signals:
    void actionExecuted( CntActionLauncher* aAction );
    
private slots:
    void progress( QContactAction::State status );
    
private:
    QString mAction;
    QContactAction* mContactAction;
    QContactManager* mContactManager;
    };
#endif /* CNTACTION_H_ */
