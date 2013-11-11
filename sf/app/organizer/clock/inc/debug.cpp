/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Utility to print debug information.
*
*/



#include <QFile>
#include <QTextStream>
#include <QTime>

#include "debug.h"
#include "clockcommon.h"

void Debug::writeDebugMsg(QString msg)
{
	QFile file(CITY_INFO_DB_PATH + CLOCK_DEBUG_FILE);

	file.open(QIODevice::ReadWrite | QIODevice::Append);
	msg = QTime::currentTime().toString("hh:mm:ss.zzz") + "- " + msg;

	QTextStream textStream(&file);
	textStream << msg << endl;

	file.close();
}

// End of file	--Don't remove this.
