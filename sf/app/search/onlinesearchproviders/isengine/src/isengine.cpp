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


#include <QDesktopServices>
#include <xqapplicationmanager.h>
#include <QUrl>
#include <qservicemanager.h>
#include "isengine.h"
#include "isproviderdbmanager.h"
QTM_USE_NAMESPACE
const QString KAppUid = "0x2002C377";

IsEngine::IsEngine(QObject *parent) :
    QObject(parent), m_activityManager(0)
    {
    QServiceManager serviceManager;
    m_activityManager = serviceManager.loadInterface(
            "com.nokia.qt.activities.ActivityManager");
    
    m_dbHandler = new IsProviderDBManager;
    }

IsEngine::~IsEngine()
    {
    if (m_activityManager)
        {
        delete m_activityManager;
        }
    
    delete m_dbHandler; 
    }

/*!
 * \brief   Sets selected provider
 *          Returns false if something goes wrong with the database access
 */
bool IsEngine::SetSelectedProvider(const int providerId)
    {    
    return m_dbHandler->SetSelectedProvider(providerId);    
    }

/*!
 * \brief   Gets selected provider
 */
ServiceProvider* IsEngine::SelectedProvider()
    {    
    return m_dbHandler->SelectedProvider();    
    }

/*!
 * \brief   Gets list of providers allowed in the current country.
 *          Returns false if something goes wrong with the database access
 */
bool IsEngine::Providers(QList<ServiceProvider>& providers)
    {    
    return m_dbHandler->GetProviders(providers);    
    }

/*!
 * \brief   Performs internet search by creating search url and opening the browser
 *          with it.
 */
void IsEngine::PerformWebSearch(const QString &searchParam)
    {
    ServiceProvider* provider = SelectedProvider();
    
    if (provider)
        {
        QString url;
        if ( searchParam.length() )
            {
            // add search param to url
            url.append((provider->ProviderUrl().arg(searchParam)));        
            }
        else
            {
            // search with empty param
            url.append((provider->ProviderUrl().arg("")));
            }
        

        // Launch the browser
        QDesktopServices::openUrl(QUrl(url));
        }
    }

/*!
 * \brief   Performs in device search
 * 
 * \param searchParam a const QString & argument.
 */
void IsEngine::PerformInDeviceSearch(const QString &searchParam) const
    {
    QUrl url;
    url.setScheme("appto");
    url.setHost(KAppUid);
    url.addQueryItem("activityname", "SearchDeviceQueryView");

    url.addQueryItem("query", searchParam);

    QString debugString = url.toString();
    bool ok = QMetaObject::invokeMethod(m_activityManager, "launchActivity",
            Q_ARG(QUrl, url));

    }
