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


#ifndef SEARCH_GLOBAL_H
#define SEARCH_GLOBAL_H

#include <qdebug.h>

#ifdef NFT
#ifdef Q_OS_SYMBIAN
#include <e32hal.h>
#endif
#endif

//settings storage path 
const QString SETTINGS_INI_PATH("C:\\private\\2002c377\\searchsettings.ini");


#define SEARCHAIWDECLINDEVICE "SearchDeviceQueryView"
#define SEARCHAIWDECLONLINE "SearchWebQueryView"
#define SEARCHAIWDECLMAINVIEW "SearchView"

#define SEARCHAIWQUERY "query"

#define SEARCHAPPUID "2002C377"

#define DEFAULT_SEARCH_FIELD "_aggregate"


#define SUGGESTIONLINKS "links"

#define FIRSTLINEFIELD "_hlexcerpt"
#define MAILBOXFIELD "MailBoxId"
#define FOLDERFIELD "FolderId"
#define NAMEFIELD "Name"
#define EXTENSIONFIELD "Extension"
#define ISFOLDERFIELD "IsFolder"
#define MEDIAIDFIELD "MediaId"

#define MEDIACATEGORY "media"
#define AUDIOCATEGORY "audio"
#define VIDEOCATEGORY "video"
#define IMAGECATEGORY "image"
#define EMAILCATEGORY "email"
#define CONTACTCATEGORY "contact"
#define CALENDARCATEGORY "calendar"
#define BOOKMARKCATEGORY "bookmark"
#define APPLICATIONCATEGORY "applications"
#define NOTESCATEGORY "notes"
#define FILECATEGORY "file"
#define MESSAGECATEGORY "msg"
#define FILEFOLDERCATEGORY "file folder"

//#define OST_TRACE_COMPILER_IN_USE //Uncomment to enable performance measurements.

#ifdef OST_TRACE_COMPILER_IN_USE
#define PERF_APP_LAUNCH_END(str) qDebug() << "Search_App_Launch :"<<str<<"at:"<<QTime::currentTime().second()<<":"<<QTime::currentTime().msec();
#else
#define PERF_APP_LAUNCH_END(str)
#endif 

#ifndef SEARCH_UNITTESTING
    #define SEARCH_CLASS(aClassName)
#else
    #define SEARCH_CLASS(aClassName) class aClassName;
#endif 

#ifndef SEARCH_UNITTESTING
    #define SEARCH_FRIEND_CLASS(aClassName)
#else
    #define SEARCH_FRIEND_CLASS(aClassName) friend class aClassName;
#endif 

#define SEARCHCATCHDEBUG qDebug("Catched in: %s on line: %d",__FILE__,__LINE__);
#define SEARCHDEBUG(a) qDebug("%s in: %s on line: %d", (a),  __FILE__,__LINE__);

#ifdef NFT
    #define SEARCHTEST(aText)                  qDebug() << QString(aText)
#ifdef Q_OS_SYMBIAN
    inline void SEARCHFreeRam(const QString &aFunction)
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
    
    #define SEARCH_FUNC_ENTRY(aFunction)   SEARCHFreeRam(QString(aFunction)+" entry")
    #define SEARCH_FUNC_EXIT(aFunction)    SEARCHFreeRam(QString(aFunction)+" exit")
#else
    #define SEARCH_FUNC_ENTRY(aFunction)   qDebug()<< aFunction << " entry";
    #define SEARCH_FUNC_EXIT(aFunction)    qDebug()<< aFunction << " exit";
#endif
    
#else
    #define SEARCHTEST(aText)
    #define SEARCH_FUNC_ENTRY(aFunction)
    #define SEARCH_FUNC_EXIT(aFunction)
#endif

#endif // SEARCH_GLOBAL_H

