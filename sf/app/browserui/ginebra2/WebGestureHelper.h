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

#ifndef WEBGESTUREHELPER_H_
#define WEBGESTUREHELPER_H_
#include "qstmgestureapi.h"
#include "qstmgestureevent.h"
#include <QApplication>
//#include "BWFGlobal.h"

#if !defined(Q_WS_X11)
#define XEvent void
#endif


class QStm_QtDummyGestureRecognizer;

class WebGestureHelper
{
public:
    WebGestureHelper(QWidget* ctrl);

    virtual ~WebGestureHelper();

    QStm_GestureEngineApi* gestureEngine() { return m_gestureEngine; }
    QStm_GestureContext*   gestureContext(){ return m_gestures; }
    bool symbianEventFilter(const QSymbianEvent *event);
    bool x11EventFilter (XEvent* event);
    void setupGestureEngine(QWidget* ctrl);

private:
    QStm_GestureEngineApi* m_gestureEngine;
    QStm_GestureContext* m_gestures;
    QStm_GestureParameters* m_parameters;
    QWidget* m_ctrl;
    QStm_QtDummyGestureRecognizer* m_dummyRecognizer;
    Qt::GestureType m_qstmGestureType;

};




#define browserApp   (static_cast<BrowserApp*>(QApplication::instance()))

class BrowserApp : public QApplication
{
    Q_OBJECT
public:
    BrowserApp(int argc, char** argv);
    virtual bool symbianEventFilter(const QSymbianEvent *event);
    virtual bool x11EventFilter (XEvent* event);
    void  setGestureHelper(WebGestureHelper* gh);
    WebGestureHelper* gestureHelper() { return m_gestureHelper; }

    void setMainWindow(QWidget* mw) { m_mainWindow = mw; }
    QWidget* mainWindow() { return m_mainWindow; }
    QStm_GestureEngineApi* gestureEngine() { return m_gestureEngine; }

private:
    WebGestureHelper*        m_gestureHelper;
    QWidget*                 m_mainWindow;
    QStm_GestureEngineApi*   m_gestureEngine;
};



class WebGestureHelperEventFilter : public QObject
{
public:
    WebGestureHelperEventFilter() {}
    virtual ~WebGestureHelperEventFilter() {}
    static  WebGestureHelperEventFilter* instance();
    bool eventFilter(QObject* receiver, QEvent* event);

private:
    static WebGestureHelperEventFilter* m_instance;
};


class QStm_QtDummyGestureRecognizer : public QGestureRecognizer,
                                      public QStm_GestureListenerApiIf
{
public:
    QStm_QtDummyGestureRecognizer(QStm_GestureContext* ctx);
    ~QStm_QtDummyGestureRecognizer();
    QGesture* create(QObject* target);
    QGestureRecognizer::Result recognize(QGesture *state,
                                         QObject *watched,
                                         QEvent *event);
    void reset(QGesture *state);

    //from QStm_GestureListenerApiIf
    QStm_ProcessingResult handleGestureEvent(qstmGesture::QStm_GestureUid uid, qstmGesture::QStm_GestureIf* gesture);

private:
    QStm_GestureContext* m_context;
    QStm_Gesture* m_currentGesture;
};

#endif /* GESTUREHELPER_H_ */
