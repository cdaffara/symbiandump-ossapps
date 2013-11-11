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
* Description: 
*
*/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <HbMainWindow>

class EngineWrapper;
class MainView;
class SettingsView;

class MainWindow : public HbMainWindow
{
    Q_OBJECT
public:
    MainWindow(EngineWrapper &engine, QWidget *parent = 0);

public slots:
    void showMainView();
    void showSettings();

private:
    void clearViews();

private:
    EngineWrapper &mEngine;
    MainView* mMainView;
    SettingsView* mSettingsView;
};

#endif // MAINWINDOW_H
