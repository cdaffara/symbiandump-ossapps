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
* Description:  Main test class for hsHomeScreenStatePlugin library.
*
*/


#include <hblineedit.h>
#include <hbaction.h>

#include "hscollectionnamedialog.h"
#include "t_hsmenuworkerstateplugin.h"


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void MenuStatesTest::HsCollectionNameDialog_ConstructWithNoParams()
{
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARK;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
    {
        int collCount = 0;

        {
            HsCollectionNameDialog dialog;
            QCOMPARE(dialog.inputMode(), HbInputDialog::TextInput);
            QCOMPARE(dialog.promptText(), hbTrId("txt_applib_title_collection_name"));
            QCOMPARE(dialog.lineEdit()->maxLength(), 248);
            QCOMPARE(dialog.value().toString(), hbTrId("txt_applib_dialog_entry_collection"));
            collCount = dialog.mOtherCollectionsNames.count();
        }

        {

            const int id = HsMenuService::createCollection(QString("TestConstruct"));

            QTest::qWait(3000);

            HsCollectionNameDialog dialog;

            QCOMPARE(dialog.mOtherCollectionsNames.count(), collCount + 1);

            HsMenuService::removeCollection(id);

            QTest::qWait(3000);

        }
    }
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARKEND;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void MenuStatesTest::HsCollectionNameDialog_ConstructWithId()
{
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARK;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
    {
        int id = HsMenuService::createCollection(QString("TestConstruct"));

        QTest::qWait(3000);

        HsCollectionNameDialog dialogItem(id);
        QCOMPARE(dialogItem.value().toString(), HsMenuService::getName(id));
        QCOMPARE(dialogItem.uniqueCollectionName(), dialogItem.value().toString());
        HsMenuService::removeCollection(id);

        QTest::qWait(3000);

    }
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARKEND;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void MenuStatesTest::HsCollectionNameDialog_uniqueCollectionName()
{
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARK;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
    {
        QString testNewName(QString("TestNewName") +
                            QDateTime::currentDateTime().toString("ddmmyyyy_hh_mm_ss_zzz"));

        int id = HsMenuService::createCollection(testNewName);

        QString otherTestName(QString("OtherTestName") +
                            QDateTime::currentDateTime().toString("ddmmyyyy_hh_mm_ss_zzz"));

        int otherId = HsMenuService::createCollection(otherTestName);

        QTest::qWait(3000);

        HsCollectionNameDialog dialog(id);
        
        QCOMPARE(dialog.uniqueCollectionName(), testNewName);

        dialog.setValue(otherTestName);

        QCOMPARE(dialog.uniqueCollectionName().contains("1"), QBool(true) );

        HsMenuService::removeCollection(id);
        HsMenuService::removeCollection(otherId);
    }
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARKEND;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void MenuStatesTest::HsCollectionNameDialog_closeEvent()
{
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARK;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
        {
        mFinishedEmited = false;
        HsCollectionNameDialog *dlg = new HsCollectionNameDialog();
        dlg->open(this, SLOT(testSlot(HbAction*)));
        
        QVERIFY(dlg->testAttribute(Qt::WA_DeleteOnClose));
        
        dlg->close();
        
        QVERIFY(mFinishedEmited);

        }
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARKEND;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
}
