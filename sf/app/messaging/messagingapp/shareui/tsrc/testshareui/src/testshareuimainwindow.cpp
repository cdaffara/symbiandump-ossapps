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

#include "testshareuimainwindow.h"
#include "testshareuiview.h"
#include <QtDebug>

TestShareUiMainWindow::TestShareUiMainWindow(QWidget *parent)
    : HbMainWindow(parent)
{



}

TestShareUiMainWindow::~TestShareUiMainWindow()
{

}


bool TestShareUiMainWindow::init()
    {
    qDebug() << "[CvProto.cpp] " << "init()";
    if((mView = new TestShareUiView(this)) == NULL)
        {
        qDebug() << "[CvProto.cpp] " << "DeviceView alloc failed";
        return false;
        }
    
    if(!mView->init())
        {
        qDebug() << "[CvProto.cpp] " << "DeviceView init failed";
        return false;        
        }
    addView(mView);
    setCurrentView(mView,false);
    qDebug() << "[CvProto.cpp] " << "init() succeeded";
    return true;

    }
