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
* Description:  Implementation of the FTU default runtime provider.
*
*/


#include "ftudefaultruntimeprovider.h"
#include "ftudefaultruntime.h"


#include <qserviceinterfacedescriptor.h>
#include <qabstractsecuritysession.h>
#include <qservicecontext.h>

// ---------------------------------------------------------------------------
// FtuDefaultRuntimeProvider::createInstance()
// ---------------------------------------------------------------------------
//
QObject *FtuDefaultRuntimeProvider::createInstance(const QServiceInterfaceDescriptor &descriptor,
                                                   QServiceContext *context,
                                                   QAbstractSecuritySession *session)
{
    Q_UNUSED(context);
    Q_UNUSED(session);

    if (descriptor.interfaceName() == "com.nokia.ftu.runtime.FtuRuntime") {
        return new FtuDefaultRuntime(this);
    } else {
        return 0;
    }
}

#ifndef COVERAGE_MEASUREMENT
Q_EXPORT_PLUGIN2(ftudefaultruntimeprovider, FtuDefaultRuntimeProvider)
#endif //COVERAGE_MEASUREMENT






