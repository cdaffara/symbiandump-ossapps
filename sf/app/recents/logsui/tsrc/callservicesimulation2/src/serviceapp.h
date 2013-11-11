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

#include <QObject>
#include <QModelIndex>
#include <QMap>
#include <QStringList>
#include <xqserviceprovider.h>
#include <QGraphicsWidget>

class DialerService;
class HbLineEdit;
class QGraphicsLinearLayout;
class DialerService;

class ServiceApp : public QGraphicsWidget
{
    Q_OBJECT
public:
    ServiceApp(DialerService* service);
    ~ServiceApp();

    void setLabelNumber(QString label,QString number);
    
private:
    QGraphicsLinearLayout *mMainLayout;
    HbLineEdit* mLabel;
    HbLineEdit* mNumber;
    DialerService* mService;
};

class DialerService : public XQServiceProvider
{
    Q_OBJECT
public:
    DialerService();
    ~DialerService();
    
    QString label(){
        return mLabel;
    }
    QString number(){
        return mNumber;
    }

signals:

    void labelChanged(QString label);
    void numberChanged(QString number);
    
public slots:
    void dial(const QString& number);
    void dial(const QString& number,int contactId);
    void dialVideo(const QString& number);
    void dialVideo(const QString& number,int contactId);
    
private:
    QString mLabel;
    QString mNumber;
    bool mAsyncAnswer;
};

#endif
