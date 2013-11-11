/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*/

#ifndef MVCXMYVIDEOSCOLLECTIONPLUGINTESTEROBSERVER_H
#define MVCXMYVIDEOSCOLLECTIONPLUGINTESTEROBSERVER_H

//  INCLUDES

// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;

// DATA TYPES
//enum ?declaration
//typedef ?declaration
//extern ?data_type;

// CLASS DECLARATION

/**
*  MVCXMyVideosCollectionPluginTesterObserver
*  ?other_description_lines
*
*  @lib ?library
*  @since ?Series60_version
*/
class MVCXMyVideosCollectionPluginTesterObserver
    {
    public: // New functions

        /*
         * Message from CVCXMyVideosCollectionPluginTester.
         */        
        virtual void HandleVcxMvTesterMessageL( TInt aMessage, TInt aError ) = 0;
    };

#endif      // MVCXMYVIDEOSCOLLECTIONPLUGINTESTEROBSERVER_H

// End of File
