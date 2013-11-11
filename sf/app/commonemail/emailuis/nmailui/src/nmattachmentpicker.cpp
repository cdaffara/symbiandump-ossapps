/*
* Copyright (c) 2009 - 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Utility class for launching different file pickers
*
*/

#include "nmuiheaders.h"

static const int NmAttachmentPickerStillMode = 0;
static const int NmAttachmentPickerVideoMode = 1;

/*!
    \class NmAttachmentPicker
    \brief Attachment picker for mail editor
*/

/*!
    Constructor
*/
NmAttachmentPicker::NmAttachmentPicker(QObject *parent):
    QObject(parent),
    mRequest(NULL)
{
    NM_FUNCTION;
}

/*!
    Destructor
*/
NmAttachmentPicker::~NmAttachmentPicker()
{
    NM_FUNCTION;

    delete mRequest;
}

/*!
    Send request to start Photo picker
*/
void NmAttachmentPicker::fetchImage()
{
    NM_FUNCTION;

    fetch(XQI_IMAGE_FETCH, XQOP_IMAGE_FETCH);
}

/*!
    Send request to start Music picker
*/
void NmAttachmentPicker::fetchAudio()
{
    NM_FUNCTION;

    fetch(XQI_MUSIC_FETCH, XQOP_MUSIC_FETCH);
}

/*!
    Send request to start Video picker
*/
void NmAttachmentPicker::fetchVideo()
{
    NM_FUNCTION;

    fetch(XQI_VIDEO_FETCH, XQOP_VIDEO_FETCH);
}

/*!
    Send request to start general file picker
*/
void NmAttachmentPicker::fetchOther()
{
    NM_FUNCTION;

    QString path;
    path = FmFileDialog::getOpenFileName(0, hbTrId("txt_mail_dialog_select_file"));

    QString temp = QDir::toNativeSeparators(path);
    emit attachmentsFetchOk(QVariant(temp));
}

/*!
    Send request to retrieve image from camera
*/
void NmAttachmentPicker::fetchCameraStill()
{
    NM_FUNCTION;

    fetchFromCamera(NmAttachmentPickerStillMode);
}

/*!
    Send request to retrieve video from camera
*/
void NmAttachmentPicker::fetchCameraVideo()
{
    NM_FUNCTION;

    fetchFromCamera(NmAttachmentPickerVideoMode);
}

/*!
    Send request to retrieve image/video from camera
*/
void NmAttachmentPicker::fetchFromCamera(int mode)
{
    NM_FUNCTION;

    int cameraIndex(0);            //primary camera
    int quality(0);                //default
    bool allowModeSwitch(false);   //not allowed to change
    bool allowCameraSwitch(true);  //allow changes
    bool allowQualityChange(true); //allow changes

    QVariantMap parameters;
    parameters.insert(XQCAMERA_INDEX, cameraIndex);
    parameters.insert(XQCAMERA_QUALITY, quality);
    parameters.insert(XQCAMERA_MODE_SWITCH, allowModeSwitch);
    parameters.insert(XQCAMERA_INDEX_SWITCH, allowCameraSwitch);
    parameters.insert(XQCAMERA_QUALITY_CHANGE, allowQualityChange);

    QList<QVariant> args;
    args << mode;
    args << parameters;

    fetch(XQI_CAMERA_CAPTURE, "capture(int,QVariantMap)", &args);
}
/*!
    Construct & send appmgr request to start appropriate picker
    param <interface> the interface to be connected to
    param <operation> the operation of the interface
    param <args> the arguments that needed by the operation
*/
void NmAttachmentPicker::fetch(const QString &interface,
    const QString &operation, const QList<QVariant> *args)
{
    NM_FUNCTION;

    delete mRequest;
    mRequest = NULL;
    XQApplicationManager appMgr;
    mRequest = appMgr.create(interface, operation, true);

    if (mRequest) {
        mRequest->setSynchronous(false);
        if (args) {
            mRequest->setArguments(*args);
        }
        // Set the title pane text for the picker window
		// sub-title comes from the pickers
        QVariant title(mTitle);
        XQRequestInfo info;
        info.setInfo(XQINFO_KEY_WINDOW_TITLE, title);
        mRequest->setInfo(info);

        connect(mRequest, SIGNAL(requestOk(const QVariant&)),
                this, SIGNAL(attachmentsFetchOk(const QVariant&)));

        connect(mRequest, SIGNAL(requestError(int, const QString&)),
                this, SIGNAL(attachmentsFetchError(int, const QString&)));

        mRequest->send();
    }
}

/*!
	Launch the correct fetcher based on the clicked list item.
*/
void NmAttachmentPicker::selectFetcher(HbListWidgetItem *listItem)
{
    NM_FUNCTION;

    QString itemText = listItem->text();
    if (itemText == hbTrId("txt_mail_list_photo")) {
        fetchImage();
    }
    else if (itemText == hbTrId("txt_mail_list_sound")) {
        fetchAudio();
	}
    else if (itemText == hbTrId("txt_mail_list_video")) {
        fetchVideo();
	}
    else if (itemText == hbTrId("txt_mail_list_other")) {
        QMetaObject::invokeMethod(this, "fetchOther", Qt::QueuedConnection);
	}
    else if (itemText == hbTrId("txt_mail_list_new_photo")) {
        fetchCameraStill();
	}
    else if (itemText == hbTrId("txt_mail_list_new_video")) {
        fetchCameraVideo();
    }
}

/*!
    Sets the title used in the picker views
*/
void NmAttachmentPicker::setTitle(const QString &title)
{
    NM_FUNCTION;
    
    mTitle = title;
}
