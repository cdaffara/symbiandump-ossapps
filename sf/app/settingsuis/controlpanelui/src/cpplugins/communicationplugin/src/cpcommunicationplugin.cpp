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

#include "cpcommunicationplugin.h"
#include "cpcommunicationgroupitemdata.h"
#include <cpitemdatahelper.h>

CpCommunicationPlugin::CpCommunicationPlugin()
{
}

CpCommunicationPlugin::~CpCommunicationPlugin()
{
}

QList<CpSettingFormItemData*> CpCommunicationPlugin::createSettingFormItemData(CpItemDataHelper &/*itemDataHelper*/) const
{
    CpCategorySettingFormItemData *itemData = 
            new CpCommunicationGroupItemData(
            HbDataFormModelItem::GroupItem,
            hbTrId("txt_cp_subhead_connectivity"),
            QString("cpcommunicationplugin.cpcfg") );
    return QList<CpSettingFormItemData*>() << itemData;
}

Q_EXPORT_PLUGIN2(cpcommunicationplugin, CpCommunicationPlugin);
