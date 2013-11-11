/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "ut_cpdivertplugin.h"
#include "qtestmains60ui.h"
#include <psetcalldivertingwrapper.h>
#include <psetwrapper.h> 
#include <psuinotes.h> 
#include <sssettingswrapper.h> 
#include <hbdialog.h>
#include <qaction>
#include <hblabel.h>
#include <HbListWidgetItem>
#include <HbListWidget>
#include <HbDialog>
#include <HbLineEdit>
#include <HbAbstractViewItem>
#include <HbDataFormModel>
#include "cpitemdatahelper.h"
#include "cpdivertitemdata.h"
#include "psetwrappertypes.h"

#define private public
#include "cpdivertplugin.h"
#include "cpdivertplugingroup.h"


Q_DECLARE_METATYPE(PsCallDivertingCondition)
Q_EXTERN_C const char * qt_plugin_query_verification_data();
Q_EXTERN_C ::QObject *  qt_plugin_instance();

class CPsetContainer
{
public:
    CPsetContainer(){};
    ~CPsetContainer(){};
};

void fillNumber(QString& number, PsService /*service*/)
{
    number = "1234567";
}
    

/*!
  UT_CpDivertPlugin::UT_CpDivertPlugin
 */
UT_CpDivertPlugin::UT_CpDivertPlugin() 
    : m_divertplugin(NULL)
{
    const char * verificationData = qt_plugin_query_verification_data();
	qRegisterMetaType<PsCallDivertingCondition>(
        "PsCallDivertingCondition");
}

/*!
  UT_CpDivertPlugin::~UT_CpDivertPlugin
 */
UT_CpDivertPlugin::~UT_CpDivertPlugin()
{
    delete m_divertpluginGroup;
    delete m_helper;
    delete m_divertplugin;
}

/*!
  UT_CpDivertPlugin::init
 */
void UT_CpDivertPlugin::init()
{
    initialize();

    CPsetContainer tmpPsetContainer;
    PSetCallDivertingWrapper *tmpDivWrapper = 
            new PSetCallDivertingWrapper(tmpPsetContainer, this);
    EXPECT(PSetWrapper, callDivertingWrapper).returns(tmpDivWrapper); // FIXME to mock framework
    m_divertplugin = (CpDivertPlugin*)qt_plugin_instance();
    
    QVERIFY(m_divertplugin);
    
    m_helper = new CpItemDataHelper;
    QList<CpSettingFormItemData*> list;
    list.append(m_divertplugin->createSettingFormItemData(*m_helper));
    m_divertpluginGroup = qobject_cast<CpDivertPluginGroup *>(list.takeFirst());
    QVERIFY(m_divertpluginGroup);
    
    m_dataForm = new HbDataFormModel;
    m_dataForm->appendDataFormItem(m_divertpluginGroup);
    
    QVERIFY(verify());
}

/*!
  UT_CpDivertPlugin::cleanup
 */
void UT_CpDivertPlugin::cleanup()
{
    reset();
    delete m_dataForm;
    m_dataForm = NULL;
    
    //delete m_divertpluginGroup; // dataForm owned
    m_divertpluginGroup = NULL;

    delete m_helper;
    m_helper = NULL;
    
    delete m_divertplugin;
    m_divertplugin = NULL;

}

/*!
  UT_CpDivertPlugin::t_memleak
 */
void UT_CpDivertPlugin::t_memleak()
{
    
}

/*!
  UT_CpDivertPlugin::t_createSettingFormItemData
 */
void UT_CpDivertPlugin::t_createSettingFormItemData()
{
    CpItemDataHelper itemDataHelper;
    CPsetContainer tmpContainer;
    PSetCallDivertingWrapper tmpDivWrapper(tmpContainer); 
    
    EXPECT(PSetWrapper, callDivertingWrapper).returns(&tmpDivWrapper); 
    
    CpDivertPlugin* p = (CpDivertPlugin*)qt_plugin_instance(); // Get static
    QList<CpSettingFormItemData*> list;
    list.append(p->createSettingFormItemData(itemDataHelper));
    qDeleteAll(list);
    QVERIFY(verify());
}

/*!
  UT_CpDivertPlugin::t_changeDivertingStateRequested
 */
void UT_CpDivertPlugin::t_changeDivertingStateRequested()
{
    //except user cancels
    
    appendAction("txt_phone_title_all_calls", selectAction, "Cancel");
    EXPECT(PSetCallDivertingWrapper, getDefaultNumbers);
    m_divertpluginGroup->m_DataItemVoiceAllCalls->setContentWidgetData("text", "");
    m_divertpluginGroup->m_DataItemVoiceAllCalls->setContentWidgetData(
        "checkState", Qt::Checked);
    m_divertpluginGroup->m_DataItemVoiceAllCalls->thisItemClicked();
    waitForQueueEmpty();
    QVERIFY(verify());
    
    //except user selects vmb
    appendAction("txt_phone_title_all_calls", selectItem, "txt_phone_list_to_voice_mailbox");
    EXPECT(PSetCallDivertingWrapper, getDefaultNumbers);
    EXPECT(PSetCallDivertingWrapper, queryVoiceMailBoxNumber).willOnce(invoke(fillNumber)).returns(0);
    EXPECT(SsSettingsWrapper, get);
    EXPECT(PSetCallDivertingWrapper, setCallDiverting);
    EXPECT(PsUiNotes, noteShowing).returns(false);
    EXPECT(PsUiNotes, showGlobalProgressNote);
    m_divertpluginGroup->m_DataItemVoiceAllCalls->setContentWidgetData(
        "checkState", Qt::Checked);
    m_divertpluginGroup->m_DataItemVoiceAllCalls->thisItemClicked();
    waitForQueueEmpty();
    QVERIFY(verify()); // Verify item click
    PSCallDivertingCommand command;
    command.iNumber = "12345";
    command.iStatus = DivertingStatusActive;
    command.iCondition = qvariant_cast<PsCallDivertingCondition>(
            m_divertpluginGroup->m_DataItemVoiceAllCalls->property("condition"));
    command.iServiceGroup = ServiceGroupVoice;
    EXPECT(PsUiNotes, showNotificationDialog);
    m_divertpluginGroup->handleDivertingChanged(command, false);
    EXPECT(PsUiNotes,cancelNote);
    m_divertpluginGroup->divertRequestProcessed();
    QVERIFY(verify()); // Verify result processing

    //except user selects one of the default numbers
    m_divertpluginGroup->m_DataItemVoiceAllCalls->setContentWidgetData("text", "");
    m_divertpluginGroup->m_DataItemVoiceAllCalls->setContentWidgetData(
        "checkState", Qt::Checked);
    appendAction("txt_phone_title_all_calls", selectItem, "0401234567");
    EXPECT(PSetCallDivertingWrapper,getDefaultNumbers);
    EXPECT(SsSettingsWrapper,get);
    EXPECT(PSetCallDivertingWrapper,setCallDiverting);
    EXPECT(PsUiNotes, noteShowing).returns(false);
    EXPECT(PsUiNotes, showGlobalProgressNote);
    m_divertpluginGroup->m_DataItemVoiceAllCalls->thisItemClicked();
    waitForQueueEmpty();
    command.iNumber = "0401234567";
    command.iStatus = DivertingStatusActive;
    command.iCondition = qvariant_cast<PsCallDivertingCondition>(
            m_divertpluginGroup->m_DataItemVoiceAllCalls->property("condition"));
    command.iServiceGroup = ServiceGroupVoice;
    EXPECT(PsUiNotes, cancelNote);
    EXPECT(PsUiNotes, showNotificationDialog);
    EXPECT(PSetCallDivertingWrapper, setNewDefaultNumber).with(QString("0401234567"));
    m_divertpluginGroup->handleDivertingChanged(command, true);
    EXPECT(PsUiNotes, cancelNote);
    m_divertpluginGroup->divertRequestProcessed();
    QVERIFY(verify());
    
    // Divert is disabled
    EXPECT(SsSettingsWrapper, get);
    EXPECT(PSetCallDivertingWrapper, setCallDiverting); // Disable divert
    EXPECT(PsUiNotes, noteShowing).returns(false);
    EXPECT(PsUiNotes, showGlobalProgressNote);
    m_divertpluginGroup->m_DataItemVoiceAllCalls->setContentWidgetData(
        "checkState", Qt::Unchecked);
    m_divertpluginGroup->m_DataItemVoiceAllCalls->thisItemClicked();
    waitForQueueEmpty();
    command.iNumber = "";
    command.iStatus = DivertingStatusInactive;
    command.iCondition = qvariant_cast<PsCallDivertingCondition>(
            m_divertpluginGroup->m_DataItemVoiceAllCalls->property("condition"));
    command.iServiceGroup = ServiceGroupVoice;
    EXPECT(PsUiNotes, showNotificationDialog);
    m_divertpluginGroup->handleDivertingChanged(command, false);
    //EXPECT(PsUiNotes, cancelNote);
    m_divertpluginGroup->divertRequestProcessed();
    QVERIFY(verify());
    
    //except user selects other number, inserts number and cancels
    /* BUG in framework (Crash in QGestureManager::getState due to QWeakPointer) */
    /*
    appendAction("All voice calls:", selectItem, "Other number");
    appendAction("Number:", insertText, "12345");
    appendAction("Number:", selectAction, "Cancel");
    EXPECT(PSetCallDivertingWrapper, getDefaultNumbers);
    m_divertpluginGroup->m_DataItemVoiceAllCalls->thisItemClicked();
    waitForQueueEmpty();
    QVERIFY(verify());
    */
}

/*!
  UT_CpDivertPlugin::t_itemShown
 */
void UT_CpDivertPlugin::t_itemShown()
{
    EXPECT(PSetCallDivertingWrapper, getCallDivertingStatus);
    EXPECT(PsUiNotes, noteShowing).returns(false);
    EXPECT(PsUiNotes, showGlobalProgressNote);
    m_divertpluginGroup->itemShown(
            m_dataForm->indexFromItem(m_divertpluginGroup->m_DataItemVoiceAllCalls));
    m_divertpluginGroup->itemShown(
            m_dataForm->indexFromItem(m_divertpluginGroup->m_DataItemVoiceIfBusy));
    m_divertpluginGroup->itemShown(
            m_dataForm->indexFromItem(m_divertpluginGroup->m_DataItemVoiceIfNotAnswered));
    m_divertpluginGroup->itemShown(
            m_dataForm->indexFromItem(m_divertpluginGroup->m_DataItemVoiceIfOutOfReach));
    m_divertpluginGroup->itemShown(
            m_dataForm->indexFromItem(m_divertpluginGroup->m_DataItemVoiceIfNotAvailable));
    
    QList<PSCallDivertingStatus*> list;
    PSCallDivertingStatus divertStatus;
    list.append(&divertStatus);
    divertStatus.iCondition = qvariant_cast<PsCallDivertingCondition>(
            m_divertpluginGroup->m_DataItemVoiceAllCalls->property("condition"));
    m_divertpluginGroup->handleDivertingStatus(list, true);
    m_divertpluginGroup->divertRequestProcessed();
    divertStatus.iCondition = qvariant_cast<PsCallDivertingCondition>(
            m_divertpluginGroup->m_DataItemVoiceIfBusy->property("condition"));
    m_divertpluginGroup->handleDivertingStatus(list, true);
    m_divertpluginGroup->divertRequestProcessed();
    divertStatus.iCondition = qvariant_cast<PsCallDivertingCondition>(
            m_divertpluginGroup->m_DataItemVoiceIfNotAnswered->property("condition"));
    m_divertpluginGroup->handleDivertingStatus(list, true);
    m_divertpluginGroup->divertRequestProcessed();
    divertStatus.iCondition = qvariant_cast<PsCallDivertingCondition>(
            m_divertpluginGroup->m_DataItemVoiceIfOutOfReach->property("condition"));
    m_divertpluginGroup->handleDivertingStatus(list, true);
    m_divertpluginGroup->divertRequestProcessed();
    divertStatus.iCondition = qvariant_cast<PsCallDivertingCondition>(
            m_divertpluginGroup->m_DataItemVoiceIfNotAvailable->property("condition"));
    m_divertpluginGroup->handleDivertingStatus(list, true);
    m_divertpluginGroup->divertRequestProcessed();
    
    m_divertpluginGroup->divertRequestProcessed(); // Test overflow

    // Test, Do not check status again
    m_divertpluginGroup->itemShown(
            m_dataForm->indexFromItem(m_divertpluginGroup->m_DataItemVoiceAllCalls));
    m_divertpluginGroup->itemShown(
            m_dataForm->indexFromItem(m_divertpluginGroup->m_DataItemVoiceIfBusy));
    m_divertpluginGroup->itemShown(
            m_dataForm->indexFromItem(m_divertpluginGroup->m_DataItemVoiceIfNotAnswered));
    m_divertpluginGroup->itemShown(
            m_dataForm->indexFromItem(m_divertpluginGroup->m_DataItemVoiceIfOutOfReach));
    m_divertpluginGroup->itemShown(
            m_dataForm->indexFromItem(m_divertpluginGroup->m_DataItemVoiceIfNotAvailable));
    

    QVERIFY(verify());
}

/*!
  UT_CpDivertPlugin::t_popUpTimerQuery
 */
void UT_CpDivertPlugin::t_popUpTimerQuery()
{
    const QString delayLnString("txt_phone_list_ln_seconds");
    
    appendAction("txt_phone_title_if_not_answered", selectItem, "txt_phone_list_enter_number_manually");
    appendAction("txt_phone_info_number", insertText, "12345");
    appendAction("txt_phone_info_number", selectAction, "OK");
    appendAction("txt_phone_title_delay", selectItem, delayLnString);
    // expect user chooses other number and inserts number and timeout
    EXPECT(PSetCallDivertingWrapper, getDefaultNumbers);    
    EXPECT(SsSettingsWrapper, get);
    EXPECT(PSetCallDivertingWrapper, setCallDiverting);
    m_divertpluginGroup->m_DataItemVoiceIfNotAnswered->setContentWidgetData(
        "checkState", Qt::Checked);
    m_divertpluginGroup->m_DataItemVoiceIfNotAnswered->thisItemClicked();
    waitForQueueEmpty();
    PSCallDivertingCommand command;
    command.iNumber = "12345";
    command.iNoReplyTimer = 15;
    command.iStatus = DivertingStatusActive;
    command.iCondition = qvariant_cast<PsCallDivertingCondition>(
            m_divertpluginGroup->m_DataItemVoiceIfNotAnswered->property("condition"));
    command.iServiceGroup = ServiceGroupVoice;
    EXPECT(PsUiNotes, showNotificationDialog);
    m_divertpluginGroup->handleDivertingChanged(command, false);
    EXPECT(PsUiNotes, cancelNote);
    m_divertpluginGroup->divertRequestProcessed();
    QVERIFY(verify());
    
    EXPECT(SsSettingsWrapper, get);
    EXPECT(PSetCallDivertingWrapper, setCallDiverting);
    m_divertpluginGroup->m_DataItemVoiceIfNotAnswered->setContentWidgetData(
        "checkState", Qt::Unchecked);
    m_divertpluginGroup->m_DataItemVoiceIfNotAnswered->thisItemClicked();
    waitForQueueEmpty();
    command.iNumber = "";
    command.iNoReplyTimer = 0;
    command.iStatus = DivertingStatusInactive;
    command.iCondition = qvariant_cast<PsCallDivertingCondition>(
            m_divertpluginGroup->m_DataItemVoiceIfNotAnswered->property("condition"));
    command.iServiceGroup = ServiceGroupVoice;
    EXPECT(PsUiNotes, showNotificationDialog);
    m_divertpluginGroup->handleDivertingChanged(command, false);
    EXPECT(PsUiNotes, cancelNote);
    m_divertpluginGroup->divertRequestProcessed();
    QVERIFY(verify());

}

/*!
  UT_CpDivertPlugin::t_handleDivertingChanged
 */
void UT_CpDivertPlugin::t_handleDivertingChanged()
{
    PSCallDivertingCommand c;
    
    c.iStatus = DivertingStatusActive;
    c.iServiceGroup = ServiceGroupAllTeleservices;
    c.iNumber = QString("0401234567890");
    EXPECT(PSetCallDivertingWrapper, setNewDefaultNumber);
    m_divertpluginGroup->handleDivertingChanged(c, true);
    
    QVERIFY(verify());
    
    c.iServiceGroup = ServiceGroupAllTeleservices;
    c.iCondition = DivertConditionUnconditional;
    c.iNumber = QString("0401234567890");
    EXPECT(PSetCallDivertingWrapper, setNewDefaultNumber);
    m_divertpluginGroup->handleDivertingChanged(c, false);
    
    QVERIFY(verify());
    
    c.iStatus = DivertingStatusInactive;
    c.iCondition = DivertConditionBusy;
    m_divertpluginGroup->handleDivertingChanged(c, true);
    m_divertpluginGroup->handleDivertingChanged(c, false);
    
    c.iStatus = DivertingStatusNotRegistered;
    c.iCondition = DivertConditionNoReply;
    m_divertpluginGroup->handleDivertingChanged(c, true);
    
    c.iStatus = DivertingStatusNotProvisioned;
    c.iCondition = DivertConditionNotReachable;
    m_divertpluginGroup->handleDivertingChanged(c, true);
    
    c.iStatus = DivertingStatusUnknown;
    c.iCondition = DivertConditionAllCalls;
    m_divertpluginGroup->handleDivertingChanged(c, true);
    
    c.iStatus = (PsCallDivertingStatus)5;
    c.iCondition = DivertConditionAllConditionalCases;
    m_divertpluginGroup->handleDivertingChanged(c, true);
    
    QVERIFY(verify());
}

/*!
  UT_CpDivertPlugin::t_handleDivertingStatus
 */
void UT_CpDivertPlugin::t_handleDivertingStatus()
{
    QList<PSCallDivertingStatus*> list;
    PSCallDivertingStatus divertStatus;
    
    divertStatus.iTimeout =5;
    divertStatus.iStatus = DivertingStatusActive;
    list.append(&divertStatus);
    m_divertpluginGroup->handleDivertingStatus(list, true);
    divertStatus.iTimeout =0;
    list.replace(0, &divertStatus);
    m_divertpluginGroup->handleDivertingStatus(list, true);
    
    divertStatus.iStatus = DivertingStatusInactive;
    list.replace(0, &divertStatus);
    m_divertpluginGroup->handleDivertingStatus(list, true);
    
    divertStatus.iStatus = DivertingStatusNotRegistered;
    list.replace(0, &divertStatus);
    m_divertpluginGroup->handleDivertingStatus(list, true);
    m_divertpluginGroup->handleDivertingStatus(list, false);
    
    divertStatus.iStatus = DivertingStatusNotProvisioned;
    list.replace(0, &divertStatus);
    m_divertpluginGroup->handleDivertingStatus(list, true);
    
    divertStatus.iStatus = DivertingStatusUnknown;
    list.replace(0, &divertStatus);
    m_divertpluginGroup->handleDivertingStatus(list, true);
    
    divertStatus.iStatus = (PsCallDivertingStatus)5;
    list.replace(0, &divertStatus);
    m_divertpluginGroup->handleDivertingStatus(list, true);

}

/*!
  UT_CpDivertPlugin::t_handleDivertingError
 */
void UT_CpDivertPlugin::t_handleDivertingError()
{
    m_divertpluginGroup->handleDivertingError(-1);
}

/*!
  UT_CpDivertPlugin::appendAction
 */
void UT_CpDivertPlugin::appendAction(
        const QString& dialog, actionType actionType, const QString& action)
{
    if (!actionQueue.count()) {
        startTimer(2000);
    }
    qDebug() << "appendAction(" << dialog << actionType << action <<")";
    actionQueue.append(new dialogAction(dialog, actionType, action));
}

/*!
  UT_CpDivertPlugin::executeAction
 */
void UT_CpDivertPlugin::executeAction( const dialogAction &action )
{
    qDebug() << "executeAction(" << action.dialog << action.type << action.item <<")";
    switch (action.type) {
    case selectAction:
        doAndVerifyAction(action.dialog, action.item);
        break;
    case selectItem:
        selectItemFromListWidget(action.dialog, action.item);
        break;
    case insertText:
        HbDialog *dialog = visibleDialog(action.dialog);
        QVERIFY( dialog );
        HbLineEdit* editor = qobject_cast<HbLineEdit*>(
                dialog->contentWidget() );
        QVERIFY( editor );
        editor->setText(action.item);
        break;
    default:
        break;
    }
}

/*!
  UT_CpDivertPlugin::visibleDialog
 */
HbDialog *UT_CpDivertPlugin::visibleDialog( const QString &heading )
{
    QList<QGraphicsItem*> items = mainWindow->scene()->items();
    foreach (QGraphicsItem* item, items) {
        HbDialog *w = qobject_cast<HbDialog*>(item->parentWidget());
        if (w && w->isVisible() && ( qobject_cast<HbLabel*>(w->headingWidget())->plainText() == heading )) {
            qDebug() << "visibleDialog: " << w->getStaticMetaObject().className() <<
                    "contentWidget: " << w->contentWidget()->getStaticMetaObject().className();
            return w;
        }
    }
    
    return 0;
}

/*!
  UT_CpDivertPlugin::selectItemFromListWidget
 */
void UT_CpDivertPlugin::selectItemFromListWidget(
        const QString& dialog, const QString& item )
{
    HbDialog* d = visibleDialog(dialog);
    QVERIFY(d);
    HbListWidget *list = qobject_cast<HbListWidget*>(d->contentWidget());
    QVERIFY(list);
    
    bool ok=false;
    HbListWidgetItem *itemObject = 0;
    for (int i=0; i < list->count();i++) {
        itemObject = list->item(i);
        if (itemObject->text() == item) {
            ok=true;
            QSignalSpy spy( d, SIGNAL(aboutToClose()));
            list->setCurrentItem(itemObject);
            d->close();
            qDebug() << "selectItemFromListWidget: " << itemObject->text();
            while (!spy.count()) {
                QTest::qWait(50);
            }
            QTest::qWait(50);
            break;
        }
        
    }
    
    QVERIFY(ok);
}

/*!
  UT_CpDivertPlugin::doAndVerifyAction
 */
void UT_CpDivertPlugin::doAndVerifyAction(
        const QString& dialog, const QString& action )
{
    HbDialog* d = visibleDialog(dialog);
    QVERIFY(d);
    QAction* o;
    bool ok=false;
    foreach (o, d->actions()) {
        if (o->text() == action) {
            ok=true;
            QSignalSpy spy( d, SIGNAL(aboutToClose()));
            o->trigger();
            qDebug() << "doAndVerifyAction: " << o->text();
            while (!spy.count()) {
                QTest::qWait(50);
            }
            QTest::qWait(50);
            break;
        }
    }
    QVERIFY(ok);
}

/*!
  UT_CpDivertPlugin::timerEvent
 */
void UT_CpDivertPlugin::timerEvent(
        QTimerEvent* event )
{
    QString currentTest(QTest::currentTestFunction());
    qDebug() << "timerEvent:" << currentTest;
    killTimer(event->timerId());
    executeAction(*actionQueue.takeFirst());
    if (actionQueue.count()) {
        startTimer(1000);
    } else {
        emit queueEmpty();
    }

    qDebug() << "timerEvent, OUT";
}

/*!
  UT_CpDivertPlugin::waitForQueueEmpty
 */
void UT_CpDivertPlugin::waitForQueueEmpty()
{
    if (actionQueue.count()) {
        QSignalSpy spy(this, SIGNAL(queueEmpty()));
        while (spy.count() == 0)
            QTest::qWait(200);
    }
}

QTEST_MAIN_S60UI(UT_CpDivertPlugin)
