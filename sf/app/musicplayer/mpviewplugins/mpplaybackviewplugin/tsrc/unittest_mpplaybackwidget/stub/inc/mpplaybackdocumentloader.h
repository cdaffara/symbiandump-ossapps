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
* Description: stub for Music Player playback view custom document loader.
*
*/

#ifndef MPPLAYBACKDOCUMENTLOADER_H
#define MPPLAYBACKDOCUMENTLOADER_H


#include <hbdocumentloader.h>


class MpPlaybackDocumentLoader : public HbDocumentLoader
{
public:
    virtual QObject *createObject(const QString& type, const QString &name);
};

#endif  // MPPLAYBACKDOCUMENTLOADER_H

