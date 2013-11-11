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
*     Bio control for Provisioning documents.
*
*/

#include <hbmainwindow.h>
#include <CpQtSpMainWindow.h>
#include <CpQtSpView.h>
#include <HbScrollArea>
#include <HbWidget>
#include <HbAction>
#include <QCoreApplication>

// ----------------------------------------------------------------------------
// CpQtSpMainWindow::CpQtSpMainWindow
// Description: Construtor
// ----------------------------------------------------------------------------

CpQtSpMainWindow::CpQtSpMainWindow(QWidget *parent)
    : HbMainWindow(parent)
{
    iServiceProvider = new CpQtSp(this);
    connect(iServiceProvider,SIGNAL(showView()),this, SLOT(showView()));        
    connect(this,SIGNAL(aboutToChangeOrientation()),this,SLOT(OrientationChange()));
    iView = new CpQtSpView(iServiceProvider,this);
    }

// ----------------------------------------------------------------------------
// CpQtSpMainWindow::~CpQtSpMainWindow
// Description: Destructor
// ----------------------------------------------------------------------------

CpQtSpMainWindow::~CpQtSpMainWindow()
{    
    delete iServiceProvider;
}

// ----------------------------------------------------------------------------
// CpQtSpMainWindow::OrientationChange
// Description: Slot for orientation change notification
// ----------------------------------------------------------------------------

void CpQtSpMainWindow::OrientationChange()
	{
    if(iView->getOrientationFlag())
        {
		if(orientation() == Qt::Horizontal)
			{
				iView->setHorizontal();
			}
			else
			{
				iView->setVertical();
			}
        }
	}

// ----------------------------------------------------------------------------
// CpQtSpMainWindow::showView
// Description: Slot for construction of provisioning view - called by CpQtSp
// ----------------------------------------------------------------------------

void CpQtSpMainWindow::showView()
{  
   iView->constructView();
}

void CpQtSpMainWindow::buildView()
 {
    HbScrollArea *area = new HbScrollArea();
    area->setFlag(QGraphicsItem::ItemClipsChildrenToShape, false);    
    area->setScrollDirections(Qt::Horizontal|Qt::Vertical);     
    area->setVerticalScrollBarPolicy(HbScrollArea::ScrollBarAsNeeded);
    area->setHorizontalScrollBarPolicy(HbScrollArea::ScrollBarAsNeeded);
                
    HbWidget* cont = new HbWidget();
    layout = new QGraphicsLinearLayout(Qt::Vertical);
    iView->buildView(layout);    
    cont->setLayout(layout);
    area->setContentWidget(cont);    
    HbToolBar* toolBar = iView->toolBar();
    HbAction* saveAction = new HbAction(hbTrId("txt_common_button_save"));     
    connect(saveAction, SIGNAL(triggered()), iView, SLOT(saveProvisoningMessage()));        
    HbAction* DeleteAction = new HbAction(hbTrId("txt_device_update_menu_delete"));
    connect(DeleteAction, SIGNAL(triggered()), iView, SLOT(deleteMessage()));
    toolBar->addAction(saveAction);
    toolBar->addAction(DeleteAction);
    toolBar->setOrientation(Qt::Horizontal);
    iView->setWidget(area);        
    addView(iView);
}

void CpQtSpMainWindow::closeWindow()
    {
    this->close();
    }
    
