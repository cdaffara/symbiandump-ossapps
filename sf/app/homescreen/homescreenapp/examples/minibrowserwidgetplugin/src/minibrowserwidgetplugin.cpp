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

#include "minibrowserwidgetplugin.h"
#include "minibrowserwidget.h"

/*!
    \ingroup group_minibrowser_widget
    \class MiniBrowserWidgetPlugin
    \brief Example implementation for home screen widget plugin.
*/    
    
/*!
    Initialize plugin for minibrowser widget. Contains necessary information about 
    the minibrowser widget that it can be loaded through QT Service Framework.
*/
QObject *MiniBrowserWidgetPlugin::createInstance(const QServiceInterfaceDescriptor &descriptor,
                                             QServiceContext *context,
                                             QAbstractSecuritySession *session)
{
    Q_UNUSED(context);
    Q_UNUSED(session);

    if (descriptor.interfaceName() == QLatin1String("com.nokia.symbian.IHomeScreenWidget")) {
        return new MiniBrowserWidget();
    } else {
        return 0;
    }
}

Q_EXPORT_PLUGIN2(minibrowserwidgetplugin, MiniBrowserWidgetPlugin)
