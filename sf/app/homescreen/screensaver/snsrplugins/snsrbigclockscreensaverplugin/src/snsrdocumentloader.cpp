/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Screensaver custom document loader.
*
*/

#include "snsrdocumentloader.h"

#include "snsranalogclockwidget.h"
#include "snsroledclockwidget.h"
#include "snsrlabel.h"
#include "snsrindicatorwidget.h"
#include "snsroledtimelabel.h"
#include "snsrswipewidget.h"

/*!
    \class SnsrDocumentLoader
    \ingroup group_snsrbigclockscreensaverplugin
    \brief Custom screensaver docml layout loader.

    Used to create the widgets from docml.
 */

/*!
    Creates the requested widget.
    \param type Static type name.
    \param name Object name.
    \retval Created object.
 */
QObject *SnsrDocumentLoader::createObject(const QString &type, const QString &name)
{
    if (type == SnsrAnalogClockWidget::staticMetaObject.className()) {
        QObject *object = new SnsrAnalogClockWidget();
        object->setObjectName(name);
        return object;
    }

    if (type == SnsrOledClockWidget::staticMetaObject.className()) {
        QObject *object = new SnsrOledClockWidget();
        object->setObjectName(name);
        return object;
    }

    if (type == SnsrLabel::staticMetaObject.className()) {
        QObject *object = new SnsrLabel();
        object->setObjectName(name);
        return object;
    }
    
    if (type == SnsrIndicatorWidget::staticMetaObject.className()) {
        QObject *object = new SnsrIndicatorWidget();
        object->setObjectName(name);
        return object;
    }

    if (type == SnsrOledTimeLabel::staticMetaObject.className()) {
        QObject *object = new SnsrOledTimeLabel();
        object->setObjectName(name);
        return object;
    }    
    
     if (type == SnsrSwipeWidget::staticMetaObject.className()) {
        QObject *object = new SnsrSwipeWidget();
        object->setObjectName(name);
        return object;
    }

    return HbDocumentLoader::createObject(type, name);
}
