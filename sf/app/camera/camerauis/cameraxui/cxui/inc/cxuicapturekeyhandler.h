/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef CXUICAPTUREKEYHANDLER_H
#define CXUICAPTUREKEYHANDLER_H

#include <QObject>

class QEvent;
class CxeEngine;
class CxuiCaptureKeyHandlerPrivate;

class CxuiCaptureKeyHandler : public QObject
{
    Q_OBJECT
public:
    CxuiCaptureKeyHandler(CxeEngine &aEngine);
    virtual ~CxuiCaptureKeyHandler();

public:

    void startListeningKeys();
    void stopListeningKeys();
    bool isAutofocusKeyPressed();
    bool isCaptureKeyPressed();

    bool handleKeyEvent(QEvent *event);

signals:
    // Note that these signals should be connected to view classes only, because
    // CxuiViewManager will handle all signal connections and disconnections
    void autofocusKeyPressed();
    void autofocusKeyReleased();
    void captureKeyPressed();
    void captureKeyReleased();

private:
    CxuiCaptureKeyHandlerPrivate *d_ptr;
    Q_DECLARE_PRIVATE(CxuiCaptureKeyHandler)
};

#endif // CXUICAPTUREKEYHANDLER_H
