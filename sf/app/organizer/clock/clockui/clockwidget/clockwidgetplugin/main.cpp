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
* Description:  Clock widget
*
*/

// User includes
#include "clockwidgetplugin.h"
#include "clockwidget.h"

/*!
    \class ClockWidgetPlugin

    This class is a wrapper plug-in DLL that references the custom widget DLLs that need to be part of the widget package. 
*/


/*!
    Creates an instance of each of the types of custom widget supported in this plug-in. 
    The code uses the parameter type to control which type of object must be created. 
    It also sets the created object's name 
*/
QObject *ClockWidgetPlugin::createObject(const QString &type, const QString &name)
{
     if (type == ClockWidget::staticMetaObject.className()) {
        QObject *object = new ClockWidget();
        object->setObjectName(name);
        return object;
    }
    return 0;
}

/*!
    Get a QMetaObject for each custom widget and add it to the list of supported objects. 
    A QMetaObject pointer must be added for every custom widget that is created by the plug-in.
*/ 
QList<const QMetaObject *> ClockWidgetPlugin::supportedObjects()
{
    QList<const QMetaObject *> list;
    list.append( &ClockWidget::staticMetaObject );
    return list;
}

// End of file  --Don't remove this.
