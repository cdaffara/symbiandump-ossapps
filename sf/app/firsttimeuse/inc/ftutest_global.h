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


#ifndef FTUTEST_GLOBAL_H
#define FTUTEST_GLOBAL_H

#include <QDebug>

#ifdef NFT
#ifdef Q_OS_SYMBIAN
#include <e32hal.h>
#endif
#endif

#ifndef FTU_TEST
    #define FTU_TEST_CLASS(aClassName)
#else
    #define FTU_TEST_CLASS(aClassName) class aClassName;
#endif //FTU_TEST

#ifndef FTU_TEST
    #define FTU_TEST_FRIEND_CLASS(aClassName)
#else
    #define FTU_TEST_FRIEND_CLASS(aClassName) friend class aClassName;
#endif //FTU_TEST

#define FTUCATCHDEBUG qDebug("Catched in: %s on line: %d",__FILE__,__LINE__);
#define FTUDEBUG(a) qDebug("%s in: %s on line: %d", (a),  __FILE__,__LINE__);

#ifdef NFT
    #define FTUTEST(aText)                  qDebug() << QString(aText)
#ifdef Q_OS_SYMBIAN
    inline void FTUTestFreeRam(const QString &aFunction)
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
    
    #define FTUTEST_FUNC_ENTRY(aFunction)   FTUTestFreeRam(QString(aFunction)+" entry")
    #define FTUTEST_FUNC_EXIT(aFunction)    FTUTestFreeRam(QString(aFunction)+" exit")
#else
    #define FTUTEST_FUNC_ENTRY(aFunction)   qDebug()<< aFunction << " entry";
    #define FTUTEST_FUNC_EXIT(aFunction)    qDebug()<< aFunction << " exit";
#endif
    
#else
    #define FTUTEST(aText)
    #define FTUTEST_FUNC_ENTRY(aFunction)
    #define FTUTEST_FUNC_EXIT(aFunction)
#endif

#endif // FTUTEST_GLOBAL_H

