/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0""
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
#ifndef MAINVIEW_H
#define MAINVIEW_H

#include <hbview.h>
#include <xqappmgr.h>

class MainView : public HbView
{
	Q_OBJECT
public:
    explicit MainView(QGraphicsItem *parent = 0);
    virtual ~MainView();
private:
	void init();
private slots:
    void launchInProcessProfileView();
    
    void launchQtHighwayProfileView();   
    
    void handleReturnValue(const QVariant &returnValue);
    void handleError(int errorCode,const QString &errorMessage);
    
    void closeSettingView();
    
private:
    Q_DISABLE_COPY(MainView)
private:
    XQApplicationManager mAppMgr;
    XQAiwRequest *mRequest;
};

#endif
