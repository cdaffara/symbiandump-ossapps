/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  HbMenu2 implementation.
*
*/


#include <hbmenu.h>
#include <hbmainwindow.h>
#include <hbinstance.h>
#include "hbstubs_helper.h"
#include <hbdialog.h>
#include <hbapplication.h>
#include <hbcolorscheme.h>
#include <hbview.h>
#include <hbactivitymanager.h>
#include <QCoreApplication>
#include <QApplication>
#include <QTimer>
#include <QGesture>
#include <hblistview.h>
#include <hblistviewitem.h>
#include <hbmessagebox.h>
#include <hbnotificationdialog.h>
#include <hbstringutil.h>
#include "xqkeycapture.h"
#include "tstasksettings.h"

int actionCount = 0;
Qt::Orientation windowOrientation = Qt::Vertical;
bool testMenuShown = false;
HbPopup::Placement testPopupShowPlacement = HbPopup::TopLeftCorner;
bool testDialogShown = false;
HbMainWindow* testWindow = 0;
HbView* testView = 0;
int testViewCount = 0;
HbAction* testSoftkeyAction = 0;
QString selectedActionString = "none";
bool testSingleShotTimer = false;
bool testQuitCalled = false;
Qt::GestureState testState = Qt::NoGesture;
bool testIsWidgetOpen = false;
bool testIsWidgetRaised = false;
QColor testColor = Qt::white;
QList<HbView *> testViews;
QList<QVariantHash> testActivities;
HbActivityManager testActivityManager;
Hb::ActivationReason testActivationReason = Hb::ActivationReasonNormal;
QString testActivityId = "LogsViewMatches";
QList<HbListViewItem*> testViewItems;
bool testEnsureVisibleCalled = false;
bool testScrollToCalled = false;
Qt::LayoutDirection testLayoutDirection = Qt::LeftToRight;
bool testConversionEnabled = false;
bool testScrollBarPolicySet = false;
HbScrollArea::ScrollBarPolicy testScrollBarPolicy = HbScrollArea::ScrollBarAsNeeded;
QWindowSurface* testWindowSurface = 0;
QString testNotifDialogText;
bool testIsTsTaskVisibilitySet = false;
bool testTsTaskVisibility = false;

class QWindowSurface{
public:
    QWindowSurface(){}
    ~QWindowSurface(){}
};

void HbStubHelper::reset()
{
    actionCount = 0;
    testMenuShown = false;
    testSingleShotTimer = false;
    testQuitCalled = false;
    testIsWidgetRaised = false;
    testDialogShown = false;
    testColor = Qt::white;
    testActivationReason = Hb::ActivationReasonNormal;
    testActivityId = "LogsViewMatches";
    testActivities.clear();
    qDeleteAll(testViewItems);
    testViewItems.clear();
    testScrollToCalled = false;
    testEnsureVisibleCalled = false;
    testPopupShowPlacement = HbPopup::TopLeftCorner;
    testConversionEnabled = false;
    testScrollBarPolicySet = false;
    delete testWindowSurface;
    testWindowSurface = 0;
    testIsTsTaskVisibilitySet = false;
    testTsTaskVisibility = false;
}

int HbStubHelper::widgetActionsCount()
{
    return actionCount;
}

bool HbStubHelper::menuShown()
{
    return testMenuShown;
}

int HbStubHelper::menuShownPlacement()
{
    return testPopupShowPlacement;
}

bool HbStubHelper::dialogShown()
{
    return testDialogShown;
}


bool HbStubHelper::singleShotTimerActive()
{
    return testSingleShotTimer;
}

bool HbStubHelper::quitCalled()
{
    return testQuitCalled;
}

void HbStubHelper::setGestureState(int state)
{
    testState = static_cast<Qt::GestureState> (state);
}


Qt::GestureState QGesture::state() const
{
    return testState;
}


bool HbStubHelper::isWidgetOpen()
{
    return testIsWidgetOpen;
}

bool HbStubHelper::isWidgetRaised()
{
    return testIsWidgetRaised;
}

void HbStubHelper::setWidgetOpen(bool isOpen)
{
    testIsWidgetOpen = isOpen;
}

void HbStubHelper::setColorScheme(QColor col)
{
    testColor = col;
}

void HbStubHelper::setActivityReason(Hb::ActivationReason reason)
{
    testActivationReason = reason;
}

void HbStubHelper::setActivityId(QString activityId)
{
    testActivityId = activityId;
}

QList<HbListViewItem*>& HbStubHelper::listItems()
{
    return testViewItems;
}

bool HbStubHelper::listScrollToCalled()
{
    return testScrollToCalled;
}

bool HbStubHelper::listEnsureVisibleCalled()
{
    return testEnsureVisibleCalled;
}

void HbStubHelper::stringUtilDigitConversion(bool enabled)
{
    testConversionEnabled = enabled;
}

bool HbStubHelper::listScrollBarPolicySet()
{
    return testScrollBarPolicySet;
}

void HbStubHelper::resetListScrolling()
{
    testEnsureVisibleCalled = false;
    testScrollBarPolicySet = false;
    testScrollToCalled = false;
}

void HbStubHelper::createWindowSurface()
{
    if ( !testWindowSurface ){
        testWindowSurface = new QWindowSurface;
    }
}

bool HbStubHelper::isTsTaskVisibilitySet()
{
    return testIsTsTaskVisibilitySet;
}

bool HbStubHelper::tsTaskVisibility()
{
    return testTsTaskVisibility;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void HbNotificationDialog::launchDialog(const QString &title, QGraphicsScene *scene)
{
    Q_UNUSED(scene);
    testNotifDialogText = title;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
QCoreApplication::QCoreApplication(int &argc, char **argv)
{
    
}


void QCoreApplication::quit()
{
    testQuitCalled = true; 
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
QApplication::QApplication(QApplication::QS60MainApplicationFactory factory, int &argc, char **argv, int version) 
: QCoreApplication(argc, argv)
{
    
}

QApplication::~QApplication()
{
    
}

void QApplication::setLayoutDirection(Qt::LayoutDirection direction)
{
    testLayoutDirection = direction;
}

Qt::LayoutDirection QApplication::layoutDirection()
{
    return testLayoutDirection;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
bool QGraphicsWidget::close()
{
    testIsWidgetOpen = false;
    return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void QWidget::setVisible(bool visible)
{
    Q_UNUSED(visible);
}

void QWidget::raise()
{
    testIsWidgetRaised = true;
}

QWindowSurface* QWidget::windowSurface() const
{
    return testWindowSurface;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
QPixmap QPixmap::grabWidget(QWidget *widget, const QRect &rect)
{
    Q_UNUSED(widget);
    Q_UNUSED(rect);
    return QPixmap();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
HbActivityManager::HbActivityManager(QObject *parent) : QObject(parent)
{
    
}
HbActivityManager::~HbActivityManager()
{
    
}
    
bool HbActivityManager::addActivity(const QString &activityId, const QVariant &data, const QVariantHash &parameters)
{
    testActivities.append(parameters);
    return true;
}

bool HbActivityManager::removeActivity(const QString &activityId)
{
    if ( !testActivities.isEmpty() ){
        testActivities.takeFirst();
    }
    return true;
}
QList<QVariantHash> HbActivityManager::activities() const
{
    return testActivities;
}

bool HbActivityManager::waitActivity()
{
    return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
HbApplication::HbApplication(int &argc, char *argv[], Hb::ApplicationFlags flags) :
    QApplication(0, argc, argv, 0), mTestFlags(flags)
{
}

HbApplication::~HbApplication()
{
}
    
void HbApplication::quit()
{ 
    testQuitCalled = true; 
}

HbActivityManager *HbApplication::activityManager()
{
    return &testActivityManager;
}

Hb::ActivationReason HbApplication::activateReason() const
{
    return testActivationReason;
}
QVariantHash HbApplication::activateParams() const
{
    return QVariantHash();
}
QString HbApplication::activateId() const
{
    return testActivityId;
}
QVariant HbApplication::activateData()
{
    return QVariant();
}
    
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void HbMenu::open(QObject *receiver, const char *member)
{
   Q_UNUSED(receiver)
   Q_UNUSED(member)   
   testMenuShown = true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void HbPopup::setPreferredPos(const QPointF& position, HbPopup::Placement placement)
{
    Q_UNUSED(position);
    testPopupShowPlacement = placement;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void HbDialog::open(QObject *receiver, const char *member)
{
   Q_UNUSED(receiver)
   Q_UNUSED(member)
   testDialogShown = true;
}

void HbDialog::open()
{
   testDialogShown = true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void HbMessageBox::question(const QString &questionText,
                            QObject *receiver ,
                            const char *member,
                            HbMessageBox::StandardButtons buttons,
                            QGraphicsWidget *headingWidget,
                            QGraphicsScene *scene,
                            QGraphicsItem *parent)
{
    Q_UNUSED(questionText)
    Q_UNUSED(receiver)
    Q_UNUSED(member)
    Q_UNUSED(buttons)
    Q_UNUSED(scene)
    Q_UNUSED(parent)
    testDialogShown = true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void QGraphicsWidget::addAction(QAction *action)
{
    Q_UNUSED(action)
    actionCount++;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
HbMainWindow::HbMainWindow(QWidget *parent, Hb::WindowFlags windowFlags) : d_ptr(0)
{
    Q_UNUSED(parent)
    Q_UNUSED(windowFlags)
    testViewCount = 0;
    testWindow = this;
    testViews.clear();
}

HbMainWindow::~HbMainWindow()
{
    testWindow = 0;
}
    
void HbMainWindow::setOrientation(Qt::Orientation orientation, bool animate)
{
    Q_UNUSED(animate)
    windowOrientation = orientation; 
}

Qt::Orientation HbMainWindow::orientation() const
{
    return windowOrientation;
}

QRectF HbMainWindow::layoutRect() const
{
    if (windowOrientation == Qt::Vertical) {
        return QRectF(0, 0, 360, 640);
    } else {
        return QRectF(0, 0, 640, 360);
    }
}

HbView *HbMainWindow::addView(QGraphicsWidget *widget)
{
    testViewCount++;
    HbView* view = static_cast<HbView*>(widget);
    testViews.append( view );
    return view;
}

void HbMainWindow::setCurrentView(HbView *view, bool animate, Hb::ViewSwitchFlags flags)
{
    Q_UNUSED(flags)
    testView = view;
}

QList<HbView *> HbMainWindow::views() const
{
    return testViews;
}

HbView *HbMainWindow::currentView() const
{
    return testView;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
HbInstance::HbInstance() : d(0)
{

}

HbInstance::~HbInstance()
{

}

HbInstance* HbInstance::instance()
{
    static HbInstance hbTestInstance;
    return &hbTestInstance;
}
    
QList<HbMainWindow *> HbInstance::allMainWindows() const
{
    QList<HbMainWindow *> list;
    list.append(testWindow);
    return list;
}

void QTimer::singleShot(int msec, QObject *receiver, const char *member)
{
    testSingleShotTimer = true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
bool HbStyle::parameter(const QString &parameter, qreal &value, const HbDeviceProfile &profile) const
{
    Q_UNUSED( profile );
    value = parameter.length();
    return true;
}


QColor HbColorScheme::color( const QString &colorRole )
{
    Q_UNUSED(colorRole);
    return testColor;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void HbListView::scrollTo(const QModelIndex &index, ScrollHint hint)
{
    Q_UNUSED(index);
    Q_UNUSED(hint);
    testScrollToCalled = true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
QList<HbAbstractViewItem *> HbAbstractItemView::visibleItems() const
{
    QList<HbAbstractViewItem*> visible;
    foreach ( HbListViewItem* item, testViewItems ){
        visible.append( item );
    }
    return visible;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void HbScrollArea::ensureVisible(const QPointF &position, qreal xMargin, qreal yMargin)
{
    Q_UNUSED(position);
    Q_UNUSED(xMargin);
    Q_UNUSED(yMargin);
    testEnsureVisibleCalled = true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void HbScrollArea::setVerticalScrollBarPolicy(ScrollBarPolicy policy)
{
    testScrollBarPolicySet = true;
    testScrollBarPolicy = policy;
}

HbScrollArea::ScrollBarPolicy HbScrollArea::verticalScrollBarPolicy() const
{
    return testScrollBarPolicy;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
QString HbStringUtil::convertDigitsTo( const QString str, const DigitType digitType )
{
    Q_UNUSED(digitType);
    if ( testConversionEnabled ){
        return ( QString("conv") + str );
    }
    return str;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
XQKeyCapture::XQKeyCapture()
{
    
}

XQKeyCapture::~XQKeyCapture()
{
    
}

bool XQKeyCapture::captureKey(Qt::Key aKey,
    Qt::KeyboardModifiers aModifiersMask,
    Qt::KeyboardModifiers aModifier)
{
    Q_UNUSED(aModifiersMask);
    Q_UNUSED(aModifier);
    mKeys.append(aKey);
    return true;
}
    
bool XQKeyCapture::cancelCaptureKey(Qt::Key aKey,
    Qt::KeyboardModifiers aModifiersMask,
    Qt::KeyboardModifiers aModifier)
{
    Q_UNUSED(aModifiersMask);
    Q_UNUSED(aModifier);
    int index = mKeys.indexOf(aKey);
    if ( index >= 0 ){
        mKeys.takeAt(index);
    }
    return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TsTaskSettings::TsTaskSettings()
{
    
}
TsTaskSettings::~TsTaskSettings()
{
    
}

bool TsTaskSettings::setVisibility(bool visible)
{
    testTsTaskVisibility = visible;
    testIsTsTaskVisibilitySet = true;
    return true;
}

