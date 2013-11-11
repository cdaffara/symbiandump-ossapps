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
#ifndef CPPERSONALIZATIONGROUPITEMDATA_H
#define CPPERSONALIZATIONGROUPITEMDATA_H

#include <QObject>
#include <cpcategorysettingformitemdata.h>

class CpProfileModel;
class CpItemDataHelper;
class CpSettingFormEntryItemData;
class CpMasterVolumeValueController;

class CpPersonalizationGroupItemData: public CpCategorySettingFormItemData
{
    Q_OBJECT
public:
    CpPersonalizationGroupItemData(HbDataFormModelItem::DataItemType type,
        const QString &label,
        const QString &configFile = QString(),
        const HbDataFormModelItem *parent = 0);

    ~CpPersonalizationGroupItemData();
//private slots:
    //void masterVolumeValueChanged(int value);
//	void onVibraValueChange(int isVibra);
private:
    virtual void beforeLoadingConfigPlugins(CpItemDataHelper &itemDataHelper);
    virtual void afterLoadingConfigPlugins(CpItemDataHelper &itemDataHelper);
};

#endif /* CPPERSONALIZATIONGROUPITEMDATA_H */
