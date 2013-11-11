/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Handler container
*
*/



#ifndef CVTENGHANDLERCONTAINER_H
#define CVTENGHANDLERCONTAINER_H

//  INCLUDES
#include    <e32base.h>
#include    "CVtEngMediaHandler.h"
#include    "CVtEngSessionHandler.h"
#include    "CVtEngAudioHandler.h"
#include    "CVtEngDtmfHandler.h"

// FORWARD DECLARATIONS
class MVtEngSessionInfo;
class MVtEngMedia;
class MVtEngAudio;

// CLASS DECLARATION

/**
*  Container for media, audio and session handlers.
*
*  @lib videoteleng
*  @since Series60 2.6
*/
NONSHARABLE_CLASS( CVtEngHandlerContainer ) : public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CVtEngHandlerContainer* NewL(  );
        
        /**
        * Destructor.
        */
        virtual ~CVtEngHandlerContainer();

    public: // New functions
            

        /**
        * Returns session handler.
        * @return session handler
        */
        inline MVtEngSessionInfo& Session() const;

        /**
        * Returns media handler.
        * @return media handler
        */
        inline MVtEngMedia& Media() const;

        /**
        * Returns audio handler.
        * @return audio handler
        */
        inline MVtEngAudio& Audio() const;


        /**
        * Returns DTMF handler.
        * @return DTMF handler
        */
        inline CVtEngDtmfHandler& Dtmf() const;

        /*
        * Uninitializes audio and session handlers.
        * Media handler uninitialization is done
        * asynchronously using directly the handler.
        */
        void Uninitialize();

        /**
        * Creates DTMF handler
        * @param aH324Config H324 configure interface
        */
        void CreateDtmfHandlerL( MVtH324ConfigCommand* aH324Config );

    private:

        /**
        * C++ default constructor.
        */
        CVtEngHandlerContainer();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private:    // member data

        // media handler
        CVtEngMediaHandler* iMedia;

        // session handler
        CVtEngSessionHandler* iSession;

        // audio handler
        CVtEngAudioHandler* iAudio;


        // DTMF handler
        CVtEngDtmfHandler*   iDtmf;

    };

#include "CVtEngHandlerContainer.inl"

#endif      // CVTENGHANDLERCONTAINER_H
            
// End of File
