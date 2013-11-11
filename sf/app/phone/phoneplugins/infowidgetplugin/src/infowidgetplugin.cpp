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

#include <qserviceinterfacedescriptor.h>
#include <qabstractsecuritysession.h>
#include <qservicecontext.h>

#include "infowidgetplugin.h"
#include "infowidget.h"


/*!
  \class InfoWidgetPlugin
  
  \brief Homescreen Widget for providing operator information   
*/
QObject *InfoWidgetPlugin::createInstance(const QServiceInterfaceDescriptor &descriptor,
                                             QServiceContext *context,
                                             QAbstractSecuritySession *session)
{
    Q_UNUSED(context);
    Q_UNUSED(session);

    if (descriptor.interfaceName() == QLatin1String("com.nokia.symbian.IHomeScreenWidget")) {
        return new InfoWidget();
    } else {
        return 0;
    }
}


Q_EXPORT_PLUGIN2(infowidgetplugin, InfoWidgetPlugin)
