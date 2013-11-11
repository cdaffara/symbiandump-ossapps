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

#include <QSqlQuery>
#include <QSqlError>
#include <qsysteminfo.h>
using namespace QtMobility;
#include <QVariant>
#include <QStringList.h>

#include "isproviderdbmanager.h"
#include "isproviderdbconstants.h"


/*!
 * \brief Constructor
 */
IsProviderDBManager::IsProviderDBManager( QObject *parent ) :  QObject(parent)
    {    
    }

/*!
 * \brief Destructor
 */
IsProviderDBManager::~IsProviderDBManager()
    {
    }

/*!
 * \brief Opens the database
 */
bool IsProviderDBManager::OpenDatabase()
    {
    m_db = QSqlDatabase::addDatabase(databaseType, connectionName);
    m_db.setDatabaseName(databaseName);
    
    if ( !m_db.isValid())
        {
        CloseDatabase();
        return false;
        }
    
    if (!m_db.isOpen())
        {
        if (!m_db.open())
            {
            CloseDatabase();
            return false;
            }
        }
    return true;
    }

/*!
 * \brief Closes the database
 */
void IsProviderDBManager::CloseDatabase()
    {    
  //  if (m_db.isValid() && m_db.isOpen())
  //      {
        m_db.close();
   //     }
    }

/*!
 * \brief Reads providers allowed in the current country
 */
bool IsProviderDBManager::GetProviders(QList<ServiceProvider>& providers)
    {
    bool ok = OpenDatabase();

    if (ok)
        {
        // Create query that gets providers allowed this country
        QString queryString(providerQuery);        
        // get country code and append to query
        queryString.append(GetCountryCode());
        queryString.append("\'");
    
        QSqlQuery query(QSqlDatabase::database(connectionName));    
        query.prepare(queryString);
        query.exec();
        // read all found providers
        while (query.next())
            {
            ServiceProvider* provider = new ServiceProvider();

            int providerId = query.value(0).toInt();
            provider->SetId(providerId);
            
            QString providerName = query.value(1).toString();
            provider->SetName(providerName);
            
            QString providerDescription = query.value(2).toString();
            provider->SetDescription(providerDescription);

            HbIcon icon(query.value(3).toString());            
            provider->SetIcon(icon);
            
            QString pluginName = query.value(10).toString();
            provider->SetPluginName(pluginName);
            providers.append(*provider);
            }    
        }
    
    CloseDatabase();
    
    return ok;
    }

/*!
 * \brief Gets selected provider
 */
ServiceProvider* IsProviderDBManager::SelectedProvider()
    {
    ServiceProvider* provider = NULL;
    
    bool ok = OpenDatabase();
    if (ok)
        {
        // Create query that gets providers allowed this country       
        QSqlQuery query(QSqlDatabase::database(connectionName));    
        query.prepare(selectedProviderQuery);
        query.exec();  

        // read all found providers
        while (query.next())
            {
            provider = new ServiceProvider();
            
            int providerId = query.value(0).toInt();
            provider->SetId(providerId);
            
            QString providerName = query.value(1).toString();
            provider->SetName(providerName);
            
            QString providerDescription = query.value(2).toString();
            provider->SetDescription(providerDescription);

            HbIcon icon(query.value(3).toString());            
            provider->SetIcon(icon);           
            
            QString providerUrl = query.value(4).toString();
            provider->SetProviderUrl(providerUrl);
            }    
        }
    
    CloseDatabase();
    
    return provider;
    }

/*!
 * \brief Sets selected provider
 */
bool IsProviderDBManager::SetSelectedProvider(int providerId)
    {    
    bool ok = OpenDatabase();
    if (ok)
        {
        // Clear selection status from all providers
        QSqlQuery query(QSqlDatabase::database(connectionName));    
        query.prepare(resetProviderSelection);
        ok = query.exec();
        
        if (ok)
            {
            // Set selection status to selected provider    
            QString queryString(setSelectedProvider);
            queryString.append( QString::number(providerId));        
       
            query.clear();
            query.prepare(queryString);
            ok = query.exec();        
            }
        }
    
    CloseDatabase();
    
    return ok;
    }

/*!
 * \brief Reads country list from database
 */
bool IsProviderDBManager::GetCountryList(QStringList& countryList)
    {
    bool ok = OpenDatabase();
    if (ok)
        {
        // Create query that gets country list   
        QSqlQuery query(QSqlDatabase::database(connectionName));    
        query.prepare(countryQuery);
        ok = query.exec();

        // read all found providers
        while (query.next())
            {
                countryList << query.value(0).toString();
            }     
        }
    
    CloseDatabase();   
    
    return ok;
    }

/*!
 * \brief Return current country code retrieved from system
 */
QString IsProviderDBManager::GetCountryCode()
    {
    QSystemInfo *sysInfo = new QSystemInfo(this);
    return "FI";//sysInfo->currentCountryCode();
    }
