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
* Description: Music Player media wall view custom document loader stub for testing mpmediawallview.
*
*/

#ifndef MPMEDIAWALLDOCUMENTLOADER_H
#define MPMEDIAWALLDOCUMENTLOADER_H


#include <hbdocumentloader.h>


class MpMediaWallDocumentLoader : public HbDocumentLoader
{
public:
    virtual QObject *createObject(const QString& type, const QString &name);
};

#endif  // MPMEDIAWALLDOCUMENTLOADER_H

