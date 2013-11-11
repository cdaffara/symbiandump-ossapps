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
#ifndef IRVIEWMANAGER_H
#define IRVIEWMANAGER_H

#include <QStack>
#include <hbmainwindow.h>

#include "irviewdefinitions.h"

class IRApplication;
class IRBaseView;
class QTimer;

class ActivityInformation
{
public:
    QPixmap screenShot;
    QString activityId;
    TIRViewId viewId;
};

class IRViewManager : public HbMainWindow
{
    Q_OBJECT
public:
    IRViewManager();
    ~IRViewManager();
    void setApplication(IRApplication *aApplication);
    
    IRBaseView* getView(TIRViewId aViewId, bool aCreateIfNotExist = false);
    TIRViewId getExitingView();
    
    bool isViewInStack(TIRViewId aViewId) const;
    
    void activateView(TIRViewId aViewId, bool aPushCurrentView = true);
    
    void activateView(IRBaseView *aView, bool aPushCurrentView = true);
    
    void pushViewById(TIRViewId aViewId);
    
    TIRViewId currentViewId() const;
    
    bool isExiting() const;
       
    void saveScreenShot();
    
    void saveActivity();
    
    void removeActivity();    
    
    
public slots:
    void backToPreviousView();

protected:
    void mousePressEvent(QMouseEvent *aEvent);
    void mouseReleaseEvent(QMouseEvent *aEvent);

    void paintEvent(QPaintEvent *aEvent);
    
private slots:
    void handleViewReady();
    void handleCurrentViewChanged(HbView *aView);
    void crossLineReset();
    void exitTimeout();
    void handleSaveScreenShot();
    
private:
    void backToView(TIRViewId aViewId);
    IRBaseView* createView(IRApplication* aApplication, TIRViewId aViewId);
    void clearStack();
    void updateSoftkey();
    
    bool readyToQuit();
    
    void switchToNextView(IRBaseView *aView);
    void backupActivity();
    
private:
    IRApplication* iApplication;
    QStack<IRBaseView*> iViewStack;
    HbAction       *iBackAction;
    HbAction       *iExitAction;
    
    bool iCrossLineAReady;
    bool iCrossLineBReady;
    bool iCrossLineEnable;

    bool iCrossLineShowing;

    QLineF iCrossLineA;
    QLineF iCrossLineB;

    QTimer *iCrossLineTimer;
    QTimer *iExitTimer;
    
    bool iExiting;    
    QMap<TIRViewId, QPixmap> iScreenShots;
    ActivityInformation iActivityBackup;
    bool iIsEmbedded;
};

#endif
