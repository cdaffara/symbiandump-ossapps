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
* Description:  FM Radio home screen widget plugin
*
*/

// System includes
#include <qserviceinterfacedescriptor.h>
#include <qabstractsecuritysession.h>
#include <qservicecontext.h>

// User includes
#include "radiohswidgetplugin.h"
#include "radiohswidget.h"

// Constants
/*! Home screen widget service interface name. */
const QLatin1String HOME_SCREEN_WIDGET_INTERFACE_NAME(
    "com.nokia.symbian.IHomeScreenWidget");

/*!
    \class RadioHsWidgetPlugin
    \brief Implementats the home screen widget plugin.

    RadioHsWidgetPlugin implements needed functions for creating instance of
    RadioHsWidget.
*/

// ======== MEMBER FUNCTIONS ========

/*!
    Initialize plugin for FM Radio home screen widget. Contains necessary information about 
    the RadioWidget that it can be loaded through QT Service Framework.
    
    \returns Pointer to a new RadioHsWidget or to the NULL. 
*/
QObject *RadioHsWidgetPlugin::createInstance(const QServiceInterfaceDescriptor &descriptor,
                                             QServiceContext *context,
                                             QAbstractSecuritySession *session)
{
    Q_UNUSED(context);
    Q_UNUSED(session);

    // If descriptor interface name matches
    if (descriptor.interfaceName() == HOME_SCREEN_WIDGET_INTERFACE_NAME) {
        // Create and return new FM Radio home screen widget.
        return new RadioHsWidget();
    } else {
        // Otherwise return NULL.
        return NULL;
    }
}

Q_EXPORT_PLUGIN2(radiohswidgetplugin, RadioHsWidgetPlugin)
