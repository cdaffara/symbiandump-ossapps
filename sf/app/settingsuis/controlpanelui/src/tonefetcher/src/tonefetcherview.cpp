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
 *     The source file for tone fetcher view.
 *     
 */
#include "tonefetcherview.h"
#include "tonefetcherwidget.h"
#include <hbaction.h>
#include <hbtoolbar.h>
#include "tonefetcher.h"
#include <qcoreapplication.h>

ToneFetcherView::ToneFetcherView(ToneFetcher *service) : mServiceProvider(service)
{    
    setTitle(hbTrId("Tone Selection"));//need change according to ps file
    initToolBar();
    initMainWidget();
    QMetaObject::connectSlotsByName(this);
    
}

ToneFetcherView::~ToneFetcherView()
{    
    removeToolBarAction();
}

void ToneFetcherView::initMainWidget()
{
    mWidget = new ToneFetcherWidget(this, this);
    Q_ASSERT(mWidget);
    setWidget(mWidget);
    //mWidget->setCurrentToolBarType( ToneServiceWidget::GeneralTone );
    connect(mWidget, SIGNAL(triggerToolBar(bool)), this, SLOT(enableToolBar(bool)));
}

void ToneFetcherView::quit()
{    
    connect(mServiceProvider, SIGNAL(returnValueDelivered()), qApp, SLOT(quit()));
    mServiceProvider->complete();
}

void ToneFetcherView::initToolBar()
{
    mToolBarLeftAction = new HbAction(this);
    mToolBarLeftAction->setObjectName("leftAction");
    mToolBarLeftAction->setText(hbTrId("Play/Pause"));//need change according to ps file
    toolBar()->addAction(mToolBarLeftAction);
    mToolBarLeftAction->setEnabled(false);
    
    mToolBarRightAction = new HbAction(this);
    mToolBarRightAction->setObjectName("rightAction");
    mToolBarRightAction->setText(hbTrId("Select"));//need change according to ps file
    mToolBarRightAction->setEnabled(false);    
    toolBar()->addAction(mToolBarRightAction);
    
    toolBar()->setOrientation(Qt::Horizontal);
    toolBar()->setEnabled(false); 
    
}

void ToneFetcherView::on_leftAction_triggered()
{
    mWidget->playOrPause();   
    
}

void ToneFetcherView::on_rightAction_triggered()
{       
    emit itemSelected(mWidget->getCurrentItemPath());
    quit();
    /*QDir dir("c:\\data\\Sounds\\Simple\\");
    dir.remove("def.aac");
    dir.remove("abc.aac");   */ 
}

void ToneFetcherView::enableToolBar(bool enable)
{
    mToolBarRightAction->setEnabled(enable);
    mToolBarLeftAction->setEnabled(enable);
    toolBar()->setEnabled(enable); 
}

void ToneFetcherView::removeToolBarAction()
{
    toolBar()->removeAction(mToolBarRightAction);
    toolBar()->removeAction(mToolBarLeftAction);
}
//End of File
