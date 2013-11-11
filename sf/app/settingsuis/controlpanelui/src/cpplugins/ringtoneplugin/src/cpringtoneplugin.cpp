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
#include "cpringtoneplugin.h"
#include "cppersonalizationentryitemdata.h"

CpRingTonePlugin::CpRingTonePlugin()
{
}

CpRingTonePlugin::~CpRingTonePlugin()
{
}

QList<CpSettingFormItemData*> CpRingTonePlugin::createSettingFormItemData(CpItemDataHelper &itemDataHelper) const
{
    CpPersonalizationEntryItemData *itemData 
        = new CpPersonalizationEntryItemData(
                itemDataHelper,
                hbTrId("txt_cp_dblist_ringtone"),
				QString(""),
				"qtg_large_ring_tone");
    return QList<CpSettingFormItemData*>() << itemData;
}

Q_EXPORT_PLUGIN2(cpringtoneplugin, CpRingTonePlugin);
