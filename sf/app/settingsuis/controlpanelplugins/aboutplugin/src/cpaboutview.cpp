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

#include "cpaboutview.h"
#include "cpaboutthirdpartyview.h"
#include "cpaboutopensourceview.h"
#include "cpaboututils.h"
#include <hbgroupbox.h>
#include <HbMainWindow>
#include <HbInstance>
#include <HbAction>
#include <HbScrollArea>
#include <QGraphicsLinearLayout>
#include <HbParameterLengthLimiter>

/*!
    \class CpAboutView
*/
    
/*!
    Constructor
*/
CpAboutView::CpAboutView(QGraphicsItem *parent) 
:   CpBaseSettingView(0, parent),
    mThirdPartyView(0),
    mThirdPartyBackAction(0),
    mOpenSourceView(0),
    mOpenSourceBackAction(0)
{
    //application title
    setTitle(hbTrId("txt_cp_title_control_panel"));
    //view layout
    QGraphicsLinearLayout *layout = new QGraphicsLinearLayout(Qt::Vertical);  
    layout->setContentsMargins(0, 0, 0, 0);
    //view's title
    HbGroupBox *label = new HbGroupBox();
    label->setHeading(hbTrId("txt_cp_subhead_about"));
    layout->addItem(label);
    
    //phone model
    QString phoneInfo(HbParameterLengthLimiter("txt_cp_subhead_device_model").arg(\
            CpAboutUtils::getPhoneModel()));   
    phoneInfo.append(doubleHtmlLineBreak);   
    //product release
    phoneInfo.append(hbTrId("txt_cp_subhead_product_release"));
    phoneInfo.append(htmlLineBreak);  
    phoneInfo.append(HbParameterLengthLimiter("txt_cp_info_product_release").arg(\
            CpAboutUtils::getProductRelease()));
    phoneInfo.append(doubleHtmlLineBreak);
    //software version
    phoneInfo.append(hbTrId("txt_cp_subhead_software_version"));
    phoneInfo.append(htmlLineBreak); 
    phoneInfo.append(HbParameterLengthLimiter("txt_cp_info_software_vesion").arg(\
                CpAboutUtils::getSoftwareVersion()));
    phoneInfo.append(doubleHtmlLineBreak);    
    //produce type
    phoneInfo.append(hbTrId("txt_cp_subhead_type"));
    phoneInfo.append(htmlLineBreak);
    phoneInfo.append(HbParameterLengthLimiter("txt_cp_info_type").arg(\
            CpAboutUtils::getPhoneType()));        
    //first text edit for showing phone and release info
    HbTextEdit *first = CpAboutUtils::createTextEdit();
    first->setHtml(CpAboutUtils::preprocessText(phoneInfo));    
    layout->addItem(first);   
    //second text edit for showing nokia copy right.
    HbTextEdit *second = CpAboutUtils::createTextEdit();
    QString info(hbTrId("txt_cp_info_1"));    
    second->setHtml(CpAboutUtils::preprocessText(info));   
    layout->addItem(second);    
    //thirdParty content.
    HbTextEdit *thirdParty = CpAboutUtils::createTextEdit();    
    thirdParty->setHtml(CpAboutUtils::linkHtmlContent(hbTrId("txt_cp_3rd_party_notices")));    
    layout->addItem(thirdParty);
    connect(thirdParty, SIGNAL(anchorTapped(QString)), this, SLOT(openThirdPartyView()));
    //Open source notices
    HbTextEdit *openSourceEdit = CpAboutUtils::createTextEdit();
    openSourceEdit->setHtml(CpAboutUtils::linkHtmlContent(hbTrId("txt_cp_open_source_software_notices")));
    layout->addItem(openSourceEdit);
    connect(openSourceEdit, SIGNAL(anchorTapped(QString)), this, SLOT(openOpenSourceView()));
    
    HbScrollArea* scrollArea = new HbScrollArea();    
    QGraphicsWidget* widget = new QGraphicsWidget();    
    widget->setLayout(layout);   
    scrollArea->setContentWidget(widget);    
    scrollArea->setScrollDirections(Qt::Vertical);
    setWidget(scrollArea);   
}

/*!
    Destructor
*/
CpAboutView::~CpAboutView()
{
 //   delete mModel;
    delete mThirdPartyBackAction;
    delete mOpenSourceBackAction;
}

/*!
    Opens the third party view
*/  
void CpAboutView::openThirdPartyView() 
{
    HbMainWindow *mainWindow = hbInstance->allMainWindows().first();    
    mThirdPartyView = new CpAboutThirdPartyView(); 
    mainWindow->addView(mThirdPartyView);
    
    mThirdPartyBackAction = new HbAction(Hb::BackNaviAction);
    mThirdPartyView->setNavigationAction(mThirdPartyBackAction);
    connect(mThirdPartyBackAction, SIGNAL(triggered()),
            this, SLOT(handleThirdPartyViewBackAction()));
    
    mThirdPartyView->setTitle(hbTrId("txt_cp_title_control_panel"));
            
    mainWindow->setCurrentView(mThirdPartyView);
}

/*!
    Opens the open source view
*/
void CpAboutView::openOpenSourceView()
{
    HbMainWindow *mainWindow = hbInstance->allMainWindows().first();
           
    mOpenSourceView = new CpAboutOpenSourceView();
    mainWindow->addView(mOpenSourceView);
       
    mOpenSourceBackAction = new HbAction(Hb::BackNaviAction);
    mOpenSourceView->setNavigationAction(mOpenSourceBackAction);
    connect(mOpenSourceBackAction, SIGNAL(triggered()),
            this, SLOT(handleOpenSourceViewBackAction()));
       
    mOpenSourceView->setTitle(hbTrId("txt_cp_title_control_panel"));
               
    mainWindow->setCurrentView(mOpenSourceView);
}

/*!
    Handles the back action from Third Party view
*/
void CpAboutView::handleThirdPartyViewBackAction()
{
    HbMainWindow *mainWindow = hbInstance->allMainWindows().first();
    mainWindow->removeView(mThirdPartyView);
    mThirdPartyView->deleteLater();
}

/*!
    Handles the back action from Open Source view
*/
void CpAboutView::handleOpenSourceViewBackAction()
{
    HbMainWindow *mainWindow = hbInstance->allMainWindows().first();
    mainWindow->removeView(mOpenSourceView);
    mOpenSourceView->deleteLater();
}
