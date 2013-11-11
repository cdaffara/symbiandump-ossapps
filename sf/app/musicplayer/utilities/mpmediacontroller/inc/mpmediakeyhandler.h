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
* Description: Music Player media key handler.
*
*/

#ifndef MPMEDIAKEYHANDLER_H
#define MPMEDIAKEYHANDLER_H

#include <QObject>

class MpMediaKeyHandlerPrivate;

class MpMediaKeyHandler : public QObject
{
    Q_OBJECT

    friend class MpMediaKeyHandlerPrivate;

public:

    explicit MpMediaKeyHandler( QObject *parent = 0 );
    virtual ~MpMediaKeyHandler();

private:

    Q_DISABLE_COPY(MpMediaKeyHandler)
    MpMediaKeyHandlerPrivate *d_ptr;

};

#endif // MPMEDIAKEYHANDLER_H
