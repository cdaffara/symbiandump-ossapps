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
* Description: Global Screensaver macros for testing purposes.
*
*/

#ifndef SNSRTEST_GLOBAL_H
#define SNSRTEST_GLOBAL_H

#ifndef SCREENSAVER_TEST
    #define SCREENSAVER_TEST_CLASS(aClassName)
    #define SCREENSAVER_TEST_FRIEND_CLASS(aClassName)
#else
    #define SCREENSAVER_TEST_CLASS(aClassName) class aClassName;
    #define SCREENSAVER_TEST_FRIEND_CLASS(aClassName) friend class aClassName;
#endif //SCREENSAVER_TEST

/*!    
To enable logging of function entry and exit use the following flag for qmake:
-config nft
To enable logging of function entry and exit aswell as RAM usage (Symbian only!)
use the following flag for qmake:
-config nft_ram
*/

#ifdef NFT
    #include <QDebug>
    #include <QTime>

#ifdef NFT_RAM && Q_OS_SYMBIAN
    #include <hal.h>
    #include <e32std.h>

    #define SCREENSAVER_TEST_FREERAM_ENTRY(function) \
    TInt gEntryFreeRam(0); \
    TInt gEntryFreeHeap(0); \
    TInt gEntryTotalHeap(0); \
    { \
        HAL::Get(HAL::EMemoryRAMFree, gEntryFreeRam); \
        RHeap &heap = User::Heap(); \
        TInt biggestBlock(0); \
        gEntryFreeHeap = heap.Available(biggestBlock); \
        gEntryTotalHeap = heap.Size(); \
        qDebug("(snsr nft RAM in) " function \
            " - Memory (kB) - Free RAM: %d, Heap size: %d, Free heap: %d", \
            gEntryFreeRam >> 10, \
            gEntryTotalHeap >> 10, \
            gEntryFreeHeap >> 10); \
    }
    
    #define SCREENSAVER_TEST_FREERAM_EXIT(function) \
    { \
        TInt freeRAM(0); \
        HAL::Get(HAL::EMemoryRAMFree, freeRAM); \
        RHeap &heap = User::Heap(); \
        TInt biggestBlock(0); \
        TInt freeHeap = heap.Available(biggestBlock); \
        TInt totalHeap = heap.Size(); \
        qDebug("(snsr nft RAM out) " function \
            " - Memory (kB) - Free RAM: %d (%+d), Heap size: %d (%+d), Free heap: %d (%+d)", \
            freeRAM >> 10, (freeRAM-gEntryFreeRam) >> 10, \
            totalHeap >> 10, (totalHeap-gEntryTotalHeap) >> 10, \
            freeHeap >> 10, (freeHeap-gEntryFreeHeap) >> 10); \
    }
#else // NFT_RAM && Q_OS_SYMBIAN
    #define SCREENSAVER_TEST_FREERAM_ENTRY(function)
    #define SCREENSAVER_TEST_FREERAM_EXIT(function)
#endif // NFT_RAM && Q_OS_SYMBIAN

    #define SCREENSAVER_TEST_TIME_ENTRY(function) \
    QTime _ENTRY_TIME; \
    _ENTRY_TIME.start(); \
    { \
        qDebug() << "(snsr nft time in)" << function << "-" << \
            _ENTRY_TIME.toString("hh:mm:ss.zzz"); \
    }
        
    #define SCREENSAVER_TEST_TIME_EXIT(function) \
    { \
        qDebug() << "(snsr nft time out)" << function << "-" << \
            QTime::currentTime().toString("hh:mm:ss.zzz") << \
            "elapsed:" << _ENTRY_TIME.elapsed() << "ms"; \
    }

    #define SCREENSAVER_TEST_FUNC_ENTRY(function)   SCREENSAVER_TEST_TIME_ENTRY(function) \
        SCREENSAVER_TEST_FREERAM_ENTRY(function)
    #define SCREENSAVER_TEST_FUNC_EXIT(function)    SCREENSAVER_TEST_TIME_EXIT(function) \
        SCREENSAVER_TEST_FREERAM_EXIT(function)
#else //NFT
    #define SCREENSAVER_TEST_FUNC_ENTRY(function)
    #define SCREENSAVER_TEST_FUNC_EXIT(function)
#endif //NFT

#endif // SNSRTEST_GLOBAL_H
