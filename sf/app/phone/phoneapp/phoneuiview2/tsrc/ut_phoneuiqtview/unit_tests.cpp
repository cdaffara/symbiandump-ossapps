/*!
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
* Description:  Unit tests for PhoneUIQtView.
*
*/

#include <QtTest/QtTest>
#include <QtGui>
#include <QVariant>
#include <QKeyEvent>
#include <QTimer>
#include <QShortcutEvent>
#include <QSignalSpy>
#include <hbapplication.h>
#include <hbmainwindow.h>
#include <hbinstance.h>
#include <hbmainwindow.h>
#include <bubblemanagerif.h>
#define protected public
#define private public
#include "phoneuiqtview.h"
#undef public
#include "phoneaction.h"
#include <hbtoolbar.h>
#include <hbvolumesliderpopup.h>
#include <hbaction.h>
#include <dialpad.h>
#include <hblineedit.h>
#include "xqserviceutil.h"
#include "phoneaction.h"

//CONSTANTS
static const int KMAXVOLUME = 10;
static const int KMINVOLUME = 0;
static const int KVOLUMECOMMAND = 5;

bool m_qtimer_stop_called;
QString m_networkName;
bool m_take_menu_called;
bool m_set_menu_called;

#define PHONE_QT_VIEW_TEST_MAIN(TestObject) \
int main(int argc, char *argv[]) \
{ \
    HbApplication app(argc, argv); \
    TestObject tc; \
    QResource::registerResource("../hbcore.rcc"); \
    int ret = QTest::qExec(&tc, argc, argv); \
    /* Core dump if HbIconLoader instance is not destroyed before the application instance. */ \
    /* HbIconLoader uses QCoreApplication::aboutToQuit() signal to destroy itself. */ \
    /* app.exec() where the signal is normally emitted is not called here. */ \
    /* So, invoking the signal explicitly. */ \
    QMetaObject::invokeMethod(&app, "aboutToQuit", Qt::DirectConnection); \
    return ret; \
}

class TestPhoneUIQtView : public QObject
{
    Q_OBJECT
    
public:
    TestPhoneUIQtView ();
    ~TestPhoneUIQtView ();


public slots:
    void initTestCase ();
    void cleanupTestCase (); 
    void init ();
    void cleanup ();
    
private slots:
    void testAddBubbleCommand ();
    void testHideToolbar ();
    void testShowToolbar ();
    void testAddToolbarActions ();
    void testSetVolumeSliderValue ();
    void testRemoveVolumeSlider ();
    void testVolumeSliderValue ();
    void testVolumeSliderChanged ();
    void testParticipantListAction ();
    void testSetExpandAction ();
    void testPhoneAction ();
    void testSetOrientation ();
    void testBackButtonClicked();
    void testShowDialpad();
    void testDialpadText();
    void testSetMenuActions();
    void testLongEndKeyPressEventOutsideTelephony();
    void testNetworkNameChanged();
    void testCaptureKey();
    void testRestrictedMode();


private:
    int createCallHeader();

private:
    PhoneUIQtView *m_view; // class under test
    HbMainWindow *m_main_window;

};

void QTimer::stop()
{
    m_qtimer_stop_called = true;   
}
    
TestPhoneUIQtView::TestPhoneUIQtView ()
{    
}

TestPhoneUIQtView::~TestPhoneUIQtView ()
{
}

void TestPhoneUIQtView::initTestCase ()
{	
    m_main_window = new HbMainWindow();
    m_view = new PhoneUIQtView(*m_main_window);
    QCOMPARE(m_networkName, QString("Unit test network"));
}

void TestPhoneUIQtView::cleanupTestCase ()
{
    delete m_view;
}

void TestPhoneUIQtView::init ()
{
    m_qtimer_stop_called = false;
    m_take_menu_called = false;
    m_set_menu_called = false;
}

void TestPhoneUIQtView::cleanup ()
{
}

void TestPhoneUIQtView::testAddBubbleCommand ()
{
    m_view->bubbleManager ().startChanges ();
    int bubbleId = createCallHeader();
    int bubbleId2 = createCallHeader();
    
    PhoneAction action1;
    action1.setText("Command 1");
    action1.setCommand(1);
    action1.setActionRole(PhoneAction::None);
    
    PhoneAction action2;
    action2.setText("Command 2");
    action2.setCommand(2);
    action2.setActionRole(PhoneAction::Accept);
    
    PhoneAction action3;
    action3.setText("Command 3");
    action3.setCommand(3);
    action3.setActionRole(PhoneAction::Decline);
    
    m_view->addBubbleCommand (bubbleId, action1);
    m_view->addBubbleCommand (bubbleId, action2);
    m_view->addBubbleCommand (bubbleId2, action3);
    m_view->bubbleManager ().endChanges ();
    
    m_view->bubbleManager ().startChanges ();
    m_view->clearBubbleCommands (bubbleId);
    m_view->clearBubbleCommands (bubbleId2);
    m_view->clearBubbleCommands (bubbleId2);
    m_view->bubbleManager ().endChanges ();
}

void TestPhoneUIQtView::testHideToolbar ()
{
    m_view->hideToolbar ();
    QCOMPARE (m_hideCalled, true);	
}

void TestPhoneUIQtView::testShowToolbar ()
{
    m_view->showToolbar ();
    QCOMPARE (m_showCalled, true);
}

void TestPhoneUIQtView::testAddToolbarActions ()
{
    m_actions.clear();
    HbIcon icon ("foo.jpg");
    PhoneAction *testAction = new PhoneAction ();
    testAction->setIcon(icon);
    testAction->setText("foo");
    testAction->setCommand(20);
    QList<PhoneAction*> actions;
    actions.append(testAction);
    
    m_view->setToolbarActions(actions);
    QCOMPARE (m_action->icon(), icon);
    QCOMPARE(true, m_toolBarUpdateCalled);
    
    m_view->setToolbarActions(actions);
    
    m_toolBarUpdateCalled = false;
    PhoneAction *testAction2 = new PhoneAction ();
    testAction2->setIcon(icon);
    testAction2->setText("foo2");
    testAction2->setCommand(21);
    actions.append(testAction2);
    
    m_view->setToolbarActions(actions);
    QCOMPARE(true, m_toolBarUpdateCalled);
    actions.removeOne(testAction2);
    
    m_toolBarUpdateCalled = false;
    m_view->setToolbarActions(actions);
    QCOMPARE(true, m_toolBarUpdateCalled);
    
    actions.clear();
    delete testAction;
    delete testAction2;
}

void TestPhoneUIQtView::testSetVolumeSliderValue ()
{
    m_view->setVolumeSliderValue (1, KVOLUMECOMMAND, KMAXVOLUME, KMINVOLUME);
    QCOMPARE (m_volumeSliderContructorCalled, true);
    QCOMPARE (m_sliderRangeMin, KMINVOLUME);
    QCOMPARE (m_sliderRangeMax, KMAXVOLUME);
    QCOMPARE (m_volumeSliderSetTimeoutCalled, true);
    QCOMPARE (m_sliderTimeout, 10000);
    QCOMPARE (m_volumeSliderSetValueCalled, true);
    QCOMPARE (m_sliderValue, 1);
    QCOMPARE (m_volumeSliderShowCalled, true);
    
    // reset values
    m_volumeSliderContructorCalled = false;
    m_volumeSliderSetValueCalled = false;
    m_sliderRangeMin = -1;
    m_sliderRangeMax = -1;
    m_volumeSliderSetTimeoutCalled = false;
    m_sliderTimeout = -1;
    m_volumeSliderSetValueCalled = false;
    m_sliderValue = -1;
    m_volumeSliderShowCalled = false;

    // The second call shouldn't create a new instance
    m_view->setVolumeSliderValue (2, KVOLUMECOMMAND, KMAXVOLUME, KMINVOLUME);
    QCOMPARE (m_volumeSliderContructorCalled, false);
    QCOMPARE (m_sliderRangeMin, KMINVOLUME);
    QCOMPARE (m_sliderRangeMax, KMAXVOLUME);
    QCOMPARE (m_volumeSliderSetTimeoutCalled, false);
    QCOMPARE (m_sliderTimeout, -1);
    QCOMPARE (m_volumeSliderSetValueCalled, true);
    QCOMPARE (m_sliderValue, 2);
    QCOMPARE (m_volumeSliderShowCalled, false);
}

void TestPhoneUIQtView::testRemoveVolumeSlider ()
{
    m_view->setVolumeSliderValue (1, KVOLUMECOMMAND, KMAXVOLUME, KMINVOLUME);
    m_view->removeVolumeSlider ();
    QCOMPARE (m_volumeSliderHideCalled, true);
}

void TestPhoneUIQtView::testVolumeSliderValue ()
{
    m_sliderValue = -1;
    int value = m_view->volumeSliderValue ();
    QCOMPARE (m_sliderValue, -1);
    m_view->setVolumeSliderValue (3, KVOLUMECOMMAND, KMAXVOLUME, KMINVOLUME);
    value = m_view->volumeSliderValue ();
    QCOMPARE (m_volumeSliderValueCalled, true);
    QCOMPARE (value, 3);
}

void TestPhoneUIQtView::testVolumeSliderChanged ()
{
    m_view->setVolumeSliderValue (3, KVOLUMECOMMAND, KMAXVOLUME, KMINVOLUME);
    QSignalSpy spy (m_view, SIGNAL(command(int)));
    m_view->volumeSliderChanged (1);
    QCOMPARE (spy.count(), 1);
    int command = qvariant_cast<int>(spy.at (0).at (0));
    QCOMPARE (command, (int)KVOLUMECOMMAND);
}

void TestPhoneUIQtView::testParticipantListAction ()
{
    m_view->bubbleManager ().startChanges ();
    m_view->addParticipantListAction (1,"Command 1", HbIcon ());
    m_view->addParticipantListAction (2,"Command 2", HbIcon ());
    m_view->bubbleManager ().endChanges ();

    m_view->bubbleManager ().startChanges ();
    m_view->clearParticipantListActions ();
    m_view->clearParticipantListActions ();
    m_view->bubbleManager ().endChanges ();
}

void TestPhoneUIQtView::testSetExpandAction ()
{
    m_view->bubbleManager ().startChanges ();
    int bubbleId = createCallHeader();
    int bubbleId2 = createCallHeader();

    m_view->setExpandAction(bubbleId, 1);
    m_view->setExpandAction(bubbleId2, 2);
    m_view->bubbleManager ().endChanges ();

    m_view->bubbleManager ().startChanges ();
    m_view->removeExpandAction(bubbleId);
    m_view->removeExpandAction(bubbleId2);
    m_view->bubbleManager().removeCallHeader(bubbleId);
    m_view->bubbleManager().removeCallHeader(bubbleId2);
    m_view->bubbleManager ().endChanges ();
}

void TestPhoneUIQtView::testPhoneAction ()
{
    PhoneAction *action = new PhoneAction ();
    QString string("Action");
    action->setText(string);
    HbIcon icon;
    action->setIcon(icon);
    int commandId(1);
    action->setCommand(commandId);
    action->setDisabled(true);
    QCOMPARE( string, action->text() );
    QCOMPARE( icon, action->icon() );
    QCOMPARE( commandId, action->command() );
    QCOMPARE( true, action->isDisabled() );
    delete action;
}

void TestPhoneUIQtView::testSetOrientation ()
{

    m_setOrientationCalled = false;
    
    m_view->handleOrientationChange(Qt::Vertical);
    QVERIFY( false == m_setOrientationCalled );
    
    m_view->handleOrientationChange(Qt::Horizontal);
    QVERIFY( true == m_setOrientationCalled );
    
}

void TestPhoneUIQtView::testBackButtonClicked()
{
    // not possible to mock xqservice because it is in \epoc32\include
    // calling backButtonClicked would send this test background.
    // m_view->backButtonClicked();
}

void TestPhoneUIQtView::testShowDialpad()
{
    m_view->hideDialpad();
    m_view->showDialpad();
    m_view->showDialpad();
    QVERIFY(true == m_view->isDialpadVisible());
    m_view->hideDialpad();
}

void TestPhoneUIQtView::testDialpadText()
{
    m_view->showDialpad();
    m_view->clearDialpad();
    QVERIFY(QString("") == m_view->dialpadText());
    m_view->clearAndHideDialpad();
}

void TestPhoneUIQtView::testSetMenuActions()
{
    QList<PhoneAction*> actions;
    m_view->setMenuActions(actions);
    
    QVERIFY(m_take_menu_called);
    
    PhoneAction* phoneAction = new PhoneAction; 
    phoneAction->setText(QString("test"));
    phoneAction->setCommand(1);
    actions.append(phoneAction);
    
    m_view->setMenuActions(actions);
    qDeleteAll(actions);
    actions.clear();
    
    m_take_menu_called = false;
    m_view->setMenuActions(actions);
    QVERIFY(m_take_menu_called);
}

//Private methods
int TestPhoneUIQtView::createCallHeader ()
{
    int bubble = m_view->bubbleManager().createCallHeader ();

    return bubble;
}

void TestPhoneUIQtView::testLongEndKeyPressEventOutsideTelephony()
{
    /*QSignalSpy spy(m_view, SIGNAL(endKeyLongPress()));
    QKeyEvent* event = new QKeyEvent(QEvent::ShortcutOverride, Qt::Key_No, Qt::NoModifier);

    bool returnValue = m_view->eventFilter(new QObject(), event);

    QVERIFY(returnValue == true);
    QCOMPARE(spy.count(), 1);*/
}

void TestPhoneUIQtView::testNetworkNameChanged()
{
    // Title is changed for GmsMode
    m_view->networkNameChanged(QSystemNetworkInfo::GsmMode, QString("test"));
    QCOMPARE(m_networkName, QString("test"));

    // Title is changed for WcdmaMode
    m_view->networkNameChanged(QSystemNetworkInfo::WcdmaMode, QString("test2"));
    QCOMPARE(m_networkName, QString("test2"));

    // Other modes shouldn't affect the title
    m_view->networkNameChanged(QSystemNetworkInfo::CdmaMode, QString("another operator"));
    QCOMPARE(m_networkName, QString("test2"));
}

void TestPhoneUIQtView::testCaptureKey()
{
    m_view->captureKey( Qt::Key_No, true );
    m_view->captureKey( Qt::Key_No, true );	
    m_view->captureKey( Qt::Key_Yes, false );
    m_view->captureKey( Qt::Key_No, false );
    m_view->captureKey( Qt::Key_No, false );		
}

void TestPhoneUIQtView::testRestrictedMode()
{
    qRegisterMetaType<QKeyEvent*>("QKeyEvent*");
    QKeyEvent yesDown(QEvent::KeyPress, Qt::Key_Yes, Qt::NoModifier);
    QKeyEvent yesUp(QEvent::KeyRelease, Qt::Key_Yes, Qt::NoModifier);
    QKeyEvent oneDown(QEvent::KeyPress, Qt::Key_1, Qt::NoModifier);
    QKeyEvent oneUp(QEvent::KeyRelease, Qt::Key_1, Qt::NoModifier);
    QSignalSpy downSpy(m_view, SIGNAL(keyPressed(QKeyEvent*)));
    QSignalSpy upSpy(m_view, SIGNAL(keyReleased(QKeyEvent*)));

    m_view->setRestrictedMode(true);
    m_view->m_dialpad->editor().setText("1"); // emits signal
    QCOMPARE(m_view->m_dialpad->isCallButtonEnabled(), false);
    m_view->eventFilter(0, &oneDown);
    m_view->eventFilter(0, &oneUp);
    m_view->eventFilter(0, &yesDown);
    m_view->eventFilter(0, &yesUp);
    QCOMPARE(downSpy.count(), 1); // Only Key_1 is passed
    QCOMPARE(upSpy.count(), 1); // Only Key_1 is passed
    QCOMPARE(m_view->m_dialpad->isCallButtonEnabled(), false);
    QCOMPARE(m_view->m_backAction->isEnabled(), false);
    
    // Test backbutton
    m_view->setBackButtonVisible(true);
    QCOMPARE(m_view->m_backAction->isEnabled(), false);

    // Test return to normal mode
    downSpy.clear();
    upSpy.clear();
    m_view->setRestrictedMode(false);
    m_view->m_dialpad->editor().setText("1"); // emits signal
    QCOMPARE(m_view->m_dialpad->isCallButtonEnabled(), true);
    m_view->eventFilter(0, &oneDown);
    m_view->eventFilter(0, &oneUp);
    m_view->eventFilter(0, &yesDown);
    m_view->eventFilter(0, &yesUp);
    QCOMPARE(downSpy.count(), 2);
    QCOMPARE(upSpy.count(), 2);
    QCOMPARE(m_view->m_dialpad->isCallButtonEnabled(), true);
    QCOMPARE(m_view->m_backAction->isEnabled(), true);
}

HbMenu * HbView::takeMenu()
{
    m_take_menu_called = true; 
    return 0;
}

void HbView::setMenu(HbMenu* menu)
{
    m_set_menu_called = true;
}

void HbView::setTitle (const QString &title)
{
    m_networkName = title;
}


PHONE_QT_VIEW_TEST_MAIN(TestPhoneUIQtView)
Q_DECLARE_METATYPE(QKeyEvent *)
#include "unit_tests.moc"
