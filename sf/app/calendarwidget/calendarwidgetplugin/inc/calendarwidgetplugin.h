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

#ifndef CALENDARWIDGETPLUGIN_H
#define CALENDARWIDGETPLUGIN_H

// System includes
#include <QObject>
/*!
    \class QServicePluginInterface
    \brief The service framework interface implemented by the plugin.  
    \details The plugin is registered to the service framework. Homescreen loads the widgets through the
        service framework. The plugin is loaded as a widget only if it implements (which is the case of
        calendar widget) the interface <b>com.nokia.symbian.IHomeScreenWidget</b>
*/
#include <qserviceplugininterface.h>

QTM_USE_NAMESPACE

/*!
    \class CalendarWidgetPlugin
    \brief Responsible for loading the whole widget. Called by the service framework.
*/

// Class declaration
class CalendarWidgetPlugin : public QObject, public QServicePluginInterface
{
    Q_OBJECT
    Q_INTERFACES(QtMobility::QServicePluginInterface)

public:
    /*! 
        \fn QObject *createInstance(const QServiceInterfaceDescriptor &descriptor,
                                    QServiceContext *context,
                                    QAbstractSecuritySession *session);

........Invoked by service framework.
........@param descriptor contains the interface name. If the interface name contained in the descriptor matches
               the one implemented createInstance returns and instance of CalendarWidget, otherwise it returns 
               NULL. 
        @param context Not used.
        @param session Not used.
    */
    QObject *createInstance(const QServiceInterfaceDescriptor &descriptor,
                            QServiceContext *context,
                            QAbstractSecuritySession *session);
};

#endif // CALENDARWIDGETPLUGIN_H
