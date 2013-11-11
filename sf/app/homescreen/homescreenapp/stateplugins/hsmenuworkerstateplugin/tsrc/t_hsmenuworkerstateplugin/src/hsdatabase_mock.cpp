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

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QVariantHash>
#include <QDir>

#include "hsdatabase.h"
#include "hsdomainmodeldatastructures.h"

HsDatabase::HsDatabase(QObject *parent)
  : QObject(parent)
{
}

HsDatabase::~HsDatabase()
{
 
}

void HsDatabase::setConnectionName(const QString &name)
{
    mConnectionName = name;
}
 
QString HsDatabase::connectionName() const
{
    return mConnectionName;
}

void HsDatabase::setDatabaseName(const QString &name)
{
    mDatabaseName = QDir::toNativeSeparators(name);
}
 
QString HsDatabase::databaseName() const
{
    return mDatabaseName;
}

bool HsDatabase::open()
{
    return true;
}
 
void HsDatabase::close()
{
    
}

bool HsDatabase::transaction()
{
    return true;
}
 
bool HsDatabase::rollback()
{
    return true;
}
 
bool HsDatabase::commit()
{
    return true;
}

bool HsDatabase::scene(HsSceneData &)
{
    return true;
}

bool HsDatabase::pages(QList<HsPageData> &)
{
    return true;
}

bool HsDatabase::page(HsPageData &)
{
   return true;
}

bool HsDatabase::insertPage(HsPageData &)
{
   return true;
}

bool HsDatabase::updatePage(const HsPageData &)
{
    return true;
}

bool HsDatabase::deletePage(int )
{
   return true;
}

bool HsDatabase::widgets(int , QList<HsWidgetData> &)
{
   return true;
}

bool HsDatabase::widgets(const QString &, QList<HsWidgetData> &)
{
    return true;
}

bool HsDatabase::widgets(const QString &, const QVariantHash &, int &count)
{
    count = 0;
    return true;
}
bool HsDatabase::widget(HsWidgetData &)
{
    return true;
}

bool HsDatabase::insertWidget(HsWidgetData &)
{
    return true;
}

bool HsDatabase::updateWidget(const HsWidgetData &)
{
    return true;
}

bool HsDatabase::deleteWidget(int )
{
    return true;
}

bool HsDatabase::deleteWidgets(const QString &)
{
    return true;
}

bool HsDatabase::widgetPresentation(HsWidgetPresentationData &)
{
    return true;
}

bool HsDatabase::setWidgetPresentation(const HsWidgetPresentationData &)
{
    return true;
}

bool HsDatabase::deleteWidgetPresentation(int, Qt::Orientation)
{
    return true;
}

bool HsDatabase::widgetPreferences(int , QVariantHash &)
{
    return true;
}

bool HsDatabase::widgetPreference(int , const QString &, QVariant &)
{
    return true;
}

bool HsDatabase::setWidgetPreferences(int , const QVariantHash &)
{
    return true;
}
 
void HsDatabase::setInstance(HsDatabase *instance)
{
    if (mInstance != instance) {
        HsDatabase *oldInstance = mInstance;
        mInstance = instance;
        delete oldInstance;
    }
}

HsDatabase *HsDatabase::instance()
{
    return mInstance;
}

HsDatabase *HsDatabase::takeInstance()
{
    HsDatabase *instance = mInstance;
    mInstance = 0;
    return instance;
}
  
bool HsDatabase::checkConnection() const
{
    return true;
}

bool HsDatabase::configuration(QVariantHash &configuration)
{
    Q_UNUSED(configuration)
    return true;
}

bool HsDatabase::updateWidgetZValues(const QHash<int, qreal> &data, Qt::Orientation orientation)
{
    Q_UNUSED(data)
    Q_UNUSED(orientation)
    return true;
}

bool HsDatabase::updateWidgetPresentations(const QList<HsWidgetPresentationData> &data, Qt::Orientation orientation)
{
    Q_UNUSED(data)
    Q_UNUSED(orientation)
    return true;
}

bool HsDatabase::matchWidgetPreferences(const QVariantHash &preferences, const QMultiMap<QString, QString>& storedPreferences)
{
    Q_UNUSED(preferences)
    Q_UNUSED(storedPreferences)
    return true;
}

HsDatabase *HsDatabase::mInstance(0);
