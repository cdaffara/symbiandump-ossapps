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

#include "fbfileentry.h"

#include <HbExtendedLocale>

// ---------------------------------------------------------------------------

FbFileEntry::FbFileEntry(TFileEntry aFileEntry)
: mFileEntry(aFileEntry)
{
}

// ---------------------------------------------------------------------------

FbFileEntry::~FbFileEntry()
{
}

// ---------------------------------------------------------------------------

QString FbFileEntry::fullName()
{
    return QString::fromUtf16(mFileEntry.iFullName.Ptr(), mFileEntry.iFullName.Length());
}

// ---------------------------------------------------------------------------

QString FbFileEntry::path()
{
    return QString::fromUtf16(mFileEntry.iPath.Ptr(), mFileEntry.iPath.Length());
}

// ---------------------------------------------------------------------------

int FbFileEntry::attributes()
{
    return mFileEntry.iEntry.iAtt;
}

QString FbFileEntry::attributesString()
{
    QString attributesString;
    if (mFileEntry.iEntry.IsArchive()) {
        attributesString.append("A");
    } else {
        attributesString.append("-");
    }
    if (mFileEntry.iEntry.IsHidden()) {
        attributesString.append("H");
    } else {
        attributesString.append("-");
    }
    if (mFileEntry.iEntry.IsReadOnly()) {
        attributesString.append("R");
    } else {
        attributesString.append("-");
    }
    if (mFileEntry.iEntry.IsSystem()) {
        attributesString.append("S");
    } else {
        attributesString.append("-");
    }

    return attributesString;
}

// ---------------------------------------------------------------------------

int FbFileEntry::size()
{
    return mFileEntry.iEntry.iSize;
}

// ---------------------------------------------------------------------------

QString FbFileEntry::sizeString()
{
    QString sizeString;
    if (size() < 1024) {
        sizeString.append(QString::number(size()));
        sizeString.append(" B");
    } else if (size() < 1024*1024) {
        qreal sizeKB = static_cast<qreal>(size() / 1024);
        sizeString.append( QString::number(sizeKB, 'f',1));
        sizeString.append(" kB");
    } else {
        qreal sizeMB = static_cast<qreal>(size() / (1024*1024));
        sizeString.append( QString::number(sizeMB, 'f',1));
        sizeString.append(" MB");
    }
    return sizeString;
}

// ---------------------------------------------------------------------------

//QDateTime FbFileEntry::modified()
//{
//    QDateTime modified;
//    TTime time1Jan1970UTC = TTime(_L("19700101:000000.000000"));
//    TTimeIntervalSeconds intervalSeconds;
//
//    TTime entryModified = mFileEntry.iEntry.iModified;
//
//    TInt err = /*mFileEntry.iEntry.iModified*/ entryModified.SecondsFrom(time1Jan1970UTC, intervalSeconds);
//    if (err == KErrNone) {
//        quint32 secsSince1Jan1970UTC = intervalSeconds.Int();
//        modified = QDateTime::fromTime_t(secsSince1Jan1970UTC);
//        qDebug() << "QDateTime:: " << modified.toString();
//        modified.setTimeSpec(Qt::LocalTime);
//        qDebug() << "QDateTime:: " << modified.toString() << modified.toLocalTime().toString();
//    }
//    return modified;
//}

// ---------------------------------------------------------------------------

QString FbFileEntry::modifiedString()
{
    // TODO use HbExtendedLocale to convert date and time
//    QDateTime modifiedInLocalTime = modified();//.toLocalTime();
//    HbExtendedLocale locale = HbExtendedLocale::system();
//    QString date = locale.format(modifiedInLocalTime.date(), r_qtn_date_usual);
//    QString time = locale.format(modifiedInLocalTime.time(), r_qtn_time_usual);
//
//    return date + " " + time;
    QString modifiedString;
    TTime entryModified = mFileEntry.iEntry.iModified;
    RTz Tz;
    TInt err = Tz.Connect();
    if ( err == KErrNone && Tz.ConvertToLocalTime( entryModified ) == KErrNone )
    {
        entryModified = mFileEntry.iEntry.iModified;
        TBuf<128> extraData;
        _LIT(KCurrentDate,"%D%M%*Y%/0%1%/1%2%/2%3%/3 %-B%:0%J%:1%T%:2%S%:3%+B");
        entryModified.FormatL(extraData, KCurrentDate);
        modifiedString = QString::fromUtf16(extraData.Ptr(), extraData.Length());
    }
    Tz.Close();

    return modifiedString;
}

// ---------------------------------------------------------------------------

//int FbFileEntry::Uid(int id)
//{
//    return id;//mFileEntry.iEntry.iType[id];
//}

// ---------------------------------------------------------------------------

bool FbFileEntry::isDir()
{
    return mFileEntry.iEntry.IsDir();
}

// ---------------------------------------------------------------------------


QString FbFileEntry::name()
{
    return QString::fromUtf16(mFileEntry.iEntry.iName.Ptr(), mFileEntry.iEntry.iName.Length());
}

// ---------------------------------------------------------------------------

int FbFileEntry::dirEntries()
{
    return mFileEntry.iDirEntries;
}

// ---------------------------------------------------------------------------

QString FbFileEntry::dirEntriesString()
{
    QString dirEntriesString;
    dirEntriesString.append(QString::number(dirEntries()));
    if (dirEntries() == 1) {
        dirEntriesString.append(" entry");
    } else {
        dirEntriesString.append(" entries");
    }
    return dirEntriesString;
}

// ---------------------------------------------------------------------------

int FbFileEntry::iconId()
{
    return mFileEntry.iIconId;
}

// ---------------------------------------------------------------------------
