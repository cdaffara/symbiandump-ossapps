/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 * This component and the accompanying materials are made available
 * under the terms of "Eclipse Public License v1.0"
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
#include <cpsettingformentryitemdataimpl.h>
#include <cpitemdatahelper.h>
#include "cptelephonyplugin.h"
#include "cptelephonypluginview.h"
#include "cppluginlogging.h"


/*!
    CpTelephonyPlugin::CpTelephonyPlugin() 
*/
CpTelephonyPlugin::CpTelephonyPlugin() : 
    QObject(0),
    m_localisation(0)
{
    // Install plugin specific msg handler
    INSTALL_TRACE_MSG_HANDLER;  
    DPRINT << ": IN";
    
    // Set scoped pointer 
    m_localisation.reset(new PhoneLocalisation(this)); 
    
    // Install required translations
    m_localisation->installTranslator(
            PhoneLocalisation::
            TranslationFileCommon);
    m_localisation->installTranslator(
            PhoneLocalisation::
            TranslationFileTelephoneCp);

    DPRINT << ": OUT"; 
}

/*!
    CpTelephonyPlugin::~CpTelephonyPlugin() 
*/
CpTelephonyPlugin::~CpTelephonyPlugin()
{
    DPRINT; 
    
    // Uninstall plugin specific msg handler
    UNINSTALL_TRACE_MSG_HANDLER; 
}

/*!
    CpTelephonyPlugin::createSettingFormItemData() const
*/
QList<CpSettingFormItemData*> CpTelephonyPlugin::createSettingFormItemData(
        CpItemDataHelper &itemDataHelper) const
{
    DPRINT;

    QList<CpSettingFormItemData*> ret;
    ret.append(new CpSettingFormEntryItemDataImpl<CpTelephonyPluginView>(
            itemDataHelper, 
            hbTrId("txt_cp_dblist_telephone"), 
            hbTrId(""), 
            HbIcon(QString("qtg_large_voice_call"))));
    return ret;
}

/*!
    CpTelephonyPlugin::createSettingView() const
*/
CpBaseSettingView *CpTelephonyPlugin::createSettingView(const QVariant &hint) const
{
    QVariantHash hash = hint.value<QVariantHash>();
    
    if( hash.value("view").toString().compare("divert_view",Qt::CaseInsensitive) == 0){
        QVariantList params;
        params << QVariant("cpdivertplugin");
        return new CpTelephonyPluginView(params);
    }
    return 0;
}


Q_EXPORT_PLUGIN2(cptelephonyplugin, CpTelephonyPlugin);


// End of File. 
