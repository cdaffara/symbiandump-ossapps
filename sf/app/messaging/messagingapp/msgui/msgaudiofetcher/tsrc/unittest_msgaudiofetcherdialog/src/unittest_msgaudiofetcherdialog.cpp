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

#include "unittest_msgaudiofetcherdialog.h"
#include <QtTest/QtTest>
#include <HbTextItem>
#include <HbMainWindow>
#include <HbColorScheme>
#include <HbAction>
#include <QtTest/QSignalSpy>
#include <Qstring>
#include "msguitest.h"
#define LOC_SELECT_SELECT hbTrId("txt_common_button_select")
const QString LIST_ITEM_TITLE("qtc_list_item_title_normal");
TestMsgAudioFetcherDialog::TestMsgAudioFetcherDialog(): mAudioFetcherDialog(0),mTestMainWindow(0)
    {

    }
TestMsgAudioFetcherDialog::~TestMsgAudioFetcherDialog()
    {
    
    }
void TestMsgAudioFetcherDialog::initTestCase()
    {
        mTestMainWindow = new HbMainWindow();
        mAudioFetcherDialog = new MsgAudioFetcherDialog();
        mAudioFetcherDialog->show();
    }

void TestMsgAudioFetcherDialog::test_Constructor()
    {
        QVERIFY(mAudioFetcherDialog->dismissPolicy() == HbDialog::NoDismiss);
        QVERIFY(mAudioFetcherDialog->frameType() == HbDialog::Strong);
        QVERIFY(mAudioFetcherDialog->isModal()==true);    
    }

void TestMsgAudioFetcherDialog::test_initMainWidget()
    {
        HbTextItem* heading =  static_cast<HbTextItem*>(mAudioFetcherDialog->headingWidget());
        QVERIFY(heading != NULL);
        QVERIFY(heading->textColor() == HbColorScheme::color( LIST_ITEM_TITLE ));
        QVERIFY(heading->alignment() == Qt::AlignLeft);      
    }

void TestMsgAudioFetcherDialog::test_initActions()
    {
        QVERIFY(mAudioFetcherDialog->mLeftAction != NULL);
        QVERIFY(mAudioFetcherDialog->mLeftAction->text() == LOC_SELECT_SELECT);
    }

void TestMsgAudioFetcherDialog::test_doDelayedConstruction()
{ 
    QVERIFY(mAudioFetcherDialog->mListView != NULL);
    QVERIFY(mAudioFetcherDialog->mListView->selectionMode() == HbAbstractItemView::SingleSelection);
    QVERIFY(mAudioFetcherDialog->mFetcherModel != NULL);
    QVERIFY(long((mAudioFetcherDialog->mListView->model())) == long(mAudioFetcherDialog->mFetcherModel));
    QVERIFY(mAudioFetcherDialog->mListView->uniformItemSizes() == TRUE);
    QVERIFY(mAudioFetcherDialog->contentWidget() == mAudioFetcherDialog->mListView);
    QVERIFY(mAudioFetcherDialog->mAudioPreview != NULL);    
}



void TestMsgAudioFetcherDialog::test_onSelectAction()
    {  
        QSignalSpy audioSelect(mAudioFetcherDialog, SIGNAL(audioSelected(QString&)));
        QCOMPARE( audioSelect.count(), 0 );
        mAudioFetcherDialog->onSelectAction();
        QCOMPARE( audioSelect.count(), 1 );        
    }

void TestMsgAudioFetcherDialog::test_reset()
    {
        mAudioFetcherDialog->reset();
        QVERIFY(mAudioFetcherDialog->mSelected == false);
        QVERIFY(mAudioFetcherDialog->mLeftAction->isEnabled() == false);
    }
void TestMsgAudioFetcherDialog::cleanupTestCase()
    {
        if(mAudioFetcherDialog){
            delete mAudioFetcherDialog;
            mAudioFetcherDialog = NULL;
        }
        if (mTestMainWindow) {
            delete mTestMainWindow;
            mTestMainWindow = NULL;
        }
    }

MSGUI_TEST_MAIN(TestMsgAudioFetcherDialog)
