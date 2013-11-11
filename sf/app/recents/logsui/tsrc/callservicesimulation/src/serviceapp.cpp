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

#include <QApplication>
#include <QKeyEvent>
#include <QLabel>
#include <QVBoxLayout>
#include <QStackedWidget>
#include <QImageReader>
#include <QDebug>
#include <QTimer>
#include <QPushButton>
#include <QLineEdit>
#include <QListView>


#include "serviceapp.h"
#include <xqserviceutil.h>

ServiceApp::ServiceApp(QWidget *parent, Qt::WFlags f)
    : QWidget(parent, f)
{

    mService = new DialerService(this);
    /* Adjust the palette */
#if defined(Q_WS_S60)
    QPalette p = qApp->palette();
    QColor color(80,20,20);
    QColor bg(20,20,20);
    p.setColor(QPalette::Highlight, color.lighter(200));
    p.setColor(QPalette::Text, Qt::white);
    p.setColor(QPalette::Base, bg);
    p.setColor(QPalette::WindowText, Qt::white);
    p.setColor(QPalette::Window, bg);
    p.setColor(QPalette::ButtonText, Qt::white);
    p.setColor(QPalette::Button, color.lighter(150));
    p.setColor(QPalette::Link, QColor(240,40,40));

    qApp->setPalette(p);
#endif

    QPushButton *quitButton = new QPushButton(tr("Quit"));
    connect(quitButton, SIGNAL(clicked()), this, SLOT(quit()));

    /*
    mEndCallButton = new QPushButton(tr("End Call"));
    mEndCallButton->setEnabled(false);
    connect(mEndCallButton, SIGNAL(clicked()), this, SLOT(endCall()));
    */
    QString t = "SERVICE DIAL -> ";
    t = t + (XQServiceUtil::isEmbedded() ? " EMBEDDED" : " NOT EMBEDDED");
    QLabel *title = new QLabel(t);
  
    mLabel = new QLabel(tr("PHONE"));
    mNumber = new QLabel(tr("******"));

    QVBoxLayout *vl = new QVBoxLayout;
    vl->setMargin(0);
    vl->setSpacing(0);

    vl->addWidget(title);
    vl->addWidget(mLabel);
    vl->addWidget(mNumber);
    vl->addWidget(quitButton);

		setLayout(vl);
#if defined(Q_WS_X11) || defined(Q_WS_WIN)
    setFixedSize(QSize(360,640)); // nHD
#elif defined(Q_WS_S60)
    showMaximized();
    showFullScreen();
#endif
//    new DialerService(this);
}


ServiceApp::~ServiceApp()
{
}

void ServiceApp::quit()
{
    if (mService->asyncAnswer()) {
        connect(mService, SIGNAL(returnValueDelivered()), qApp, SLOT(quit()));
        mService->complete(mNumber->text());
    }
    else {
        qApp->quit();
    }
}

void ServiceApp::endCall()
{
    //QVBoxLayout *vl = qobject_cast<QVBoxLayout *>(layout()) ;
    //vl->removeWidget(mEndCallButton);

    //XQServiceUtil::toBackground(true);
}

void ServiceApp::setLabelNumber(QString label,QString number)
{
    //QVBoxLayout *vl = qobject_cast<QVBoxLayout *>(layout()) ;
    //vl->insertWidget(1,mEndCallButton);
    mLabel->setText(label);
    mNumber->setText(number);
}


DialerService::DialerService(ServiceApp* parent)
: XQServiceProvider(QLatin1String("com.nokia.services.telephony"),parent),mServiceApp(parent)
{
publishAll();
}

DialerService::~DialerService()
{
}

void DialerService::complete(QString number)
{
    completeRequest(0,number.toInt());
}

void DialerService::dial(const QString& number)
{
    qDebug() << "DialerService::dial (number):" << number;
    QString label = "PHONE dialing (number):" ;
    mServiceApp->setLabelNumber(label,number);
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
    mServiceApp->setLabelNumber(label,num);
}

void DialerService::dialVideo(const QString& number)
{
    qDebug() << "DialerService::dialVideo (number):" << number;
    QString label = "PHONE dialing video (number):" ;
    mServiceApp->setLabelNumber(label,number);
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
    mServiceApp->setLabelNumber(label,num);
}
