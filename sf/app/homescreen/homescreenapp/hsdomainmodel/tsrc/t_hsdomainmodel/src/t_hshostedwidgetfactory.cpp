/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "t_hsdomainmodel.h"
#include "hshostedwidgetfactory.h"


void TestHsDomainModel::testHostedWidgetFactoryStaticInterface()
{    
    HsHostedWidgetFactory *stored = HsHostedWidgetFactory::takeInstance();
    
    HsHostedWidgetFactory *factory = new HsHostedWidgetFactory;    
    HsHostedWidgetFactory::setInstance(factory);
    QVERIFY(HsHostedWidgetFactory::instance() == factory);    
    HsHostedWidgetFactory *factory2 = HsHostedWidgetFactory::takeInstance();    
    QVERIFY(factory == factory2);
    delete factory;
    
    HsHostedWidgetFactory::setInstance(stored);
}
