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
* Description: Music Player media wall view custom document loader.
*
*/

#include <hgmediawall.h>

#include "mpmediawalldocumentloader.h"
#include "mptrace.h"


/*!
    \class MpMediaWallDocumentLoader
    \brief Music Player media wall view custom document loader.

    Media wall view document loader extends Orbit document loader to enable
    loading of custom widgets.
*/

/*!
 Creates and returns the object of the given \a type.

 \reimp
 */
QObject *MpMediaWallDocumentLoader::createObject(const QString& type, const QString &name)
{
    TX_ENTRY_ARGS("type=" << type << ", name=" << name);

    if ( type == HgMediawall::staticMetaObject.className() ) {
        QObject *object = new HgMediawall();
        object->setObjectName(name);
        TX_EXIT
        return object;
    }

    TX_EXIT
    return HbDocumentLoader::createObject(type, name);
}

