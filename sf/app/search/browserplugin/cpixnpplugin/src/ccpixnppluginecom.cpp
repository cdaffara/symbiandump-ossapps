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
* Description:  Interface to Browser
*
*/

// INCLUDE FILES
#include <implementationproxy.h>
#include "CCPixNPPluginEcom.h"
#include "CCPixNPPlugin.h"

#include "NPUtils.h"

#include <uri8.h>

/***************************************************/
/**********ECOM STYLE CPix PLUGIN ENTRY POINTS******/
/***************************************************/

_LIT8( KFileScheme, "file" );
_LIT8( KHttpScheme, "http" );
_LIT8( KNokiaDomain, "nokia.com" ); 

// -----------------------------------------------------------------------------
// CCPixNPPluginEcom::NewL
// -----------------------------------------------------------------------------
CCPixNPPluginEcom* CCPixNPPluginEcom::NewL( TAny* aInitParam )
    {
    TFuncs* funcs = REINTERPRET_CAST( TFuncs*, aInitParam );
    CCPixNPPluginEcom* self = new (ELeave) CCPixNPPluginEcom( funcs->iNetscapeFuncs );
    CleanupStack::PushL( self );
    self->ConstructL( funcs->iPluginFuncs );
    CleanupStack::Pop( self );
    Dll::SetTls ( (void*) self );
    
        
    return self;
    }

// -----------------------------------------------------------------------------
// CCPixNPPluginEcom::ConstructL
// -----------------------------------------------------------------------------
void CCPixNPPluginEcom::ConstructL( NPPluginFuncs* aPluginFuncs )
    {
    InitializeFuncs( aPluginFuncs );
    }

// -----------------------------------------------------------------------------
// CCPixNPPluginEcom::CCPixNPPluginEcom
// -----------------------------------------------------------------------------
CCPixNPPluginEcom::CCPixNPPluginEcom( NPNetscapeFuncs* aNpf )
    : CEcomBrowserPluginInterface(),
      iNpf( aNpf )
    {
    }

// -----------------------------------------------------------------------------
// CCPixNPPluginEcom::~CCPixNPPluginEcom
// -----------------------------------------------------------------------------
CCPixNPPluginEcom::~CCPixNPPluginEcom()
    {
    }

// -----------------------------------------------------------------------------
// KImplementationTable
// -----------------------------------------------------------------------------
const TImplementationProxy KImplementationTable[] =
    {
    {{KCPixNPPluginImplementationValue}, (TProxyNewLPtr)CCPixNPPluginEcom::NewL}
    };

// -----------------------------------------------------------------------------
// ImplementationGroupProxy
// Returns the filters implemented in this DLL
// Returns: The filters implemented in this DLL
// -----------------------------------------------------------------------------
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
    {
    aTableCount = sizeof(KImplementationTable) / sizeof(TImplementationProxy);
    return KImplementationTable;
    }

// -----------------------------------------------------------------------------
// InitializeFuncs
// -----------------------------------------------------------------------------
EXPORT_C NPError InitializeFuncs( NPPluginFuncs* aPpf )
    {
    aPpf->size          = sizeof(NPPluginFuncs);
    aPpf->version       = 1;
    aPpf->newp          = NewNPP_NewProc(CPixPluginNewp);
    aPpf->destroy       = NewNPP_DestroyProc(CPixPluginDestroy);
    aPpf->setwindow     = 0;
    aPpf->newstream     = 0;
    aPpf->destroystream = 0;
    aPpf->asfile        = 0;
    aPpf->writeready    = 0;
    aPpf->write         = 0;
    aPpf->print         = 0;
    aPpf->event         = 0;
    aPpf->urlnotify     = 0;
    aPpf->javaClass     = 0;
    aPpf->getvalue      = NewNPP_GetValueProc(CPixPluginGetvalue);
    aPpf->setvalue      = 0;
    return NPERR_NO_ERROR;
    }

// -----------------------------------------------------------------------------
// NPP_Shutdown
// -----------------------------------------------------------------------------
EXPORT_C void NPP_Shutdown(void)
    {
    CCPixNPPluginEcom* CPixPluginEcom = (CCPixNPPluginEcom*)Dll::Tls();
    delete CPixPluginEcom;
    Dll::SetTls( NULL );
    }

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPixPluginNewp
// Create a new instance of a plugin. This is non-leaving method.
// Returns: NPError: Error codes recognized by Browser
// -----------------------------------------------------------------------------
NPError CPixPluginNewp( NPMIMEType /*pluginType*/,  // Not used locally
                  NPP aInstance,            // Stores pointer to SystemInfo 
                  uint16 /*mode*/,          // 
                  CDesCArray* argn,         // The number of arguments passed
                  CDesCArray* argv,         // The values of arguments in the array
                  NPSavedData* /*saved*/ )  // 
    {
    if ( !CPixPluginAcceptLoadAttempt( aInstance ) ) 
    	{
    	return NPERR_MODULE_LOAD_FAILED_ERROR; 
    	}
    TRAPD( err, CPixPluginConstructL( aInstance, argn, argv ) );
    if ( err == KErrNoMemory )
        {        
        return NPERR_OUT_OF_MEMORY_ERROR;
        }
    if ( err != KErrNone )
        {
        return NPERR_MODULE_LOAD_FAILED_ERROR;
        }
        
    return NPERR_NO_ERROR;
    }

TBool CPixPluginAcceptLoadAttempt( NPP aInstance )
	{
    CCPixNPPluginEcom* pluginEcom = (CCPixNPPluginEcom*) Dll::Tls();
    NPNetscapeFuncs* netscapeFuncs = pluginEcom->Funcs();
    
	// Disabled, because NPN_GetValue was not found in npscript.lib
	TBool accept = EFalse;
    NPObject *windowObj = NULL;
    if ( netscapeFuncs->getvalue( aInstance, NPNVWindowNPObject, &windowObj ) == NPERR_NO_ERROR 
    	 && windowObj )
    	{
        NPIdentifier propertyId = NPN_GetStringIdentifier( "location" );
        NPVariant location;
        if ( NPN_GetProperty( aInstance, windowObj, propertyId, &location ) )
        	{
            NPObject* locationObj = location.value.objectValue;
            if ( locationObj ) 
            	{
				propertyId = NPN_GetStringIdentifier( "href" );
				NPVariant href;
				if ( NPN_GetProperty( aInstance, locationObj, propertyId, &href ) ) 
					{
					TRAP_IGNORE( 
						HBufC8* buf = nputils::VariantToHBufC8LC( href );
						if ( buf ) 
							{
							TUriParser8 parser; 
							parser.Parse( *buf );
							if ( parser.Extract( EUriScheme ) == KFileScheme() ) 
								{
								accept = ETrue; 
								}
							else if ( parser.Extract( EUriScheme ) == KHttpScheme() )
								{
								TDesC8 host = parser.Extract( EUriHost );
								if (host.Right( KNokiaDomain().Length() ) == KNokiaDomain() )
									{
									accept = ETrue;
									}
								}
							CleanupStack::PopAndDestroy( buf ); 
							}
						); 
					}
            	}
			}
    	}
    return accept; 
	}



// -----------------------------------------------------------------------------
// CPixPluginConstructL
// This is the leaving method to create the plugin.  We have leaving setters
// that need to be wrapped.
// Returns: void
// -----------------------------------------------------------------------------
void CPixPluginConstructL( NPP aInstance,      // Stores pointer to CPixPlugin 
                     CDesCArray* /*argn*/,      // The number of arguments passed
                     CDesCArray* /*argv*/ )     // The values of arguments in the array
    {
    CCPixNPPlugin* plugin = CCPixNPPlugin::NewL();
    plugin->SetInstance( aInstance, NULL );
    aInstance->pdata = plugin;
     
    CCPixNPPluginEcom* CPixPluginEcom = (CCPixNPPluginEcom*) Dll::Tls();
    NPNetscapeFuncs* netscapeFuncs = CPixPluginEcom->Funcs();
    NPBool value_false = 0;
    netscapeFuncs->setvalue( aInstance, NPPVpluginWindowBool, (void*)&value_false );        // not owing the window
    netscapeFuncs->setvalue( aInstance, NPPVpluginTransparentBool, (void*)&value_false );   // speed up drawing
    }
// -----------------------------------------------------------------------------
// CPixDestroy(NPP aInstance, NPSavedData**)
// Called by Browser to destroy the plugin
// Returns: NPError: Error Code
// -----------------------------------------------------------------------------
NPError CPixPluginDestroy( NPP aInstance,               // Link to Browser
                     NPSavedData** /*save*/ )   // Not used locally
    {
    CCPixNPPlugin* pluginInstance = STATIC_CAST( CCPixNPPlugin*, aInstance->pdata );
    delete pluginInstance;
    
    return NPERR_NO_ERROR;
    }

        
// -----------------------------------------------------------------------------
// CPixPluginGetvalue		
// -----------------------------------------------------------------------------
//
NPError CPixPluginGetvalue( NPP aInstance, NPPVariable variable, void* ret_value )	
    {
	if (variable == NPPVpluginScriptableNPObject)
		{        
		CCPixNPPlugin* plugin = (CCPixNPPlugin*)aInstance->pdata;
		CPixPluginObject* pluginObject = (CPixPluginObject *)NPN_CreateObject (aInstance, CPixPluginClass);
		pluginObject->plugin = plugin; 
		plugin->SetInstance( aInstance, &pluginObject->object );
		void** ret = (void**)ret_value;
		*ret = (void*)pluginObject;
		} 
    return NPERR_NO_ERROR;        
    }


//  End of File
