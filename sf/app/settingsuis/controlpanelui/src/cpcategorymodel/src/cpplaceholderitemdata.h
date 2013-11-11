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
* Description:  Placeholder for plugin configured in cpcfg file but the target is missing.
*
*/

#ifndef CPPLACEHOLDERITEMDATA_H
#define CPPLACEHOLDERITEMDATA_H

#include <cpsettingformentryitemdata.h>

class CpPluginConfig;
class CpItemDataHelper;

class CpPlaceHolderItemData : public CpSettingFormEntryItemData
{
    Q_OBJECT
public:
    CpPlaceHolderItemData(CpItemDataHelper &itemDataHelper,const CpPluginConfig &pluginConfig);
    CpPlaceHolderItemData(CpItemDataHelper &itemDataHelper,const QString &displayName,const QString &pluginFile);
    virtual ~CpPlaceHolderItemData();
private slots:
	virtual void onLaunchView();
private:
	virtual CpBaseSettingView *createSettingView() const;
};

#endif //CPPLACEHOLDERITEMDATA_H

//End of File
