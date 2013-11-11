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

#include "fbdriveentry.h"

// ---------------------------------------------------------------------------

FbDriveEntry::FbDriveEntry(TDriveEntry aDriveEntry)
: mDriveEntry(aDriveEntry)
{
}

// ---------------------------------------------------------------------------

FbDriveEntry::~FbDriveEntry()
{
}

// ---------------------------------------------------------------------------

QChar FbDriveEntry::driveLetter()
{
    return QChar(TUint(mDriveEntry.iLetter));
}

// ---------------------------------------------------------------------------

int FbDriveEntry::number()
{
    return mDriveEntry.iNumber;
}

// ---------------------------------------------------------------------------

QString FbDriveEntry::mediaTypeString()
{
    return QString::fromUtf16(mDriveEntry.iMediaTypeDesc.Ptr(), mDriveEntry.iMediaTypeDesc.Length());
}

// ---------------------------------------------------------------------------

QString FbDriveEntry::attributesString()
{
    return QString::fromUtf16(mDriveEntry.iAttributesDesc.Ptr(), mDriveEntry.iAttributesDesc.Length());
}

// ---------------------------------------------------------------------------

int FbDriveEntry::iconId()
{
    return mDriveEntry.iIconId;
}

// ---------------------------------------------------------------------------

qint64 FbDriveEntry::volumeInfoFree()
{
    return mDriveEntry.iVolumeInfo.iFree;
}

// ---------------------------------------------------------------------------

qint64 FbDriveEntry::volumeInfoSize()
{
    return mDriveEntry.iVolumeInfo.iSize;
}

// ---------------------------------------------------------------------------
