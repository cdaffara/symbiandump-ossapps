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

#ifndef HSWIDGETCOMPONENTREGISTRY_H
#define HSWIDGETCOMPONENTREGISTRY_H

#include <QObject>
#include <QVariant>
#include <qservicemanager.h>



#include "hsdomainmodeldatastructures.h"
#include "hsdomainmodel_global.h"
#include "cadefs.h"

#include "hstest_global.h"
HOMESCREEN_TEST_CLASS(TestHsDomainModel)

class HsWidgetComponent;
class CaEntry;
class HsWidgetComponentDescriptor;

QTM_USE_NAMESPACE

class HSDOMAINMODEL_EXPORT HsWidgetComponentRegistry : public QObject
{
    Q_OBJECT

public:
    static HsWidgetComponentRegistry *instance();    
    ~HsWidgetComponentRegistry();
	
	HsWidgetComponent *component(const QString &uri);
	
	void uninstallComponent(const HsWidgetComponentDescriptor &componentDescriptor);
	
private:
    Q_DISABLE_COPY(HsWidgetComponentRegistry)
    HsWidgetComponentRegistry(QObject *parent = 0);
    
    void handleEntryAdded(const CaEntry &entry, const QString &uri);
    void handleEntryRemoved(const CaEntry &entry, const QString &uri);
    void handleEntryUpdated(const CaEntry &entry, const QString &uri);
    void registerService(const CaEntry &entry, const QString& uri, bool reset = false );
   
   
private slots:
    void onEntryChanged(const CaEntry &entry, ChangeType changeType);

private:
	QHash<QString, HsWidgetComponent *> mRegistry;
	QServiceManager mServiceManager;
	static HsWidgetComponentRegistry *mInstance;
    
    HOMESCREEN_TEST_FRIEND_CLASS(TestHsDomainModel)
};

#endif // HSWIDGETCOMPONENTREGISTRY_H
