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
* Description:  Mocked runtime plugin.
*
*/

#include <qserviceinterfacedescriptor.h>
#include <qabstractsecuritysession.h>
#include <qservicecontext.h>

#include "mockstateplugins.h"
#include "mockloadscenestate.h"
#include "mockidlestate.h"
#include "mockapplibrarystate.h"
#include "mockmenuworkerstate.h"
#include "mockrootstate.h"
#include "mockbackuprestorestate.h"

QObject *MockStatePlugins::createInstance(const QServiceInterfaceDescriptor &descriptor,
                                                QServiceContext *context,
                                                QAbstractSecuritySession *session)
{
    Q_UNUSED(context);
    Q_UNUSED(session);

    if (descriptor.interfaceName() == "com.nokia.homescreen.state.HsLoadSceneState") {
        return new MockLoadSceneState();
    } else if (descriptor.interfaceName() == "com.nokia.homescreen.state.HsIdleState") {
        return new MockIdleState();
    } else if (descriptor.interfaceName() == "com.nokia.homescreen.state.HsAppLibraryState") {
        return new MockAppLibraryState();
    } else if (descriptor.interfaceName() == "com.nokia.homescreen.state.HsMenuWorkerState") {
        return new MockMenuWorkerState();
    } else if (descriptor.interfaceName() == "com.nokia.homescreen.state.HsRootState") {
        return new MockRootState();
    } else if (descriptor.interfaceName() == "com.nokia.homescreen.state.HsBackupRestoreState") {
        return new MockBackupRestoreState();
    } else {
        return 0;
    }
}

Q_EXPORT_PLUGIN2(mockstateplugins, MockStatePlugins)
