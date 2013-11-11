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
#include "t_searchactivity.h"
#include <qurl.h>
#include <qservicemanager.h>
#include <hbcheckbox.h>
QTM_USE_NAMESPACE

const QString KAppUid = "0x2002C377";

SearchTestActivity::SearchTestActivity(QGraphicsItem *parent) :
    HbWidget(parent)
    {

    QServiceManager serviceManager;
    mActivityManager = serviceManager.loadInterface(
            "com.nokia.qt.activities.ActivityManager");
    //mActivityManager->run
    if (!mActivityManager)
        {
        qFatal(
                "Cannot initialize critical com.nokia.qt.activities.ActivityManager service.");
        }

    mColumn1Layout = new QGraphicsLinearLayout(Qt::Vertical);
    mColumn2Layout = new QGraphicsLinearLayout(Qt::Vertical);
    mButtonLayout = new QGraphicsLinearLayout(Qt::Horizontal);
    mMainLayout = new QGraphicsLinearLayout(Qt::Vertical);
    mCheckBoxLayout = new QGraphicsLinearLayout(Qt::Horizontal);

    HbCheckBox* check1 = new HbCheckBox(this);
    check1->setText("contact");
    HbCheckBox* check2 = new HbCheckBox(this);
    check2->setText("media");
    HbCheckBox* check3 = new HbCheckBox(this);
    check3->setText("calendar");
    HbCheckBox* check4 = new HbCheckBox(this);
    check4->setText("notes");
    HbCheckBox* check5 = new HbCheckBox(this);
    check5->setText("image");
    HbCheckBox* check6 = new HbCheckBox(this);
    check6->setText("audio");
    HbCheckBox* check7 = new HbCheckBox(this);
    check7->setText("video");
    HbCheckBox* check8 = new HbCheckBox(this);
    check8->setText("bookmark");
    HbCheckBox* check9 = new HbCheckBox(this);
    check9->setText("msg");
    HbCheckBox* check10 = new HbCheckBox(this);
    check10->setText("file");
    HbCheckBox* check11 = new HbCheckBox(this);
    check11->setText("applications");
    HbCheckBox* check12 = new HbCheckBox(this);
    check12->setText("email");

    mColumn1Layout->addItem(check1);
    mColumn1Layout->addItem(check2);    
    mColumn1Layout->addItem(check3);
    mColumn1Layout->addItem(check4);
    mColumn1Layout->addItem(check5);    
    mColumn1Layout->addItem(check6);
    
    mColumn2Layout->addItem(check7);
    mColumn2Layout->addItem(check8);
    mColumn2Layout->addItem(check9);
    mColumn2Layout->addItem(check10);
    mColumn2Layout->addItem(check11);
    mColumn2Layout->addItem(check12);
    
    mCheckBoxLayout->addItem(mColumn1Layout);
    mCheckBoxLayout->addItem(mColumn2Layout); 
    

    mIndevice = new HbPushButton("InDevice");
    mIndevice->setMaximumHeight(50);
    mOnline = new HbPushButton("Online");
    mOnline->setMaximumHeight(50);
    
    mButtonLayout->addItem(mIndevice);
    mButtonLayout->addItem(mOnline);
    
    
    mLineEdit = new HbLineEdit("");
    mLineEdit->setPlaceholderText("Search Query");
    
    

    mMainLayout->addItem(mCheckBoxLayout);
    mMainLayout->addItem(mLineEdit);
    mMainLayout->addItem(mButtonLayout);

    
    
    connect(mIndevice, SIGNAL(released()), this, SLOT(launchDeviceApp()));
    connect(mOnline, SIGNAL(released()), this,
            SLOT(launchInternetApp()));

    setLayout(mMainLayout);

    mselectioncheckbox.append(check1);
    mselectioncheckbox.append(check2);
    mselectioncheckbox.append(check3);
    mselectioncheckbox.append(check4);
    mselectioncheckbox.append(check5);
    mselectioncheckbox.append(check6);
    mselectioncheckbox.append(check7);
    mselectioncheckbox.append(check8);
    mselectioncheckbox.append(check9);
    mselectioncheckbox.append(check10);
    mselectioncheckbox.append(check11);
    mselectioncheckbox.append(check12);
    }

SearchTestActivity::~SearchTestActivity()
    {    
    delete mMainLayout;    
    delete mActivityManager;    
    }

void SearchTestActivity::launchDeviceApp()
    {
    QUrl url;
    url.setScheme("appto");
    url.setHost(KAppUid);
    url.addQueryItem("activityname", "SearchDeviceQueryView");
    url.addQueryItem("query",mLineEdit->text());
    /*
     * QString str = "a";
     * url.addQueryItem("query", str);
    */
    
    for (int i = 0; i < mselectioncheckbox.count(); i++)
        {
        if (mselectioncheckbox.at(i)->isChecked())
            {
            QString str = mselectioncheckbox.at(i)->text();
            url.addQueryItem(str, str);
            }
        }
    QString debugString = url.toString();
    bool ok = QMetaObject::invokeMethod(mActivityManager, "launchActivity",
            Q_ARG(QUrl, url));
    if (ok)
        {
        //mResultLabel->setPlainText("ok");
        }
    else
        {
        //mResultLabel->setPlainText("fail");
        }
    }

void SearchTestActivity::launchInternetApp()
    {
    QUrl url;
    url.setScheme("appto");
    url.setHost(KAppUid);
    url.addQueryItem("activityname", "SearchWebQueryView");
    url.addQueryItem("query", mLineEdit->text());
    /*QString str = "a";
    url.addQueryItem("query", str);*/

    for (int i = 0; i < mselectioncheckbox.count(); i++)
        {
        if (mselectioncheckbox.at(i)->isChecked())
            {
            QString str = mselectioncheckbox.at(i)->text();
            url.addQueryItem(str, str);
            }
        }
    QString debugString = url.toString();
    bool ok = QMetaObject::invokeMethod(mActivityManager, "launchActivity",
            Q_ARG(QUrl, url));
    if (ok)
        {
        //mResultLabel->setPlainText("ok");
        }
    else
        {
        //mResultLabel->setPlainText("fail");
        }

    }

