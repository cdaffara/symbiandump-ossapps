/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/

#ifndef CCPIXNPPLUGINECOM_H
#define CCPIXNPPLUGINECOM_H

//  INCLUDES
#include <CEcomBrowserPluginInterface.h>

#ifdef __S60_50__
#include <npscript.h>
#else
#include <Javascriptcore/npruntime.h>
#endif

// CLASS DECLARATION

/******************************************
This class is specific to ECom Style Plugin.  This is used for passing
plugin specific initialization information to and from browser.
*******************************************/
IMPORT_C NPError InitializeFuncs( NPPluginFuncs* aPpf );

// CONSTANTS
const TInt KCPixNPPluginImplementationValue = 0x2001f6f9; // Matches implementation_uid in rss file

/**
 *  CPixNPPluginEcom
 *  Used for passing plugin specific initialization information to and from browser.
 *  @since S60 v3.2
 */
class CCPixNPPluginEcom : public CEcomBrowserPluginInterface
    {
    public: // Functions from base classes

        /**
        * Two-phased constructor.
        */
        static CCPixNPPluginEcom* NewL( TAny* aInitParam );

        /**
        * Destructor
        */
        virtual ~CCPixNPPluginEcom();

        /**
        * Passed from the browser to the plugin and delcares what
        * functions the browser makes available
        */
        NPNetscapeFuncs* Funcs() const { return iNpf; }

    private:

        // Default constructor
        CCPixNPPluginEcom( NPNetscapeFuncs* aNpf );

        // By default Symbian 2nd phase constructor is private.
        void ConstructL( NPPluginFuncs* aPluginFuncs );

        // Netscape Function Pointer
        NPNetscapeFuncs* iNpf;
    };

/**
********************************************************************************
    Plugin API methods. Source located in CSystemInfoApi.cpp
*********************************************************************************
*/

/**
 * Create a plugin.
 */

NPError CPixPluginNewp( NPMIMEType pluginType, NPP instance, uint16 mode,
                        CDesCArray* argn, CDesCArray* argv, NPSavedData* saved );

/**
 * Checks, whether it is ok to load the plugin. The page, which attempts
 * to load plugin should be originated in a secure domain, like *.nokia.com or
 * the local filesystem. 
 */
TBool CPixPluginAcceptLoadAttempt( NPP instance );
/**
 * Wrapper the errors
 */
void CPixPluginConstructL( NPP aInstance, CDesCArray* argn, CDesCArray* argv );

/**
 * Destroy a plugin.
 */
NPError CPixPluginDestroy( NPP aInstance, NPSavedData** save );

/**
 * Generic hook to retrieve values/attributes from the plugin.
 */
NPError CPixPluginGetvalue( NPP aInstance, NPPVariable variable, void *ret_value );


#endif /*CCPIXNPPLUGINECOM_H*/

// End of File
