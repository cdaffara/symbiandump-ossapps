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
#ifndef CXEIMAGEDATAQUEUESYMBIAN_H
#define CXEIMAGEDATAQUEUESYMBIAN_H

//  Include Files

#include <QList>

#include "cxeimagedataqueue.h"
#include "cxeimagedataitemsymbian.h"

//  Constants


//  Class Definitions

class QImage;
class CxeImageDataItem;

class CxeImageDataQueueSymbian : public CxeImageDataQueue
{
    Q_OBJECT

public:
    CxeImageDataQueueSymbian();
    virtual ~CxeImageDataQueueSymbian();


public: // from CxeImageDataQueue
    int size() const;
    void clear();
    //void deleteImage( int index );
    //void renameImage( int index, const QString& path );
    CxeImageDataItem &operator[]( int index );

public:  // public member functions, not in client API
    CxeImageDataItemSymbian *startSave( QByteArray data, QString path, int id, bool addLocation);

private: // private data members
    QList<CxeImageDataItemSymbian*> mList;
};



#endif  // __CXEIMAGEDATAQUEUESYMBIAN_H
