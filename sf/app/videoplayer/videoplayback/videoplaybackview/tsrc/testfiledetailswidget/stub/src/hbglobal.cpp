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
* Description:  class for HbGlobal
*
*/

// Version : %version:   5 %



#include "hbglobal.h"
#include <qstring>


QString hbTrId( QString string, int n )
{
    Q_UNUSED( n );

    QString loc = "";

    if ( string == "txt_videos_list_description" )
    {
        loc = "Description";
    }
    else if ( string == "txt_videos_list_duration" )
    {
        loc = "Duration";
    }
    else if ( string == "txt_videos_list_date" )
    {
        loc = "Date";
    }
    else if ( string == "txt_videos_list_modified" )
    {
        loc = "Modified";
    }
    else if ( string == "txt_videos_list_location" )
    {
        loc = "Location";
    }
    else if ( string == "txt_videos_list_author" )
    {
        loc = "Author";
    }
    else if ( string == "txt_videos_list_copyright" )
    {
        loc = "Copyright";
    }
    else if ( string == "txt_videos_list_language" )
    {
        loc = "Language";
    }
    else if ( string == "txt_videos_list_keywords" )
    {
        loc = "Keywords";
    }
    else if ( string == "txt_videos_list_resolution" )
    {
        loc = "Resolution";
    }
    else if ( string == "txt_videos_list_format" )
    {
        loc = "Format";
    }
    else if ( string == "txt_videos_list_collection_name" )
    {
        loc = "Collection";
    }
    else if ( string == "txt_videos_list_title" )
    {
        loc = "Title";
    }
    else if ( string == "txt_videos_list_file_size" )
    {
        loc = "Size";
    }
    else if ( string == "txt_videos_list_bitrate" )
    {
        loc = "Bitrate";
    }
    else if ( string == "txt_videos_list_l1l2l3" )
    {
        loc = "%L1:%L2:%L3";
    }
    else if ( string == "txt_videos_list_l1_kb" )
    {
        loc = "%L1 kB";
    }
    else if ( string == "txt_videos_list_l1_mb" )
    {
        loc = "%L1 MB";
    }
    else if ( string == "txt_videos_list_l1_gb" )
    {
        loc = "%L1 GB";
    }
    else if ( string == "txt_videos_list_l1_kbps" )
    {
        loc = "%L1 kbps";
    }
    else if ( string == "txt_videos_list_l1_mbps" )
    {
        loc = "%L1 Mbps";
    }
    else if ( string == "txt_videos_list_l1l2" )
    {
        loc = "%L1x%L2";
    }
    else if ( string == "txt_videos_list_file_path" )
    {
        loc = "File path";
    }
    else if ( string == "txt_videos_list_file_name" )
    {
        loc = "File name";
    }

    return loc;
}

//End of file
