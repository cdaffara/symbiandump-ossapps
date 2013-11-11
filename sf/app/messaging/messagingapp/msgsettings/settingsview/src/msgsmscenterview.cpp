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
 * Description:This class is for sms message center view 
 *
 */
#include <hbaction.h>
#include <hbgroupbox.h>
#include <QGraphicsLinearLayout>

#include "msgsmscenterview.h"
#include "msgsmscentersettingsform.h"
#include "debugtraces.h"

#define LOC_NEW_SMS_CENTRE hbTrId("txt_messaging_title_new_sms_message_centre")
#define LOC_EDIT_SMS_CENTRE hbTrId("txt_messaging_title_edit_sms_message_centre")

MsgSMSCenterView::MsgSMSCenterView(int view, QGraphicsItem *parent) :
    MsgBaseView(parent)
{
    // Create parent layout.
    QGraphicsLinearLayout *mainLayout = new QGraphicsLinearLayout(Qt::Vertical);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // Create view heading.
    HbGroupBox *viewHeading = new HbGroupBox();
    if (view == -1)
    {
        viewHeading->setHeading(LOC_NEW_SMS_CENTRE);
    }
    else
    {
        viewHeading->setHeading(LOC_EDIT_SMS_CENTRE);
    }

    mSMSCenterSettingsForm = new MsgSMSCenterSettingsForm(view);

    HbAction* backAction = new HbAction(Hb::BackNaviAction, this);
    setNavigationAction(backAction);

    connect(mSMSCenterSettingsForm,
            SIGNAL(deleteMessageCentreAndClose()),
            this,
            SLOT(onCloseMessageCenterView()));

    connect(backAction, SIGNAL(triggered()), this, SLOT(onBackAction()));
    
    mainLayout->addItem(viewHeading);
    mainLayout->addItem(mSMSCenterSettingsForm);
    this->setLayout(mainLayout);
}

MsgSMSCenterView::~MsgSMSCenterView()
{
    setParent(NULL);
}

void MsgSMSCenterView::onBackAction()
{
    QDEBUG_WRITE("MsgSMSCenterView::onBackAction");

    //commit the change
    mSMSCenterSettingsForm->commitChanges();
    setNavigationAction(NULL);
    emit smsCenterEditViewClosed();
}

void MsgSMSCenterView::onCloseMessageCenterView()
{
    QDEBUG_WRITE("MsgSMSCenterView::onCloseMessageCenterView");

    setNavigationAction(NULL);
    emit smsCenterEditViewClosed();
}

//eof
