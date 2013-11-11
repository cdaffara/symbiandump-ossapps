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

#include "MsgAudioFetcherModel.h"
class HbMainWindow;
class HbListView;
class TestMsgAudioFetcherModel:public QObject
    {
    Q_OBJECT
public:
    TestMsgAudioFetcherModel();
    ~TestMsgAudioFetcherModel();
private slots:

    void initTestCase();
    
    void test_Constructor();
    
    void test_init();
    
    void test_addToneFiles();
    
    void test_insertIndex();
    
    void test_clearAll();
    
    void test_queryComplete();
    
    void test_isDRM_data();
    
    void test_isDRM();

    void cleanupTestCase();
private:

    MsgAudioFetcherModel* mMsgAudioFetcherModel;
    HbMainWindow* mTestMainWindow;
    HbListView* mListView;
    };
