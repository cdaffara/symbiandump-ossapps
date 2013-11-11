/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
*
*/

#ifndef CPQTSPMAINWINDOW_H
#define CPQTSPMAINWINDOW_H

#include <hbmainwindow.h>
#include <HbWidget>
#include <QGraphicsLinearLayout>

class CpQtSpView;
class CpQtSp;

class CpQtSpMainWindow : public HbMainWindow
{
    Q_OBJECT
    
public:
     CpQtSpMainWindow(QWidget *parent=0);
     ~CpQtSpMainWindow(); 
     void buildView();
     void closeWindow();
     
public slots:
    void showView();
    void OrientationChange();
private:
    CpQtSp* iServiceProvider;
    CpQtSpView* iView;    
    HbWidget* widget;
    QGraphicsLinearLayout* layout;    
};

#endif // CPQTSPMAINWINDOW_H
