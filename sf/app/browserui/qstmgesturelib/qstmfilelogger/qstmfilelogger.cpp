/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation, version 2.1 of the License.
* 
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this program.  If not, 
* see "http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html/".
*
* Description:
*
*/
#include <QDir>
#include <QApplication>
#include <QDateTime>
#include "qstmfilelogger.h"

QStm_FileLogger* QStm_FileLogger::m_instance = 0;

QStm_FileLogger::QStm_FileLogger()
{
	QString fname = getLogFileName();
	m_file = new QFile(fname);
	m_file->open(QIODevice::ReadWrite | QIODevice::Text);
	m_stream = new QTextStream(m_file);
}


QStm_FileLogger::~QStm_FileLogger()
{
	delete m_stream;
	m_file->close();
	delete m_file;
}

QString QStm_FileLogger::getLogFileName()
{
#ifdef Q_OS_SYMBIAN	
    QString path = "E:/Others/Logs";
    QDir logdir(path);
    logdir.mkpath(path);
#else 
    QString path = QDir::tempPath();
#endif    
    
    QString appName = qApp->applicationFilePath();
    QFileInfo fi(appName);
    QString baseName = fi.baseName();
    QString fileName = path % "/" % baseName % 
    		           QDateTime::currentDateTime().toString("ddMMyy_hhmmss") % 
    		           ".log";
    return fileName;
}


void QStm_FileLogger::log(const char* fmt,...)
{
	va_list list;
	QString logStr;
	va_start(list, fmt);
	logStr.vsprintf(fmt, list);
	logger()->doLog(logStr);
	va_end(list);
}


void QStm_FileLogger::log(const QString& text)
{
	logger()->doLog(text);
}

void QStm_FileLogger::doLog(const QString& text) 
{ 
	*m_stream << QDateTime::currentDateTime().toString("dd-MM-yy hh:mm:ss.zzz") << " " << 
			text << "\n"; 
}

void QStm_FileLogger::doLog(const char* text) 
{ 
	*m_stream << QDateTime::currentDateTime().toString("dd-MM-yy hh:mm:ss.zzz") << " " << 
			text << "\n"; 
}
 
QStm_FileLogger* QStm_FileLogger::logger()
{
	if (!m_instance) {
		m_instance = new QStm_FileLogger();
	}
	return m_instance;
}
