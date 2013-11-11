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

#ifndef INFOWIDGETLOGGING_H
#define INFOWIDGETLOGGING_H

#include <QDebug>


/*!
  Define MSG_OUTPUT_RDEBUG 
  for enabling RDebug prints in development tracing. 
  Shouldn't be used in release code.
  */
#undef MSG_OUTPUT_RDEBUG


#ifdef MSG_OUTPUT_RDEBUG
#ifdef Q_OS_SYMBIAN
#include <e32debug.h>

static void infoWidgetMsgOutput(QtMsgType type, const char *msg)
{
    switch (type) {
        
    case QtDebugMsg:
        RDebug::Printf("InfoWidget Debug: %s\n", msg);
        break;
    case QtWarningMsg:
        RDebug::Printf("InfoWidget Warning: %s\n", msg);
        break;
    case QtCriticalMsg:
        RDebug::Printf("InfoWidget Critical: %s\n", msg);
        break;
    case QtFatalMsg:
        RDebug::Printf("InfoWidget Fatal: %s\n", msg);
        abort();
    }
}

    #define INSTALL_TRACE_MSG_HANDLER qInstallMsgHandler(infoWidgetMsgOutput)
    #define UNINSTALL_TRACE_MSG_HANDLER qInstallMsgHandler(0)
#endif //Q_OS_SYMBIAN
#else 
    #define INSTALL_TRACE_MSG_HANDLER 
    #define UNINSTALL_TRACE_MSG_HANDLER 
#endif //MSG_OUTPUT_RDEBUG

/*!
  Debug macros
  */
#if _DEBUG 
#define DPRINT qDebug() << __PRETTY_FUNCTION__
#else
#define DPRINT if(0) qDebug()
#endif 
#define DWARNING qWarning() << __PRETTY_FUNCTION__
#define DCRITICAL qCritical() << __PRETTY_FUNCTION__

#endif // INFOWIDGETLOGGING

