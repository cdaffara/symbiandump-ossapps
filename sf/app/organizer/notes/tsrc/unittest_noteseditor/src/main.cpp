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

// System includes
#include <QTest>
#include <HbApplication>

// User includes
#include "unittest_noteseditor.h"

int main(int argc, char *argv[])
{
	HbApplication app(argc, argv);
	TestNotesEditor tc;
	int ret = QTest::qExec(&tc, argc, argv);
	// Core dump if HbIconLoader instance is not destroyed before the 
	// application instance. HbIconLoader uses QCoreApplication::aboutToQuit() 
	// signal to destroy itself. app.exec() where the signal is normally emitted
	// is not called here. So, invoking the signal explicitly.
	QMetaObject::invokeMethod(&app, "aboutToQuit", Qt::DirectConnection);
	return ret;
}

// End of file	--Don't remove this.
