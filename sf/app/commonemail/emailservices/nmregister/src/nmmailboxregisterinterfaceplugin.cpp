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
* Description: Plugin factory.
*
*/

#include "emailtrace.h"

#include <qserviceinterfacedescriptor.h>
#include <qabstractsecuritysession.h>
#include <qservicecontext.h>

#include "nmmailboxregisterinterfaceplugin.h"
#include "nmmailboxregisterinterface.h"

#ifdef COVERAGE_MEASUREMENT
#pragma CTC SKIP
#endif //COVERAGE_MEASUREMENT

QObject *NmMailboxRegisterInterfacePlugin::createInstance(const QServiceInterfaceDescriptor &descriptor,
                                                QServiceContext *context,
                                                QAbstractSecuritySession *session)
{
    NM_FUNCTION;
    
    Q_UNUSED(context);
    Q_UNUSED(session);
    
    if (descriptor.interfaceName() == QLatin1String("com.nokia.symbian.IEmailRegisterAccount")) {
        QT_TRY{
            return new NmMailboxRegisterInterface(this);
        }QT_CATCH(...){
            return 0;
        }
    } else {
        return 0;
    }
}

Q_EXPORT_PLUGIN2(nmregister, NmMailboxRegisterInterfacePlugin)

#ifdef COVERAGE_MEASUREMENT
#pragma CTC ENDSKIP
#endif //COVERAGE_MEASUREMENT
