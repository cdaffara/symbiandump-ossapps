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
* Description:  Homescreen widget plugin
*
*/

#ifndef CONTACTWIDGETHSPLUGIN_H
#define CONTACTWIDGETHSPLUGIN_H

#include <QObject>
#include <qserviceplugininterface.h>

#include "contactwidgetplugin_global.h"

QTM_USE_NAMESPACE

/**
* ContactWidgetHsPlugin
*/
class ContactWidgetHsPlugin : public QObject, public QServicePluginInterface
{
    Q_OBJECT
    Q_INTERFACES(QtMobility::QServicePluginInterface)

public:

    QObject *createInstance(const QServiceInterfaceDescriptor &descriptor,
                            QServiceContext *context,
                            QAbstractSecuritySession *session);

private:


    CONTACTWIDGET_TEST_FRIEND_CLASS(TestContactWidgetPlugin)
};

#endif // CONTACTWIDGETPLUGINHS_H
