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
#ifndef CXEFAKEFILESAVETHREAD_H
#define CXEFAKEFILESAVETHREAD_H

#include "cxefilesavethread.h"
#include "cxeimagedataitem.h"


class CxeFakeFileSaveThread : public CxeFileSaveThread
{
    Q_OBJECT
public:
    CxeFakeFileSaveThread();
    virtual ~CxeFakeFileSaveThread();
    void save(CxeImageDataItem *data);

public slots:
    void handleVideoSaved(CxeError::Id status, const QString &filename);
    void handleSnapshotReady(CxeError::Id status, const QImage &snapshot, const QString &filename);
    void handleSnapshotReady(CxeError::Id status, const QImage &snapshot, int id);
};

#endif // CXEFAKEFILESAVETHREAD_H
