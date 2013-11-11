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

#ifndef CNTMAINWINDOW_H
#define CNTMAINWINDOW_H

#include <hbmainwindow.h>

#include "cntglobal.h"
#include <cntviewparams.h>

class CntDefaultViewManager;
class XqKeyCapture;

class QTPBK_EXPORT CntMainWindow : public HbMainWindow
{
    Q_OBJECT
    friend class TestCntMainWindow;

public:
    CntMainWindow(QWidget *parent=0, int defaultView = namesView);
    virtual ~CntMainWindow();

private:
    CntDefaultViewManager        *mViewManager;
    int                           mDefaultView;

};

#endif // CNTMAINWINDOW_H
