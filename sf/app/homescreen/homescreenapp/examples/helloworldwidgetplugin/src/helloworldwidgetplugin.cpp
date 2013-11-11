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
* Description:  Example of home screen widget
*
*/

#include <qserviceinterfacedescriptor.h>
#include <qabstractsecuritysession.h>
#include <qservicecontext.h>

#include "helloworldwidgetplugin.h"
#include "helloworldwidget.h"

/*!
    \ingroup group_helloworld_widget
    \class HelloWorldWidgetPlugin
    \brief Example implementation for home screen widget plugin.
*/    
    
/*!
    Initialize plugin for hello world widget. Contains necessary information about 
    the hello world widget that it can be loaded through QT Service Framework.
*/
// Start of snippet 1
QObject *HelloWorldWidgetPlugin::createInstance(const QServiceInterfaceDescriptor &descriptor,
                                             QServiceContext *context,
                                             QAbstractSecuritySession *session)
{
    Q_UNUSED(context);
    Q_UNUSED(session);

    if (descriptor.interfaceName() == QLatin1String("com.nokia.symbian.IHomeScreenWidget")) {
        return new HelloWorldWidget();
    } else {
        return 0;
    }
}
// End of snippet 1

// Start of snippet 2
Q_EXPORT_PLUGIN2(helloworldwidgetplugin, HelloWorldWidgetPlugin)
// End of snippet 2
