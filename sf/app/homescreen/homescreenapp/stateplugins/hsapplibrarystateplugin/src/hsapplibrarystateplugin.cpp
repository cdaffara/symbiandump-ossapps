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
 * Description: Application library state plugin.
 *
 */


#include <qserviceinterfacedescriptor.h>
#include <qabstractsecuritysession.h>
#include <qservicecontext.h>

#include "hsapplibrarystateplugin.h"
#include "hsapplibrarystate.h"

#ifdef COVERAGE_MEASUREMENT
#pragma CTC SKIP
#endif //COVERAGE_MEASUREMENT

/*!
    Creates a HsAppLibraryState if interface name in \a descriptor
	is com.nokia.homescreen.state.HsAppLibraryState.
*/
QObject *HsMenuAppLibraryStatePlugin::createInstance(const QServiceInterfaceDescriptor &descriptor,
        QServiceContext *context,
        QAbstractSecuritySession *session)
{
    Q_UNUSED(context);
    Q_UNUSED(session);

    if (descriptor.interfaceName() == QLatin1String("com.nokia.homescreen.state.HsAppLibraryState")) {
        return new HsAppLibraryState();
    } else {
        return 0;
    }
}

Q_EXPORT_PLUGIN2(hsapplibrarystateplugin, HsMenuAppLibraryStatePlugin)

#ifdef COVERAGE_MEASUREMENT
#pragma CTC ENDSKIP
#endif //COVERAGE_MEASUREMENT
