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

#ifndef ISPROVIDERDBMANAGER_H
#define ISPROVIDERDBMANAGER_H

#include <qglobal.h>
#include <QObject>
#include <QtCore/QStringList.h>
#include <QSqlDatabase>
#include "serviceprovider.h"

#if defined(ISPROVIDERDBSERVICES_LIB)
#define ISPROVIDERDBLIB_EXPORT Q_DECL_EXPORT
#else
#define ISPROVIDERDBLIB_EXPORT Q_DECL_IMPORT
#endif



class QSqlDatabase;

/*!
 *
 *
 */
class ISPROVIDERDBLIB_EXPORT IsProviderDBManager : public QObject
{
    Q_OBJECT
    
public:
    IsProviderDBManager( QObject *parent = 0 );
	~IsProviderDBManager();

	bool GetProviders(QList<ServiceProvider>& providers);
	bool GetCountryList(QStringList& countryList);
	ServiceProvider* SelectedProvider();
	bool SetSelectedProvider(int providerId);
	
private:
	bool OpenDatabase();
	void CloseDatabase();
	QString GetCountryCode();
	
private:
	QSqlDatabase  m_db;
};

#endif // ISPROVIDERDBMANAGER_H
