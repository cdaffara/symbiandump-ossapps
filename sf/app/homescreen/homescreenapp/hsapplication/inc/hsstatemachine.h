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
* Description:  Default implementation of the home screen runtime.
*
*/

#ifndef HSSTATEMACHINE_H
#define HSSTATEMACHINE_H

#include <QStateMachine>
#include <qmobilityglobal.h>

#ifdef Q_OS_SYMBIAN
#include <XQKeyCapture>
#endif

#include "hstest_global.h"

class HsContentService;
QTM_BEGIN_NAMESPACE
class QValueSpacePublisher;
QTM_END_NAMESPACE

QTM_USE_NAMESPACE

HOMESCREEN_TEST_CLASS(t_hsapplication)

class HsStateMachine : public QStateMachine
{
    Q_OBJECT

public:
    HsStateMachine(QObject *parent = 0);
    ~HsStateMachine();

signals:
    void event_exit();
    void event_toIdle();
    void event_toAppLib();
protected:
    bool eventFilter(QObject *watched, QEvent *event);

private:
    Q_DISABLE_COPY(HsStateMachine)

    void registerAnimations();
    void createStatePublisher();
    void createContentServiceParts();
    void createStates();
	void updatePSKeys();
    void captureEndKey(bool enable); 

private slots:
	void onIdleStateEntered();
	void onIdleStateExited();
	void activityRequested(const QString &name);
	

private:
    HsContentService *mContentService;	
	
    bool mHomeScreenActive;
    bool mIdleStateActive;
    bool mEndKeyCaptured;
    
	QValueSpacePublisher *mPublisher;
	
#ifdef Q_OS_SYMBIAN
    XQKeyCapture keyCapture;
#endif
    
    HOMESCREEN_TEST_FRIEND_CLASS(t_hsapplication)
};

#endif
