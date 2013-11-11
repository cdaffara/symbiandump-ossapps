/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This class plays files with the Helix Engine
 *
*/

// Version : %version: 10 %



#ifndef _CMPXVIDEOHELIXPLAYBACK_H_
#define _CMPXVIDEOHELIXPLAYBACK_H_

//
//  INCLUDES
//
#include <mpxplaybackplugin.h>
#include <mmf/common/mmfcontroller.h>
#include <mpxplaybackpluginversion2.h>
#include <mpxplaybackpluginobserver.h>

#include "mpxvideo_debug.h"
#include "mpxhelixplaybackplugindefs.h"
#include "mpxvideoplaybackcontroller.h"

//
//  FORWARD DECLARATIONS
//
class CMPXVideoPlaybackController;

//
//  CLASS DECLARATION
//

/*
 *  CMpxVideoHelixPlayback class
 *
 *  Plug-in basically handles Video Playback with the Helix Engine
 *  @lib mpxvideohelixplayback.lib
 *
 */

NONSHARABLE_CLASS( CMPXVideoHelixPlayback ) : public CMPXPlaybackPluginVersion2
{
    public:
        //
        //  Constructors and destructor
        //

        /*
         *  Two-phased constructor.
         *  @param aInitParams, initialization parameter
         *  @return a pointer to the created instance
         */
        static CMPXVideoHelixPlayback* NewL( TAny* aInitParams );

        /*
         *  Destructor
         *  Destroy the object and release all memory objects
         */
        ~CMPXVideoHelixPlayback();

    private:
        //
        //  CMPXPlaybackPlugin Implementation
        //

        /*
         *  Set observer
         *
         *  @param aObs observer
         */
        inline void SetObserver( MMPXPlaybackPluginObserver& aObs );

        /*
         *  Initializes a clip for playback
         *  @param aSong the song path
         */
        void InitialiseL( const TDesC& aSong );

        /*
         *  Initializes a song for playback
         *  @param aFile file handle of a song
         */
        void InitialiseL( RFile& aFile );

        /*
         *  Executes a command on the selected song
         * @param aCmd a command
         * @param aData, data
         */
        void CommandL( CMPXCommand& aCmd );

        // Supposedly being deprecated
        // We're forced to implement this as its declared as a
        // pure virtual function by the MPX framework
        void CommandL(TMPXPlaybackCommand aCmd, TInt aData=0);

        /*
         *  Sets a property of the plugin
         *  @param aProperty a property
         *  @param aValue the value of the setting
         */
        void SetL( TMPXPlaybackProperty aProperty , TInt aValue );

        /*
         *  Gets a property of the plugin (async)
         *  @param aProperty a property
         */
        void PropertyL( TMPXPlaybackProperty aProperty ) const;

        /*
         *  Gets a list of sub players
         *  @return a list of names of sub players
         */
        inline void SubPlayerNamesL();

        /*
         *  Select a sub player
         *  @param aIndex index to the sub player
         */
        void SelectSubPlayerL( TInt aIndex );

        /*
         *  Returns current sub player name
         *  @return friendly name of the current the sub player
         */
        const TDesC& SubPlayerName();

        /*
         *  Current sub player index
         *  @return index to the sub player
         */
        inline TInt SubPlayerIndex() const;

        /*
         *  Media properties of the current file (async)
         *  @param aAttrs attributes requested
         */
        void MediaL( const TArray<TMPXAttribute>& aAttrs );

        /*
         * Cancel async request
         */
        void CancelRequest();

        /**
        * Initializes a file for playback.
        *
        * @since S60 9.2
        * @param aUri URI of the item
        * @param aType the mime type of the item
        * @param aAccessPoint the access point
        * @param aPosition the starting position
        */
        void InitStreamingL(const TDesC& aUri, const TDesC8& aType, TInt aAccessPoint, TInt aPosition); 

        /**
        * Initializes a file handle for playback.
        *
        * @since S60 9.2
        * @param aFile file handle of a file
        * @param aAccessPoint the access point
        * @param aPosition the starting position
        */
        void InitStreamingL(RFile& aFile, TInt aAccessPoint, TInt aPosition);  

        /**
        * Initializes filename and handle
        *
        * @since S60 9.2
        * @param aUri URI of the item
        */
        RFile& OpenFileHandleL( const TDesC& aUri );

        /**
        * Open file handle
        *
        * @since S60 9.2
        * @param aUri URI of the item
        * @param aFile a 32-bit file handle
        */
        void OpenFileHandleL( const TDesC& aUri, RFile& aFile );

        /**
        *  Checks if the Uri is a streaming link
        *
        * @since S60 9.2
        * @param aUri URL or file name
        */
        void CheckForStreamingUrlL( const TDesC& aUri );

#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API

        /*
         *  Initializes a song for playback
         *  @param aFile RFile64 of a song
         *  @param aPosition the starting position
         */
        void Initialise64L( RFile64& aFile, TInt aPosition ); 

        /**
        * Initializes a file handle for playback.
        *
        * @since S60 9.2
        * @param aFile RFile64 of a file
        * @param aAccessPoint the access point
        * @param aPosition the starting position
        */
        void InitStreaming64L( RFile64& aFile, TInt aAccessPoint, TInt aPosition ); 

        /**
        * Initializes filename and handle
        *
        * @since S60 9.2
        * @param aUri URI of the item
        */
        RFile64& OpenFileHandle64L( const TDesC& aUri );

        /**
        * Open file handle
        *
        * @since S60 9.2
        * @param aUri URI of the item
        * @param aFile a 64-bit file handle
        */
        void OpenFileHandle64L( const TDesC& aUri, RFile64& aFile );

#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API


        /*
        *  Initializes a clip for playback
        *  @since S60 9.2
        *  
        *  @param aSong the song path, 
        *  @param aPosition the starting position 
        */
        void InitialiseWithPositionL( const TDesC& aSong, TInt aPosition = 0 );     
                
        /**
        * Initializes a song for playback.
        *
        * @since S60 9.2
        * @param aFile file handle of a song
        * @param aPosition the starting position
        */
        void InitialiseWithPositionL(RFile& aSong, TInt aPosition = 0 );        


    private:
        /*
         *  C++ default constructor
         */
        CMPXVideoHelixPlayback();

        /*
         *  the second phase constructor ConstructL to safely construct things
         *  that can leave
         */
        void ConstructL();

    private:
        //
        //  Data
        //
    	CMPXVideoPlaybackController*  iVideoPlaybackCtlr;
        RFs                           iFs;
};

// INLINE METHODS

inline
void CMPXVideoHelixPlayback::SetObserver( MMPXPlaybackPluginObserver& aObs )
{
    MPX_ENTER_EXIT(_L("CMPXVideoHelixPlayback::SetObserver()"));
    iObs = &aObs;
    iVideoPlaybackCtlr->SetMPXObserver(aObs);
}

inline
TInt CMPXVideoHelixPlayback::SubPlayerIndex() const
{
   MPX_ENTER_EXIT(_L("CMPXVideoHelixPlayback::SubPlayerIndex()"));
   return KErrNotFound;
}

inline
const TDesC& CMPXVideoHelixPlayback::SubPlayerName()
{
    MPX_ENTER_EXIT(_L("CMPXVideoHelixPlayback::SubPlayerName()"));
    return KNullDesC;
}

#endif
