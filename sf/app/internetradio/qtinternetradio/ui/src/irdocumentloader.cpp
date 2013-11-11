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
* Description:
*
*/
#include <HbTextItem>
#include <HbMarqueeItem>
#include "irdocumentloader.h" 
#include "irplayingbanner.h"

QObject *IRDocumentLoader::createObject(const QString& aType, const QString &aName)
{
    if (aType == HbTextItem::staticMetaObject.className())
    {
        QObject *object = new HbTextItem;
        object->setObjectName(aName);
        return object;
    }
    else if (aType == HbMarqueeItem::staticMetaObject.className())
    {
        QObject *object = new HbMarqueeItem;
        object->setObjectName(aName);
        return object;
    }
    else if (aType == IRPlayingBanner::staticMetaObject.className())
    {
        QObject *object = new IRPlayingBanner;
        object->setObjectName(aName);
        return object;
    }

    return HbDocumentLoader::createObject(aType, aName);
}
