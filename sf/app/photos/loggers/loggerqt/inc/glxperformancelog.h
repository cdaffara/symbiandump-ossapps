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
* Description:   ?Description
*
*/

#ifndef GLXPERFORMANCELOG_H
#define GLXPERFORMANCELOG_H

#include<QTime>
#include<QFile>
#include<QDir>

#ifdef BUILD_LOGGERQT
#define GLX_LOGGERQT_EXPORT Q_DECL_EXPORT
#else
#define GLX_LOGGERQT_EXPORT Q_DECL_IMPORT
#endif

class GLX_LOGGERQT_EXPORT GlxPerformanceLog
{	
public:	
	GlxPerformanceLog(char *str = "") ;
	~GlxPerformanceLog();
	
	void start();
	void stop();
	void end();
	void done(char *msg = "");
	
	void appendPerformanceLog(char *msg = "");
	void setInfoMsg(char *str)  {mInfoMsg = str;}
	void appendTimeStamp(char *msg = "");
	static QFile mLogFile;
	static qint16 mObjectCounter;


private:
	qreal getAverageTime();
	quint32 getTime();
	
private :
	QTime   mTime;
	quint32 mSpendTime;
	quint32 mTotalTime;
	quint16 mIteration;
	char   *mInfoMsg;	
};


#endif /* GLXPERFORMANCELOG_H_ */
