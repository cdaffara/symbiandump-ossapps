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

#ifndef NMCONVERTER_H_
#define NMCONVERTER_H_

#include <e32base.h>
#include "CFSMailCommon.h"

#include "nmbasedef.h"
#include "nmcommon.h"

#include "CFSMailAddress.h"
#include "nmaddress.h"

class NmConverter
{
public:
    IMPORT_C static NmMailSortCriteria FSMailSortCriteria2NM(TFSMailSortCriteria criteria);// const;
    IMPORT_C static TFSMailSortCriteria NMMailSortCriteria2FS(NmMailSortCriteria criteria);

    IMPORT_C static QString toQString(const TDesC& aText);
    IMPORT_C static QDateTime toQDateTime(const TTime& aTime);

    IMPORT_C static TTime toTTime(const QDateTime &aTime);
    
    IMPORT_C static NmId mailMsgIdToNmId(const TFSMailMsgId aId);
    IMPORT_C static TFSMailMsgId nmIdToMailMsgId(const NmId aId);
    IMPORT_C static HBufC* qstringToHBufCLC( const QString &string );
    
    IMPORT_C static RPointerArray<CFSMailAddress> toRPointerArray(
        const QList<NmAddress> &list);

};

#endif /* NMCONVERTER_H_ */
