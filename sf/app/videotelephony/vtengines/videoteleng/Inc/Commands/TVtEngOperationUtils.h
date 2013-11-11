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
* Description:  Handler utili
*                
*
*/



#ifndef TVTENGOPERATIONUTILS
#define TVTENGOPERATIONUTILS

//  INCLUDES
#include <e32std.h>
#include "vtengcommands.h"


// FORWARD DECLARATIONS
class CVtEngHandlerContainer;
class CVtEngOperation;
// CLASS DECLARATION

/**
*  Handles rendering operation
* 
*  @lib videoteleng
*  @since 2.6
*/
class TVtEngRenderUtil
    {
    public:
        
        static void HandleL( 
            CVtEngOperation& aOperation,
            CVtEngHandlerContainer& aHandlers );
        
        static void Cancel(             
            CVtEngOperation& aOperation,
            CVtEngHandlerContainer& aHandlers );        
    };

/**
*  Handles audio routing operation.
*
*  @lib videoteleng
*  @since 2.6
*/
class TVtEngAudioRoutingUtil
    {
    public:
        
        /**
        * 
        * @param aOperation operation
        */
        static void HandleL( 
            CVtEngOperation& aOperation );
    };


/**
*  Handles audio playback operation.
*
*  @lib videoteleng
*  @since 2.6
*/
class TVtEngAudioPlaybackUtil
    {
    public:
        
        static void HandleL( 
            CVtEngOperation& aOperation,
            CVtEngHandlerContainer& aHandlers );
    };

/**
*  Handles media transport operation like pausing/resuming.
*
*  @lib videoteleng
*  @since 2.6
*/
class TVtEngMediaTransportUtil
    {
    public:
        
        static void HandleL( 
            CVtEngOperation& aOperation,
            CVtEngHandlerContainer& aHandlers );
    };

/**
*  
*
*  @lib videoteleng
*  @since 2.6
*/
class TVtEngViewFinderConfigureUtil
    {
    public:
        
        static void HandleL( 
            CVtEngOperation& aOperation,
            CVtEngHandlerContainer& aHandlers );
    };

/**
*  
*
*  @lib videoteleng
*  @since 2.6
*/
class TVtEngCameraConfigUtil
    {
    public:
        
        static void HandleL( 
            CVtEngOperation& aOperation,
            CVtEngHandlerContainer& aHandlers );
    };

/**
*  Handles engine initialization.
*
*  @lib videoteleng
*  @since 2.6
*/
class TVtEngEngineInitUtil
    {
    public:
        
        static void HandleL( 
            CVtEngOperation& aOperation,
            CVtEngHandlerContainer& aHandlers );

        static void Cancel( CVtEngOperation& aOperation );
    };
    
  
/**
*  Handles extension operations.
*
*  @lib videoteleng
*  @since S60 v3.2
*/
class TVtEngExtensionUtil
    {
    public:
        
        /**
        * Handles extension operations
        * @param aOperation operation
        */
        static void HandleL( 
            CVtEngOperation& aOperation );
    };
#endif      // TVTENGOPERATIONUTILS
            
// End of File
