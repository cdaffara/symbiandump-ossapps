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
* Description: CVideoCollectionUtils class implementation
* 
*/

// INCLUDE FILES


#include <qobject.h>
#include "videocollectionutils.h"
#include <QTime>
#include <hbextendedlocale.h>

QString VideoCollectionUtils::mPrepareLengthStringReturnValue = "test duration";
QString VideoCollectionUtils::mPrepareSizeStringReturnValue = "test size";

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
// CVideoCollectionUtils
// -----------------------------------------------------------------------------
//
VideoCollectionUtils::VideoCollectionUtils()
{
    
}

// -----------------------------------------------------------------------------
// ~CVideoCollectionUtils
// -----------------------------------------------------------------------------
//
VideoCollectionUtils::~VideoCollectionUtils()
{

}

// -----------------------------------------------------------------------------
// prepareLengthString
// -----------------------------------------------------------------------------
//
QString VideoCollectionUtils::prepareLengthString(quint32 length)
{
    const int secondsInMinute( 60 );
    const int secondsInHour( 3600 );

	quint32 hours = length / secondsInHour;
	quint32 minutes = length / secondsInMinute % secondsInMinute;
	quint32 seconds = length % secondsInMinute;

    QString hrs(QString::number(hours));
    if(hours < 10)
    {
    	hrs.prepend(QString::number(0));
    }
    QString mins(QString::number(minutes));
    if(minutes < 10)
    {
    	mins.prepend(QString::number(0));
    }
    QString secs(QString::number(seconds));
    if(seconds < 10)
    {
    	secs.prepend(QString::number(0));
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
    const int videoSizeGB( 0x40000000 );
    const int videoSizeHalfGB( 0x20000000 );
    const int videoSizeMB( 0x100000 );
    const int videoSizeHalfMB( 0x80000 );
    const int videoSizeKB( 0x400 );
    const int videoSizeHalfKB( 0x200 );
    
    QString sizeStr("");
    
    if ( size > 0 )
    {
        quint32 dispSize = 0;
        
        if ( size >= videoSizeGB )
        {
            dispSize  = size + videoSizeHalfGB;
            dispSize /= videoSizeGB;
            sizeStr = QString(QObject::tr("%1 GB").arg(QString::number(dispSize)));
        }
        else if ( size >= videoSizeMB )
        {
            dispSize  = size + videoSizeHalfMB;
            dispSize /= videoSizeMB;
            sizeStr = QString(QObject::tr("%1 MB").arg(QString::number(dispSize)));
        }
        else if (size >= videoSizeKB) 
        {
            dispSize  = size + videoSizeHalfKB;
            dispSize /= videoSizeKB;
            sizeStr = QString(QObject::tr("%1 kB").arg(QString::number(dispSize)));
        }
        else
        {
            sizeStr = QString(QObject::tr("%1B").arg(QString::number(size)));
        }
    }
    
    return sizeStr;
}

// End of file
