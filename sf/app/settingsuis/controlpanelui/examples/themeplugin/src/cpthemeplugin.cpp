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
#include "cpthemeplugin.h"
#include "cpthemepluginentryitemdata.h"

CpThemePlugin::CpThemePlugin()
{
}

CpThemePlugin::~CpThemePlugin()
{
}

QList<CpSettingFormItemData*> CpThemePlugin::createSettingFormItemData(CpItemDataHelper &itemDataHelper) const
{
	CpSettingFormEntryItemData *entryItem = new CpThemePluginEntryItemData(
		itemDataHelper,tr("Theme"),tr("Name of the Theme"),HbIcon(":/image/qgn_menu_note.svg"));
	return QList<CpSettingFormItemData*>() << entryItem;
}

Q_EXPORT_PLUGIN2(cpthemeplugin, CpThemePlugin);
