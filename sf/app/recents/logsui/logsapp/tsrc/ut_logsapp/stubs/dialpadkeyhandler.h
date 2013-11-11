/*!
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
* Description: Dialpad key handler.
*
*/

#ifndef DIALPADKEYHANDLER_H
#define DIALPADKEYHANDLER_H

#include <QObject>

class Dialpad;
class DialpadVoiceMailboxEventFilter;
class DialpadBluetoothEventFilter;
class HbMainWindow;

/*!
    DialpadKeyHandler
    Class provides key handling for dialpad component.

    @code
    Dialpad *dialpad = new Dialpad();
    DialpadKeyHandler *keyhandler = new DialpadKeyHandler(dialpad, this);
    @endcode
    
*/
class DialpadKeyHandler : public QObject
{
    Q_OBJECT

public:
    explicit DialpadKeyHandler(Dialpad *dialPad, HbMainWindow& mainWindow, QObject *parent = 0);
    virtual ~DialpadKeyHandler();

private:
};

#endif // DIALPADKEYHANDLER_H
