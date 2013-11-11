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
 * Description: hsmenuclient.cpp
 *
 */

#include <QString>
#include <QDebug>

#include <caquery.h>
#include <caentry.h>
#include <caicondescription.h>
#include <caservice.h>
#include <hsmenuclient.h>

#include "hsapp_defs.h"
#include "hsmenuclient_p.h"

/*!
    \class HsMenuClient
    \ingroup group_hsmenuclient
    \brief Implements home screen menu api, see @ref page_hsmenuclient for 
    usage details.
    
*/

HsMenuClient::HsMenuClient(QObject *parent)
    :QObject(parent), m_d(new HsMenuClientPrivate(this))
{
 
}

HsMenuClient::~HsMenuClient()
{
    delete m_d;
}

/*!
 Add an Templated Application Entry or Update if entryPreferences contains paramter id see 
 \ref section_add_parametrized_widget for exmaple usages
 \param entryPreference entry prefereneces
 \retval true if operation was successful
 */
bool HsMenuClient::add(const QVariantMap &entryPreference)
{    
    return m_d->add(entryPreference);
}
/*!
 Remove an Templated Application Entry see 
 \ref section_remove_tapp for exmaple usages
 \param entry entry prefereneces
 \retval true if operation was successful
 */
bool HsMenuClient::remove(const QVariantMap &entry) const
{
    return m_d->remove(entry);
}
/*!
 Get list of Templated Application Entrys see 
 \ref section_list_tapp for exmaple usages
 \param queryPreference prefereneces for query db
 \retval QList
 */
QList<QVariantMap > HsMenuClient::getList(const QVariantMap &queryPreference)
{
    return m_d->getList(queryPreference);
}

/*!
 Constructor
 \param publishPublic pointer to public contentPublish
 */
HsMenuClientPrivate::HsMenuClientPrivate(HsMenuClient *publishPublic) :
    m_q(publishPublic)
{

}

/*!
 destructor
 */
HsMenuClientPrivate::~HsMenuClientPrivate()
{
    
}

bool HsMenuClientPrivate::add(const QVariantMap &entryPreference)
{
    bool result = false;
    QMap<QString, QVariant> pref = entryPreference;
    
    CaIconDescription iconDesc;
    if(pref.contains(hsItemId))
        {
            QSharedPointer<CaEntry> update_entry = CaService::instance()->getEntry(pref.take(hsItemId).toInt());
            
            if(pref.contains(hsItemLocName))
                {
                update_entry->setText(pref.take(hsItemLocName).toString(),true);
                }
            else
                {
                update_entry->setText(pref.take(hsItemName).toString());   
                }
            
            if(pref.contains(hsItemLocDescription))
                {
                update_entry->setDescription(pref.take(hsItemLocDescription).toString(), true);
                }
            else
                {
                update_entry->setDescription(pref.take(hsItemDescription).toString());
                }                                             
            
            iconDesc.setFilename(pref.take(hsIconFileName).toString());
            iconDesc.setSkinId(pref.take(hsIconName).toString());
            iconDesc.setApplicationId(pref.take(hsIconApplicationId).toString());
            update_entry->setIconDescription(iconDesc);
            QMapIterator<QString, QVariant> k(pref);
            while (k.hasNext()) {
                k.next();
                update_entry->setAttribute(k.key(),k.value().toString());
            }
            
            result = CaService::instance()->updateEntry(*update_entry);
        }
    else if ((pref.contains(hsItemName) || pref.contains(hsItemLocName)) && pref.contains(hsitemLaunchUri) && pref.contains(hsitemPublisherId))
        {
            CaEntry add_entry(ItemEntryRole);
            add_entry.setEntryTypeName(Hs::templatedApplicationTypeName);
            // mandatory values
            if(pref.contains(hsItemLocName))
                {
                add_entry.setText(pref.take(hsItemLocName).toString(),true);
                }
            else
                {
                add_entry.setText(pref.take(hsItemName).toString());
                }
            
            iconDesc.setFilename(pref.take(hsIconFileName).toString());
            iconDesc.setSkinId(pref.take(hsIconName).toString());
            iconDesc.setApplicationId(pref.take(hsIconApplicationId).toString());            
            add_entry.setIconDescription(iconDesc);  
            
            if(pref.contains(hsItemLocDescription))
                {
                add_entry.setDescription(pref.take(hsItemLocDescription).toString(), true);
                }
            else
                {
                add_entry.setDescription(pref.take(hsItemDescription).toString());
                }  
            QMapIterator<QString, QVariant> k(pref);
            while (k.hasNext()) {
                k.next();
                add_entry.setAttribute(k.key(),k.value().toString());
            }

            add_entry.setFlags(add_entry.flags() & ~RemovableEntryFlag);
            
            QSharedPointer<CaEntry> entry = CaService::instance()->createEntry(add_entry);
            if (!entry.isNull()) {
                result = true;
            }      
        }
    else
        {
            return false;
        }
    return result;
    
  
}
bool HsMenuClientPrivate::remove(const QVariantMap &entry) const
{
    bool result = false;
    if (entry.contains(hsItemId))
        {    
            result = CaService::instance()->removeEntry(entry.value(hsItemId).toInt());
        }
    return result;
}

QList<QVariantMap > HsMenuClientPrivate::getList(const QVariantMap &queryPreference)
{
    QList<QVariantMap> list;
    QMap<QString, QVariant> map = queryPreference;
    
    map.remove(hsItemName);
    map.remove(hsItemDescription);
    
    CaQuery query;
    // define query
    query.setFlagsOn(VisibleEntryFlag);
    query.setEntryRoles(ItemEntryRole);
    query.setEntryTypeNames(QStringList(Hs::templatedApplicationTypeName));
    
    
    // add atributes to caquery
    QMapIterator<QString, QVariant> k(map);
    while (k.hasNext()) {
        k.next();
        query.setAttribute(k.key(),k.value().toString());
    }  
    QList< QSharedPointer<CaEntry> > entries = CaService::instance()->getEntries(query);
    map.clear();
    if (entries.count()) {
        QMap<QString, QString> attrMap;
        QMapIterator<QString, QString> k(attrMap);
        //walk trought entrys and extract keys into map
        for (int i = 0; i < entries.count(); ++i) {
            map[hsItemId] = entries.at(i)->id();
            map[hsItemName] = entries.at(i)->text();
            map[hsItemDescription] = entries.at(i)->description();
            CaIconDescription iconDesc = entries.at(i)->iconDescription();
            map[hsIconFileName] = iconDesc.filename();
            map[hsIconName] = iconDesc.skinId();
            map[hsIconApplicationId] = iconDesc.applicationId();
            
            // extract entry atributes
            attrMap = entries.at(i)->attributes();
            QMapIterator<QString, QString> k(attrMap);
            // insert entrys attr into map
            while (k.hasNext()) {
                k.next();
                map.insert(k.key(),k.value());
            }
            list.append(map); //append entry map to list
            // clean used maps
            map.clear();
            attrMap.clear();
        }
    }
    return list;
}
