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



#include <qservicemanager.h>
#include <qservicefilter.h>
#include <qserviceinterfacedescriptor.h>
#include <QGraphicsWidget>
#include "hshostedwidgetfactory.h"

QTM_USE_NAMESPACE

/*!
    \class HsWidgetFactory
    \ingroup group_hsdomainmodel
    \brief Each widget is controlled by the home screen framework through a widget host.
*/


struct HsHostedWidgetFactoryImpl{
    QServiceManager mServiceManager;
    QServiceFilter mFilter;
};



HsHostedWidgetFactory *HsHostedWidgetFactory::mInstance = 0;

HsHostedWidgetFactory *HsHostedWidgetFactory::instance()
{
    if (!mInstance) {
        mInstance = new HsHostedWidgetFactory;
    }
    return mInstance;
}

HsHostedWidgetFactory *HsHostedWidgetFactory::takeInstance()
{
    HsHostedWidgetFactory *instance = mInstance;
    mInstance = 0;
    return instance;
}

HsHostedWidgetFactory *HsHostedWidgetFactory::setInstance
                                             (HsHostedWidgetFactory *instance)
{
    HsHostedWidgetFactory *old = mInstance;
    if (mInstance != instance) {
        mInstance = instance;
    }
    return old;
}

QObject *HsHostedWidgetFactory::createWidget(const QString &uri)
{
    mImpl->mFilter.setServiceName(uri);
    QList<QServiceInterfaceDescriptor> interfaces = 
        mImpl->mServiceManager.findInterfaces(mImpl->mFilter);
    
    if (interfaces.isEmpty()) {
        return 0;
    }
    // In theory we support QGraphicsWidget derived widgets
    QObject *widgetObject = mImpl->mServiceManager.loadInterface(interfaces.first());
    
    if (!qobject_cast<QGraphicsWidget *>(widgetObject)) {
        delete widgetObject;
        widgetObject = 0;
    }
    return widgetObject;
}

/*!
    Constructs a new widget host with given \a databaseId and
    \a parent item.
*/
HsHostedWidgetFactory::HsHostedWidgetFactory(QObject *parent)
  : QObject(parent),
    mImpl(new HsHostedWidgetFactoryImpl)
{
    mImpl->mFilter.setInterface("com.nokia.symbian.IHomeScreenWidget");
}

/*!
    Destructor.
*/
HsHostedWidgetFactory::~HsHostedWidgetFactory()
{
    delete mImpl;
}

