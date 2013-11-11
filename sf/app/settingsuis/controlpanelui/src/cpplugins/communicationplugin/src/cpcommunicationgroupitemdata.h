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
#ifndef CPCOMMUNICATIONGROUPITEMDATA_H
#define CPCOMMUNICATIONGROUPITEMDATA_H

#include <cpcategorysettingformitemdata.h>
class CpItemDataHelper;
class XQSettingsManager;
class XQSettingsKey;

class CpCommunicationGroupItemData: public CpCategorySettingFormItemData
{
    Q_OBJECT
public:
    explicit CpCommunicationGroupItemData(const QString &configFile = QString(),
        const HbDataFormModelItem *parent = 0);

    CpCommunicationGroupItemData(HbDataFormModelItem::DataItemType type,
        const QString &label,
        const QString &configFile = QString(),
        const HbDataFormModelItem *parent = 0);

    ~CpCommunicationGroupItemData();
private slots:
    void toggleAirplaneMode(bool toggled);
    void settingValueChanged(const XQSettingsKey &key, const QVariant &value);
private:
    virtual void beforeLoadingConfigPlugins(CpItemDataHelper &itemDataHelper);
private:
     HbDataFormModelItem *mAirplaneModeItem;  
     XQSettingsManager *mSettingManager;
};
#endif /* CPCOMMUNICATIONGROUPITEMDATA_H */
