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
*           Messaging Audio info class
*
*/



#ifndef __MSGAUDIOINFO_H
#define __MSGAUDIOINFO_H

// INCLUDES

#include <e32std.h>

#include <mdaaudiosampleplayer.h> // MMdaAudioPlayerCallback & CMdaAudioPlayerUtility
#include <MsgMediaInfo.h>

// CONSTANTS

// MACROS

// FORWARD DECLARATIONS

class RFile;
class TDataType;

// DATA TYPES

// FUNCTION PROTOTYPES

// CLASS DECLARATION

/**
* Messaging Audio Info class.
*
* @lib MsgMedia.lib
* @since 3.1
*/
NONSHARABLE_CLASS( CMsgAudioInfo ) : public CMsgMediaInfo,
                                     public MMdaAudioPlayerCallback
    {

    public:  // New methods

        /**
        * Factory method that creates this object.
        *
        * @param    aFile       IN File handle to create info class from.
        * @param    aMimeType   IN Mime type of the file.
        * @return   pointer to instance
        */
        IMPORT_C static CMsgAudioInfo* NewL(
            RFile& aFile, 
            TDataType& aMimeType );

        /**
        * Destructor
        */
        virtual ~CMsgAudioInfo();

    protected:

        /**
        * Constructor.
        *
        * @param    aMimeType   IN Mime type of the file.
        */
        CMsgAudioInfo( TDataType& aMimeType );

        /**
        * 2nd phase constructor.
        *
        * @param    aFile       IN File handle to create info class from.
        */
        void ConstructL( RFile& aFile );

        /**
        * Opens the audio file and gets info (e.g. duration).
        * Called from ConstructL.
        *
        * @param    aFile       IN File handle to create info class from.
        */
        void ResolveAudioInfoL( RFile& aFile );

    protected: // From base classes

        /**
        * From CActive
        */
    	void DoCancel();
        
        /**
        * From CActive
        */
    	void RunL();

        /**
        * From MMdaAudioPlayerCallback
        */
        virtual void MapcInitComplete(
            TInt aError, 
            const TTimeIntervalMicroSeconds& aDuration );

        /**
        * From MMdaAudioPlayerCallback
        */
        virtual void MapcPlayComplete( TInt aError );


    protected: // data

        CMdaAudioPlayerUtility* iAudioUtil;
        enum TMsgAudioState
            {
            EAudioStateIdle = 0,
            EAudioStateReady
            };
        TInt iState;
    };

#endif // __MSGAUDIOINFO_H
