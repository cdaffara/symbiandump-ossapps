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
#include "hsbackuprestoreobserver.h"

namespace
{
    template<class T>
    QVariantList toVariantList(const QList<T> &list)
    {
        QVariantList vlist;
        for (int i = 0; i < list.count(); ++i) {
            T item = list.at(i);
            vlist << item;
        }
        return vlist;
    }

    template<class T>
    QVariantList toVariantList(const T &item, int count)
    {
        QVariantList vlist;
        for (int i = 0; i < count; ++i) {
            vlist << item;
        }
        return vlist;
    }
}


/*!
    \class HsDatabase
    \ingroup group_hsdomainmodel
    \brief 
*/

/*!
    Constructs a new database with the given \a parent object.
*/
HsDatabase::HsDatabase(QObject *parent)
  : QObject(parent)
{
}

/*!
    Destroys this database.
*/
HsDatabase::~HsDatabase()
{
    close();
}

/*!
    Sets the connection name to \a name.
*/
void HsDatabase::setConnectionName(const QString &name)
{
    mConnectionName = name;
}
 
/*!
    Returns the connection name.
*/
QString HsDatabase::connectionName() const
{
    return mConnectionName;
}

/*!
    Sets the database name to \a name.
*/
void HsDatabase::setDatabaseName(const QString &name)
{
    mDatabaseName = QDir::toNativeSeparators(name);
}
 
/*!
    Returns the database name.
*/
QString HsDatabase::databaseName() const
{
    return mDatabaseName;
}

/*!
    Opens the database connection using the current connection
    values. Returns true on success, otherwise returns false.
*/  
bool HsDatabase::open()
{
	HsBackupRestoreObserver *brObserver = HsBackupRestoreObserver::instance();
	if (brObserver->checkBUR()) {	
		return false;
	}

    QSqlDatabase database;
    if (QSqlDatabase::contains(mConnectionName)) {
        database = QSqlDatabase::database(mConnectionName);
    } else {
        database = QSqlDatabase::addDatabase("QSQLITE", mConnectionName);
        database.setDatabaseName(mDatabaseName);
    }
    if (!database.isValid()) {
        close();
        return false;
    }
    if (!database.isOpen()) {
        if (!database.open()) {
            close();
            return false;
        }
    }
    return true;
}
 
/*!
    Closes the database connection.
*/
void HsDatabase::close()
{
    {
        QSqlDatabase database = QSqlDatabase::database(mConnectionName);
        if (database.isValid() && database.isOpen()) {
            database.close();
        }
    }
    QSqlDatabase::removeDatabase(mConnectionName);
}

/*!
    Begins a transaction on the database if the driver 
    supports transactions. Returns true if the operation 
    succeeded. Otherwise returns false.
*/
bool HsDatabase::transaction()
{
    return QSqlDatabase::database(mConnectionName).transaction();
}
 
/*!
    Rolls back a transaction on the database, if the driver 
    supports transactions and a transaction() has been started.
    Returns true if the operation succeeded. Otherwise returns
    false.
*/
bool HsDatabase::rollback()
{
    return QSqlDatabase::database(mConnectionName).rollback();
}
 
/*!
    Commits a transaction to the database if the driver supports 
    transactions and a transaction() has been started. Returns 
    true if the operation succeeded. Otherwise returns false.
*/
bool HsDatabase::commit()
{
    return QSqlDatabase::database(mConnectionName).commit();
}

/*!

*/
bool HsDatabase::scene(HsSceneData &data)
{
    if (!checkConnection()) {
        return false;
    }

    QSqlQuery query(QSqlDatabase::database(mConnectionName));

    QString statement =
        "SELECT id "
        "FROM Scene";
    
    if (query.prepare(statement) && query.exec() && query.next()) {        
        data.id = query.value(0).toInt();        
        return true;
    }
    
    return false;    
}

/*!

*/
bool HsDatabase::pages(QList<HsPageData> &data)
{
    if (!checkConnection()) {
        return false;
    }

    QSqlQuery query(QSqlDatabase::database(mConnectionName));

    QString statement =
        "SELECT id, indexPosition "
        "FROM Pages "
        "ORDER BY indexPosition";

    if (query.prepare(statement) && query.exec()) {
        data.clear();
        while (query.next()) {
            HsPageData d;
            d.id            = query.value(0).toInt();
            d.indexPosition = query.value(1).toInt();
            data.append(d);
        }
        return true;
    }

    return false;
}

/*!

*/
bool HsDatabase::page(HsPageData &data)
{
    if (!checkConnection()) {
        return false;
    }

    QSqlQuery query(QSqlDatabase::database(mConnectionName));

    QString statement =
        "SELECT indexPosition "
        "FROM Pages "
        "WHERE id = ?";

    if (query.prepare(statement)) {
        query.addBindValue(data.id);
        if (query.exec() && query.next()) {
            data.indexPosition = query.value(0).toInt();
            return true;
        }
    }

    return false;
}

/*!

*/
bool HsDatabase::insertPage(HsPageData &data)
{
    if (!checkConnection()) {
        return false;
    }

    QSqlQuery query(QSqlDatabase::database(mConnectionName));

    QString statement =
        "INSERT INTO Pages "
        "(indexPosition) "
        "VALUES "
        "(?)";

    if (query.prepare(statement)) {
        query.addBindValue(data.indexPosition);
        if (query.exec()) {
            data.id = query.lastInsertId().toInt();
            return true;
        }
    }

    return false;
}

/*!

*/
bool HsDatabase::updatePage(const HsPageData &data)
{
    if (!checkConnection()) {
        return false;
    }

    QSqlQuery query(QSqlDatabase::database(mConnectionName));

    QString statement =
        "UPDATE Pages "
        "SET indexPosition = ? "
        "WHERE id = ?";

    if (query.prepare(statement)) {
        query.addBindValue(data.indexPosition);
        query.addBindValue(data.id);
        return query.exec();
    }

    return false;
}

/*!

*/
bool HsDatabase::deletePage(int id)
{
    if (!checkConnection()) {
        return false;
    }

    QSqlQuery query(QSqlDatabase::database(mConnectionName));

    QString statement =
        "DELETE FROM Pages "
        "WHERE id = ?";

    if (query.prepare(statement)) {
        query.addBindValue(id);
        return query.exec();
    }

    return false;
}

/*!

*/
bool HsDatabase::widgets(int pageId, QList<HsWidgetData> &data)
{
    if (!checkConnection()) {
        return false;
    }

    QSqlQuery query(QSqlDatabase::database(mConnectionName));

    QString statement =
        "SELECT id, uri "
        "FROM Widgets "
        "WHERE pageId = ?";

    if (query.prepare(statement)) {
        query.addBindValue(pageId);
        if (query.exec()) {
            data.clear();
            while (query.next()) {
                HsWidgetData d;
                d.id     = query.value(0).toInt();
                d.uri    = query.value(1).toString();
                d.pageId = pageId;
                data.append(d);
            }
            return true;
        }
    }

    return false;
}

/*!

*/
bool HsDatabase::widgets(const QString &uri, QList<HsWidgetData> &data)
{
    if (!checkConnection()) {
        return false;
    }

    QSqlQuery query(QSqlDatabase::database(mConnectionName));

    QString statement =
        "SELECT id, pageId "
        "FROM Widgets "
        "WHERE uri = ?";

    if (query.prepare(statement)) {
        query.addBindValue(uri);
        if (query.exec()) {
            data.clear();
            while (query.next()) {
                HsWidgetData d;
                d.id     = query.value(0).toInt();
                d.uri    = uri;
                d.pageId = query.value(1).toInt();
                data.append(d);
            }
            return true;
        }
    }

    return false;
}

/*!

*/
bool HsDatabase::widgets(const QString &uri, const QVariantHash &preferences, int &count)
{
    if (!checkConnection()) {
        return false;
    }
    QSqlQuery query(QSqlDatabase::database(mConnectionName));
    if ( preferences.size() == 0 ) {
        // return widget count of the given uri
        QString statement =
            "SELECT COUNT(id) "
            "FROM Widgets "
            "WHERE uri = ?";

        if (query.prepare(statement)) {
            query.addBindValue(uri);
            if (query.exec() && query.next()) {
                count = query.value(0).toInt();
                return true;
            }
        }
    } else {

        QString statement =
            "SELECT key, value, widgetId "
            "FROM widgetPreferences "
            "WHERE widgetId IN"
            "(SELECT id FROM widgets WHERE uri = ?)";
            
        if (query.prepare(statement)) {
            query.addBindValue(uri);
            if (query.exec()) {
                count = 0;
                QMultiMap<QString, QString> foundPreferences;
                while (query.next()) {
                    foundPreferences.insert(query.value(0).toString(), query.value(1).toString());
                }
                if ( matchWidgetPreferences(preferences, foundPreferences) ) {
                    ++count;
                }
                return true;
            }
        }
    }

    return false;
}

/*!

*/
bool HsDatabase::widget(HsWidgetData &data)
{
    if (!checkConnection()) {
        return false;
    }

    QSqlQuery query(QSqlDatabase::database(mConnectionName));

    QString statement =
        "SELECT uri, pageId "
        "FROM Widgets "
        "WHERE id = ?";

    if (query.prepare(statement)) {
        query.addBindValue(data.id);
        if (query.exec() && query.next()) {
            data.uri    = query.value(0).toString();
            data.pageId = query.value(1).toInt();
            return true;
        }
    }

    return false;
}

/*!

*/
bool HsDatabase::insertWidget(HsWidgetData &data)
{
    if (!checkConnection()) {
        return false;
    }
    if (data.uri.isEmpty()) {
        return false;
    }
    QSqlQuery query(QSqlDatabase::database(mConnectionName));

    QString statement =
        "INSERT INTO Widgets "
        "(uri, pageId) "
        "VALUES "
        "(?, ?)";

    if (query.prepare(statement)) {
        query.addBindValue(data.uri);
        query.addBindValue(data.pageId);
        if (query.exec()) {
            data.id = query.lastInsertId().toInt();
            return true;
        }
    }

    return false;
}

/*!

*/
bool HsDatabase::updateWidget(const HsWidgetData &data)
{
    if (!checkConnection()) {
        return false;
    }

    QSqlQuery query(QSqlDatabase::database(mConnectionName));

    QString statement =
        "UPDATE Widgets "
        "SET uri = ?, pageId = ? "
        "WHERE id = ?";

    if (query.prepare(statement)) {
        query.addBindValue(data.uri);
        query.addBindValue(data.pageId);
        query.addBindValue(data.id);
        return query.exec();
    }

    return false;
}

/*!

*/
bool HsDatabase::deleteWidget(int id)
{
    if (!checkConnection()) {
        return false;
    }

    QSqlQuery query(QSqlDatabase::database(mConnectionName));

    QString statement =
        "DELETE FROM Widgets "
        "WHERE id = ?";

    if (query.prepare(statement)) {
        query.addBindValue(id);
        return query.exec();
    }

    return false;
}

/*!

*/
bool HsDatabase::deleteWidgets(const QString &uri)
{
    if (!checkConnection()) {
        return false;
    }

    QSqlQuery query(QSqlDatabase::database(mConnectionName));

    QString statement =
        "DELETE FROM Widgets "
        "WHERE uri = ?";

    if (query.prepare(statement)) {
        query.addBindValue(uri);
        return query.exec();
    }

    return false;
}

/*!

*/
bool HsDatabase::widgetPresentation(HsWidgetPresentationData &data)
{
    if (!checkConnection()) {
        return false;
    }

    QString key = data.orientation == Qt::Vertical ?
        QLatin1String("portrait") : QLatin1String("landscape");

    QSqlQuery query(QSqlDatabase::database(mConnectionName));

    QString statement =
        "SELECT x, y, zValue "
        "FROM WidgetPresentations "
        "WHERE key = ? AND widgetId = ?";

    if (query.prepare(statement)) {
        query.addBindValue(key);
        query.addBindValue(data.widgetId);
        if (query.exec() && query.next()) {
            data.x      = query.value(0).toReal();
            data.y      = query.value(1).toReal();
            data.zValue = query.value(2).toReal();
            return true;
        }
    }

    return false;
}

/*!

*/
bool HsDatabase::setWidgetPresentation(const HsWidgetPresentationData &data)
{
    if (!checkConnection()) {
        return false;
    }

    QString key = data.orientation == Qt::Vertical ?
        QLatin1String("portrait") : QLatin1String("landscape");

    QSqlQuery query(QSqlDatabase::database(mConnectionName));

    QString statement =
        "REPLACE INTO WidgetPresentations "
        "(key, x, y, zValue, widgetId) "
        "VALUES (?, ?, ?, ?, ?)";

    if (query.prepare(statement)) {
        query.addBindValue(key);
        query.addBindValue(data.x);
        query.addBindValue(data.y);
        query.addBindValue(data.zValue);
        query.addBindValue(data.widgetId);
        return query.exec();
    }

    return false;
}

/*!

*/
bool HsDatabase::deleteWidgetPresentation(int widgetId, Qt::Orientation orientation)
{
    if (!checkConnection()) {
        return false;
    }

    QString key = orientation == Qt::Vertical ?
        QLatin1String("portrait") : QLatin1String("landscape");

    QSqlQuery query(QSqlDatabase::database(mConnectionName));

    QString statement =
        "DELETE FROM WidgetPresentations "
        "WHERE key = ? AND widgetId = ?";

    if (query.prepare(statement)) {
        query.addBindValue(key);
        query.addBindValue(widgetId);
        return query.exec();
    }

    return false;
}

/*!

*/
bool HsDatabase::widgetPreferences(int widgetId, QVariantHash &data)
{
    if (!checkConnection()) {
        return false;
    }

    QSqlQuery query(QSqlDatabase::database(mConnectionName));

    QString statement =
        "SELECT key, value "
        "FROM WidgetPreferences "
        "WHERE widgetId = ?";

    if (query.prepare(statement)) {
        query.addBindValue(widgetId);
        if (query.exec()) {
            data.clear();
            while (query.next()) {
                data.insert(query.value(0).toString(), 
                            query.value(1));
            }
            return true;
        }
    }

    return false;
}

/*!

*/
bool HsDatabase::widgetPreference(int widgetId, const QString &key, QVariant &value)
{
    if (!checkConnection()) {
        return false;
    }

    QSqlQuery query(QSqlDatabase::database(mConnectionName));

    QString statement =
        "SELECT value "
        "FROM WidgetPreferences "
        "WHERE key = ? AND widgetId = ?";

    if (query.prepare(statement)) {
        query.addBindValue(key);
        query.addBindValue(widgetId);
        if (query.exec() && query.next()) {
            value = query.value(0);
            return true;
        }
    }

    return false;
}
 
/*!

*/
bool HsDatabase::setWidgetPreferences(int widgetId, const QVariantHash &data)
{
    if (!checkConnection()) {
        return false;
    }

    QSqlQuery query(QSqlDatabase::database(mConnectionName));

    QList<QString> deleteKeys = data.keys(QVariant());
    QVariantHash insertOrReplaceData(data);
    for (int i=0; i<deleteKeys.count(); ++i) {
        insertOrReplaceData.remove(deleteKeys.at(i));
    }

    if (!deleteKeys.isEmpty()) {
        QString statement = 
            "DELETE FROM WidgetPreferences "
            "WHERE key = ? AND widgetId = ?";
        
        if (query.prepare(statement)) {
            query.addBindValue(toVariantList(deleteKeys));
            query.addBindValue(toVariantList(widgetId, deleteKeys.count()));
            if (!query.execBatch()) {
                return false;
            }
        } else {
            return false;
        }
    }

    if (!insertOrReplaceData.isEmpty()) {
        QString statement = 
            "REPLACE INTO WidgetPreferences "
            "(key, value, widgetId) "
            "VALUES (?, ?, ?)";
            
        if (query.prepare(statement)) {
            query.addBindValue(toVariantList(insertOrReplaceData.keys()));
            query.addBindValue(toVariantList(insertOrReplaceData.values()));
            query.addBindValue(toVariantList(widgetId, insertOrReplaceData.count()));
            if (!query.execBatch()) {
                return false;
            }
        } else {
            return false;
        }
    }

    return true;
}

bool HsDatabase::configuration(QVariantHash &configuration)
{
    if (!checkConnection()) {
        return false;
    }

    QSqlQuery query(QSqlDatabase::database(mConnectionName));

    QString statement =
        "SELECT key, value "
        "FROM Configuration";

    if (query.prepare(statement) && query.exec()) {
        configuration.clear();
        while (query.next()) {
            configuration.insert(query.value(0).toString(), 
                                 query.value(1));
        }
        return true;
    }

    return false;
}

bool HsDatabase::updateWidgetZValues(const QHash<int, qreal> &data, Qt::Orientation orientation)
{
    if (!checkConnection()) {
        return false;
    }

    QString key = orientation == Qt::Vertical ?
        QLatin1String("portrait") : QLatin1String("landscape");

    QSqlQuery query(QSqlDatabase::database(mConnectionName));
    
    QString statement = 
        "UPDATE WidgetPresentations "
        "SET zValue = ? "
        "WHERE key = ? AND widgetId = ?";
    
    if (query.prepare(statement)) {
        query.addBindValue(toVariantList(data.values()));
        query.addBindValue(toVariantList(key, data.count()));
        query.addBindValue(toVariantList(data.keys()));
        if (!query.execBatch()) {
            return false;
        }
    } else {
        return false;
    }

    return true;
}

bool HsDatabase::updateWidgetPresentations(const QList<HsWidgetPresentationData> &data, Qt::Orientation orientation)
{
    if (!checkConnection()) {
        return false;
    }

    QString key = orientation == Qt::Vertical ?
        QLatin1String("portrait") : QLatin1String("landscape");

    QSqlQuery query(QSqlDatabase::database(mConnectionName));
    
    QString statement =
        "REPLACE INTO WidgetPresentations "
        "(key, x, y, zValue, widgetId) "
        "VALUES (?, ?, ?, ?, ?)";
        
    QVariantList xList;
    QVariantList yList;
    QVariantList zList;
    QVariantList widgetIdList;
    
    foreach (HsWidgetPresentationData widgetData, data) {
        xList.append(widgetData.x);
        yList.append(widgetData.y);
        zList.append(widgetData.zValue);
        widgetIdList.append(widgetData.widgetId);
    }
    
    if (query.prepare(statement)) {
        query.addBindValue(toVariantList(key, data.count()));
        query.addBindValue(xList);
        query.addBindValue(yList);
        query.addBindValue(zList);
        query.addBindValue(widgetIdList);
        if (!query.execBatch()) {
            return false;
        }
    } else {
        return false;
    }

    return true;
}


/*!
    Sets the database instance. The existing instance
    will be deleted.
*/
void HsDatabase::setInstance(HsDatabase *instance)
{
    if (mInstance != instance) {
        HsDatabase *oldInstance = mInstance;
        mInstance = instance;
        delete oldInstance;
    }
}

/*!
    Returns the database instance.
*/
HsDatabase *HsDatabase::instance()
{
    return mInstance;
}

/*!
    Returns the current database instance. Callers of this 
    function take ownership of the instance. The current 
    database instance will be reset to null.    
*/
HsDatabase *HsDatabase::takeInstance()
{
    HsDatabase *instance = mInstance;
    mInstance = 0;
    return instance;
}
  
/*!
    Checks the connection validity. Returns true if the 
    connection is valid.
*/
bool HsDatabase::checkConnection() const
{
    return QSqlDatabase::database(mConnectionName).isValid();
}

/*!
    Returns true if \a preferences are found from \a storedPreferences multimap. 
    If preferences is empty, returns true.
    
*/
bool HsDatabase::matchWidgetPreferences(const QVariantHash &preferences, const QMultiMap<QString, QString>& storedPreferences)
{
    bool contains = true;
    QList<QString> preferenceKeys = preferences.keys();
    for (int i=0; i<preferenceKeys.count() && contains; ++i) {
        contains = storedPreferences.contains(preferenceKeys[i], preferences[preferenceKeys[i]].toString());
    }
    return contains;
}

/*!
    Points to the database instance.
*/
HsDatabase *HsDatabase::mInstance(0);
