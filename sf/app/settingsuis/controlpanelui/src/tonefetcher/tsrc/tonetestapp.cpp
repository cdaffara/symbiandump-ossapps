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
 *     The source file for tone service test.
 *     
 */
#include "tonetestapp.h"
#include <xqaiwrequest.h>
#include <QGraphicsLinearLayout>
#include <hbpushbutton.h>
#include <hblineedit.h>
#include <hblabel.h>
#include <qdebug>
#include <xqservicerequest.h>

ToneTestApp::ToneTestApp(QGraphicsItem *parent) :
    HbView(parent),
    mReq(0),
    mResultEdit(0),
    mErrorCodeEdit(0),
    mErrorEdit(0)
{
    qDebug() << "ToneTestApp::ToneTestApp(";
    createLayout();
}

ToneTestApp::~ToneTestApp()
{
    qDebug() << "ToneTestApp::~ToneTestApp";
    delete mReq;
}

void ToneTestApp::handleOk(const QVariant &result)
{
    qDebug() << "ToneTestApp::handleOk";
    
    if (!result.canConvert<QString>())
    {
        mErrorEdit->setText("Corrupt result");
    }
    else
    {
        mResultEdit->setText(result.value<QString>());
        qDebug() << "ToneTestApp::handleOk" << ": result=" << result.value<QString>();
    }
    
}

void ToneTestApp::handleError(int errorCode, const QString& errorMessage)
{
    qDebug() << "ToneTestApp::handleError" << ": errorCode=" << errorCode << ", msg:" << errorMessage;
    mErrorEdit->setText(errorMessage);
    mErrorCodeEdit->setText(QString::number(errorCode));
}

void ToneTestApp::createLayout()
{
    qDebug() <<  "ToneTestApp::createLayout";
    
    QGraphicsLinearLayout* layout = new QGraphicsLinearLayout(Qt::Vertical);

    if (layout)
    {
        QGraphicsLinearLayout *topLayout = new QGraphicsLinearLayout(Qt::Vertical);

        if (topLayout)
        {
            topLayout->addStretch(5);
            
            mResultEdit = new HbLineEdit;
            HbLabel* label = new HbLabel("Result:");
            if (mResultEdit && label)
            {
                topLayout->addItem(label);
                mResultEdit->setMaxRows(5);
                topLayout->addItem(mResultEdit);
            }
            
            mErrorCodeEdit = new HbLineEdit;
            label = new HbLabel("Error code:");
            if (mErrorCodeEdit && label)
            {
                topLayout->addItem(label);
                topLayout->addItem(mErrorCodeEdit);
            }
            
            mErrorEdit = new HbLineEdit;
            label = new HbLabel("Error description:");
            if (mErrorEdit && label)
            {
                topLayout->addItem(label);
                mErrorEdit->setMaxRows(5);
                topLayout->addItem(mErrorEdit);
            }
            
            layout->addItem(topLayout);
            layout->setStretchFactor(topLayout, 5);
        }

        QGraphicsLinearLayout *bottomLayout = new QGraphicsLinearLayout(Qt::Vertical);

        if (bottomLayout)
        {
            bottomLayout->addStretch();
            HbPushButton* button = new HbPushButton("Fetch tone");
            if (button)
            {
                connect(button, SIGNAL(clicked()), this, SLOT(fetchTone()));
                bottomLayout->addItem(button);
            }
            layout->addItem(bottomLayout);
        }

        setLayout(layout); // Takes ownership of layout
    }
}

void ToneTestApp::fetchTone()
{
    qDebug() <<  "ToneTestApp::fetchSong START";
    
    mResultEdit->setText("");
    mErrorEdit->setText("");
    mErrorCodeEdit->setText("");
    
    if (!mReq)
   {
        mReq = mAppMgr.create("com.nokia.symbian.IToneFetch", "fetch()", true);
     //   XQServiceRequest snd("com.nokia.services.toneserviceprovider.tone", "queryTone()", true);
       // QVariant retValue;
                
  /*  bool res = snd.send(retValue);
    if (!res)
        {
        int returnvalue = snd.latestError();
        mErrorEdit->setText(QString::number(returnvalue));
        // mRetValue->setText("send fail!");
        }*/ 
        
        if (!mReq)
        {
            mErrorEdit->setText("Failed to create REQ");
            return;
        }
        else
        {
            connect(mReq, SIGNAL(requestOk(const QVariant&)), SLOT(handleOk(const QVariant&)));
            connect(mReq, SIGNAL(requestError(int,const QString&)), SLOT(handleError(int,const QString&)));
        }
    }
    
    // Set arguments for request (application title)
    QList<QVariant> args;
    args << QVariant(QString("<app_name>"));
    mReq->setArguments(args);

    // Make the request
    if (!mReq->send())
    {
        mErrorEdit->setText("Failed to send REQ");        
    }
}

