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
* Description:  Homescreen bookmark widget
*
*/

#include <qserviceinterfacedescriptor.h>
#include <qabstractsecuritysession.h>
#include <qservicecontext.h>

#include "hsbookmarkwidgetplugin.h"
#include "hsbookmarkwidget.h"

/*!
    \ingroup group_hsbookmark_widget
    \class HsBookmarkWidgetPlugin
    \brief Implementation for homescreen bookmark widget plugin.
*/    
    
/*!
    Initialize plugin for bookmark widget. Contains necessary information about 
    the bookmark widget that it can be loaded through QT Service Framework.
*/
QObject *HsBookmarkWidgetPlugin::createInstance(const QServiceInterfaceDescriptor &descriptor,
                                                QServiceContext *context,
                                                QAbstractSecuritySession *session)
{
    Q_UNUSED(context);
    Q_UNUSED(session);

    if (descriptor.interfaceName() == QLatin1String("com.nokia.symbian.IHomeScreenWidget")) {
        return new HsBookmarkWidget();
    } else {
        return 0;
    }
}

Q_EXPORT_PLUGIN2(hsbookmarkwidgetplugin, HsBookmarkWidgetPlugin)

