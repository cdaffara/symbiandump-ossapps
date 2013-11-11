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

#ifndef CPVOLUMEGROUPITEMDATA_H
#define CPVOLUMEGROUPITEMDATA_H

#include <cpsettingformitemdata.h>
#include <cpprofilemodel.h>

class CpProfileModel;
class CpItemDataHelper;
class CpVolumeController;
class CpVolumeGroupItemData : public CpSettingFormItemData
{
    Q_OBJECT
public:
    enum VolumeGroupModelItem {
        EVolumeSilenceItem,
        EVolumeMasterVolumeItem,
        EVolumeMasterVibraItem,
        EVolumeItemEnd
    };
public:
    explicit CpVolumeGroupItemData(CpItemDataHelper &itemDataHelper);
    virtual ~CpVolumeGroupItemData();
private:
    Q_DISABLE_COPY(CpVolumeGroupItemData)
    void initItems(CpItemDataHelper &itemDataHelper);
    QList<HbDataFormModelItem *> mItemList;
    CpVolumeController *mVolumeController;
    CpProfileModel *mProfileModel;
};

#endif
