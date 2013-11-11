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

#include "cpplaceholderitemdata.h"
#include <QTextStream>
#include <hbmessagebox.h>
#include "cppluginconfig.h"

CpPlaceHolderItemData::CpPlaceHolderItemData(CpItemDataHelper &itemDataHelper,
                                             const CpPluginConfig &pluginConfig)
                                             : CpSettingFormEntryItemData(
                                             itemDataHelper,
                                             pluginConfig.mDisplayName,
                                             pluginConfig.mPluginFile)
{
}

CpPlaceHolderItemData::CpPlaceHolderItemData(CpItemDataHelper &itemDataHelper,
                                            const QString &displayName,
                                            const QString &pluginFile) 
                                            : CpSettingFormEntryItemData (
                                            itemDataHelper,
                                            displayName,
                                            pluginFile)
{
    
}

CpPlaceHolderItemData::~CpPlaceHolderItemData()
{
}

void CpPlaceHolderItemData::onLaunchView()
{
    QString message;
    QTextStream stream(&message);
    stream << "Load plugin:"
           << description() 
           << " faild."
           << "Please check:\n"
           << "1. if the dll name is correct in configuration file.\n" 
           << "2  if the dll has been generated.\n"
           << "3. if the plugin stub file is in place.\n"
           << "4. if the dll is valid Qt plugin.";

    HbMessageBox::warning(message);
}

CpBaseSettingView *CpPlaceHolderItemData::createSettingView() const
{
    return 0;
}

//End of File

