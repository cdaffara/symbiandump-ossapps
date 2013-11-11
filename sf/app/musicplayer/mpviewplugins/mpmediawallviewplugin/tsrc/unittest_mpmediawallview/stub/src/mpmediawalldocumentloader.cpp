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
* Description: Stub for Music Player media wall view custom document loader.
*
*/

#include "mpmediawalldocumentloader.h"
#include "mpcommondefs.h"
#include "hgmediawall.h"
#include "mptrace.h"

#include <HbToolButton>

/*!
Stub implementation - load real widgets
 */
QObject *MpMediaWallDocumentLoader::createObject(const QString& type, const QString &name)
{
    TX_LOG_ARGS("MpMediaWallDocumentLoader::createObject() Stub");
    TX_ENTRY_ARGS("type=" << type << ", name=" << name);

    if ( type == HgMediawall::staticMetaObject.className() ) {
        QObject *object = new HgMediawall();
        object->setObjectName(name);
        TX_EXIT
        return object;
    }
    else if ( type == HbToolButton::staticMetaObject.className() ) {
        QObject *object = new HbToolButton();
        object->setObjectName(name);
        TX_EXIT
        return object;
    }

    TX_EXIT
    return HbDocumentLoader::createObject(type, name);
}

