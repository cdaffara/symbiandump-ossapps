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

#include "hswidgetcomponentregistry.h"
#include "hswidgetcomponent.h"
#include "hswidgetcomponentdescriptor.h"
#include "caentry.h"
#include "hsscene.h"

#include <HbMainWindow>
#include <QCoreApplication>

HsWidgetComponentRegistry *HsWidgetComponentRegistry::mInstance = 0;


/*!
    
*/
HsWidgetComponentRegistry* HsWidgetComponentRegistry::instance()
{
    if (!mInstance) {
        mInstance = new HsWidgetComponentRegistry;
        mInstance->setParent(QCoreApplication::instance());
    }
    return mInstance;
}

/*!
    
*/
HsWidgetComponentRegistry::~HsWidgetComponentRegistry()
{
    qDeleteAll(mRegistry);
}

/*!
*/
HsWidgetComponent *HsWidgetComponentRegistry::component(const QString &uri)
{
 	return new HsWidgetComponent(uri);
}

/*!
*/
void HsWidgetComponentRegistry::uninstallComponent(const HsWidgetComponentDescriptor &componentDescriptor)
{
    Q_UNUSED(componentDescriptor);
}

/*!

*/
HsWidgetComponentRegistry::HsWidgetComponentRegistry(QObject *parent)
  : QObject(parent)
{
}

/*!

*/
void HsWidgetComponentRegistry::handleEntryAdded(const CaEntry &entry, const QString &uri)
{
    Q_UNUSED(entry);
    Q_UNUSED(uri);
}

/*!
*/
void HsWidgetComponentRegistry::handleEntryRemoved(const CaEntry &entry, const QString &uri)
{
    Q_UNUSED(entry);
    Q_UNUSED(uri);
}

/*!

*/
void HsWidgetComponentRegistry::handleEntryUpdated(const CaEntry &entry, const QString &uri)
{
    Q_UNUSED(entry);
    Q_UNUSED(uri);
}

/*!

*/
void HsWidgetComponentRegistry::registerService(const CaEntry &entry, const QString& uri, bool reset)
{
    Q_UNUSED(entry);
    Q_UNUSED(uri);
    Q_UNUSED(reset);
}

/*!

*/
void HsWidgetComponentRegistry::onEntryChanged(const CaEntry &entry, ChangeType changeType)
{
    Q_UNUSED(entry);
    Q_UNUSED(changeType);
}
