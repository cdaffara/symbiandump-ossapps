/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Utility class for publishing the zoom state
*
*/



// Class header
#include "glxzoomstatepublisher.h"

// External includes
#include <bldvariant.hrh> // for feature flag
#include <cfclient.h>
#include <cflistener.h>

// Internal includes
#include <glxlog.h>

/**
 * @internal reviewed 5/02/2007 by Alex Birkett
 */
 
// -----------------------------------------------------------------------------
// NGlxZoomStatePublisher
// -----------------------------------------------------------------------------
namespace NGlxZoomStatePublisher
    {
#ifdef GLX_PUBLISH_ZOOMING_STATE
    // local constants
    _LIT( KGlxContextSource, "Application" );
    _LIT( KGlxContextType, "Photos.Zoom" );
    _LIT_SECURITY_POLICY_PASS( KGlxContextSec );

    _LIT( KGlxContextValueEnabled, "Enabled" );
    _LIT( KGlxContextValueDisabled, "Disabled" );

    /**
     * Implementation of MCFListener, needed
     * since CCFClient takes a reference
     */
    class TemporaryListener : public MCFListener
        {
        private: // From MCFListener
            /// @ref MCFListener
            void ContextIndicationL(
                const CCFContextIndication& /*aChangedContext*/ )
                {
                // nothing to do
                }
            /// @ref MCFListener
            void ActionIndicationL(
                const CCFActionIndication& /*aActionToExecute*/ )
                {
                // nothing to do
                }
            /// @ref MCFListener
            void HandleContextFrameworkError(
                TCFError /*aError*/,
                const TDesC& /*aSource*/,
                const TDesC& /*aType*/ )
                {
                // nothing to do
                }
            /// @ref MCFListener
            TAny* Extension( const TUid& /*aExtensionUid*/ ) const
                {
                // nothing to do
                return NULL;
                }
        };

// -----------------------------------------------------------------------------
// PublishStateL
// -----------------------------------------------------------------------------
EXPORT_C void PublishStateL( TBool aEnabled )
    {
    TRACER( "NGlxZoomStatePublisher::PublishStateL" );
    // publish zooming state if the feature is enabled
    TemporaryListener templistener;
    // create client and keep it on the cleanupstack
    CCFClient* client = CCFClient::NewLC( templistener );
    // define the context
    TInt err = client->DefineContext( 
        KGlxContextSource, KGlxContextType, KGlxContextSec );
    // if either success or context already there
    if( ( err == KErrNone )||
        ( err == KErrAlreadyExists ) )
        {
        // create context object and keep it on the cleanupstack
        CCFContextObject* context = CCFContextObject::NewLC();
        // set source and type
        context->SetSourceL( KGlxContextSource );
        context->SetTypeL( KGlxContextType );
        // default zoom state is disabled
        context->SetValueL( KGlxContextValueDisabled );
        if( aEnabled )
            {
            // enable zoom state
            context->SetValueL( KGlxContextValueEnabled );
            }
        // publish, cant really deal with the error so just ignore it
        (void)client->PublishContext( *context );
        CleanupStack::PopAndDestroy( context );
        }
    // release client
    CleanupStack::PopAndDestroy( client );
    } // PublishStateL
#else
// -----------------------------------------------------------------------------
// PublishStateL
// -----------------------------------------------------------------------------
EXPORT_C void PublishStateL( TBool /*aEnabled*/ )
    {
    }
#endif
    } // namespace

