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
* Description:  Implementation of VideoPlaybackUserInputHandler
*
*/

// Version : %version:  6 %



#ifndef VIDEOPLAYBACKUSERINPUTHANDLER_H_
#define VIDEOPLAYBACKUSERINPUTHANDLER_H_

// INCLUDES


// FORWARD DECLARATIONS
class CMPXVideoViewWrapper;

// CLASS DECLARATION

class CVideoPlaybackUserInputHandler : public CBase
{
    public: // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        IMPORT_C static CVideoPlaybackUserInputHandler* NewL(
                CMPXVideoViewWrapper* aWrapper );

        /**
        * Destructor.
        */
        IMPORT_C virtual ~CVideoPlaybackUserInputHandler();

    private:

        /**
        * C++ default constructor.
        */
        CVideoPlaybackUserInputHandler( CMPXVideoViewWrapper* aWrapper );

        /**
        * Symbian 2nd phase constructor.
        */
        void ConstructL();

    public:
        void ProcessKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );

        void SetForeground( TBool aForeground );

        void HandleTVOutEventL(TBool aTVOutConnected);

    private:
        TBool                                   iTVOutConnected;        // Flag to indicate if TV is connected
        TBool                                   iForeground;
        CMPXVideoViewWrapper*                   iViewWrapper;
};

#endif /*VIDEOPLAYBACKUSERINPUTHANDLER_H_*/

// End of File
