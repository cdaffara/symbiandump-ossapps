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
* Description:  Offers message creation and sending services.
 *
*/

#ifndef TESTSHAREUIVIEW_H_
#define TESTSHAREUIVIEW_H_

#include <hbview.h>
#include <hbtoolbar.h>
#include <hbmainwindow.h>

class ShareUi;

class TestShareUiView : public HbView
    {
    Q_OBJECT
    
public:
    TestShareUiView(HbMainWindow* parent = 0);
    virtual ~TestShareUiView();
    bool init();
    
public slots:

    void openCvPop();
    
private:
    HbToolBar* mToolBar;
    ShareUi* mDialog;
    };

#endif /* DEVICEVIEW_H_ */
