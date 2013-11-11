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

#include "msgaudiopreview.h"
class TestMsgAudioPreview:public QObject
    {
    Q_OBJECT
    
public:
    TestMsgAudioPreview();
    ~TestMsgAudioPreview();
private slots:

    void initTestCase();
    
    void test_Constructor();
    
    void test_PlayL_IsPlaying();
    
    void test_Stop();
    
    void cleanupTestCase();

private:
    MsgAudioPreview* mMsgAudioPreview;
    };