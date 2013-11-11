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

#ifndef ISPROVIDERDBCONSTANTS_H_
#define ISPROVIDERDBCONSTANTS_H_

// Database info
#ifdef __WINSCW__
const QString databaseName = "c:\\private\\20022f35\\providerdatabase.sq";
#else
const QString databaseName = "z:\\private\\20022f35\\providerdatabase.sq";
#endif
const QString connectionName = "providerdatabase.sq";
const QString databaseType = "QSQLITE";

// SQL Queries
const QString providerQuery = "SELECT p.ProviderId, Name, Description, Iconpath FROM Providers p INNER JOIN CountryProviderMap map ON p.ProviderId = map.ProviderId JOIN Countries ON map.CountryId = Countries.CountryId WHERE Countries.CountryCode = \'";
const QString selectedProviderQuery = "SELECT ProviderId, Name, Description, Iconpath, ProviderUrl FROM Providers WHERE Selected = 1";
const QString resetProviderSelection = "UPDATE Providers SET Selected = 0";
const QString setSelectedProvider = "UPDATE Providers SET Selected = 1 WHERE ProviderId = ";
const QString countryQuery = "SELECT Country FROM Countries";


#endif /* ISPROVIDERDBCONSTANTS_H_ */
