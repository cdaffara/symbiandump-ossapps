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
* Description:
*
*/

#ifndef HSWIDGETHOST_H
#define HSWIDGETHOST_H

#include <QObject>
#include <QMetaMethod>
#include <QMetaProperty>

#include "hsdomainmodel_global.h"
#include "hsdomainmodeldatastructures.h"

#include "hstest_global.h"
HOMESCREEN_TEST_CLASS(TestHsDomainModel)

class QStateMachine;

class HsWidgetTouchArea;
class HsWidgetComponent;
class HsPage;
class HsWidgetHostVisual;

class HSDOMAINMODEL_EXPORT HsWidgetHost : public QObject
{
    Q_OBJECT

public:
    HsWidgetHost(int databaseId, QObject *parent = 0);
    ~HsWidgetHost();

    static HsWidgetHost *createInstance(
        HsWidgetData &widgetData, 
        const QVariantHash &preferences = QVariantHash());

    int databaseId() const;

    bool setPage(HsPage *page);
    HsPage *page() const; 
    
    bool loadPresentation();
    bool loadPresentation(Qt::Orientation orientation);
    bool savePresentation();
    bool savePresentation(Qt::Orientation orientation);    
    bool savePresentation(HsWidgetPresentationData &presentation);
    bool getPresentation(HsWidgetPresentationData &presentation);
    bool removePresentation(Qt::Orientation orientation);
    
    HsWidgetHostVisual *visual() const;
signals:
    void event_startAndShow();
    void event_startAndHide();
    void event_unload();
    void event_show();
    void event_hide();
    void event_remove();
    void event_close();
    void event_finished();
    void event_faulted();
    
    void finished();
    void faulted();
    void available();
    void unavailable();

public slots:
    void startWidget(bool show = true);
    void showWidget();
    void hideWidget();
    void setOnline(bool online = true);
    
    void remove();
    void close();

    void startDragEffect();
    void startDropEffect();

protected:
/*    bool eventFilter(QObject *watched, QEvent *event);
    void gestureEvent(QGestureEvent *event);
    void mousePressEvent(QGraphicsSceneMouseEvent *) {}
*/
private:
    Q_DISABLE_COPY(HsWidgetHost)
  
    void setupStates();

    bool setProperty(const char *name, QMetaProperty &property); 
    bool setMethod(const char *signature, QMetaMethod &method);    
    bool hasSignal(const char *signature);

  
    bool setPreferencesToWidget();

private slots:
    void action_connectComponent();
    void action_disconnectComponent();
    void action_load();
    void action_unload();
    void action_initialize();
    void action_uninitialize();
    void action_show();
    void action_hide();
    void action_finished();
    void action_faulted();
    void action_remove();
    void action_notifyRemove();
    
    void onFinished();
    void onError();
    void onSetPreferences(const QStringList &names);

private:
    int mDatabaseId;
    HsWidgetHostVisual *mVisual;
    QStateMachine *mStateMachine;
    QObject *mWidget;
    HsPage *mPage;    
    HsWidgetComponent *mComponent;

    QMetaMethod mOnInitializeMethod;
    QMetaMethod mOnShowMethod;
    QMetaMethod mOnHideMethod;
    QMetaMethod mOnUninitializeMethod;    
    QMetaProperty mIsOnlineProperty;
	QMetaProperty mRootPathProperty;        
    bool mIsFinishing;

    HOMESCREEN_TEST_FRIEND_CLASS(TestHsDomainModel)
};

#endif // HSWIDGETHOST_H
