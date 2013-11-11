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
* Description:  Internet Radio home screen widget plugin
*
*/

#include <qserviceinterfacedescriptor.h>
#include <qabstractsecuritysession.h>
#include <qservicecontext.h>

#include "irhswidgetplugin.h"
#include "irhswidget.h"
#include "irqlogger.h"

/*!
    Initialize plugin for Internet Radio home screen widget. Contains necessary information about
    the internet radio widget that it can be loaded through QT Service Framework.
*/
QObject *IrHsWidgetPlugin::createInstance(const QServiceInterfaceDescriptor &aDescriptor,
                                             QServiceContext *aContext,
                                             QAbstractSecuritySession *aSession)
{
    LOG_METHOD;
    Q_UNUSED(aContext);
    Q_UNUSED(aSession);

    if (aDescriptor.interfaceName() == QLatin1String("com.nokia.symbian.IHomeScreenWidget"))
    {
        return new IrHsWidget();
    }
    else
    {
        return 0;
    }
}

Q_EXPORT_PLUGIN2(irhswidgetplugin, IrHsWidgetPlugin)
