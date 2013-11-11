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

#ifndef ISENGINE_H
#define ISENGINE_H

#include <qglobal.h>
#include <QObject>
#include <QtCore/QStringList.h>

#if defined(ISENGINESERVICES_LIB)
#  define ISENGINELIB_EXPORT Q_DECL_EXPORT
#else
#  define ISENGINELIB_EXPORT Q_DECL_IMPORT
#endif

class ServiceProvider;
class IsProviderDBManager;

/*!
 * Internet Search Engine
 *
 */
class ISENGINELIB_EXPORT IsEngine : public QObject
{
    Q_OBJECT

    
public:
	IsEngine( QObject *parent = 0 );
	~IsEngine();

	ServiceProvider* SelectedProvider();
	bool SetSelectedProvider(const int providerId);
    bool Providers(QList<ServiceProvider>& providers);
    void PerformWebSearch(const QString& searchParam);
    void PerformInDeviceSearch(const QString& searchParam) const;
	
private:
	/*!< Provider database handler*/
	IsProviderDBManager* m_dbHandler;
    /*!< Activity manager*/
    QObject *m_activityManager;
	
};

#endif // ISENGINE_H
