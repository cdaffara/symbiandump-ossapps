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


#ifndef DIALERWIDGETPLUGIN_H
#define DIALERWIDGETPLUGIN_H

#include <QObject>
#include <qserviceplugininterface.h>

QTM_USE_NAMESPACE


/**
 * @ingroup group_hsdialerwidgetplugin
 * @brief Homescreen dialer widget plugin.
 *
 * This library includes homescreen dialer widget implemented by homescreen scrum team.
 * This plugin provides widgets that are described in the  hsdialerwidgetplugin.xml plugin
 * xml file. 
 *
 * @since S60 ?S60_version.
 */
class DialerWidgetPlugin : public QObject, public QServicePluginInterface
{
    Q_OBJECT
    Q_INTERFACES(QtMobility::QServicePluginInterface)

public:
    QObject *createInstance(const QServiceInterfaceDescriptor &descriptor,
                            QServiceContext *context,
                            QAbstractSecuritySession *session);

};

#endif //DIALERWIDGETPLUGIN_H
