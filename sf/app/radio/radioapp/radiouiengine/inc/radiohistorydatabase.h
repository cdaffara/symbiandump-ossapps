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

#ifndef RADIOHISTORYDATABASE_H
#define RADIOHISTORYDATABASE_H

// System includes
#include <QScopedPointer>

// User includes

// Forward declarations
class QSqlDatabase;
class QSqlQuery;

/*!
 *
 */
class RadioHistoryDatabase
{
public:

    RadioHistoryDatabase();
    ~RadioHistoryDatabase();

    void addItem( const QString& artist, const QString& title,
                  const QString& stationName, uint frequency, bool fromRds );

    void clearRecent();
    void clearTagged();

private:

    bool connectToDatabase();

    void commitTransaction( QSqlQuery& query );

private: // data

    QScopedPointer<QSqlDatabase>    mDatabase;

};

#endif // RADIOHISTORYDATABASE_H
