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
* 
* Description:
*     
*
*/

#include <hbdialog.h>
#include <hblistwidget.h>

#include <hbapplication.h>
#include <hbmainwindow.h>
#include <hbview.h>
#include <hblabel.h>
#include <hbaction.h>
#include <hblistwidgetitem.h>

#include <QGraphicsLinearLayout>


class TestMainWindow : public HbMainWindow
{
    Q_OBJECT
public:
    TestMainWindow();
    ~TestMainWindow();

    void init();
private slots:
    void activatedListItem(HbListWidgetItem *item);

private:
    void getExistingDirectory();
    void getOpenFileName();
    void getSaveFileName();

    void getExistingDirectory_SetDir();
    void getOpenFileName_SetDir();
    void getSaveFileName_SetDir();

    void getExistingDirectory_SetFilter();
    void getExistingDirectory_SetFilter_AllDirs();
    void getOpenFileName_SetFilter();
    void getSaveFileName_SetFilter();
};
