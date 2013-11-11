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


#include <QGraphicsLinearLayout>
#include <QDebug>
#include <hblineedit.h>

#include "serviceapp.h"
#include <xqserviceutil.h>

ServiceApp::ServiceApp( DialerService* service )
    : QGraphicsWidget(), mService( service )
{
    qDebug() << "ServiceApp::ServiceApp ->";
    mMainLayout = new QGraphicsLinearLayout(Qt::Vertical);
    mLabel = new HbLineEdit("Callservice simulation");
    mNumber = new HbLineEdit("");  
    mMainLayout->addItem(mLabel);
    mMainLayout->addItem(mNumber);
    setLayout(mMainLayout);
    if ( mService ){
        setLabelNumber( mService->label(), mService->number() );
        connect( mService, SIGNAL(labelChanged(QString)), 
             mLabel, SLOT(setText(const QString &)) );
        connect( mService, SIGNAL(numberChanged(QString)), 
             mNumber, SLOT(setText(const QString &)) );
    }
    qDebug() << "ServiceApp::ServiceApp <-";
  
}


ServiceApp::~ServiceApp()
{
    qDebug() << "ServiceApp::~ServiceApp ->";
    delete mMainLayout;
    delete mLabel;
    delete mNumber;
    delete mService;
    qDebug() << "ServiceApp::~ServiceApp <-";
}

void ServiceApp::setLabelNumber(QString label,QString number)
{
    mLabel->setText(label);
    mNumber->setText(number);
}


DialerService::DialerService()
: XQServiceProvider(QLatin1String("com.nokia.services.telephony"),0)
{
    publishAll();
}

DialerService::~DialerService()
{
    qDebug() << "DialerService::~DialerService <->";
}

void DialerService::dial(const QString& number)
{
    qDebug() << "DialerService::dial (number):" << number;
    QString label = "PHONE dialing (number):" ;
    mLabel = label;
    mNumber = number;
    emit labelChanged(mLabel);
    emit numberChanged(mNumber);
}

void DialerService::dial(const QString& number, int contactId)
{
    qDebug() << "DialerService::dial to num:" << number << "contactId:" << contactId;
    QString label = "PHONE dialing (number+contact):" ;
    QString num;
    num.append( number );
    num.append(" ");
    QString contactStr;
    contactStr.setNum(contactId);
    num.append( contactStr );
    mLabel = label;
    mNumber = number;
    emit labelChanged(mLabel);
    emit numberChanged(mNumber);
}

void DialerService::dialVideo(const QString& number)
{
    qDebug() << "DialerService::dialVideo (number):" << number;
    QString label = "PHONE dialing video (number):" ;
    mLabel = label;
    mNumber = number;
    emit labelChanged(mLabel);
    emit numberChanged(mNumber);
}

void DialerService::dialVideo(const QString& number, int contactId)
{
    qDebug() << "DialerService::dialVideo to num:" << number << "contactId:" << contactId;
    QString label = "PHONE dialing video (number+contact):" ;
    QString num;
    num.append( number );
    num.append(" ");
    QString contactStr;
    contactStr.setNum(contactId);
    num.append( contactStr );
    mLabel = label;
    mNumber = number;
    emit labelChanged(mLabel);
    emit numberChanged(mNumber);
}
