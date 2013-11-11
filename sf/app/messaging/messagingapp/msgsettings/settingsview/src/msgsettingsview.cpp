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
 * Description:This is the first view for msgsettings plugin 
 *
 */
#include <hbmainwindow.h>
#include <hbgroupbox.h>
#include <QGraphicsLinearLayout>
#include <HbAction>

#include "msgsettingsview.h"
#include "msgsettingsform.h"
#include "msgsmscenterview.h"
#include "debugtraces.h"

//LOCALAIZED CONSTANTS 
#define LOC_MESSAGE_SETTINGS_HEADING hbTrId("txt_messaging_title_messaging_settings")

MsgSettingsView::MsgSettingsView(SettingsView settingsView,QGraphicsItem *parent):
MsgBaseView(parent),
mSMSCenterView(0),
mSettingsForm(0),
mCurrentView(settingsView)
{
    mMainWindow = this->mainWindow();

    // Create parent layout.
    QGraphicsLinearLayout *mainLayout = new QGraphicsLinearLayout(Qt::Vertical);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // Create view heading.
    HbGroupBox *viewHeading = new HbGroupBox();
    viewHeading->setHeading(LOC_MESSAGE_SETTINGS_HEADING);

    mSettingsForm = new MsgSettingsForm(settingsView);

    connect(mSettingsForm,
            SIGNAL(newSMSCCenterClicked(int)),
            this,
            SLOT(onNewSMSCCenterClicked(int)));
    

    mainLayout->addItem(viewHeading);
    mainLayout->addItem(mSettingsForm);

    this->setLayout(mainLayout);
    
    //sms settings need to be created so launch MsgSMSCenterView in edit mode.
    if(settingsView == SMSView)
    {
        onNewSMSCCenterClicked(-1);
    }
}

MsgSettingsView::~MsgSettingsView()
{
}

void MsgSettingsView::onNewSMSCCenterClicked(int index)
{
    //open the sms center views
    mSMSCenterView = new MsgSMSCenterView(index);

    connect(mSMSCenterView,
            SIGNAL(smsCenterEditViewClosed()),
            this,
            SLOT(onSmsCenterEditViewClosed()));

    mMainWindow->addView(mSMSCenterView);
    mMainWindow->setCurrentView(mSMSCenterView);
}

void MsgSettingsView::onSmsCenterEditViewClosed()
{

    //sms center view was directly launched, no need to go back to settings view.
    if(mCurrentView == SMSView)
    {
        // mSMSCenterView will be deleted by MainWindow. 
        this->navigationAction()->trigger();
        return;
    }
    //remove the view 
    mMainWindow->removeView(mSMSCenterView);
    // Delete the view since the ownership is transferred to caller.
    // @see HbMainWindow::removeView()
    delete mSMSCenterView;
    mSMSCenterView = NULL;
    
    // This check is needed in case when smsc center view is 
    // launched directly 
    if(mMainWindow->views().count() > 0)
    {
        QCRITICAL_WRITE("MsgViewManager::onSmsCenterEditViewClosed count>0");
        
        //refresh the form
        mSettingsForm->refreshViewForm();
        
        //add the current view on top
        mMainWindow->setCurrentView(this);
    }
}

//eof

