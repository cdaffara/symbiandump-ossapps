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

#ifndef HSMENUSERVICE_GLOBAL_H
#define HSMENUSERVICE_GLOBAL_H

// System includes
#include <qglobal.h>
#include <QDebug>

#ifdef MENUSERVICE_LIB
#define MENUSERVICE_EXPORT Q_DECL_EXPORT
#else
#ifdef MENUSERVICE_TEST
#define MENUSERVICE_EXPORT
#else
#define MENUSERVICE_EXPORT Q_DECL_IMPORT
#endif
#endif

#ifndef MENUSERVICE_UNITTEST
#define HS_SERVICE_TEST_CLASS(className)
#else
#define HS_SERVICE_TEST_CLASS(className) class className;
#endif

#ifndef MENUSERVICE_UNITTEST
#define HS_SERVICE_TEST_FRIEND_CLASS(className)
#else
#define HS_SERVICE_TEST_FRIEND_CLASS(className) friend class className;
#endif

/*!
 To enable logging of function entry and exit use the following flag for qmake:
 -config nft
 To include in logs extra information about RAM and heap usage, define an additional constant e.g. in ProductVariant.hrh:
 #define NFT_RAM
 */

#ifdef NFT
#ifdef Q_OS_SYMBIAN
#include <hal.h>
#include <e32std.h>
#endif
#endif

#ifdef NFT

#define HSMENUTEST(aText) qDebug() << QString(aText)

#ifdef Q_OS_SYMBIAN
#ifdef NFT_RAM
#define HSMENUTEST_FREERAM_ENTRY(function) \
    TInt HSMENUTEST_ENTRY_RAM(0); \
    TInt HSMENUTEST_ENTRY_HEAP(0); \
    TInt HSMENUTEST_ENTRY_HEAP_SIZE(0); \
    { \
        TInt allRAM(0); \
        HAL::Get(HAL::EMemoryRAM, allRAM); \
        HAL::Get(HAL::EMemoryRAMFree, HSMENUTEST_ENTRY_RAM); \
        RHeap &heap = User::Heap(); \
        TInt biggestBlock(0); \
        HSMENUTEST_ENTRY_HEAP = heap.Available(biggestBlock); \
        HSMENUTEST_ENTRY_HEAP_SIZE = heap.Size(); \
        qDebug("(nft) " function " - Memory (kB) - Free RAM: %d, Heap size: %d, Free heap: %d", \
               HSMENUTEST_ENTRY_RAM >> 10, HSMENUTEST_ENTRY_HEAP_SIZE >> 10, \
               HSMENUTEST_ENTRY_HEAP >> 10); \
    }

#define HSMENUTEST_FREERAM_EXIT(function) \
    { \
        TInt allRAM(0); \
        TInt freeRAM(0); \
        HAL::Get(HAL::EMemoryRAM, allRAM); \
        HAL::Get(HAL::EMemoryRAMFree, freeRAM); \
        RHeap &heap = User::Heap(); \
        TInt biggestBlock(0); \
        TInt heapFree = heap.Available(biggestBlock); \
        TInt heapSize = heap.Size(); \
        qDebug("(nft) " function " - Memory (kB) - Free RAM: %d, Heap size: %d, Free heap: %d", \
               freeRAM >> 10, heapSize >> 10, heapFree >> 10); \
        qDebug("(nft) " function " - Memory (kB) - RAM diff: %d, Heap size diff: %d, Free heap diff: %d", \
               (freeRAM-HSMENUTEST_ENTRY_RAM) >> 10, (heapSize-HSMENUTEST_ENTRY_HEAP_SIZE) >> 10, \
               (heapFree-HSMENUTEST_ENTRY_HEAP) >> 10); \
    }

#else

#define HSMENUTEST_FREERAM_ENTRY(function)
#define HSMENUTEST_FREERAM_EXIT(function)

#endif

#define HSMENUTEST_FUNC_ENTRY(function)   HSMENUTEST_FREERAM_ENTRY(function) \
    qDebug()<< function << " entry";
#define HSMENUTEST_FUNC_EXIT(function)     qDebug()<< function << " exit"; \
    HSMENUTEST_FREERAM_EXIT(function)

#else //Q_OS_SYMBIAN
#define HSMENUTEST_FUNC_ENTRY(function)   qDebug()<< function << " entry";
#define HSMENUTEST_FUNC_EXIT(function)    qDebug()<< function << " exit";
#endif //Q_OS_SYMBIAN
#else //NFT
#define HSMENUTEST_FUNC_ENTRY(function)
#define HSMENUTEST_FUNC_EXIT(function)
#define HSMENUTEST(aText) 
#endif //NFT


#endif // HSMENUSERVICE_GLOBAL_H
