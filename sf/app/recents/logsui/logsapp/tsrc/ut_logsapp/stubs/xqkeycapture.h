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

#ifndef XQKEYCAPTURE_H
#define XQKEYCAPTURE_H

#include <qnamespace.h>
#include <QList>
#include <QEvent>

class XQKeyCapture {
    
public:
    XQKeyCapture();

    ~XQKeyCapture();

    bool captureKey(Qt::Key aKey,
        Qt::KeyboardModifiers aModifiersMask = Qt::NoModifier,
        Qt::KeyboardModifiers aModifier = Qt::NoModifier);
		
    bool cancelCaptureKey(Qt::Key aKey,
        Qt::KeyboardModifiers aModifiersMask = Qt::NoModifier,
        Qt::KeyboardModifiers aModifier = Qt::NoModifier);

public: // Test stuff
    QList<Qt::Key> mKeys;
};

#endif /* XQKEYCAPTURE_H */
