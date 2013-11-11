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
* Description:  Homescreen widget plugin
*
*/

#include <qserviceinterfacedescriptor.h>
#include <qabstractsecuritysession.h>
#include <qservicecontext.h>

#include "hsclockwidgetplugin.h"
#include "hsclockwidget.h"

#ifdef COVERAGE_MEASUREMENT
#pragma CTC SKIP
#endif //COVERAGE_MEASUREMENT

/*!
  \class HsClockWidgetPlugin
  \ingroup group_hsclockwidgetplugin
  \brief Homescreen clock widget provider collects ready-made homescreen clock widget(s).
 
  This library includes homescreen clock widgets implemented by homescreen scrum team.
  This plugin provides widgets that are described in the  hsclockwidgetplugin.xml plugin
  xml file.
 
*/
QObject *HsClockWidgetPlugin::createInstance(const QServiceInterfaceDescriptor &descriptor,
                                             QServiceContext *context,
                                             QAbstractSecuritySession *session)
{
    Q_UNUSED(context);
    Q_UNUSED(session);

    if (descriptor.interfaceName() == QLatin1String("com.nokia.symbian.IHomeScreenWidget")) {
        return new HsClockWidget();
    } else {
        return 0;
    }
}

Q_EXPORT_PLUGIN2(hsclockwidgetplugin, HsClockWidgetPlugin)

#ifdef COVERAGE_MEASUREMENT
#pragma CTC ENDSKIP
#endif //COVERAGE_MEASUREMENT
