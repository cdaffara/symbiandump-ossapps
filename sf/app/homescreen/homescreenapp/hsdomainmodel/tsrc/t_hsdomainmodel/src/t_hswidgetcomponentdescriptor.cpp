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
* Description:  Wallpaper test.
*
*/


#include "t_hsdomainmodel.h"
#include "hswidgetcomponentdescriptor.h"

void TestHsDomainModel::testWidgetComponentDescriptor()
{
    HsWidgetComponentDescriptor widgetDescriptor;
    
    QVERIFY(!widgetDescriptor.isValid());
    
    widgetDescriptor.setUri(QString("uri"));
    QVERIFY(!widgetDescriptor.isValid());
    
    widgetDescriptor.setUri(QString(""));
    widgetDescriptor.setTitle(QString("title"));
    QVERIFY(!widgetDescriptor.isValid());

    widgetDescriptor.setUri(QString("uri"));
    widgetDescriptor.setTitle(QString("title"));
    QVERIFY(widgetDescriptor.isValid());
    
    widgetDescriptor.setVersion(QString("foobar"));
    QVERIFY(!widgetDescriptor.isValid());
    widgetDescriptor.setVersion(QString("1.0"));
    widgetDescriptor.setServiceXml(QString("foobar"));
    QVERIFY(widgetDescriptor.isValid());
    
    
}
