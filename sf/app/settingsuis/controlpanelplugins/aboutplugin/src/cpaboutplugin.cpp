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
#include "cpaboutplugin.h"
#include "cpaboutview.h"
#include <cpsettingformentryitemdataimpl.h>

/*!
  \class CpAboutPlugin
*/

/*!
    Constructor
*/
CpAboutPlugin::CpAboutPlugin()
{
}

/*!
    Destructor
*/
CpAboutPlugin::~CpAboutPlugin()
{
}

/*!
    Creates setting form item data
*/
QList<CpSettingFormItemData*> CpAboutPlugin::createSettingFormItemData(CpItemDataHelper &itemDataHelper) const
{
    const QString aboutIconName = "qtg_large_info";
    return QList<CpSettingFormItemData*>() 
            << new CpSettingFormEntryItemDataImpl<CpAboutView>(
			   itemDataHelper,
			   hbTrId("txt_cp_list_about"), 
			   QString(), // shows two liner in list
			   HbIcon(aboutIconName));
}

Q_EXPORT_PLUGIN2(cpaboutplugin, CpAboutPlugin);
