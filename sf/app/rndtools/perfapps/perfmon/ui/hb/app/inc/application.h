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

#ifndef APPLICATION_H
#define APPLICATION_H

#include <HbApplication>


class QSymbianEvent;

class Application : public HbApplication
{
    Q_OBJECT

public:
    Application(int &argc, char *argv[]);

signals:
    void foregroundEvent(bool foreground);

protected:
#ifdef Q_OS_SYMBIAN
    bool symbianEventFilter(const QSymbianEvent * event);
#endif
};

#endif // APPLICATION_H
