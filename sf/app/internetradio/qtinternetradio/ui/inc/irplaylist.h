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

#ifndef IRPLAYLIST_H
#define IRPLAYLIST_H

#include <QString>
#include <QList>

class IRQPreset;
class QTextStream;

class IRPlayListItem
{
public:
    IRPlayListItem(const QString &aFile, const QString &aTitle, int aLength);
    
    const QString & file() const;
    const QString & title() const;
    
private:
    QString iFile;
    QString iTitle;
    int     iLength;
};

class IRPlayList
{
public:
    IRPlayList();
    ~IRPlayList();
    
    void parseFile(const QString &aFileName);
    int getNumberOfEntries() const;
    IRPlayListItem* getEntry(int aIndex);
    
    /*
     * create an object of IRQPreset for item at aIndex.
     * Note : caller must be responsible for deleting the returned object
     */
    IRQPreset* getPresetForEntry(int aIndex);
    
    void deleteItem(int aIndex);
    const QString& getFileName() const;
    
private:
    void parsePlsFile(const QString &aFileName);
    void parseM3uFile(const QString &aFileName);
    void clearPlayList();
    QString readValue(QTextStream &aStream, const QString &aKey);
    
private:
    QList<IRPlayListItem*> iItemList;
    QString iFileName;
};

#endif //IRPLAYLIST_H
