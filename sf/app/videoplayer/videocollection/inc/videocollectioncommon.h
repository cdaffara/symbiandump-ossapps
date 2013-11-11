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
* Description:  definition of common data for video collection components
*
*/

#ifndef __VIDEOCOLLECTIONCOMMON_H__
#define __VIDEOCOLLECTIONCOMMON_H__


#include <qobject.h>
#include <qhash.h>
#include <mpxitemid.h>

namespace VideoCollectionCommon
{
    /**
    * enums indicating keys for the detail data 
    * fetched from video model
    */
    enum VideoDetailKey
    {
        KeyUndefined = Qt::UserRole,
        KeyDateTime,
		KeySizeValue,
		KeyStatus,
		KeyFilePath,
        KeyLast,
        KeyNumberOfItems,
        KeyTitle
    };
    
    /**
    * Keys for the metadata map fetched from video model.
    * These are strings, because QVariant only supports QMap<QString, QVariant>
    */
    const char* const MetaKeyDate = "MetaKeyDate";
    const char* const MetaKeyDurationString = "MetaKeyDurationString";
    const char* const MetaKeySizeString = "MetaKeySizeString";
    const char* const MetaKeyStarRating = "MetaKeyStarRating";
    const char* const MetaKeyDRMInfo = "MetaKeyDRMInfo";
    const char* const MetaKeyServiceURL = "MetaKeyServiceURL";
    const char* const MetaKeyDescription = "MetaKeyDescription";
    const char* const MetaKeyModifiedDate = "MetaKeyModifiedDate";
    const char* const MetaKeyShotLocation = "MetaKeyShotLocation";
    const char* const MetaKeyAuthor = "MetaKeyAuthor";
    const char* const MetaKeyCopyright = "MetaKeyCopyright";
    const char* const MetaKeyAudioType = "MetaKeyAudioType";
    const char* const MetaKeyLanguageString = "MetaKeyLanguageString";
    const char* const MetaKeyKeywords = "MetaKeyKeywords";
    const char* const MetaKeyVideoResolutionString = "MetaKeyVideoResolutionString";
    const char* const MetaKeyBitRate = "MetaKeyBitRate";
    const char* const MetaKeyFormat = "MetaKeyFormat";
    const char* const MetaKeyFileName = "MetaKeyFileName";
    const char* const MetaKeyFilePath = "MetaKeyFilePath";
    const char* const MetaKeyVideoTitle = "MetaKeyVideoTitle";
    
    enum VideoItemStatus
    {
        StatusNone = 0,
        StatusDeleted
    };
    
    /**
     * Labels for the different details. Needs to be in the same
     * order than the VideoDetailLabelKeys.
     */
    const char * const VideoDetailLabels[] = {
            QT_TR_NOOP("txt_videos_list_title"),
            QT_TR_NOOP("txt_videos_list_file_name"),
            QT_TR_NOOP("txt_videos_list_file_path"),
    		QT_TR_NOOP("txt_videos_list_drm"),
            QT_TR_NOOP("txt_videos_list_service"),
            QT_TR_NOOP("txt_videos_list_description"),
            QT_TR_NOOP("txt_videos_list_duration"),
            QT_TR_NOOP("txt_videos_list_date"),
            QT_TR_NOOP("txt_videos_list_modified"),
            QT_TR_NOOP("txt_videos_list_location"),
            QT_TR_NOOP("txt_videos_list_author"),
            QT_TR_NOOP("txt_videos_list_copyright"),
            QT_TR_NOOP("txt_videos_list_audio_type"),
            QT_TR_NOOP("txt_videos_list_language"),
            QT_TR_NOOP("txt_videos_list_keywords"),
            QT_TR_NOOP("txt_videos_list_file_size"),
            QT_TR_NOOP("txt_videos_list_resolution"),
            QT_TR_NOOP("txt_videos_list_format"),
            QT_TR_NOOP("txt_videos_list_bitrate")            
};

    /**
     * Order of keys in the labels const array.
     */
    const char * const VideoDetailLabelKeys[] = {
            MetaKeyVideoTitle,
            MetaKeyFileName,
            MetaKeyFilePath,
            MetaKeyDRMInfo,
            MetaKeyServiceURL,
            MetaKeyDescription,
            MetaKeyDurationString,
            MetaKeyDate,
            MetaKeyModifiedDate,
            MetaKeyShotLocation,
            MetaKeyAuthor,
            MetaKeyCopyright,
            MetaKeyAudioType,
            MetaKeyLanguageString,
            MetaKeyKeywords,
            MetaKeySizeString,
            MetaKeyVideoResolutionString,
            MetaKeyFormat,
            MetaKeyBitRate            
    };
    
    /**
     * Collection level app browsed to
     */
    enum TCollectionLevels
    {
        ELevelInvalid 	  = -1,
    	ELevelCategory 	  = 2,
        ELevelVideos   	  = 3,
        ELevelDefaultColl = 4,
        ELevelAlbum    	  = 5
    };
    
    /**
     * Container path levels.
     */
    enum ContainerPathLevels
    {
        PathLevelCategories = 1,
        PathLevelVideos = 2
    };
    
    /**
     * Collection related asyncronous 
     * operations status codes
     */
    enum VideoCollectionAsyncStatus
    {
        statusSingleDeleteFail = 1,
        statusMultipleDeleteFail,
        statusDeleteInProgress,
        statusDeleteSucceed,
        statusSingleRemoveFail,
        statusMultiRemoveFail,
        statusRemoveSucceed,
        statusVideosAddedToCollection,
        statusAllVideosAlreadyInCollection,
        statusVideosRemovedFromCollection
    };

    /**
     * Utility macro to trace heap consumption.
     */
#ifdef _DEBUG
    #define TRACE_HEAP(prefix)\
    {\
        int totalAllocSpace = 0;\
        User::AllocSize( totalAllocSpace );\
        qDebug("[VIDEOCOLLECTION][HEAP]: %s, Total allocated space : %d",\
            prefix,\
            totalAllocSpace);\
    }
#else
    #define TRACE_HEAP(prefix)
#endif
}
#endif	// __VIDEOCOLLECTIONCOMMON_H__
