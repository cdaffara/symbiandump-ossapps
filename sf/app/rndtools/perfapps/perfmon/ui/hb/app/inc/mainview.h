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

#ifndef MAINVIEW_H
#define MAINVIEW_H

#include <hbview.h>

#include "valuedatacontainer.h"
#include "graphdatacontainer.h"

class HbAction;
class EngineWrapper;

class MainView : public HbView
{
    Q_OBJECT

public:
    MainView(EngineWrapper &engine);
    ~MainView();

private:
    void createMenu();

public slots:
    void showValues();
    void showGraphs();
    void toggleLogging();
    void showAbout();

signals:
    void settingsCommandInvoked();

private:
    void updateLoggingAction();

private:
    EngineWrapper &mEngine;

    ValueDataContainer *mValueDataContainer;
    GraphDataContainer *mGraphDataContainer;

    HbAction *mValuesAction;
    HbAction *mGraphAction;
    HbAction *mSwitchViewAction;
    HbAction *mLoggingAction;
};

#endif // MAINVIEW_H
