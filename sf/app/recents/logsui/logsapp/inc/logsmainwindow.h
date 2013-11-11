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
#ifndef LOGSMAINWINDOW_H
#define LOGSMAINWINDOW_H


#include <QObject>
#include <hbmainwindow.h>

class XQKeyCapture;

/**
 * 
 */
class LogsMainWindow : public HbMainWindow
    {
    Q_OBJECT
    
public:
    
    LogsMainWindow();
    virtual ~LogsMainWindow();
    
    void sendAppToBackground();
    void bringAppToForeground();
    
    bool isForeground() const;
    
protected: // From HbMainWindow
    
    void keyPressEvent( QKeyEvent *event );
    bool eventFilter(QObject *obj, QEvent *event);
    
signals:
        
    void callKeyPressed();
    void localeChanged();
    void appGainedForeground();
    void appLostForeground();
    
private slots:

    void obscuredCalled();
    void revealedCalled();

private:
    void startKeyCapture();
    void stopKeyCapture();
private:
    
    XQKeyCapture* mKeyCapture;
    bool mForeground;
    bool mLocaleChanged;
    
    friend class UT_LogsMainWindow;
    friend class UT_LogsServiceHandler;
    friend class UT_LogsViewManager;
    
    };


#endif //LOGSMAINWINDOW_H
