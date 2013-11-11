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

#include "cxestillimagedesktop.h"
#include "cxutils.h"

int CxeStillImageDesktop::mNextId = CxeStillImage::INVALID_ID + 1;

CxeStillImageDesktop::CxeStillImageDesktop()
  : mSnapshot(), 
    mDataItem(NULL), 
    mSaved(false),
    mId(++mNextId)
{
}

CxeStillImageDesktop::~CxeStillImageDesktop()
{
    mDataItem = NULL;
}

CxeImageDataItem *CxeStillImageDesktop::dataItem()
{
    return mDataItem;
}

QString CxeStillImageDesktop::filename() const
{
    return mFilename;
}

QPixmap CxeStillImageDesktop::snapshot() const
{
    return mSnapshot;
}

bool CxeStillImageDesktop::saved() const
{
    return mSaved;
}

int CxeStillImageDesktop::id() const
{
    return mId;
}

void CxeStillImageDesktop::setSnapshot(QPixmap pixmap)
    {
    mSnapshot = pixmap;
    }

void CxeStillImageDesktop::setFilename(const QString &filename)
    {
    mFilename = filename;
    }
void CxeStillImageDesktop::setSaved(bool saved)
    {
    mSaved = saved;
    }

void CxeStillImageDesktop::setDataItem(CxeImageDataItem *dataItem)
    {
    mDataItem = dataItem;
    }
