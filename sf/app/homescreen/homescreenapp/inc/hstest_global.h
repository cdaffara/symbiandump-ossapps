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
* Description:  The dll header file.
*
*/


#ifndef HSTEST_GLOBAL_H
#define HSTEST_GLOBAL_H

#include <QDebug>
#ifdef NFT
#ifdef Q_OS_SYMBIAN
#include <e32hal.h>
#endif
#endif

#ifndef HOMESCREEN_TEST
    #define HOMESCREEN_TEST_CLASS(aClassName)
#else
    #define HOMESCREEN_TEST_CLASS(aClassName) class aClassName;
#endif //HOMESCREEN_TEST

#ifndef HOMESCREEN_TEST
    #define HOMESCREEN_TEST_FRIEND_CLASS(aClassName)
#else
    #define HOMESCREEN_TEST_FRIEND_CLASS(aClassName) friend class aClassName;
#endif //HOMESCREEN_TEST

#define HSCATCHDEBUG qDebug("Catched in: %s on line: %d",__FILE__,__LINE__);
#define HSDEBUG(a) qDebug() << a << " in: " << __FILE__ << " on line: " << __LINE__;


#ifdef NFT
    #define HSTEST(aText)                  qDebug() << QString(aText)
#ifdef Q_OS_SYMBIAN
    inline void HSTestFreeRam(const QString &aFunction)
    {
    TMemoryInfoV1Buf meminfo;
    UserHal::MemoryInfo(meminfo);
    int memSize = meminfo().iTotalRamInBytes >> 10;
    int memFree = meminfo().iFreeRamInBytes >> 10;

    RHeap& ownHeap = User::Heap();
    TInt biggestBlock = 0;
    int heapFree = ownHeap.Available(biggestBlock) >> 10;
    int heapSize = ownHeap.Size() >> 10;
    qDebug()<< aFunction << "memFree" << memFree << "kb memSize" << memSize << "kb heapFree" << heapFree << "kb heapSize" << heapSize;
    }

    #define HSTEST_FUNC_ENTRY(aFunction)   HSTestFreeRam(QString(aFunction)+" entry")
    #define HSTEST_FUNC_EXIT(aFunction)    HSTestFreeRam(QString(aFunction)+" exit")
#else //Q_OS_SYMBIAN
    #define HSTEST_FUNC_ENTRY(aFunction)   qDebug()<< aFunction << " entry";
    #define HSTEST_FUNC_EXIT(aFunction)    qDebug()<< aFunction << " exit";
#endif //Q_OS_SYMBIAN
#else //NFT
    #define HSTEST(aText)                   qDebug() << QString(aText)
    #define HSTEST_FUNC_ENTRY(aFunction)
    #define HSTEST_FUNC_EXIT(aFunction)
#endif //NFT

#endif // HSTEST_GLOBAL_H

