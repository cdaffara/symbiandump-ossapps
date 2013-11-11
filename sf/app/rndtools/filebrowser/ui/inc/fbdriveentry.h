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

#ifndef FBDRIVEENTRY_H
#define FBDRIVEENTRY_H

#include "FBFileUtils.h"
#include <QString>
#include <QDate>


/**
 * class that is used for communicating between Symbian and Qt code.
 */
class FbDriveEntry {

public:
    
    /**
     * Constructor
     */
    FbDriveEntry(TDriveEntry aDriveEntry);
    
    /**
     * Destructor
     */
    ~FbDriveEntry();
    
    /* Functions that are called from UI */
public: 
    QChar driveLetter();
    int number();
    QString mediaTypeString();
    QString attributesString();
    int iconId();
    qint64 volumeInfoFree();
    qint64 volumeInfoSize();

private:
    TDriveEntry mDriveEntry;
    };

#endif //FBDRIVEENTRY_H
