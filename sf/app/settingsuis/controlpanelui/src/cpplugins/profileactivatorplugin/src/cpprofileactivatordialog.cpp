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
#include "cpprofileactivatordialog.h"

#include <hbradiobuttonlist.h>
#include <hbaction.h>
#include <hbdataformmodelitem.h>
#include <hbpopup.h>
#include <hblabel.h>
#include "cpprofileactivatorentryitem.h"
#include <QGraphicsLinearLayout>
#include <QGraphicsWidget>

CpProfileActivatorDialog::CpProfileActivatorDialog(CpSettingFormItemData *profileActivator,
                                                    CpProfileModel &profileModel,
                                                    QGraphicsItem *parent):HbDialog(parent),
                                                     mProfileModel(profileModel),
                                                     mProfileList(0),
                                                     mProfileActivator(profileActivator)
{
    mTitleLabel = new HbLabel(this);
    mTitleLabel->setPlainText(hbTrId("txt_cp_title_profile"));
    mContentWidget = new QGraphicsWidget(this);
    setContentWidget(mContentWidget);
    mLayout = new QGraphicsLinearLayout();
    mLayout->setOrientation( Qt::Vertical );
    mProfileIds << EProfileWrapperGeneralId
                << EProfileWrapperMeetingId;
    mProfileList = new HbRadioButtonList();
    mProfileList->setItems(profileModel.profileNames());
    mLayout->addItem(mTitleLabel);
    mLayout->addItem(mProfileList);
       
    int currentId = mProfileModel.activeProfileId();
    mProfileList->setSelected(mProfileIds.indexOf(static_cast<ProfileWrapperProfileId>(currentId)));
    
    
        
    mConfirmProfile = new HbAction(hbTrId("txt_common_button_ok"));
    mCancelProfile = new HbAction(hbTrId("txt_common_button_cancel"));
    
    connect(mConfirmProfile, SIGNAL(triggered(bool)), this, SLOT(confirmProfileSelection()));
    connect(mCancelProfile, SIGNAL(triggered(bool)), this, SLOT(cancelProfileSelection()));
        
    
    this->addAction(mConfirmProfile);
    this->addAction(mCancelProfile);    
    this->setModal(true);
    this->setDismissPolicy(HbPopup::NoDismiss);
    this->setTimeout(HbPopup::NoTimeout);
    mContentWidget->setLayout( mLayout );
}

CpProfileActivatorDialog::~CpProfileActivatorDialog()
{
    
}
void CpProfileActivatorDialog::confirmProfileSelection()
{
    int currentIndex = mProfileList->selected();
    if (currentIndex == -1) {
        return;
    }
    
    // the best choice is no need to convert the index to id
    mProfileModel.activateProfile(mProfileIds.at(currentIndex));
}
void CpProfileActivatorDialog::cancelProfileSelection()
{
    
}
