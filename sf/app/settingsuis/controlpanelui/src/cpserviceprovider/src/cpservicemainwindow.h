/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0""
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

#ifndef CPSERVICEMAINWINDOW_H
#define CPSERVICEMAINWINDOW_H

#include <hbmainwindow.h>
#include <QPointer>

class CpLauncherService;
class CpBaseSettingView;

class CpServiceMainWindow : public HbMainWindow
{
    Q_OBJECT
public:
    explicit CpServiceMainWindow(QWidget *parent = 0);
    virtual ~CpServiceMainWindow();
    
    /*
     * set the setting view as current view
     */
    void setSettingView(CpBaseSettingView *settingView);
    
    /*
     * close current setting view
     */
    void closeSettingView();
    
public slots:
    void quit();
private:
    CpLauncherService *mLauncherService;
    HbView *mPreviousView;
    QPointer<CpBaseSettingView> mSettingViewPointer;
};

#endif // CPSERVICEMAINWINDOW_H

//End of File
