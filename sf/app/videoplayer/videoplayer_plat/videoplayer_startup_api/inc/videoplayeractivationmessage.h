/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:   Defines activation message for videoplayer application.

*
*/



#ifndef VIDEOPLAYERACTIVATIONMESSAGE_H
#define VIDEOPLAYERACTIVATIONMESSAGE_H

// INCLUDES

#include <e32base.h>

// CONSTANTS

/**
 * Minimum length of this object in bytes, can be used to check length of 
 * parameter string that is passed to applications and should be this object.
 */
const TInt KVideoPlayerActivationMessageMinLength = 20;

/**
 * Max length of path.
 */
const TInt KVideoPlayerMsgMaxPath = 255;   

/**
 * Custom Uid when sending activation message.
 */
const TInt KVideoPlayerVodStartPlayer = 10500; 

/**
 * Custom Uid when sending activation message to make progressive download.
 */
const TInt KVideoPlayerStartPDlPlayer = 10501; 

// CLASS DECLARATION

/**
 * Data class for VideoPlayer activation message.
 *
 * @lib IptvUtil.dll
 * @since R4
 */
class TVideoPlayerActivationMessage
    {
    public: // Data types.

        /**
         * Message types.
         */ 
        enum TMessageType
            {
            ENoType = 0,
            ELaunchVideoToPlayer,
            ELaunchLastWatchedVideo,
            EOpenVideoStorage,
            EOpenInternetVideos,
            ELaunchServiceById,
            EOpenServiceCatalogue,
            EOpenVideoShop,
            ELiveTVMessage,
            EOpenRecordingsCategory,
            EOpenVCSettings,
            EOpenVCSettingsFeedSubscriptions,
            EOpenVCSettingsAddFeed,
            ELiveTVMessageLastWatched,
            EOpenVCSettingsLiveTVSubscriptions,
            EOpenVCSettingsEditFeed
            };

        /**
         * Message sender.
         */
        enum TMessageSender
            {
            ENoSender = 0,
            ENotification,
            EActiveSpace,
            EIptvMainLastWatched,
            EMatrixMenu
            };       

    public: // Common data members

        /**        
         * TMessageType
         */
        TMessageType                         iMsgType;
        
        /**        
         * TMessageSender
         */        
        TMessageSender                       iMsgSender;
        
        /**        
         * Service Id
         */        
        TUint32                              iServiceId;
        
        /**        
         * Group Id
         */        
        TUint32                              iGroupId;

#if !defined(__SERIES60_30__) && !defined(__SERIES60_31__) && !defined(__SERIES60_32__)
        /**        
         * Channel Id
         */        
        TInt64                               iChannelId;
#endif

        /**
         * Application Uid of message sender.
         */
        TUid                                 iSenderUid;
        
    public: // Vod data members.

        /**
         * Deprecated. Do not use.
         */
        TUid                                 iUid;

        /**
         * Deprecated. Do not use.
         */
        TInt32                               iActive;

        /**
         * Deprecated. Do not use.
         */
        TBool                                iSingle;

        /**
         * File Id (Global file Id 1/2).
         */
        TUint32                              iFileId;

        /**        
         * Drive Id, see TDriveNumber (Global file Id 2/2).
         */
        TInt                                 iDrive;  

        /**        
         * Full path to video.
         */
        TBuf<KVideoPlayerMsgMaxPath>                iFullPath;
    };

#endif // XVIDEOPLAYERACTIVATIONMESSAGE_H
