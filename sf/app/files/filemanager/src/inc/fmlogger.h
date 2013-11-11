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
 * 
 * Description:
 *     The header file of logger
 */
#ifndef FMLOGGER_H
#define FMLOGGER_H

#define FMLOG_PATH QString( "C:\\data\\fileman.txt" )

#include <QString>
#include <QFile>
#include <QTextStream>
#include <QDateTime>

#ifdef _DEBUG_LOG_ENABLE_
    #define FM_LOG(str) FmLogger::log( str );
#else
    #define FM_LOG(str)
#endif

class FmLogger
{
public:

    static bool log( const QString &log )
    {
#ifdef _DEBUG_LOG_ENABLE_
        QString logStr( QDateTime::currentDateTime().toString("hh:mm:ss:zzz") + " " + log + "\r\n" );    
        QFile file( FMLOG_PATH );
        if ( !file.open( QIODevice::WriteOnly | QIODevice::Append ) )
        {
            return false;
        }
        QTextStream out( &file );
        out << logStr;
#else
        Q_UNUSED( log );
#endif
        return true;
    }
};

#endif
