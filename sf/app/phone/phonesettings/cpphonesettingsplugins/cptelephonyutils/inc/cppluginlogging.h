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

#ifndef CPPHONESETTINGSPLUGINSLOGGING_H
#define CPPHONESETTINGSPLUGINSLOGGING_H

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

static void cpPhoneSettingsPluginsMsgOutput(QtMsgType type, const char *msg)
{
    switch (type) {
    
    case QtDebugMsg:
        RDebug::Printf("CpPhoneSettingsPlugins Debug: %s\n", msg);
        break;        
    case QtWarningMsg:
        RDebug::Printf("CpPhoneSettingsPlugins Warning: %s\n", msg);
        break;
    case QtCriticalMsg:
        RDebug::Printf("CpPhoneSettingsPlugins Critical: %s\n", msg);
        break;        
    case QtFatalMsg:
        RDebug::Printf("CpPhoneSettingsPlugins Fatal: %s\n", msg);
        abort();
        break; 
    default:
        break; 
    }
}

    #define INSTALL_TRACE_MSG_HANDLER qInstallMsgHandler(cpPhoneSettingsPluginsMsgOutput)
    #define UNINSTALL_TRACE_MSG_HANDLER qInstallMsgHandler(0)
#else //Q_OS_SYMBIAN
    #define INSTALL_TRACE_MSG_HANDLER
    #define UNINSTALL_TRACE_MSG_HANDLER
#endif
#else 
    #define INSTALL_TRACE_MSG_HANDLER 
    #define UNINSTALL_TRACE_MSG_HANDLER 
#endif //MSG_OUTPUT_RDEBUG

/*!
  Debug macros
  */
#define DPRINT qDebug() << __PRETTY_FUNCTION__
#define DWARNING qWarning() << __PRETTY_FUNCTION__
#define DCRITICAL qCritical() << __PRETTY_FUNCTION__

#endif // CPPHONESETTINGSPLUGINSLOGGING_H

