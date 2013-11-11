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
* Description:  Mocked Mocked widget plugin, which trys 
*               to changes it's size illegally.
*
*/

#include <qserviceinterfacedescriptor.h>
#include <qabstractsecuritysession.h>
#include <qservicecontext.h>

#include "mockwidgetcheater.h"
#include "mockwidgetplugincheater.h"

QObject *MockWidgetPluginCheater::createInstance(const QServiceInterfaceDescriptor &descriptor,
                                                 QServiceContext *context,
                                                 QAbstractSecuritySession *session)
{
    Q_UNUSED(context);
    Q_UNUSED(session);

    if (descriptor.interfaceName() == "com.nokia.symbian.IHomeScreenWidget") {
        return new MockWidgetCheater(QSizeF(100,100));
    } else {
        return 0;
    }
}

Q_EXPORT_PLUGIN2(mockwidgetplugincheater, MockWidgetPluginCheater)
