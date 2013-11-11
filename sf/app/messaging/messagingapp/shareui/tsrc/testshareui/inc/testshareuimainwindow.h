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

#ifndef TESTSHAREUIMAINWINDOW_H
#define TESTSHAREUIMAINWINDOW_H

#include <hbmainwindow.h>

class TestShareUiView;


class TestShareUiMainWindow : public HbMainWindow
{
    Q_OBJECT

public:
    TestShareUiMainWindow(QWidget *parent = 0);
    ~TestShareUiMainWindow();
    bool init();
private:
    TestShareUiView*        mView;

};

#endif // SENDUITESTMAINWINDOW_H
