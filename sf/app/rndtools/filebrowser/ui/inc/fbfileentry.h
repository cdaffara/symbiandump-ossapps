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

#ifndef FILEEENTRY_H
#define FILEEENTRY_H

#include "FBFileUtils.h"
#include <QString>
#include <QDate>
#include <QStringList>

/**
 * class that is used for communicating between Symbian and Qt code.
 */
class FbFileEntry
{
public:
    /**
     * Constructor
     */
    FbFileEntry(TFileEntry aFileEntry);
    
    /**
     * Destructor
     */
    ~FbFileEntry();
    
    /* Functions that are called from UI */
public: 
    QString fullName();
    QString path();

    int attributes();
    QString attributesString();
    bool isArchive() { return mFileEntry.iEntry.IsArchive(); }
    bool isHidden() { return mFileEntry.iEntry.IsHidden(); }
    bool isReadOnly() { return mFileEntry.iEntry.IsReadOnly(); }
    bool isSystem() { return mFileEntry.iEntry.IsSystem(); }

    int size();
    QString sizeString();

//    QDateTime modified();
    QString modifiedString();
//    int Uid(int id);
    bool isDir();
    QString name();

    int dirEntries();
    QString dirEntriesString();

    int iconId();

private:
    TFileEntry mFileEntry;
    };

#endif //FBFILEEENTRY_H
