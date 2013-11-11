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

#include <QByteArray>
#include <QString>

#include "cxefakeimagedataitem.h"
#include "cxeimagedataitem.h"
#include "cxutils.h"

CxeFakeImageDataItem::CxeFakeImageDataItem(int index, QByteArray data, QString filename, int id, bool addLocation)
    : mId(id),
      mIndex(index),
      mData(data),
      mFileName(filename),
      mAddLocationInfo(addLocation),
      mState(CxeImageDataItem::SavePending)
{
    CX_DEBUG_IN_FUNCTION();
}

CxeFakeImageDataItem::~CxeFakeImageDataItem()
{
    CX_DEBUG_IN_FUNCTION();
}

CxeError::Id CxeFakeImageDataItem::save()
{
    CX_DEBUG_ENTER_FUNCTION();
    mState = CxeImageDataItem::Saved;
    emit imageSaved(CxeError::None, mFileName, mId);
    CX_DEBUG_EXIT_FUNCTION();
    return CxeError::None;
}


CxeImageDataItem::State CxeFakeImageDataItem::state() const
{
    CX_DEBUG_ENTER_FUNCTION();
    CX_DEBUG_EXIT_FUNCTION();
    return mState;
}

int CxeFakeImageDataItem::id() const
{
    return mId;
}

QString CxeFakeImageDataItem::path() const
{
    return mFileName;
}


bool CxeFakeImageDataItem::addLocation() const
{
    return mAddLocationInfo;
}

// end of file
