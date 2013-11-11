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
* Description: Music Widget custom document loader.
*
*/


#include <hbtoolbutton.h>
#include <hbmarqueeitem.h>

#include "musicwidgetdocumentloader.h"
#include "mpalbumcoverwidget.h"
#include "mptrace.h"


/*!
    \class MusicWidgetDocumentLoader
    \brief Music Widget custom document loader.

    Music Widget document loader extends Orbit document loader to enable
    loading of custom widgets.
*/

/*!
 Creates and returns the object of the given \a type.

 \reimp
 */
QObject *MusicWidgetDocumentLoader::createObject(const QString& type, const QString &name)
{
    TX_ENTRY_ARGS("type=" << type << ", name=" << name);

    if ( type == MpAlbumCoverWidget::staticMetaObject.className() ) {
        QObject *object = new MpAlbumCoverWidget();
        object->setObjectName(name);
        TX_EXIT
        return object;
    }

    if (type == HbMarqueeItem::staticMetaObject.className())
    {
        QObject *object = new HbMarqueeItem;
        object->setObjectName(name);
        TX_EXIT
        return object;
    }

    TX_EXIT
    return HbDocumentLoader::createObject(type, name);
}

