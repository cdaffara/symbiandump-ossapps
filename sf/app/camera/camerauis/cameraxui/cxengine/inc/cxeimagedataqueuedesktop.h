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
#ifndef CXEIMAGEDATAQUEUEDESKTOP_H
#define CXEIMAGEDATAQUEUEDESKTOP_H

//  Include Files

#include <QList>

#include "cxeimagedataqueue.h"

//  Constants


//  Class Definitions

class QImage;
class CxeImageDataItem;

class CxeImageDataQueueDesktop : public CxeImageDataQueue
{
    Q_OBJECT

public:
    CxeImageDataQueueDesktop(); 
    virtual ~CxeImageDataQueueDesktop();


public: // from CxeImageDataQueue
    int size() const;
    void clear();
    CxeImageDataItem &operator[](int index);

public:  // public member functions, not in client API
    CxeImageDataItem *startSave(QByteArray data, QString path, int id);

private: // private data members
    QList<CxeImageDataItem *> mList;
};

#endif  // __CXEIMAGEDATAQUEUEDESKTOP_H
