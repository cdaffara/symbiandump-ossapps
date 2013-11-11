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
 * Description:  Item decorator of the message list view.
 *
 */

#include "msgaudioselectionengine.h"
class TestMsgAudioSelectionEngine:public QObject
    {
    Q_OBJECT
public:
    TestMsgAudioSelectionEngine();
    ~TestMsgAudioSelectionEngine();
private slots:
    void initTestCase();
    void test_Constructor();
    void test_CreateMetaDataSession();
    void test_QueryTones();
    void test_SetAttr();
    void test_PropertyDefL();
    void test_HandleObjectNotification();
    void test_HandleObjectPresentNotification();
    void test_HandleSessionOpened();
    void test_HandleSessionError();    
    void cleanupTestCase();
private:
    MsgAudioSelectionEngine* mMsgAudioSelectionEngine;
    };
 
