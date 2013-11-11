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

#include "cpthemepluginentryitemdata.h"
#include <cpitemdatahelper.h>
#include <hbfiledialog.h>
#include <hbaction.h>
#include <QFileInfo>

CpThemePluginEntryItemData::CpThemePluginEntryItemData(CpItemDataHelper &itemDataHelper,
													   const QString &text /*= QString()*/,
													   const QString &description /*= QString()*/,
													   const HbIcon &icon /*= HbIcon()*/,
													   const HbDataFormModelItem *parent /*= 0*/)
													   : CpSettingFormEntryItemData(itemDataHelper,
													   text,
													   description,
													   icon,
													   parent)
{
}

CpThemePluginEntryItemData::~CpThemePluginEntryItemData()
{
}

void CpThemePluginEntryItemData::onLaunchView()
{
	HbFileDialog *dlg = new HbFileDialog();
	dlg->setDirectory(QString("C:/ControlPanel/resource"));
	dlg->setTitle("Select file:");
	dlg->setPrimaryActionText(QString("Done"));
	dlg->setSecondaryActionText(QString("Close"));
	QString filePath = dlg->getFileName();
	if (!filePath.isEmpty()) {
	   setDescription(QFileInfo(filePath).baseName());
	   setEntryItemIcon(HbIcon(filePath));
	}
	delete dlg;
}

CpBaseSettingView *CpThemePluginEntryItemData::createSettingView() const
{
	return 0;
}
