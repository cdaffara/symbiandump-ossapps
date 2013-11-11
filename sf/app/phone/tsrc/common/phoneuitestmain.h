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

#ifndef PHONEUITESTMAIN
#define PHONEUITESTMAIN

#include <hbapplication.h>
#include <hbmainwindow.h>

char *new_argv[3];
HbMainWindow *mainWindow;
#define PHONE_UITEST_MAIN(TestObject) \
int main(int argc, char *argv[]) \
{ \
HbApplication *app = new HbApplication(argc, argv); \
QString str = "C:\\data\\" + QFileInfo(QCoreApplication::applicationFilePath()).baseName() + ".log"; \
QByteArray   bytes  = str.toAscii(); \
\
char arg1[] = "-o"; \
\
new_argv[0] = argv[0]; \
new_argv[1] = arg1; \
new_argv[2] = bytes.data(); \
\
TestObject tc; \
QResource::registerResource("../hbcore.rcc"); \
mainWindow = new HbMainWindow;\
mainWindow->show(); \
int ret = QTest::qExec(&tc, 3, new_argv); \
/* Core dump if HbIconLoader instance is not destroyed before the application instance. */ \
/* HbIconLoader uses QCoreApplication::aboutToQuit() signal to destroy itself. */ \
/* app.exec() where the signal is normally emitted is not called here. */ \
/* So, invoking the signal explicitly. */ \
QMetaObject::invokeMethod(app, "aboutToQuit", Qt::DirectConnection); \
delete mainWindow; \
delete app; \
return ret; \
}

#endif
