/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation, version 2.1 of the License.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this program.  If not,
* see "http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html/".
*
* Description:
*
*/
#include <QDesktopWidget>
#include <QGraphicsView>
#include <QGraphicsItem>

#include "WebGestureHelper.h"
#include "qstmgestureevent.h"
#include "qstmfilelogger.h"
#include "qstmuievent_if.h"

using namespace qstmGesture;
using namespace qstmUiEventEngine;

#define ENABLE_GESTURE_LIB 1


WebGestureHelperEventFilter* WebGestureHelperEventFilter::m_instance = 0;

WebGestureHelperEventFilter* WebGestureHelperEventFilter::instance()
{
    if (!m_instance) {
        m_instance = new WebGestureHelperEventFilter();
    }
    return m_instance;
}

bool WebGestureHelperEventFilter::eventFilter(QObject* receiver, QEvent* event)
{
    QStm_Gesture* g = getQStmGesture(event);
    if (g) {
        g->sendMouseEvents(receiver);
        return true;
    }
    return false;
}



WebGestureHelper::WebGestureHelper(QWidget* ctrl) :
                             m_ctrl(ctrl)
{
    m_gestureEngine = browserApp->gestureEngine();
    setupGestureEngine(m_ctrl);
}


WebGestureHelper::~WebGestureHelper()
{
    delete m_gestures;
    m_gestures = NULL;
    delete m_gestureEngine;
    QGestureRecognizer::unregisterRecognizer(m_qstmGestureType);
    delete m_dummyRecognizer;

}


void WebGestureHelper::setupGestureEngine(QWidget* ctrl)
{
    m_gestures = m_gestureEngine->createContext(qptrdiff(ctrl));
#if defined(_DEBUG)
    m_gestures->setLogging(0);
#endif
    QStm_GestureParameters& param = m_gestures->config();
    param.setEnabled(qstmGesture::EGestureUidTap, true);
    param[qstmGesture::EDoubleTapTimeout] = 300;

    param.setEnabled(qstmGesture::EGestureUidPan, true);
    param[qstmGesture::EPanSpeedLow] = 0;
    param[qstmGesture::EPanSpeedHigh] = 100;
    param[qstmGesture::EPanDisabledWhileHovering] = false;
    param[qstmGesture::EMoveTolerance] = 2;

    param.setEnabled(qstmGesture::EGestureUidHover, false);
    param[qstmGesture::EHoverSpeed] = 0;
    param[qstmGesture::EHoverDisabledWhilePanning] = false;

    param.setEnabled(qstmGesture::EGestureUidLeftRight,  false);
    param.setEnabled(qstmGesture::EGestureUidUpDown,     false);

    param.setEnabled(qstmGesture::EGestureUidFlick,      true);
    param[qstmGesture::EFlickSpeed] = 25; /*param[stmGesture::EPanSpeedHigh];*/

    param.setEnabled(qstmGesture::EGestureUidRelease,    true);
    param.setEnabled(qstmGesture::EGestureUidTouch,      true);

    param.setEnabled(qstmGesture::EGestureUidEdgeScroll, false);
    param[qstmGesture::EEdgeScrollRange] = 20;

    param.setEnabled(qstmGesture::EGestureUidCornerZoom, false);
    param[qstmGesture::EZoomCornerSize] = 7;

    param.setEnabled(qstmGesture::EGestureUidPinch, true);
    param.setEnabled(qstmGesture::EGestureUidLongPress, true);

    param.setEnabled(qstmGesture::EGestureUidUnknown, true);

    QStm_GestureArea& touchArea = *param.area(qstmGesture::ETouchArea);
    QStm_GestureArea& tTimeArea = *param.area(qstmGesture::ETouchTimeArea);
    QStm_GestureArea& holdArea  = *param.area(qstmGesture::EHoldArea);

    touchArea.m_shape    = QStm_GestureArea::QStm_Shape(1);
    touchArea.m_timeout  = 150;
    touchArea.m_size     = QSize(7, 0);

    tTimeArea.m_shape    = QStm_GestureArea::QStm_Shape(1);
    tTimeArea.m_timeout  = 150;
    tTimeArea.m_size     = QSize(7, 0);

    holdArea.m_shape    = QStm_GestureArea::QStm_Shape(1);
    holdArea.m_timeout  = 1500;
    holdArea.m_size     = QSize(7, 0);

    param[ qstmGesture::ESuppressTimeout     ] = 0;
    param[ qstmGesture::EMoveSuppressTimeout ] = 0;
    param[ qstmGesture::ECapacitiveUpUsed    ] = false;
    param[ qstmGesture::EAdjustYPos          ] = false;
    param[ qstmGesture::EEnableFiltering     ] = 1;
    param[ qstmGesture::EWServMessageInterception ] = false;

    m_gestures->activate(ctrl);

    /*
     * Only one instance of dummy recognizer is needed.
     * First context will have none-null pointer to it.
     */
    if (QStm_Gesture::assignedType() == Qt::CustomGesture) {
        m_dummyRecognizer = new QStm_QtDummyGestureRecognizer(m_gestures);
        m_gestures->addListener(m_dummyRecognizer);
        m_qstmGestureType = QGestureRecognizer::registerRecognizer(m_dummyRecognizer);
        QStm_Gesture::setAssignedGestureType(m_qstmGestureType);
    }
}


bool  WebGestureHelper::symbianEventFilter(const QSymbianEvent *event)
{
    return m_gestures->handleSymbianPlatformEvent(event);

}


bool WebGestureHelper::x11EventFilter (XEvent* event)
{
    return m_gestures->handleX11PlatformEvent(event);
}


BrowserApp::BrowserApp(int argc, char** argv) : QApplication(argc, argv)
{
    m_gestureEngine = new QStm_GestureEngineApi();
}

void BrowserApp::setGestureHelper(WebGestureHelper* gh)
{
    m_gestureHelper = gh;
}

bool BrowserApp::symbianEventFilter(const QSymbianEvent *event)
{
#if(ENABLE_GESTURE_LIB)
    return m_gestureHelper->symbianEventFilter(event);
#else
    return false;
#endif
}


bool BrowserApp::x11EventFilter ( XEvent* event )
{
#if (ENABLE_GESTURE_LIB)
    return m_gestureHelper->x11EventFilter(event);
#else
    return false;
#endif
}

QStm_QtDummyGestureRecognizer::QStm_QtDummyGestureRecognizer(QStm_GestureContext* ctx) :
                                      QGestureRecognizer(),
                                      m_context(ctx),
                                      m_currentGesture(NULL)
{
}

QStm_QtDummyGestureRecognizer::~QStm_QtDummyGestureRecognizer()
{
}


QGesture* QStm_QtDummyGestureRecognizer::create(QObject* /*target*/)
{
    return new QStm_Gesture();
}

QGestureRecognizer::Result QStm_QtDummyGestureRecognizer::recognize(QGesture *state,
                                                                    QObject */*watched*/,
                                                                    QEvent *event)
{
    QGestureRecognizer::Result ret = QGestureRecognizer::Ignore;
    if (event->type() == QStm_GestureEvent::stmGestureEventType()) {
        QStm_Gesture* gesture = static_cast<QStm_Gesture*>(state);
        *gesture = *m_currentGesture;
        ret = (m_currentGesture->gestureState() == Qt::GestureFinished) ? QGestureRecognizer::FinishGesture :
                                                                   QGestureRecognizer::TriggerGesture;
    }
    return ret;
}

void QStm_QtDummyGestureRecognizer::reset(QGesture */*state*/)
{

}



QStm_GestureListenerApiIf::QStm_ProcessingResult QStm_QtDummyGestureRecognizer::handleGestureEvent(
                                              QStm_GestureUid uid, QStm_GestureIf* gesture)
{
    if (!m_currentGesture) {
        Q_ASSERT(gesture);
        m_currentGesture = new QStm_Gesture();
    }

    int stmGestType = gesture ? gesture->getType() : -1;

    switch (uid) {
        case EGestureUidTap:
        {
            QStm_TapType type = qstmGesture::QStm_TapType(stmGestType);
            if (type == qstmGesture::ETapTypeDouble) {
                m_currentGesture->setGestureStmType(QStmDoubleTapGestureType);
            }
            else  {
                m_currentGesture->setGestureStmType(QStmTapGestureType);
            }
            break;
        }

        case EGestureUidTouch:
        {
            m_currentGesture->setGestureStmType(QStmTouchGestureType);
            break;
        }

        case EGestureUidRelease:
        {
            m_currentGesture->setGestureStmType(QStmReleaseGestureType);
            break;
        }

        case EGestureUidFlick:
        {
            m_currentGesture->setGestureStmType(QStmFlickGestureType);
            break;
        }

        case EGestureUidLeftRight:
        {
            m_currentGesture->setGestureStmType(QStmLeftRightGestureType);
            break;
        }

        case EGestureUidUpDown:
        {
            m_currentGesture->setGestureStmType(QStmUpDownGestureType);
            break;
        }

        case EGestureUidPan:
        {
            m_currentGesture->setGestureStmType(QStmPanGestureType);
            break;
        }

        case EGestureUidHover:
        {
            m_currentGesture->setGestureStmType(QStmHoverGestureType);
            break;
        }

        case EGestureUidLongPress:
        {
            m_currentGesture->setGestureStmType(QStmLongPressGestureType);
            break;
        }

        case EGestureUidEdgeScroll:
        {
            m_currentGesture->setGestureStmType(QStmEdgeScrollGestureType);
            break;
        }

        case EGestureUidCornerZoom:
        {
            m_currentGesture->setGestureStmType(QStmCornerZoomGestureType);
            break;
        }

        case EGestureUidPinch:
        {
            m_currentGesture->setGestureStmType(QStmPinchGestureType);
            break;
        }
        case EGestureUidUnknown:
        {
            m_currentGesture->setGestureStmType(QStmUknownGestureType);
            break;
        }
    }

    if (gesture) { //gesture enter
        m_currentGesture->setGestureSubType(gesture->getType());
        m_currentGesture->setDirection(gesture->getDirection());
        QPoint vec = gesture->getLengthAndDirection();
        m_currentGesture->setLengthAndDirection(vec);
        m_currentGesture->setSpeed(gesture->getSpeed());
        QPoint pos = gesture->getLocation();
        m_currentGesture->setPosition(pos);
        m_currentGesture->setDetails(gesture->getDetails());
        m_currentGesture->setSpeedVec(gesture->getSpeedVec());
        QWidget* w = static_cast<QWidget*>(gesture->target());
        m_currentGesture->setTarget(w);
        m_currentGesture->setGestureState(Qt::NoGesture);
    }
    else {
        m_currentGesture->setGestureState(Qt::GestureFinished);
    }

    QStm_GestureEvent dummy;
    m_currentGesture->sendEvent(&dummy);

    return QStm_GestureListenerApiIf::EContinue;
}




