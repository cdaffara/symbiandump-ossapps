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
 *     Zhiqiang Yang <zhiqiang.yang@nokia.com>
 * 
 * Description:
 *     The header file of File Manager main window
 */

#ifndef FMMAINWINDOW_H
#define FMMAINWINDOW_H

#include "fmcommon.h"
#include "fmviewmanager.h"

#include <hbmainwindow.h>
#include <QTimer>
class HbAction;

class FmMainWindow : public HbMainWindow
{
    Q_OBJECT
public:
    FmMainWindow();
    ~FmMainWindow();

signals:
    void applicationReady();

private slots:
    void onOrientationChanged( Qt::Orientation orientation );
    void delayedLoading();
    void onApplicationReady();
private:
    void init();

    FmViewManager *mViewManager;
    bool mFirstViewLoaded;

    // timer used to get opportunity for emit applicationReady signal.
    QTimer mShowTimer;
};

#endif

