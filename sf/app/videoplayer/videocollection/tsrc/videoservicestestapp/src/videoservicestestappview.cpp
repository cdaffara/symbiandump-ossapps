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

#include <xqaiwrequest.h>
#include <QGraphicsLinearLayout>
#include <hbpushbutton.h>
#include <hblineedit.h>
#include <hblabel.h>
#include <hbaction.h>
#include <hbinputdialog.h>
#include <qdebug>
#include <vcxmyvideosdefs.h>
#include <xqaiwdecl.h>
#include "videoservicestestappview.h"

VideoServicesTestAppView::VideoServicesTestAppView(QGraphicsItem *parent) :
    HbView(parent),
    mReq(0),
    mResultEdit(0),
    mErrorCodeEdit(0),
    mErrorEdit(0)
{
    qDebug() << "VideoServicesTestAppView::VideoServicesTestAppView";
    createLayout();
}


VideoServicesTestAppView::~VideoServicesTestAppView()
{
    qDebug() << "VideoServicesTestAppView::~VideoServicesTestAppView";
    delete mReq;
}


void VideoServicesTestAppView::handleOk(const QVariant &result)
{
    qDebug() << "VideoServicesTestAppView::handleOk";

    if (!result.canConvert<QStringList>())
    {
        mErrorEdit->setText("Corrupt result");
    }
    else
    {
        QString text = result.value<QString>();
        if (text.isEmpty())
        {
            text = "No result available in this service";
        }
        mResultEdit->setText(text);
        qDebug() << "VideoServicesTestAppView::handleOk" << ": result=" << text;
    }

}


void VideoServicesTestAppView::handleError(int errorCode, const QString& errorMessage)
{
    qDebug() << "VideoServicesTestAppView::handleError" << ": errorCode=" << errorCode << ", msg:" << errorMessage;
    mErrorEdit->setText(errorMessage);
    mErrorCodeEdit->setText(QString::number(errorCode));
}


void VideoServicesTestAppView::createLayout()
{
    qDebug() <<  "VideoServicesTestAppView::createLayout";

    QGraphicsLinearLayout* layout = new QGraphicsLinearLayout(Qt::Vertical);

    if (layout)
    {
        QGraphicsLinearLayout *topLayout = new QGraphicsLinearLayout(Qt::Vertical);

        if (topLayout)
        {
        	//topLayout->addStretch(5);

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
            //layout->setStretchFactor(topLayout, 5);
        }

        QGraphicsLinearLayout *bottomLayout = new QGraphicsLinearLayout(Qt::Vertical);

        if (bottomLayout)
        {
            HbPushButton* button = new HbPushButton("Fetch video OLD (removed)");
            if (button)
            {
                connect(button, SIGNAL(clicked()), this, SLOT(fetchVideoDeprecatedNewService()));
                bottomLayout->addItem(button);
            }

            button = new HbPushButton("Fetch video NEW IF NEW S");
            if (button)
            {
                connect(button, SIGNAL(clicked()), this, SLOT(fetchVideo()));
                bottomLayout->addItem(button);
            }

            button = new HbPushButton("Browse \"Captured\" videos OLD(removed)");
            if (button)
            {
                connect(button, SIGNAL(clicked()), this, SLOT(browseCapturedVideosDeprecatedNewService()));
                bottomLayout->addItem(button);
            }


            button = new HbPushButton("Browse \"Captured\" videos NEW IF");
            if (button)
            {
                connect(button, SIGNAL(clicked()), this, SLOT(browseCapturedVideos()));
                bottomLayout->addItem(button);
            }

            layout->addItem(bottomLayout);
        }

        setLayout(layout); // Takes ownership of layout
    }
}


void VideoServicesTestAppView::fetchVideo()
{
    qDebug() <<  "VideoServicesTestAppView::fetchVideo START";

    mResultEdit->setText("");
    mErrorEdit->setText("");
    mErrorCodeEdit->setText("");

    delete mReq;
    mReq = 0;
    mReq = mAppMgr.create(XQI_VIDEO_FETCH, XQOP_VIDEO_FETCH, true);

    qDebug() <<  "VideoServicesTestAppView::fetchVideo: mReq=" << mReq;

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

    // Set arguments for request (application title)
    XQRequestInfo info;
    info.setInfo("WindowTitle", "<NEW IF app_name>");
    mReq->setInfo(info);

    // Make the request
    if (!mReq->send())
    {
        mErrorEdit->setText("Failed to send REQ");
        qDebug() <<  "VideoServicesTestAppView::fetchVideo: XQAiwRequest::send returned false";
    }
    // req no longer needed, remove it
    delete mReq;
    mReq = 0;

    qDebug() <<  "VideoServicesTestAppView::fetchVideo END";
}


void VideoServicesTestAppView::browseCapturedVideos()
{
    qDebug() <<  "VideoServicesTestAppView::browseCapturedVideos START";

    mResultEdit->setText("");
    mErrorEdit->setText("");
    mErrorCodeEdit->setText("");

    delete mReq;
    mReq = 0;

    //mReq = mAppMgr.create(XQI_VIDEO_BROWSE, XQOP_VIDEO_BROWSE, true);
    mReq = mAppMgr.create(XQI_VIDEO_BROWSE, "browseVideos(int,int)", true);

    qDebug() <<  "VideoServicesTestAppView::browseCapturedVideos: mReq=" << mReq;

    if (!mReq)
    {
        mErrorEdit->setText("Failed to create REQ");
        return;
    }
    else
    {
        connect(mReq, SIGNAL(requestOk(QVariant)), SLOT(handleOk(QVariant)));
        connect(mReq, SIGNAL(requestError(int,QString)), SLOT(handleError(int,QString)));
    }

    // - title
    XQRequestInfo info;
    info.setInfo("WindowTitle", "<NEW IF app name>");
    mReq->setInfo(info);

    HbInputDialog *dialog = new HbInputDialog();
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->setAdditionalRowVisible(true);
    dialog->setInputMode(HbInputDialog::IntInput, 0);
    dialog->setInputMode(HbInputDialog::IntInput, 1);
    dialog->setPromptText("Enter category", 0);
    dialog->setPromptText("Enter sort role", 1);
	dialog->setValue(KVcxMvcCategoryIdCaptured, 0);
	dialog->setValue(XQService::SortTitle, 1);
    dialog->open(this, SLOT(browseCapturedVideosFinished(HbAction *)));

    qDebug() <<  "VideoServicesTestAppView::browseCapturedVideos END";
}

void VideoServicesTestAppView::browseCapturedVideosFinished(HbAction *action)
{
    HbInputDialog *dialog = static_cast<HbInputDialog*>(sender());

    int category = dialog->value(0).toInt();
    int sortRole = dialog->value(1).toInt();

    if(dialog->actions().first() == action)
    {
    	// set arguments
        QList<QVariant> args;
        args.append(QVariant(category));
        args.append(QVariant(sortRole));
        mReq->setArguments(args);

        // Make the request
        if (!mReq->send())
        {
            mErrorEdit->setText("Failed to send REQ");
            qDebug() <<  "VideoServicesTestAppView::browseCapturedVideosFinished: XQAiwRequest::send returned false";
        }
        // req no longer needed, remove it
        delete mReq;
        mReq = 0;
    }
}

void VideoServicesTestAppView::fetchVideoDeprecatedNewService()
{
    qDebug() <<  "VideoServicesTestAppView::fetchVideo START";

    mResultEdit->setText("");
    mErrorEdit->setText("");
    mErrorCodeEdit->setText("");

    delete mReq;
    mReq = 0;
    mReq = mAppMgr.create("videoplayer", "IVideoFetch", "fetch(QString)", true);

    qDebug() <<  "VideoServicesTestAppView::fetchVideo: mReq=" << mReq;

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

    // Set arguments for request (application title)
    QList<QVariant> args;
    args << QVariant(QString("<OLD-IF-NEW-S app_name>"));
    mReq->setArguments(args);

    // Make the request
    if (!mReq->send())
    {
        mErrorEdit->setText("Failed to send REQ");
        qDebug() <<  "VideoServicesTestAppView::fetchVideo: XQAiwRequest::send returned false";
    }
    // req no longer needed, remove it
    delete mReq;
    mReq = 0;
    qDebug() <<  "VideoServicesTestAppView::fetchVideo END";
}

void VideoServicesTestAppView::browseCapturedVideosDeprecatedNewService()
{
    qDebug() <<  "VideoServicesTestAppView::browseCapturedVideos START";

    mResultEdit->setText("");
    mErrorEdit->setText("");
    mErrorCodeEdit->setText("");

    delete mReq;
    mReq = 0;
    mReq = mAppMgr.create("videoplayer", "IVideoBrowse", "browseVideos(QString,int,int)", true);

    qDebug() <<  "VideoServicesTestAppView::browseCapturedVideos: mReq=" << mReq;

    if (!mReq)
    {
        mErrorEdit->setText("Failed to create REQ");
        return;
    }
    else
    {
        connect(mReq, SIGNAL(requestOk(QVariant)), SLOT(handleOk(QVariant)));
        connect(mReq, SIGNAL(requestError(int,QString)), SLOT(handleError(int,QString)));
    }

    HbInputDialog *dialog = new HbInputDialog();
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->setAdditionalRowVisible(true);
    dialog->setInputMode(HbInputDialog::IntInput, 0);
    dialog->setInputMode(HbInputDialog::IntInput, 1);
    dialog->setPromptText("Enter category", 0);
    dialog->setPromptText("Enter sort role", 1);
	dialog->setValue(3, 0);
	dialog->setValue(0, 1);
    dialog->open(this, SLOT(browseCapturedVideosDeprecatedNewServiceFinished(HbAction *)));

    qDebug() <<  "VideoServicesTestAppView::browseCapturedVideos END";
}

void VideoServicesTestAppView::browseCapturedVideosDeprecatedNewServiceFinished(HbAction *action)
{
    HbInputDialog *dialog = static_cast<HbInputDialog*>(sender());

    const QString title = "<OLD-IF-NEW-S app name>";

    int category = dialog->value(0).toInt();
    int sortRole = dialog->value(1).toInt();

    if(dialog->actions().first() == action)
    {
    	// set arguments
        QList<QVariant> args;
        args.append(title);
        args.append(QVariant(category));
        args.append(QVariant(sortRole));
        mReq->setArguments(args);

        // Make the request
        if (!mReq->send())
        {
            mErrorEdit->setText("Failed to send REQ");
            qDebug() <<  "VideoServicesTestAppView::browseCapturedVideosFinished: XQAiwRequest::send returned false";
        }
        // req no longer needed, remove it
        delete mReq;
        mReq = 0;
    }
}

// end of file
