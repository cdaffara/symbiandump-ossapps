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
#ifndef VIDEOSERVICESTESTAPPVIEW_H_
#define VIDEOSERVICESTESTAPPVIEW_H_

#include <hbview.h>
#include <xqappmgr.h>

class XQApplicationManager;
class XQAiwRequest;
class HbLineEdit;

class VideoServicesTestAppView : public HbView
{
	Q_OBJECT

public:
	VideoServicesTestAppView(QGraphicsItem *parent=0);
	virtual ~VideoServicesTestAppView();

private slots:
    void handleOk(const QVariant &result);
    void handleError(int errorCode, const QString& errorMessage);

    void fetchVideo();
    void fetchVideoDeprecatedNewService();
    void browseCapturedVideos();
    void browseCapturedVideosDeprecatedNewService();
    
public slots:
    
    void browseCapturedVideosFinished(HbAction *action);
    void browseCapturedVideosDeprecatedNewServiceFinished(HbAction *action);

private:
    void createLayout();

private:
    XQApplicationManager mAppMgr;
    XQAiwRequest* mReq;
    HbLineEdit* mResultEdit;
    HbLineEdit* mErrorCodeEdit;
    HbLineEdit* mErrorEdit;
};

#endif /* VIDEOSERVICESTESTAPPVIEW_H_ */
