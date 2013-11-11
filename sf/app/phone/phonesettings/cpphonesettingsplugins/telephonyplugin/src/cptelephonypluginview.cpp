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


#include <QPluginLoader>
#include <hbdataformmodel.h>
#include <cpplugininterface.h>
#include <cpsettingformitemdata.h>
#include <cpitemdatahelper.h>
#include <cppluginutility.h>
#include <cppluginloader.h>
#include "cptelephonypluginview.h"
#include "cppluginlogging.h"

/*!
    \class CpTelephonyPluginView
    \brief The class CpTelephonyPluginView 
           loads child setting plugins contained 
           in "Telephony" view.     
*/

// Local constants
const char* PLUGIN_NAME_CALLSPLUGIN = "cpcallsplugin"; 
const char* PLUGIN_NAME_VMBXPLUGIN = "vmbxcpplugin"; 
const char* PLUGIN_NAME_DIVERTPLUGIN = "cpdivertplugin"; 
const char* PLUGIN_NAME_BARRINGPLUGIN = "cpbarringplugin"; 

/*!
    Constructor. 
*/
CpTelephonyPluginView::CpTelephonyPluginView() : 
    CpBaseSettingView(0,0),
    m_helper(NULL)
{
    DPRINT << ": IN";
    
    HbDataForm *form = qobject_cast<HbDataForm*>(widget());
    if (form) {
        HbDataFormModel *model = new HbDataFormModel(form);
        form->setHeading(hbTrId("txt_phone_subhead_telephone"));
        // Create and initialize plugin's item data helper
        m_helper = initializeItemDataHelper(); 
        QList<CpSettingFormItemData*> items;
        
        DPRINT << ": Loading cpcallsplugin";
        items.append(groupItemFromPlugin(
                PLUGIN_NAME_CALLSPLUGIN));
        CpSettingFormItemData* expandedItem = items.last();  

        DPRINT << ": Loading vmbxcpplugin";
        items.append(groupItemFromPlugin(
                PLUGIN_NAME_VMBXPLUGIN));

        DPRINT << ": Loading cpdivertsplugin";
        items.append(groupItemFromPlugin(
                PLUGIN_NAME_DIVERTPLUGIN));

        DPRINT << ": Loading cpbarringplugin";
        items.append(groupItemFromPlugin(
                PLUGIN_NAME_BARRINGPLUGIN));
        
        // Insert items to form model
        foreach (CpSettingFormItemData* i, items) {
            model->appendDataFormItem(i);
        }
        
        form->setModel(model);
        
        if (expandedItem) {
            QModelIndex expandedItemIndex = model->indexFromItem(expandedItem);
            
            if (expandedItemIndex.isValid()) {
                form->setExpanded(expandedItemIndex, true);
            }
        }
    }
    
    DPRINT << ": OUT";
}

/*!
    Constructor with param list. 
*/
CpTelephonyPluginView::CpTelephonyPluginView(const QVariantList &params) :
    CpBaseSettingView(0,0),
    m_helper(NULL)
{
    HbDataForm *form = qobject_cast<HbDataForm*>(widget());
    if (form){
        HbDataFormModel *model = new HbDataFormModel;
        form->setHeading(hbTrId("txt_phone_subhead_telephone"));
        m_helper = initializeItemDataHelper(); 
        QList<CpSettingFormItemData*> items;
        TBool expanded(false);
        CpSettingFormItemData* expandedItem(NULL);
        foreach (QVariant var, params){
            items.append(groupItemFromPlugin(var.toString()));
            if(!expanded){
                expandedItem = items.last();
                expanded = true;
            }
        }
        // Insert items to form model
        foreach (CpSettingFormItemData* i, items) {
            model->appendDataFormItem(i);
        }
        form->setModel(model);
        if (expandedItem) {
            QModelIndex expandedItemIndex = model->indexFromItem(expandedItem);
            
            if (expandedItemIndex.isValid()) {
                form->setExpanded(expandedItemIndex, true);
            }
        }
    }
}

/*!
    Destructor. 
*/
CpTelephonyPluginView::~CpTelephonyPluginView()
{
    DPRINT << ": IN";
    qDeleteAll(m_plugins);
    delete m_helper;
    DPRINT << ": OUT";
}

/*!
    Load group item for given plugin name. 
*/
QList<CpSettingFormItemData*> CpTelephonyPluginView::groupItemFromPlugin( 
        const QString& plugin)
{
    DPRINT << ": IN";
 
    QList<CpSettingFormItemData*> items;
    CpPluginInterface *p(NULL);
    try {
        p = CpPluginLoader::loadCpPluginInterface(plugin);
        if (p && m_helper) {
            m_plugins.append(p);
            items = p->createSettingFormItemData(*m_helper);
        }
    } catch(...) {
        DCRITICAL << ": CATCH ERROR, item creation failed!";
        delete p;
    }
    
    DPRINT << ": OUT";
    return items;
}

/*!
    Item data helper initialization. 
*/
CpItemDataHelper* CpTelephonyPluginView::initializeItemDataHelper()
{
    DPRINT;
    return new CpItemDataHelper(qobject_cast<HbDataForm*>(widget()));
}

// End of File. 
