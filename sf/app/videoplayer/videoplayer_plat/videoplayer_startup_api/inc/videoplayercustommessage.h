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
* Description:   Defines custom message for videoplayer application.
*
*
*/



#ifndef VIDEOPLAYERCUSTOMMESSAGE_H
#define VIDEOPLAYERCUSTOMMESSAGE_H

// INCLUDES

#include <e32base.h>

// CONSTANTS

/**
 * Minimum length of this object in bytes, can be used to check length of 
 * parameter string that is passed to applications and should be this object.
 */
const TInt KVideoPlayerCustomMessageMinLength = 20;

/**
 * Max length of name.
 */
const TInt KVideoPlayerMsgMaxName = 255;   

/**
 * Max length of path.
 */
const TInt KVideoPlayerMaxPath( 2048 );

// CLASS DECLARATION

/**
 * Data class for VideoPlayer custom message.
 */
class TVideoPlayerCustomMessage
    {
    public: // Data types.

        /**        
         * Video name
         */
        TBuf<KVideoPlayerMsgMaxName>    iName;
           
        /** 
         * Path to the thumbnail or icon for the video 
         */
        TBuf<KMaxPath>                  iIcon;
  
        /** 
         * Where the video content is located. URL or filename ( including path ) 
         */
        TBuf<KVideoPlayerMaxPath>       iContent;
        
    };

#endif // VIDEOPLAYERCUSTOMMESSAGE_H
