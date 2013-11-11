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

#ifndef HSHOSTEDWIDGETFACTORY_H
#define HSHOSTEDWIDGETFACTORY_H

#include <QObject>


#include "hsdomainmodel_global.h"
#include "hsdomainmodeldatastructures.h"

#include "hstest_global.h"
HOMESCREEN_TEST_CLASS(TestHsDomainModel)

struct HsHostedWidgetFactoryImpl;

class HSDOMAINMODEL_EXPORT HsHostedWidgetFactory : public QObject
{
    Q_OBJECT

public:
    static HsHostedWidgetFactory *instance();
    static HsHostedWidgetFactory *takeInstance();
    static HsHostedWidgetFactory *setInstance(HsHostedWidgetFactory *);
    ~HsHostedWidgetFactory();

    QObject *createWidget(const QString &uri);
private:
    Q_DISABLE_COPY(HsHostedWidgetFactory)
    HsHostedWidgetFactory(QObject *parent = 0);
private:
    static HsHostedWidgetFactory *mInstance;
    HsHostedWidgetFactoryImpl *mImpl;
    
    HOMESCREEN_TEST_FRIEND_CLASS(TestHsDomainModel)
};

#endif // HSWIDGETHOST_H
