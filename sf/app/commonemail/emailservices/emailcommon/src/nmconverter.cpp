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

#include "emailtrace.h"

#include "nmconverter.h"

/*!
    \brief It convert FSMailSortCriteria to NmMailSortCriteria
 */
EXPORT_C NmMailSortCriteria NmConverter::FSMailSortCriteria2NM(TFSMailSortCriteria criteria) 
{
    NM_FUNCTION;
    
    NmMailSortCriteria toReturn;

    // setting defaults
    toReturn.mField = NmMailDontCare;
    toReturn.mOrder = NmMailDescending;

    switch (criteria.iField) {
        case EFSMailDontCare:
            toReturn.mField = NmMailDontCare;
            break;
        case EFSMailSortByDate:
            toReturn.mField = NmMailSortByDate;
            break;
        case EFSMailSortBySender:
            toReturn.mField = NmMailSortBySender;
            break;
        case EFSMailSortByRecipient:
            toReturn.mField = NmMailSortByRecipient;
            break;
        case EFSMailSortBySubject:
            toReturn.mField = NmMailSortBySubject;
            break;
        case EFSMailSortByPriority:
            toReturn.mField = NmMailSortByPriority;
            break;
        case EFSMailSortByFlagStatus:
            toReturn.mField = NmMailSortByFlagStatus;
            break;
        case EFSMailSortByUnread:
            toReturn.mField = NmMailSortByUnread;
            break;
        case EFSMailSortBySize:
            toReturn.mField = NmMailSortBySize;
            break;
        case EFSMailSortByAttachment:
            toReturn.mField = NmMailSortByAttachment;
            break;
    }

    switch (criteria.iOrder) {
        case EFSMailDescending:
            toReturn.mOrder = NmMailDescending;
            break;
        case EFSMailAscending:
            toReturn.mOrder = NmMailAscending;
            break;
    }

    return toReturn;
}

/*!
    \brief It convert TDesC to QString
 */
EXPORT_C QString NmConverter::toQString(const TDesC& aText)
{
    NM_FUNCTION;
    
    return QString::fromUtf16(aText.Ptr(), aText.Length());
}

/*!
    \brief It convert TTime to QDateTime
 */
EXPORT_C QDateTime NmConverter::toQDateTime(const TTime& aTime)
{
    NM_FUNCTION;
    
    QDateTime toReturn;
    TDateTime sTime = aTime.DateTime();

    QDate qDate;
    qDate.setDate(sTime.Year(), static_cast<TInt> (sTime.Month()) + 1, sTime.Day() + 1);

    QTime qTime;
    qTime.setHMS(sTime.Hour(), sTime.Minute(), sTime.Second());

    toReturn.setDate(qDate);
    toReturn.setTime(qTime);

    return toReturn;
}

/*!
    \brief It convert QDateTime to TTime
 */
EXPORT_C TTime NmConverter::toTTime(const QDateTime &aTime)
{
    NM_FUNCTION;
    
    QDate qDate = aTime.date();
    QTime qTime = aTime.time();

    TDateTime sDateTime(qDate.year(), static_cast<TMonth> (qDate.month() - 1), qDate.day() - 1,
        qTime.hour(), qTime.minute(), qTime.second(), 0);

    return TTime(sDateTime);
}

/*!
    \brief It convert NmMailSortCriteria to TFSMailSortCriteria
 */
EXPORT_C TFSMailSortCriteria NmConverter::NMMailSortCriteria2FS(NmMailSortCriteria criteria)
{
    NM_FUNCTION;
    
    TFSMailSortCriteria toReturn;

    // setting defaults
    toReturn.iField = EFSMailDontCare;
    toReturn.iOrder = EFSMailDescending;

    switch (criteria.mField) {
        case NmMailDontCare:
            toReturn.iField = EFSMailDontCare;
            break;
        case NmMailSortByDate:
            toReturn.iField = EFSMailSortByDate;
            break;
        case NmMailSortBySender:
            toReturn.iField = EFSMailSortBySender;
            break;
        case NmMailSortByRecipient:
            toReturn.iField = EFSMailSortByRecipient;
            break;
        case NmMailSortBySubject:
            toReturn.iField = EFSMailSortBySubject;
            break;
        case NmMailSortByPriority:
            toReturn.iField = EFSMailSortByPriority;
            break;
        case NmMailSortByFlagStatus:
            toReturn.iField = EFSMailSortByFlagStatus;
            break;
        case NmMailSortByUnread:
            toReturn.iField = EFSMailSortByUnread;
            break;
        case NmMailSortBySize:
            toReturn.iField = EFSMailSortBySize;
            break;
        case NmMailSortByAttachment:
            toReturn.iField = EFSMailSortByAttachment;
            break;
    }

    switch (criteria.mOrder) {
        case NmMailDescending:
            toReturn.iOrder = EFSMailDescending;
            break;
        case NmMailAscending:
            toReturn.iOrder = EFSMailAscending;
            break;
    }

    return toReturn;
}

/*!
    converts TFSMailMsgId to NmId
 */
EXPORT_C NmId NmConverter::mailMsgIdToNmId(const TFSMailMsgId aId)
{
    NM_FUNCTION;
    
    NmId id;
    id.setId32( aId.Id() );
    id.setPluginId32( aId.PluginId().iUid );
    
    return id;  
}

/*!
    converts NmId to TFSMailMsgId
 */
EXPORT_C TFSMailMsgId NmConverter::nmIdToMailMsgId(const NmId aId)
{
    NM_FUNCTION;
    
    TFSMailMsgId id( aId.pluginId32(), aId.id32() );
    return id;
}

/*!
   converts QString to HBufC
 */
EXPORT_C HBufC* NmConverter::qstringToHBufCLC( const QString &string )
{
    NM_FUNCTION;
    
    HBufC* buf = HBufC::NewLC(string.length());    
    buf->Des().Copy( string.utf16() );
    return buf;
}

/*!
   Convert Qt NmAddress list into corresponding Symbian pointer array
 */
EXPORT_C RPointerArray<CFSMailAddress> NmConverter::toRPointerArray(
    const QList<NmAddress> &list)
{
    NM_FUNCTION;
    
    RPointerArray<CFSMailAddress> ret;

     for (QList<NmAddress>::const_iterator it = list.constBegin();
          it != list.constEnd();
          ++it) {

        CFSMailAddress *item = NULL;

        TRAP_IGNORE(item = CFSMailAddress::NewL(*it));

        if (item) {
            ret.Append(item);
        }
    }
    
    return ret;
}
