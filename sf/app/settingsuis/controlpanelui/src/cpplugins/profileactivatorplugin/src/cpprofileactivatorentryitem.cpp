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
#include "cpprofileactivatorentryitem.h"

#include "cpprofileactivatordialog.h"
#include <cpitemdatahelper.h>
#include <cpprofilemodel.h>
#include <QScopedPointer>
#include <cpprofilemonitor.h>

CpProfileActivatorEntryItem::CpProfileActivatorEntryItem(CpItemDataHelper &itemDataHelper,
                const QString &text,
                const QString &description,
                const QString &icon,
                const HbDataFormModelItem *parent)
                :CpSettingFormEntryItemData(CpSettingFormEntryItemData::ListEntryItem, itemDataHelper,text,description,
                                            icon,parent),mProfileModel(0),mProfileMonitor(0)
{
    mProfileMonitor = new CpProfileMonitor();
    mProfileModel = new CpProfileModel();
    
    int currentId = mProfileModel->activeProfileId();
    QString currentName = mProfileModel->profileName(currentId);
    this->setDescription(currentName);
    connect(mProfileMonitor, SIGNAL(profileActivated(int)), this, SLOT(onProfileActivated(int)));
    connect(mProfileMonitor, SIGNAL(profileNameModified()), this, SLOT(onProfileNameChanged()));
}

CpProfileActivatorEntryItem::~CpProfileActivatorEntryItem()
{
    delete mProfileModel;
    delete mProfileMonitor;
}

void CpProfileActivatorEntryItem::onLaunchView()
{
    CpProfileActivatorDialog *dialog = 
        new CpProfileActivatorDialog(this, *mProfileModel);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
   // dialog->open(this, SLOT(ultimateDialogSlot(HbAction*)));
    dialog->show();
    
}

void CpProfileActivatorEntryItem::onProfileActivated(int profileId)
{
    QString profileName = mProfileModel->profileName(profileId);
    this->setDescription(profileName);
}

void CpProfileActivatorEntryItem::onProfileNameChanged()
{
    QString profileName = mProfileModel->profileName(mProfileModel->activeProfileId());
    this->setDescription(profileName);
}

CpBaseSettingView *CpProfileActivatorEntryItem::createSettingView() const
{
        return 0;
}
