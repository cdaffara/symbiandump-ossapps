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
* Description:
*
*/

#ifndef CXUICAPTUREKEYHANDLER_DESKTOP_P_H
#define CXUICAPTUREKEYHANDLER_DESKTOP_P_H

#include <QObject>

class CxuiCaptureKeyHandler;
class CxeEngine;

class CxuiCaptureKeyHandlerPrivate : public QObject
{
    Q_OBJECT
public:
    CxuiCaptureKeyHandlerPrivate(CxeEngine &aEngine, CxuiCaptureKeyHandler *parent);
    virtual ~CxuiCaptureKeyHandlerPrivate();

public:

    void startListeningKeys();
    void stopListeningKeys();

    bool isAutofocusKeyPressed();
    bool isCaptureKeyPressed();

    bool handleKeyEvent(QEvent *event);

private: //private implementation patterns
    CxuiCaptureKeyHandler *const q_ptr;
    Q_DECLARE_PUBLIC(CxuiCaptureKeyHandler)

private:
    CxeEngine &mEngine;
    bool mAutofocusKeyPressed;
    bool mCaptureKeyPressed;

private: // Stub / Dummy members
    QList<int> mPrimaryCameraAutofocusKeys;

};

#endif // CXUICAPTUREKEYHANDLER_DESKTOP_P_H
