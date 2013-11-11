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
* Description:  Homescreen Friend widget plugin
*
*/

#include "contactwidgethsplugin.h"
#include "contactwidgeths.h"


#include <qserviceinterfacedescriptor.h>
#include <qabstractsecuritysession.h>
#include <qservicecontext.h>



/*!
  \class ContactWidgetHsPlugin
  \ingroup group_contactwidgethsplugin
  \brief Homescreen contact widget provider collects ready-made homescreen contact widget(s).
 
  This library includes homescreen contact widgets.
  This plugin provides widgets that are described in the  contactwidgethsplugin.xml plugin
  xml file.
 
*/
QObject *ContactWidgetHsPlugin::createInstance(const QServiceInterfaceDescriptor &descriptor,
                                             QServiceContext *context,
                                             QAbstractSecuritySession *session)
{
    Q_UNUSED(context);
    Q_UNUSED(session);

    if (descriptor.interfaceName() == QLatin1String("com.nokia.symbian.IHomeScreenWidget")) {
        return new ContactWidgetHs();
    } else {
        return 0;
		
    }
}

Q_EXPORT_PLUGIN2(contactwidgethsplugin, ContactWidgetHsPlugin)

