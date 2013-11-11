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

#include "unittest_msgaudiopreview.h"
#include <QtTest/QtTest>
#include <HbMainWindow>
#include <XQConversions>
#include "msguitest.h"

#include <audiopreference.h>
TestMsgAudioPreview::TestMsgAudioPreview()
    {
    
    }
TestMsgAudioPreview::~TestMsgAudioPreview()
    {
    
    }

void TestMsgAudioPreview::initTestCase()
    {
        mMsgAudioPreview = new MsgAudioPreview(this);
        QVERIFY(mMsgAudioPreview != NULL);
    }
    
void TestMsgAudioPreview::test_Constructor()
    {
        QVERIFY(mMsgAudioPreview->iMediaType == KErrNotFound);
        QVERIFY(mMsgAudioPreview->iRingingVolume == KErrNotFound);
        QVERIFY(mMsgAudioPreview->iRingingType == KErrNotFound);
        QVERIFY(mMsgAudioPreview->iVibra == KErrNotFound);
        QVERIFY(mMsgAudioPreview->i3DEffect == KErrNotFound);
        QVERIFY(mMsgAudioPreview->i3DEcho == KErrNotFound);
        QVERIFY(mMsgAudioPreview->iFileSize == KErrNotFound);    
        QVERIFY(mMsgAudioPreview->iFullName == 0);    
        QVERIFY(mMsgAudioPreview->iActiveProfileRead == EFalse);
        QVERIFY(mMsgAudioPreview->iPlayerStatus == MsgAudioPreview::EPlayerNotCreated);
        QVERIFY(mMsgAudioPreview->iAudioPlayerStatus == MsgAudioPreview::EPlayerNotCreated);
        QVERIFY(mMsgAudioPreview->iTonePlayerStatus == MsgAudioPreview::EPlayerNotCreated);
    }

void TestMsgAudioPreview::test_PlayL_IsPlaying()
    {
        QString str("c:\\data\\testAudioFetcher\\drm.amr");
        mMsgAudioPreview->SetAttr(str);
        QVERIFY(XQConversions::s60DescToQString( mMsgAudioPreview->iFullName->Des()) == str);
        mMsgAudioPreview->Play();
        QVERIFY(mMsgAudioPreview->IsPlaying() == true);
    }

void TestMsgAudioPreview::test_Stop()
    {
        mMsgAudioPreview->Stop();
        QVERIFY(mMsgAudioPreview->iAudioPlayer == 0);
        QVERIFY(mMsgAudioPreview->iAudioPlayerStatus == MsgAudioPreview::EPlayerNotCreated);
        QVERIFY(mMsgAudioPreview->iTonePlayer == 0);
        QVERIFY(mMsgAudioPreview->iTonePlayerStatus == MsgAudioPreview::EPlayerNotCreated);
    }

    
void TestMsgAudioPreview::cleanupTestCase()
    {
        if(mMsgAudioPreview)
            {
                delete mMsgAudioPreview;
            }        
    }
MSGUI_TEST_MAIN(TestMsgAudioPreview)
