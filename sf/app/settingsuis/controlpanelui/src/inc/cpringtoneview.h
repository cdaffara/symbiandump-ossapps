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
#ifndef CPRINGTONEVIEW_H
#define CPRINGTONEVIEW_H

#include "ringtoneviewdef.h"
#include <cpbasesettingview.h>
#include <xqappmgr.h>

class HbListWidget;
class HbListWidgetItem;
class XQAiwRequest;

class CPRINGTONEVIEW_EXPORT CpRingToneView : public CpBaseSettingView
{
    Q_OBJECT
public:
   explicit CpRingToneView( QGraphicsItem *parent = 0 );
   ~CpRingToneView();
signals:
   void selOK( const QString &strFname);
   void selError(  int errorCode, const QString& errorMessage );

private slots:
    void itemActivated( const QModelIndex &index );
    void handleOk(const QVariant &result);
    void handleError(int errorCode, const QString& errorMessage);

private:
    HbListWidget* mToneTypeList;
    XQAiwRequest* mReq;
    XQApplicationManager mAppMgr;
    //used to mark if there is a request being processed
    bool mProcessing;
    
private:    
    void launchMediaFetcher( const QString &strService, const QString &strItface, \
            const QList<QVariant> &arguments = QList<QVariant>(), const XQRequestInfo &info = XQRequestInfo() );
};

#endif // CPRINGTONEVIEW_H
