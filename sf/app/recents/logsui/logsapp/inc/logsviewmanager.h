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
#ifndef LOGVIEWMANAGER_H
#define LOGVIEWMANAGER_H


#include <QObject>
#include <QList>
#include <QVariant>
#include <xqaiwdecl.h>
#include "logsabstractviewmanager.h"

class HbMainWindow;
class LogsComponentRepository;
class LogsServiceHandler;
class LogsServiceHandlerOld;
class LogsMainWindow;
class HbView;
class LogsBaseView;
class LogsAppSettings;
class LogsForegroundWatcher;

/**
 * 
 */
class LogsViewManager : public QObject, public LogsAbstractViewManager
{
    Q_OBJECT
    
    friend class UT_LogsViewManager;
    friend class UT_LogsServiceHandler;

public:
  
    /**
     * Constructor
     * @param mainWindow
     * @param service
     */
    LogsViewManager( LogsMainWindow& mainWindow, LogsServiceHandler& service,
            LogsServiceHandlerOld& serviceOld, LogsAppSettings& settings );
    ~LogsViewManager();

public slots:

    void changeRecentViewViaService(
            XQService::LogsViewIndex view, bool showDialpad, QString dialpadText);
    void changeMatchesViewViaService(QString dialpadText);
    void changeRecentView(XQService::LogsViewIndex view, bool showDialpad);
    
public: // From LogsAbstractViewManager
    
    virtual bool activateView(LogsAppViewId viewId);
    virtual bool activateView(LogsAppViewId viewId, bool showDialpad, QVariant args);
    virtual bool activatePreviousView();
    virtual HbMainWindow& mainWindow();
    virtual void exitApplication();
    
    
private slots:

    void proceedExit();
    void handleOrientationChanged();
    void completeViewActivation();
    void saveActivity();
    void closeEmbeddedApplication();
    void appGainedForeground();
    void appLostForeground();
    void activityRequested(const QString &activityId);
    void bgStartupForegroundGained();
    
private:
    
    bool doActivateView(LogsAppViewId viewId, bool showDialpad, 
                        QVariant args, const QString& dialpadText = QString(), 
                        bool reset = false);
    bool loadActivity();
    LogsAppViewId checkMatchesViewTransition(
        LogsAppViewId viewId, const QString& dialpadText);
    void handleFirstActivation();
    LogsBaseView* createView(LogsAppViewId viewId);
    void doFakeExit();
    void endFakeExit();
    bool doLoadActivity(const QString& activityId);
    void clearActivities();
    void activateViewViaService(
        LogsAppViewId viewId, bool showDialpad, 
        const QString& dialpadText, const QVariant& args = QVariant());
    void setTaskSwitcherVisibility(bool visible);
    void doExitApplication(bool viewExitHandling = true);
    void activateDefaultView();
    
private: //data 
    
    LogsMainWindow& mMainWindow;
    LogsServiceHandler& mService;
    LogsServiceHandlerOld& mServiceOld;
    LogsAppSettings& mSettings;
    LogsComponentRepository* mComponentsRepository;
    QList<LogsBaseView*> mViewStack;
    bool mFirstActivation;
    QVariant mViewActivationArgs;
    bool mViewActivationShowDialpad;
    LogsForegroundWatcher* mBackgroundStartupWatcher;
    
};

#endif //LOGVIEWMANAGER_H
