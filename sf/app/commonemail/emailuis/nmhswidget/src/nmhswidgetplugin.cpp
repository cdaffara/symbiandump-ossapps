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
* Description:  Homescreen nmail widget plugin
*
*/

#include <qserviceinterfacedescriptor.h>
#include <qabstractsecuritysession.h>
#include <qservicecontext.h>

#include "nmhswidgetplugin.h"
#include "nmhswidget.h"
#include "emailtrace.h"

#ifdef COVERAGE_MEASUREMENT
#pragma CTC SKIP
#endif //COVERAGE_MEASUREMENT

/*!
  \class NmHsWidgetPlugin
  \ingroup group_hsclockwidgetplugin
  \brief Homescreen clock widget provider collects ready-made homescreen clock widget(s).
 
  This library includes homescreen clock widgets implemented by homescreen scrum team.
  This plugin provides widgets that are described in the  hsclockwidgetplugin.xml plugin
  xml file.
 
*/
QObject *NmHsWidgetPlugin::createInstance(const QServiceInterfaceDescriptor &descriptor,
                                             QServiceContext *context,
                                             QAbstractSecuritySession *session)
{
    NM_FUNCTION;
    Q_UNUSED(context);
    Q_UNUSED(session);

    if (descriptor.interfaceName() == QLatin1String("com.nokia.symbian.IHomeScreenWidget")) {
        return new NmHsWidget();
    } else {
        return 0;
    }
}

Q_EXPORT_PLUGIN2(nmhswidgetplugin, NmHsWidgetPlugin)

#ifdef COVERAGE_MEASUREMENT
#pragma CTC ENDSKIP
#endif //COVERAGE_MEASUREMENT
