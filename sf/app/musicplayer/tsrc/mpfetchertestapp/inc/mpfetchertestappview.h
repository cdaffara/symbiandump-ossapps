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
#ifndef MPFETCHERTESTAPPVIEW_H_
#define MPFETCHERTESTAPPVIEW_H_

#include <hbview.h>
#include <xqappmgr.h>

class XQApplicationManager;
class XQAiwRequest;
class HbLineEdit;

class MpFetcherTestAppView : public HbView
{
	Q_OBJECT

public:
	MpFetcherTestAppView(QGraphicsItem *parent=0);
	virtual ~MpFetcherTestAppView();

private slots:
    void handleOk(const QVariant &result);
    void handleError(int errorCode, const QString& errorMessage);
    
    void fetchSong();
    void playSong();
    void viewSong();
    void viewSongCaged();
    void launchHomeScreen();
    void launchHomeScreenNowPlaying();
    void launchHomeScreenNPShuffle();
    void playNPVSong();
    void endMP();
    void killMP();
    
    
    
private:
    void createLayout();
    
private:
    XQApplicationManager mAppMgr;
    XQAiwRequest* mReq;
    HbLineEdit* mResultEdit;
    HbLineEdit* mErrorCodeEdit;
    HbLineEdit* mErrorEdit;
};

#endif /* MPFETCHERTESTAPPVIEW_H_ */
