/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "emailtrace.h"

#include <cpsettingformentryitemdataimpl.h>
#include <HbApplication>
#include <QTranslator>
#include <QLocale>

#include "nmsettingsplugin.h"
#include "nmsettingsviewfactory.h"

/*!
    \class NmSettingsPlugin
    \brief The class implements CpPluginInterface which is an interface
           for plug-ins that are displayed in the control panel application.
           The class works as an entry point for NMail settings.

*/

// ======== MEMBER FUNCTIONS ========

/*!
    Constructor of NmSettingsPlugin.
*/
NmSettingsPlugin::NmSettingsPlugin()
: mTranslator(0)
{
    NM_FUNCTION;
    
    mTranslator = new QTranslator();
    QString lang = QLocale::system().name();
    QString appName = "mail_";
    QString path = "z:/resource/qt/translations/";
    mTranslator->load(appName + lang, path);
    QCoreApplication::installTranslator(mTranslator);
}

/*!
    Destructor of NmSettingsPlugin.
*/
NmSettingsPlugin::~NmSettingsPlugin()
{
    NM_FUNCTION;
    
    delete mTranslator;
}

/*!
    Creates a data form model item, that is used by the control panel framework.

    \param itemDataHelper Helper class that helps control panel plug-ins to
           connect slots to inner widgets of the setting items.

    \return List of CpSettingFormItemData items.
*/
QList<CpSettingFormItemData*> NmSettingsPlugin::createSettingFormItemData(
    CpItemDataHelper &itemDataHelper) const
{
    NM_FUNCTION;
    
    HbIcon icon("qtg_large_email");
    QList<CpSettingFormItemData*> list;

    CpSettingFormItemData* viewFactory = new NmSettingsViewFactory(itemDataHelper,
        hbTrId("txt_mail_title_control_panel"), "", icon);
    list.append(viewFactory);
    return list;
}

Q_EXPORT_PLUGIN2(nmsettingsplugin, NmSettingsPlugin);
