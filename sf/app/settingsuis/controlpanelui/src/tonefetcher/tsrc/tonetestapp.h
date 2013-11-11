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
 *     The header file for tone service test.
 *     
 */
#ifndef TONETESTAPP_H
#define TONETESTAPP_H

#include <hbview.h>
#include <xqappmgr.h>

class XQApplicationManager;
class XQAiwRequest;
class HbLineEdit;

class ToneTestApp : public HbView
{
    Q_OBJECT

public:
    ToneTestApp(QGraphicsItem *parent=0);
    virtual ~ToneTestApp();

private slots:
    void handleOk(const QVariant &result);
    void handleError(int errorCode, const QString& errorMessage);
    
    void fetchTone();
    
private:
    void createLayout();
    
private:
    XQApplicationManager mAppMgr;
    XQAiwRequest* mReq;
    HbLineEdit* mResultEdit;
    HbLineEdit* mErrorCodeEdit;
    HbLineEdit* mErrorEdit;
};

#endif // TONETESTAPP_H
