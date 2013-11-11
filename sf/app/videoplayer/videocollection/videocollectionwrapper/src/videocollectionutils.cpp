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
* Description: VideoCollectionUtils class implementation
* 
*/

// Version : %version: 16 %

// INCLUDE FILES
#include <qobject.h>
#include <QTime>
#include <HbStringUtil>
#include <hbglobal.h>
#include <hbextendedlocale.h>

#include "videocollectionutils.h"
#include "videocollectiontrace.h"

// -----------------------------------------------------------------------------
// instance
// -----------------------------------------------------------------------------
//
VideoCollectionUtils& VideoCollectionUtils::instance()
{
    static VideoCollectionUtils _collectionUtilsInstance;
    return _collectionUtilsInstance;
}
	
// -----------------------------------------------------------------------------
// VideoCollectionUtils
// -----------------------------------------------------------------------------
//
VideoCollectionUtils::VideoCollectionUtils()
{
	FUNC_LOG;
}

// -----------------------------------------------------------------------------
// ~CVideoCollectionUtils
// -----------------------------------------------------------------------------
//
VideoCollectionUtils::~VideoCollectionUtils()
{
	FUNC_LOG;
}

// -----------------------------------------------------------------------------
// prepareLengthString
// -----------------------------------------------------------------------------
//
QString VideoCollectionUtils::prepareLengthString(quint32 length)
{
	FUNC_LOG;
    const int secondsInMinute( 60 );
    const int secondsInHour( 3600 );
    
	quint32 hours = length / secondsInHour;
	quint32 minutes = length / secondsInMinute % secondsInMinute;
	quint32 seconds = length % secondsInMinute;
	
    QString hrs(HbStringUtil::convertDigits(QString::number(hours)));
    if(hours < 10)
    {
    	hrs.prepend(HbStringUtil::convertDigits(QString::number(0)));
    }
    QString mins(HbStringUtil::convertDigits(QString::number(minutes)));
    if(minutes < 10)
    {
    	mins.prepend(HbStringUtil::convertDigits(QString::number(0)));
    }
    QString secs(HbStringUtil::convertDigits(QString::number(seconds)));
    if(seconds < 10)
    {
    	secs.prepend(HbStringUtil::convertDigits(QString::number(0)));
    }
    
    QString lengthStr(hbTrId( "txt_videos_list_l1l2l3" ).arg( hrs ).arg( mins ).arg( secs ));
    
    return lengthStr.trimmed();
}

// -----------------------------------------------------------------------------
// prepareSizeString
// -----------------------------------------------------------------------------
//
QString VideoCollectionUtils::prepareSizeString(quint32 size)
{
	FUNC_LOG;
    const int videoSizeGB( 0x40000000 );
    const int videoSizeHalfGB( 0x20000000 );
    const int videoSizeMB( 0x100000 );
    const int videoSizeHalfMB( 0x80000 );
    const int videoSizeKB( 0x400 );

    QString sizeStr("");
    
    if ( size > 0 )
    {
        quint32 dispSize = 0;
     
        HbExtendedLocale locale = HbExtendedLocale::system();
        
        if ( size >= videoSizeGB )
        {
            dispSize  = size + videoSizeHalfGB;
            dispSize /= videoSizeGB;
            sizeStr = QString(hbTrId("txt_videos_list_l1_gb").arg(locale.toString(dispSize)));
        }
        else if ( size >= videoSizeMB )
        {
            dispSize  = size + videoSizeHalfMB;
            dispSize /= videoSizeMB;
            sizeStr = QString(hbTrId("txt_videos_list_l1_mb").arg(locale.toString(dispSize)));
        }
        else
        {
            dispSize  = size + videoSizeKB;
            dispSize /= videoSizeKB;
            sizeStr = QString(hbTrId("txt_videos_list_l1_kb").arg(locale.toString(dispSize)));
        }
    }

    return sizeStr;
}

// End of file
