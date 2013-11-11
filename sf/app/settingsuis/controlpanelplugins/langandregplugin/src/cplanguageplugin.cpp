/*
 * Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "cplanguageplugin.h"
#include "cplanguageview.h"
#include <QPointer>
#include <HbLocaleUtil.h>
#include <cpsettingformentryitemdataimpl.h>
#include "cplanguagepluginlog.h"


#ifdef ENABLE_CPLANG_LOG
    INIT_LOGGER (CPLANG_LOGGER_NAME,CPLANG_LOGGER_CONFIG_PATH)
#endif

static QPointer<CpSettingFormItemData> g_pluginEntryItemData;
    
/*!
 * Constructor
 */
CpLanguagePlugin::CpLanguagePlugin()
{
    CPLANG_LOG_FUNC_ENTRY("CpLanguagePlugin::CpLanguagePlugin")
}

/*!
 * Destructor
 */
CpLanguagePlugin::~CpLanguagePlugin()
{
    CPLANG_LOG_FUNC_ENTRY("CpLanguagePlugin::~CpLanguagePlugin")
}

/*!
 * Overide CpPluginInterface::createSettingFormItemData
 */
QList<CpSettingFormItemData*> CpLanguagePlugin::createSettingFormItemData(CpItemDataHelper &itemDataHelper) const
{	
    CPLANG_LOG_FUNC_ENTRY("CpLanguagePlugin::createSettingFormItemData")
        
    g_pluginEntryItemData = new CpSettingFormEntryItemDataImpl<CpLanguageView>(
        CpSettingFormEntryItemData::ListEntryItem,
        itemDataHelper,
        hbTrId("txt_cp_dblist_language_and_region"), 
        QString(),    
        "qtg_large_language" ); 
    
    updateEntryItem();
    
	return QList<CpSettingFormItemData*>() << g_pluginEntryItemData;  
}

/*!
 * update the description of the entry item.
 */
void CpLanguagePlugin::updateEntryItem()
{
    CPLANG_LOG_FUNC_ENTRY("CpLanguagePlugin::updateEntryItem")
    
        
    if (g_pluginEntryItemData) {
        QStringList allLanguages = HbLocaleUtil::supportedLanguages();
        QString currentLanguage = HbLocaleUtil::currentLanguage();
        
        CPLANG_LOG(QLatin1String("Current language:") + currentLanguage);
        
        int index = allLanguages.indexOf(currentLanguage);
        
        CPLANG_LOG(QString("Index of current language: %1").arg(index));
        
        QString localisedLanguage;
        if (index >= 0) {
            localisedLanguage = HbLocaleUtil::localisedLanguageName(currentLanguage);
            CPLANG_LOG(QLatin1String("Localised Region:") + localisedLanguage);
        }
        g_pluginEntryItemData->setDescription(localisedLanguage);
    }
}

Q_EXPORT_PLUGIN2(cplanguageplugin, CpLanguagePlugin);

//End of File
