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
* Description: Communication Launcher widget for Friend widget 
*
*/

#ifndef COMMLAUNCHERWIDGET_H
#define COMMLAUNCHERWIDGET_H

#include <HbPopup>
#include <qtcontacts.h>
#include <xqaiwrequest.h>
#include <QObjectCleanupHandler>
#include <QScopedPointer>

#include "contactwidgetplugin_global.h"

class QGraphicsLinearLayout;
class HbPushButton;
class HbDocumentLoader;
class XQApplicationManager;

QTM_USE_NAMESPACE

/**
* CommLauncherWidget
*/
class CommLauncherWidget : public HbPopup
{
    Q_OBJECT

public:

    CommLauncherWidget(QGraphicsItem *parent = 0);
    ~CommLauncherWidget();
    
    void setContact(QContact &contact);    
    void setApplicationManager(XQApplicationManager &appManager);
    void createUI();
    void openPhonebookCreateNew();
    
    QPointF commLauncherPosition(QPointF widgetPosition, QRectF& widgetBoundingRect,
                                 QRectF& sceneRect, QRectF& launcherRect);
    void selectAppearEffect(QPointF FriendPos, QPointF LauncherPos);
    
    bool isPendingRequest();
    
signals:
    void launcherClosed();
    void requestCompleted();
        
protected:
    void keyPressEvent(QKeyEvent *event);
    
private:
    HbPushButton* createButton(const QString iconName, const QString buttonName,
                               const HbDocumentLoader *documentLoader);

private slots:
    void popupAboutToShow();
    void popupAboutToClose();

    void makeCall();
    void sendMessage(); 
    void sendEmail();
    void openPhonebook();
    
    void handleRequestOk(const QVariant& value);
    
    int commLauncherWidth();

private:

    QContact *mContact;
    QGraphicsLinearLayout *mLayout;
    
    int mButtonCount;
    
    

    XQAiwRequest *mRequest;
    QContactAction* mCommLauncherAction;
    
    HbPushButton *mCallButton;
    HbPushButton *mSendMsgButton;
    HbPushButton *mEmailButton;
    HbPushButton *mPhonebookButton;
    
    XQApplicationManager *mApplicationManager;
    QString mAppearEffect;
    
    QObjectCleanupHandler mCleanupHandler;
    
    bool mPendingRequest;
    
    CONTACTWIDGET_TEST_FRIEND_CLASS(TestContactWidget)

};

#endif //COMMLAUNCHERWIDGET_H
