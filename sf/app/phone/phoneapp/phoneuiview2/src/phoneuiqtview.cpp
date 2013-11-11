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
* Description:  Phone UI's Qt view.
*
*/
#include <hbinstance.h>
#include <QSignalMapper>
#include <hbaction.h>
#include <hbtoolbar.h>
#include <hbvolumesliderpopup.h>
#include <hbnamespace.h>
#include <bubblemanager2.h>
#include <hblineedit.h>
#include <hbmenu.h>

#include <xqserviceutil.h>
#include <xqkeycapture.h>
#include <dialpad.h>
#include <dialpadkeyhandler.h>

#include "phoneuiqtview.h"
#include "phoneaction.h"
#include "qtphonelog.h"

PhoneUIQtView::PhoneUIQtView (HbMainWindow &window, QGraphicsItem *parent) :
    HbView (parent),
    m_window(window),
    m_bubbleManager(0),
    m_signalMapper(0),
    m_volumeSlider (0),
    m_expandSignalMapper(0),
    m_participantListSignalMapper(0),
    m_volumeCommandId(0),
    m_backAction(0),
    m_dialpad(0),
    m_menuSignalMapper(0),
    m_keyCapture(0),
    m_networkInfo(0),
    m_dialpadKeyHandler(0),
    m_restrictedMode(false),
    m_optionsMenu(0)
{
    PHONE_TRACE
    // Set network name
    m_networkInfo = new QSystemNetworkInfo(this);
    QString networkName = m_networkInfo->networkName(QSystemNetworkInfo::WcdmaMode);
    if(networkName.isEmpty()) {
        networkName = m_networkInfo->networkName(QSystemNetworkInfo::GsmMode);
    }
    connect(m_networkInfo, SIGNAL (networkNameChanged(QSystemNetworkInfo::NetworkMode,QString)),
            this, SLOT(networkNameChanged(QSystemNetworkInfo::NetworkMode, QString)));
    setTitle(networkName);

    // Capturing long press of end key
    m_keyCapture = new XQKeyCapture();
    
    // Dialpad
    m_dialpad = new Dialpad(m_window);
    m_dialpad->setCallButtonEnabled(false);
    m_dialpad->setTapOutsideDismiss(true);
    connect(m_dialpad,SIGNAL(aboutToClose()),this,
                SLOT(dialpadClosed()));
    
    // Call handling widget
    m_bubbleManager = new BubbleManager (this);
    setWidget(m_bubbleManager);

    // Set event filter
    m_window.installEventFilter(this);

    m_signalMapper = new QSignalMapper (this);
    connect(m_signalMapper, SIGNAL (mapped (int)), this, SIGNAL (command (int)));
    connect(&m_window,SIGNAL(orientationChanged(Qt::Orientation)),
            this,SLOT(handleOrientationChange(Qt::Orientation)));

    m_menuSignalMapper = new QSignalMapper(this);
    connect(m_menuSignalMapper, SIGNAL(mapped(int)), this, SIGNAL(command(int)));

    m_bubbleManager->handleOrientationChange(m_window.orientation());

    // change exit softkey to back button
    m_backAction = new HbAction(Hb::BackNaviAction, this);
    connect(m_backAction, SIGNAL(triggered()), this, SLOT(backButtonClicked()));
    setNavigationAction(m_backAction);

    createToolBarActions();

    // Set restricted mode off, normal state
    setRestrictedMode(false);
}

PhoneUIQtView::~PhoneUIQtView ()
{
    foreach (QList<HbAction *> *actions, m_bubbleActionMap) {
        qDeleteAll(*actions);
        delete actions;
    }
    
    qDeleteAll(m_participantListActions);
    qDeleteAll(m_expandActionMap);
    qDeleteAll(m_bubbleMap);
    qDeleteAll(m_toolbarActions);
    m_window.removeEventFilter(this);
    delete m_keyCapture;
    delete m_volumeSlider;
    delete m_dialpad;
    delete m_optionsMenu;
}

BubbleManagerIF& PhoneUIQtView::bubbleManager()
{
    return *m_bubbleManager;
}

void PhoneUIQtView::addBubbleCommand (
    int bubbleId,
    const PhoneAction& action )
{
    PHONE_TRACE
    HbAction *bubbleAction = new HbAction ();
    bubbleAction->setText (action.text());
    bubbleAction->setIcon (action.icon());
    setActionRole(action,*bubbleAction);
    m_bubbleManager->addAction (bubbleId, bubbleAction);

    QList<int> bubbles = m_bubbleMap.keys();
    bool found(false);

    for ( int i=0; i<bubbles.size(); ++i ) {
        if (bubbleId==bubbles[i]){
            connect(bubbleAction, SIGNAL (triggered ()), m_bubbleMap[bubbleId], SLOT (map ()));
            m_bubbleMap[bubbleId]->setMapping(bubbleAction, action.command());
            m_bubbleActionMap[bubbleId]->append(bubbleAction);
            found = true;
        }
    }

    if (!found) {
        QSignalMapper *mapper = new QSignalMapper();
        connect(mapper, SIGNAL (mapped (int)), this, SIGNAL (command (int)));
        connect(bubbleAction, SIGNAL (triggered ()), mapper, SLOT (map ()));
        mapper->setMapping (bubbleAction, action.command());
        QList<HbAction *> *actionList = new QList<HbAction *>();
        actionList->append( bubbleAction );
        m_bubbleActionMap.insert(bubbleId,actionList);
        m_bubbleMap.insert(bubbleId,mapper);
    }
}

void PhoneUIQtView::addParticipantListAction(
    int commandId,
    const QString& text,
    const HbIcon& icon)
{
    PHONE_TRACE
    HbAction* action = new HbAction ();
    action->setText (text);
    action->setIcon (icon);
    m_bubbleManager->addParticipantListAction(action);

    if (!m_participantListSignalMapper) {
        m_participantListSignalMapper = new QSignalMapper();
        connect(m_participantListSignalMapper, SIGNAL (mapped (int)), this, SIGNAL (command (int)));
    }

    connect(action, SIGNAL (triggered ()), m_participantListSignalMapper, SLOT (map ()));
    m_participantListSignalMapper->setMapping (action, commandId);
    m_participantListActions.append( action );
}

void PhoneUIQtView::clearParticipantListActions()
{
    PHONE_TRACE
    if (m_participantListSignalMapper) {
        m_bubbleManager->clearParticipantListActions();

        foreach (HbAction *action, m_participantListActions ) {
            m_participantListSignalMapper->removeMappings(action);
        }
		qDeleteAll(m_participantListActions);
        m_participantListActions.clear();
        delete m_participantListSignalMapper;
        m_participantListSignalMapper = 0;
    }

}

void PhoneUIQtView::clearBubbleCommands (int bubbleId)
{
    PHONE_TRACE
    m_bubbleManager->clearActions (bubbleId);
    QSignalMapper *mapper = m_bubbleMap.value(bubbleId);

    if (mapper) {
        QList<HbAction *> *actions = m_bubbleActionMap.value(bubbleId);
        if (actions) {
            foreach (HbAction *action, *actions ) {
                mapper->removeMappings(action);
            }

            qDeleteAll(*actions);
            actions->clear();
            delete actions;
        }
        
        m_bubbleMap.remove(bubbleId);
        m_bubbleActionMap.remove(bubbleId);
        delete mapper;
    }

}

void PhoneUIQtView::setToolbarActions(const QList<PhoneAction*>& actions)
{
    PHONE_TRACE
    // clear current toolbar actions
    for (int i=0;i<toolBar()->actions().count();++i) {
        m_signalMapper->removeMappings(
                static_cast<HbAction*>(toolBar()->actions().at(i)));
    }

    QList<QAction*> toolBarActions = toolBar()->actions();

    if (toolBarActions.size()<actions.size()) {
        for (int i=toolBarActions.size(); i<actions.size(); ++i) {
            toolBar()->addAction(m_toolbarActions.at(i));
        }
    } else if (toolBarActions.size()>actions.size()) {
        for (int i=toolBarActions.size(); 0<i; --i) {
            if (i>actions.size()) {
                HbAction* action = static_cast<HbAction*>(toolBarActions.at(i-1));
                toolBar()->removeAction(action);
            }
        }
    }

    for (int i=0; i<toolBar()->actions().size(); ++i) {

        if (i<actions.count()) {
            HbAction* action = static_cast<HbAction*>(toolBar()->actions().at(i));
            action->setText(actions.at(i)->text());
            action->setIcon(actions.at(i)->icon());
            action->setDisabled(actions.at(i)->isDisabled());

            m_signalMapper->setMapping(action, actions.at(i)->command());
        }
    }

    if ( m_window.orientation() == Qt::Horizontal ) {
        toolBar()->setOrientation(Qt::Horizontal);
    }
    // update toolbar
    toolBar()->update();
}

void PhoneUIQtView::hideToolbar ()
{
    toolBar()->hide ();
}

void PhoneUIQtView::showToolbar ()
{
    setFocus();
    toolBar()->show();
}

int PhoneUIQtView::volumeSliderValue ()
{
    if (m_volumeSlider) {
        return m_volumeSlider->value ();
    } else {
        return -1;
    }
}

void PhoneUIQtView::removeVolumeSlider ()
{
    if (m_volumeSlider) {
        if (m_volumeSlider->isVisible()) {
            m_volumeSlider->hide();
        }
        m_volumeSlider->deleteLater();
        m_volumeSlider = 0;
    }
}

void PhoneUIQtView::volumeSliderClosed ()
{
    removeVolumeSlider();
}

void PhoneUIQtView::setVolumeSliderValue (
        int value, int commandId, int maxVolumeValue, int minVolumeValue)
{
    PHONE_TRACE
    m_volumeCommandId = commandId;

    if (!m_volumeSlider) {
        m_volumeSlider = new HbVolumeSliderPopup ();
        m_volumeSlider->setDismissPolicy(HbDialog::TapOutside);
        m_volumeSlider->setTimeout (10000); // TODO: 10 seconds for now, replace with correct value when spec is ready and says what it is
        connect(m_volumeSlider, SIGNAL(valueChanged(int)), this, SLOT(volumeSliderChanged(int)));
        connect(m_volumeSlider, SIGNAL(aboutToClose()), this, SLOT(volumeSliderClosed()));
    }


    if (m_volumeSlider->minimum() != minVolumeValue ||
            m_volumeSlider->maximum() !=  maxVolumeValue  ) {
        m_volumeSlider->setRange (minVolumeValue, maxVolumeValue);
    }

    if (value != m_volumeSlider->value())
        m_volumeSlider->setValue (value);

    if (false == m_volumeSlider->isVisible()) {
        m_volumeSlider->show();
    }
}

void PhoneUIQtView::volumeSliderChanged(int value)
{
    Q_UNUSED (value);
    emit command (m_volumeCommandId);
}

void PhoneUIQtView::setExpandAction(int bubbleId, int commandId)
{
    PHONE_TRACE
    removeExpandAction(bubbleId);

    HbAction* action = new HbAction();
    m_bubbleManager->setExpandAction(bubbleId, action);

    if (!m_expandSignalMapper) {
        m_expandSignalMapper = new QSignalMapper(this);
        connect(m_expandSignalMapper, SIGNAL (mapped (int)),
                this, SIGNAL (command (int)));
    }

    connect(action, SIGNAL (triggered ()), m_expandSignalMapper, SLOT (map ()));
    m_expandSignalMapper->setMapping(action, commandId);

    m_expandActionMap.insert(bubbleId,action);
}

void PhoneUIQtView::removeExpandAction(int bubbleId)
{
    PHONE_TRACE
    if (m_expandActionMap.contains(bubbleId)) {
        m_bubbleManager->setExpandAction(bubbleId, 0);
        HbAction* action = m_expandActionMap.value(bubbleId);
        m_expandSignalMapper->removeMappings(action);
        m_expandActionMap.remove(bubbleId);
        delete action;
    }
}

void PhoneUIQtView::showDialpad()
{
    if (false == m_dialpad->isVisible()) {
        setDialpadPosition();
        m_dialpad->openDialpad();
    }
}

void PhoneUIQtView::hideDialpad()
{
    if (true == m_dialpad->isVisible())
        m_dialpad->closeDialpad();
}

bool PhoneUIQtView::isDialpadVisible()
{
    return m_dialpad->isVisible();
}

QString PhoneUIQtView::dialpadText()
{
    return m_dialpad->editor().text();
}

void PhoneUIQtView::clearAndHideDialpad()
{
    m_dialpad->editor().setText(QString(""));
    m_dialpad->closeDialpad();
}

void PhoneUIQtView::clearDialpad()
{
    m_dialpad->editor().setText(QString(""));
}

void PhoneUIQtView::bringToForeground()
{
    m_window.show();
}

void PhoneUIQtView::setMenuActions(const QList<PhoneAction*>& actions)
{   
    PHONE_TRACE    
    HbMenu* optionsMenu = m_optionsMenu ? m_optionsMenu : menu();
    
    for (int i=optionsMenu->actions().count(); 0<i; --i) {
        HbAction* action = static_cast<HbAction*>(optionsMenu->actions().at(i-1));
        m_menuSignalMapper->removeMappings(action);
        optionsMenu->removeAction(action);
        delete action;
    }
    for (int i=0; i<actions.count(); ++i) {
        HbAction* action = new HbAction();
        action->setText(actions.at(i)->text());
        optionsMenu->addAction(action);
        connect(action, SIGNAL(triggered()), m_menuSignalMapper, SLOT(map()));
        m_menuSignalMapper->setMapping(action, actions.at(i)->command());
    }    
    updateMenuVisibility();
}

HbMenu &PhoneUIQtView::menuReference()
{
    PHONE_TRACE
    HbMenu* optionsMenu = m_optionsMenu ? m_optionsMenu : menu();
    return *optionsMenu;
}

void PhoneUIQtView::captureKey(Qt::Key key, bool capture)
{
    if (capture) {
        if (!m_keyCaptures.contains(key)) {
            m_keyCapture->captureLongKey(key);
            m_keyCapture->captureKey(key);
            m_keyCaptures.append(key);
        }
    } else {
        if (m_keyCaptures.contains(key)) {
            m_keyCapture->cancelCaptureKey(key);
            m_keyCapture->cancelCaptureLongKey(key);
            m_keyCaptures.removeOne(key);
        }
    }
}

void PhoneUIQtView::handleOrientationChange(Qt::Orientation orientation)
{
    PHONE_TRACE
    if (orientation==Qt::Horizontal) {
        toolBar()->setOrientation(Qt::Horizontal);
    }

    m_bubbleManager->handleOrientationChange(orientation);

    setDialpadPosition();
}

void PhoneUIQtView::backButtonClicked()
{
    XQServiceUtil::toBackground(true);
}

void PhoneUIQtView::onEditorContentChanged()
{
    m_dialpad->setCallButtonEnabled(
        m_dialpad->editor().text().length());
}

void PhoneUIQtView::dialpadClosed()
{
    emit dialpadIsAboutToClose();
}

bool PhoneUIQtView::eventFilter(QObject *watched, QEvent * event)
{
    Q_UNUSED(watched);
    PHONE_TRACE2(":event type", event->type())
    
    // Allow send key only when there is callbutton enabled or no text in input field
    bool sendKeyAllowed = m_dialpad->isCallButtonEnabled() || 
            (m_dialpad->editor().text().length() == 0);
    
    if(event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
        PHONE_DEBUG2("PhoneUIQtView::eventFilter pressed key:", keyEvent->key());
        PHONE_DEBUG2("PhoneUIQtView::eventFilter isAutoRepeat:", keyEvent->isAutoRepeat());
        if ( (keyEvent->key() != Qt::Key_Yes && keyEvent->key() != Qt::Key_Enter) ||
                sendKeyAllowed) {
            emit keyPressed(keyEvent);        
            keyEvent->accept();
        }
        
        return false;
    } else if(event->type() == QEvent::KeyRelease) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
        PHONE_DEBUG2("PhoneUIQtView::eventFilter released key:", keyEvent->key());
        if ( (keyEvent->key() != Qt::Key_Yes && keyEvent->key() != Qt::Key_Enter) ||
                sendKeyAllowed) {
            emit keyReleased(keyEvent);
            keyEvent->accept();
        }
        return false;
    } else if (event->type() == QEvent::WindowActivate){
        PHONE_DEBUG("PhoneUIQtView::eventFilter WindowActivate");
        emit windowActivated();
        return false;
    } else if (event->type() == QEvent::WindowDeactivate){
        PHONE_DEBUG("PhoneUIQtView::eventFilter WindowDeactivate");
        emit windowDeactivated();
        return false;
    }else{
        return false;
    }
}

void PhoneUIQtView::setDialpadPosition()
{
    PHONE_TRACE
    QRectF screenRect(m_window.layoutRect());
                        	
    if (m_window.orientation() == Qt::Horizontal) {
            // dialpad takes half of the screen
        m_dialpad->setPos(QPointF(screenRect.width()/2,
                                  this->scenePos().y()));
        m_dialpad->setPreferredSize(screenRect.width()/2,
                                           (screenRect.height()-scenePos().y()));
    } else {
        // dialpad takes 65% of the screen height
        qreal screenHeight = screenRect.height();
        m_dialpad->setPos(QPointF(0,
                                  screenHeight/2.25));
        m_dialpad->setPreferredSize(screenRect.width(),
                                    screenHeight-screenHeight/2.25);
    }
}

void PhoneUIQtView::setActionRole(const PhoneAction& pa, HbAction& action)
{
    PHONE_TRACE
    if (pa.actionRole()==PhoneAction::Accept) {
        action.setSoftKeyRole(QAction::PositiveSoftKey);
    } else if (pa.actionRole()==PhoneAction::Decline) {
        action.setSoftKeyRole(QAction::NegativeSoftKey);
    }
}

void PhoneUIQtView::createToolBarActions()
{
    PHONE_TRACE
    for (int i=0;i<4;++i) {
        HbAction* action = new HbAction();
        connect(action, SIGNAL(triggered()), m_signalMapper, SLOT(map()));
        m_toolbarActions.append(action);
    }
}

void PhoneUIQtView::updateMenuVisibility()
{
    PHONE_TRACE
    bool visible = false;
    HbView* view = m_window.currentView();
    HbMenu* optionsMenu = m_optionsMenu ? m_optionsMenu : menu();
    foreach( QAction* action, optionsMenu->actions()){
        if(action->isVisible()){
            visible = true;
            break;
        }
    }
    PHONE_DEBUG2("PhoneUIQtView::updateMenuVisibility -  visible", visible);
    if (!visible && !m_optionsMenu) {
        PHONE_DEBUG("PhoneUIQtView::updateMenuVisibility - takeMenu");
        //takeMenu() - Removes the menu from the view and returns it to the caller.
        //ownership of the menu is transfered.        
        m_optionsMenu = view->takeMenu();
        
    } else if (visible && m_optionsMenu) {
        // ownership of the menu is transfered
        view->setMenu(m_optionsMenu);
        m_optionsMenu = 0;
    }
}

void PhoneUIQtView::shutdownPhoneApp()
{
    PHONE_DEBUG("PhoneUIQtView::shutdownPhoneApp");
    QCoreApplication::quit();
}

void PhoneUIQtView::setBackButtonVisible(bool visible)
{
    if (!m_restrictedMode) {
        m_backAction->setEnabled(visible);
    }
}

void PhoneUIQtView::setRestrictedMode(bool restrictedMode)
{
    PHONE_TRACE
    m_restrictedMode = restrictedMode;
    m_backAction->setEnabled(!restrictedMode);
    m_dialpad->setCallButtonEnabled(false);
    m_dialpad->editor().setText(""); // Clead dialpad
    if (m_restrictedMode) {
        delete m_dialpadKeyHandler;
        m_dialpadKeyHandler = 0;
        m_dialpadKeyHandler = new DialpadKeyHandler(
                m_dialpad, DialpadKeyHandler::EmergencyCall, this);
        disconnect(&m_dialpad->editor(),SIGNAL(contentsChanged()),
                this, SLOT(onEditorContentChanged())); // Let emergency handler do updating 
    } else {
        delete m_dialpadKeyHandler;
        m_dialpadKeyHandler = 0;
        // enable key sequence handling during a call
        m_dialpadKeyHandler = new DialpadKeyHandler(
                m_dialpad, DialpadKeyHandler::KeySequence, this);
        connect(&m_dialpad->editor(),SIGNAL(contentsChanged()),
                SLOT(onEditorContentChanged())); // Update our self
    }
    
}

void PhoneUIQtView::networkNameChanged(QSystemNetworkInfo::NetworkMode mode, const QString &netName)
{
    if((mode == QSystemNetworkInfo::GsmMode) || 
       (mode == QSystemNetworkInfo::WcdmaMode)) {
        setTitle(netName);
    }	
}

