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

#ifndef PHONESERVICE_H
#define PHONESERVICE_H

#include <QWidget>
#include <QModelIndex>
#include <QMap>
#include <QStringList>
#include <xqserviceprovider.h>

class QLineEdit;
class QPushButton;
class DialerService;
class QLabel;

class ServiceApp : public QWidget
{
    Q_OBJECT
public:
    ServiceApp( QWidget *parent = 0, Qt::WFlags f = 0 );
    ~ServiceApp();

    void setLabelNumber(QString label,QString number);

public slots:    
    void endCall();
    
private slots:
    void quit();
private:
    QLabel *mLabel;
    QLabel *mNumber;
    //QPushButton *mEndCallButton;
    DialerService* mService;
};

class DialerService : public XQServiceProvider
{
    Q_OBJECT
public:
    DialerService( ServiceApp *parent = 0 );
    ~DialerService();
    
    void complete(QString number);
    bool asyncAnswer() {return mAsyncAnswer;}
public slots:
    void dial(const QString& number);
    void dial(const QString& number,int contactId);
    void dialVideo(const QString& number);
    void dialVideo(const QString& number,int contactId);
    
private:
    ServiceApp* mServiceApp;
    QString mNumber;
    bool mAsyncAnswer;
};

#endif
