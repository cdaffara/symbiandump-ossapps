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

#ifndef CPMAINWINDOW_H
#define CPMAINWINDOW_H

#include <hbmainwindow.h>
class QTimer;
class CpMainWindow : public HbMainWindow
{
    Q_OBJECT    
public:
    explicit CpMainWindow(QWidget *parent = 0, Hb::WindowFlags windowFlags = Hb::WindowFlagNone);
    ~CpMainWindow();
public:
    virtual bool event(QEvent *e);
signals:
	/**
	 * Emit when control panel's startup is finished
	 */
    void applicationReady();
private slots:
    void onStartupFininshed();

private:
    QTimer *mStartupTimer;
};

#endif // CPMAINWINDOW_H

//End of File

