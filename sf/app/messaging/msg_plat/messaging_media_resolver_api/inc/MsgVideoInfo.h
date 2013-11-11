/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
*           Messaging Video Info class.
*
*/



#ifndef __MSGVIDEOINFO_H
#define __MSGVIDEOINFO_H

// INCLUDES

#include <e32std.h>
#include <w32std.h>

// VideoPlayerUtility API
#include <videoplayer.h>

// Base Class
#include <MsgMediaInfo.h>

// CONSTANTS

// MACROS

// FORWARD DECLARATIONS

class RFile;
class TDataType;
class CHXMetaDataUtility;

// DATA TYPES

// FUNCTION PROTOTYPES

// CLASS DECLARATION

/**
* Messaging Video Info class.
*
* @lib MsgMedia.lib
* @since 3.1
*/
NONSHARABLE_CLASS( CMsgVideoInfo ) : public CMsgMediaInfo
    {

    public:  // New methods

        /**
        * Factory method that creates this object.
        *
        * @param    aFile       IN File handle to create info class from.
        * @param    aMimeType   IN Mime type of the file.
        * @return   pointer to instance
        */
        IMPORT_C static CMsgVideoInfo* NewL(
            RFile& aFile, 
            TDataType& aMimeType );

        /**
        * Destructor
        */
        virtual ~CMsgVideoInfo();

        /**
        * Get Framerate.
        *
        * @return   Framerate of Video component.
        */
        inline TReal32 VideoFrameRate() const;
    
        /**
        * Get Bitrate.
        *
        * @return   Bitrate of Video component.
        */
        inline TInt VideoBitRate() const;
    
        /**
        * Get Bitrate.
        *
        * @return   Bitrate of Audio component.
        */
        inline TInt AudioBitRate() const;
    
        /**
        * Get dimensions.
        *
        * @return   Dimensions of unscaled image.
        */
        inline TSize Dimensions() const;
    
        /**
        * Is Audio.
        *
        * @return   Does video have audio clip included.
        */
        inline TBool IsAudio() const;
    
    public: // From base classes

        /**
        * From CActive
        */
    	void DoCancel();
        
        /**
        * From CActive
        */
    	void RunL();

    
    protected:

        /**
        * Constructor.
        *
        * @param    aMimeType   IN Mime type of the file.
        */
        CMsgVideoInfo( TDataType& aMimeType );

        /**
        * 2nd phase constructor.
        *
        * @param    aFile       IN File handle to create info class from.
        */
        void ConstructL( RFile& aFile );

        /**
        * Opens the audio file and gets info (e.g. duration and dimensions).
        * Called from ConstructL.
        *
        * @param    aFile       IN File handle to create info class from.
        */
        void ResolveVideoInfoL( RFile& aFile );

        /**
        * ReadInformationL
        * Reads information from CVideoPlayerUtility object into
        * member variables.
        *
        * @param    aVideoPlayer   IN VideoPlayerUtility object to 
        *                             read information from.
        */
        void ReadInformationL( CHXMetaDataUtility& dataUtil );

    protected: // data

        enum TMsgVideoState
            {
            EVideoStateIdle = 0,
            EVideoStateReady
            };

        TInt                    iState;

        TInt                    iBitRate; 
        TInt                    iAudioBitRate;
        
        TSize                   iOverallSizeInPixels;
        TReal32                 iFramesPerSecond;
        TDataType               iAudioType;
        TUid                    iControllerUid;

    };

#include <MsgVideoInfo.inl>

#endif // __MSGVIDEOINFO_H
