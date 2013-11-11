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

#ifndef SERVICEPROVIDER_H
#define SERVICEPROVIDER_H

#include <QList>
#include <QObject>
#include <HbIcon>


class ServiceProvider
{
public:
    ServiceProvider() {}
    virtual ~ServiceProvider() {}
    QString Name() const {return m_name;}
    void SetName(const QString& aName) { m_name = aName; }
    int Id() const {return m_providerId;}
    void SetId(const int providerId) { m_providerId = providerId; }
    HbIcon Icon() const {return m_icon;}
    void SetIcon(const HbIcon& icon) { m_icon = icon; }
    QString Description() const {return m_description;}
    void SetDescription(const QString& description) { m_description = description; }
    QString ProviderUrl() const {return m_providerUrl;}
    void SetProviderUrl(const QString& providerUrl) { m_providerUrl = providerUrl; }
    QString PluginName() const {return m_pluginName;}
    void SetPluginName(const QString& pluginName) { m_pluginName = pluginName; }
    
private:
	int m_providerId;
    QString m_name;
    HbIcon m_icon;
    QString m_description;
    QString m_providerUrl;
    QString m_pluginName;
};

#endif  // SERVICEPROVIDER_H
