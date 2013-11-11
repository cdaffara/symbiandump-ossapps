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

#ifndef QTESTMAINS60
#define QTESTMAINS60

#define QTEST_MAIN_S60(TestObject) \
int main(int argc, char *argv[]) \
{ \
char *new_argv[3]; \
QCoreApplication app(argc, argv); \
\
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
return QTest::qExec(&tc, 3, new_argv); \
}

#endif