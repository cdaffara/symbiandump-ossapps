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
* Description:  Document loader of the SimpleVideoPlayback MPX view plugin.
*
*/

#include "svpbdocumentloader.h"
#include "svpbview.h"
#include "trace.h"

SvpbDocumentLoader::SvpbDocumentLoader()
{
}

QObject *SvpbDocumentLoader::createObject(const QString& type, const QString &name)
{
    FUNC_LOG;
    INFO("Create object" << type << name);

    QObject *object = 0;

    if (type == SvpbView::staticMetaObject.className()) {
        object = new SvpbView;
    }

    if (object) {
        object->setObjectName(name);
        return object;
    }

    return HbDocumentLoader::createObject(type, name);
}
