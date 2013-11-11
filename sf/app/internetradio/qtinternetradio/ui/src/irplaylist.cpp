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
#include <QFile>
#include <QTextStream>
#include <QStringList>
#include <HbGlobal>
#include "irplaylist.h"
#include "irqisdsdatastructure.h"

const QString KEqual("=");
const QString KFile("File");
const QString KTitle("Title");
const QString KLength("Length");
const QString KPlsExtension(".pls");
const QString KM3uExtension(".m3u");
const QString KHttpProtocol("http");
const QString KMmsProtocol("mms");
const QString KRtspProtocol("rtsp");
const QString KM3uComment("#EXTINF");

IRPlayListItem::IRPlayListItem(const QString &aFile, const QString &aTitle, int aLength) :
                                     iFile(aFile), iTitle(aTitle), iLength(aLength)
{
}

const QString & IRPlayListItem::file() const
{
    return iFile;
}

const QString & IRPlayListItem::title() const
{
    return iTitle;
}

IRPlayList::IRPlayList()
{
}

IRPlayList::~IRPlayList()
{
    clearPlayList();
}
    
void IRPlayList::parseFile(const QString &aFileName)
{
    iFileName = aFileName;
    QString extension = aFileName.right(4);
    
    if (0 == extension.compare(KPlsExtension, Qt::CaseInsensitive))
    {
        parsePlsFile(aFileName);
    }
    else if (0 == extension.compare(KM3uExtension, Qt::CaseInsensitive))
    {
        parseM3uFile(aFileName);
    }
    else
    {
        Q_ASSERT(false);
    }
}
    
int IRPlayList::getNumberOfEntries() const
{
    return iItemList.count();
}

IRPlayListItem* IRPlayList::getEntry(int aIndex)
{
    if (aIndex >=0 && aIndex < iItemList.count())
    {
        return iItemList.at(aIndex);
    }
    
    return NULL;
}

/*
 * create an object of IRQPreset for item at aIndex.
 * Note : caller must be responsible for deleting the returned object
 */
IRQPreset* IRPlayList::getPresetForEntry(int aIndex)
{
    IRQPreset *preset = NULL;
    if (aIndex >=0 && aIndex < iItemList.count())
    {
        preset = new IRQPreset;
        IRPlayListItem *item = iItemList.at(aIndex);
        IRQChannelServerURL server;
        server.bitrate = 0;
        server.url = item->file();
        server.serverName = item->title();
        
        preset->insertChannelServer(server);
        preset->name = item->title();
        preset->nickName = item->title();
        preset->description = "";
        preset->shortDesc = "";
        preset->type = 0;
        preset->uniqID = 0;
        preset->presetId = 0;
    }
    
    return preset;
}

const QString& IRPlayList::getFileName() const
{
    return iFileName;
}

void IRPlayList::deleteItem(int aIndex)
{
    if (aIndex >=0 && aIndex < iItemList.count())
    {
        IRPlayListItem *item = iItemList.at(aIndex);
        delete item;
        iItemList.removeAt(aIndex);
    }
}

void IRPlayList::parsePlsFile(const QString &aFileName)
{
    if (aFileName.length() == 0)
    {
        return;
    }
  
    clearPlayList();
    
    QFile file(aFileName);
    if (!file.open(QIODevice::ReadOnly))
    {
        return;
    }
    
    QTextStream stream(&file);
    QString readStr = stream.readLine().trimmed();
    if (readStr.compare("[playlist]", Qt::CaseInsensitive))
    {
        file.close();
        return;
    }
    
    readStr = stream.readLine().trimmed();
    QStringList splitStr = readStr.split(KEqual);
    if (splitStr.count() != 2)
    {
        file.close();
        return;
    }
    
    int nbEntries = 0;
    if (0 == splitStr.first().compare("NumberOfEntries", Qt::CaseInsensitive))
    {
        nbEntries = splitStr.last().toInt();
    }
    
    if (nbEntries <= 0)
    {
        file.close();
        return;
    }
    
    for (int i = 0; i < nbEntries; ++i)
    {
        //read file line : File=xxx     
        QString filePath = readValue(stream, KFile);
        if (filePath.length() == 0)
        {
            break;
        }
        
        //read title line : Title=xxx
        QString title = readValue(stream, KTitle);
        if (title.length() == 0)
        {
            break;
        }
        
        //read length line : Length=xxx
        QString strLength = readValue(stream, KLength);
        if (strLength.length() == 0)
        {
            break;
        }
        
        int length = strLength.toInt();
        
        //all information is ready
        if (length == -1)
        {
            IRPlayListItem *item = new IRPlayListItem(filePath, title, length);
            iItemList.append(item);
        }
    }
    
    file.close();
}

void IRPlayList::parseM3uFile(const QString &aFileName)
{
    clearPlayList();
    
    QFile file(aFileName);
    if (!file.open(QIODevice::ReadOnly))
    {
        return;
    }
    
    QTextStream stream(&file);
    QString previousLine;
    QString currentLine;
    
    while (!stream.atEnd())
    {
        currentLine = stream.readLine().trimmed();
        if (currentLine.startsWith(KHttpProtocol, Qt::CaseInsensitive) ||
            currentLine.startsWith(KMmsProtocol, Qt::CaseInsensitive) ||
            currentLine.startsWith(KRtspProtocol, Qt::CaseInsensitive) )
        {
            IRPlayListItem *item = NULL;
            if (previousLine.startsWith(KM3uComment, Qt::CaseInsensitive))
            {
                //previousLine can be used as station name
                item = new IRPlayListItem(currentLine, previousLine, -1);
            }
            else
            {
                //no station name is available
#ifdef SUBTITLE_STR_BY_LOCID
                item = new IRPlayListItem(currentLine, hbTrId("txt_irad_info_unnamed_station"), -1);
#else
                item = new IRPlayListItem(currentLine, hbTrId("Unnamed station"), -1);                
#endif
            }
            iItemList.append(item);
        }
        
        previousLine = currentLine;
    }
    
    file.close();
}

void IRPlayList::clearPlayList()
{
    while (!iItemList.isEmpty())
    {
        IRPlayListItem *firstItem = iItemList.takeFirst();
        delete firstItem;
    }
}

QString IRPlayList::readValue(QTextStream &aStream, const QString &aKey)
{
    QString readStr;
    QStringList splitStr;
    
    do
    {
        readStr = aStream.readLine().trimmed();    
    }while (readStr.length() == 0 && !aStream.atEnd());
            
    splitStr = readStr.split(KEqual);
    if (splitStr.count() != 2 || !splitStr.first().startsWith(aKey, Qt::CaseInsensitive))
    {
        return QString();
    }
    else
    {
        return splitStr.last();
    }
}
