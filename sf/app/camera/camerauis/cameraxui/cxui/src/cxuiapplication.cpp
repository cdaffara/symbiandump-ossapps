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

#include <QObject>
#include <QString>

#include "cxutils.h"
#include "cxuiapplication.h"


/*!
    \class CxuiApplication
    \brief CxuiApplication is a wrapper used for getting platform UI events.

    CxuiApplication is a wrapper used for getting platform UI events.

    Other classes may connect to CxuiApplication::symbianEvent signal, which is
    emitted whenever a QSymbianEvent is posted to application.

*/
CxuiApplication::CxuiApplication(int &argc, char *argv[])
    : HbApplication(argc, argv, Hb::NoSplash)
{
    CX_DEBUG_IN_FUNCTION();
}

/*!
* Destructor
*/
CxuiApplication::~CxuiApplication()
{
    CX_DEBUG_IN_FUNCTION();
}

#if defined(Q_WS_S60)
/*!
* Event filter.
* Reimplemented from HbApplication.
* Provides a signal with the event for rest of camera ui to use.
* Event handling is then progressed to HbApplication to process.
*/
bool CxuiApplication::symbianEventFilter(const QSymbianEvent *e)
{
    emit symbianEvent(e);
    return HbApplication::symbianEventFilter(e);
}
#endif // Q_WS_S60

// end of file
