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
#ifndef CXEFAKEIMAGEDATAITEM_H
#define CXEFAKEIMAGEDATAITEM_H

//  Include Files
#include "cxeimagedataitem.h"
#include "cxeerror.h"

class CxeFakeImageDataItem : public CxeImageDataItem
{
    Q_OBJECT

public: // constructor and destructor
    CxeFakeImageDataItem(int index, QByteArray data, QString filename, int id, bool addLocation);
    virtual ~CxeFakeImageDataItem();

public: // from CxeImageDataItem
    CxeError::Id save();
    CxeImageDataItem::State state() const;
    int id() const;
    QString path() const;
    bool addLocation() const;

private:
    int mId;
    int mIndex;
    QByteArray mData;
    QString mFileName;
    bool mAddLocationInfo;
    CxeImageDataItem::State mState;
};

#endif  // CXEFAKEIMAGEDATAITEM_H
