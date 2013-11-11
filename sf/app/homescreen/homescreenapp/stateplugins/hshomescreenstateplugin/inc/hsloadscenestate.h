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

#ifndef HSLOADSCENESTATE_H
#define HSLOADSCENESTATE_H

#include <QState>
#ifdef Q_OS_SYMBIAN
#include <XQSettingsKey>

class XQSettingsManager;
#endif

class HsLoadSceneState : public QState
{
    Q_OBJECT

public:
    HsLoadSceneState(QState *parent = 0);
    ~HsLoadSceneState();
#ifdef Q_OS_SYMBIAN
public slots:
     void handleKeyChange(XQSettingsKey key, const QVariant &value);
#endif
signals:
    void event_history();

private slots:
    void action_loadScene();

private:
    Q_DISABLE_COPY(HsLoadSceneState)
    void showUi();
    
#ifdef Q_OS_SYMBIAN
    XQPublishAndSubscribeSettingsKey    mStartupKey;
    XQSettingsManager                   *mSettingsMgr;
#endif
};

#endif // HSLOADSCENESTATE_H
