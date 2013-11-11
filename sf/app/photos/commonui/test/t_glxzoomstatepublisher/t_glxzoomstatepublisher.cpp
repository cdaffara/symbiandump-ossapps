/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

/*
-----------------------------------------------------------------------------
This file has been generated with EUnit Pro
http://www.sysopendigia.com/qualitykit
-----------------------------------------------------------------------------
*/

/**
 * @internal reviewed 5/02/2007 by Alex Birkett
 */
 
// CLASS HEADER
#include "t_glxzoomstatepublisher.h"

// EXTERNAL INCLUDES
#include <EUnitMacros.h>
#include <EUnitDecorators.h>
#include <cfcontextquery.h>
#include <cfclient.h>
#include <cflistener.h>

// CODE UNDER TEST
#include "glxzoomstatepublisher.h"

// - Construction -----------------------------------------------------------

T_GlxZoomStatePublisher* T_GlxZoomStatePublisher::NewL()
    {
    T_GlxZoomStatePublisher* self = T_GlxZoomStatePublisher::NewLC();
    CleanupStack::Pop();
    return self;
    }

T_GlxZoomStatePublisher* T_GlxZoomStatePublisher::NewLC()
    {
    T_GlxZoomStatePublisher* self = new( ELeave ) T_GlxZoomStatePublisher;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

T_GlxZoomStatePublisher::~T_GlxZoomStatePublisher()
    {
    }

T_GlxZoomStatePublisher::T_GlxZoomStatePublisher()
    {
    }

void T_GlxZoomStatePublisher::ConstructL()
    {
    CEUnitTestSuiteClass::ConstructL();
    }

// - Test methods -----------------------------------------------------------

void T_GlxZoomStatePublisher::SetupL()
    {
    }

void T_GlxZoomStatePublisher::Teardown()
    {
    }

/*
 * Implementation of MCFListener, needed
 * since CCFClient takes a reference
 */
class T_TemporaryListener : public MCFListener
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
        void HandleContextFrameworkError( TCFError /*aError*/,
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

void T_GlxZoomStatePublisher::T_TestL()
    {
    // need a listener even though we're not interested of the events
    T_TemporaryListener temp;
    // create client first
    CCFClient* client = NULL;
    TRAPD( err, 
        {
        client = CCFClient::NewL( temp );
        } );
    // if client creation failed, we dont have the context framework server running
    // and there is no point to continue with the test
    if( !client )
        {
        return;
        }
    CleanupStack::PushL( client );

    // publish state, no zoom keys
    NGlxZoomStatePublisher::PublishStateL( EFalse );

    // check that the context is there
    _LIT( KGlxContextSource, "Application" );
    _LIT( KGlxContextType, "Photos.Zoom" );
    _LIT( KGlxContextValueEnabled, "Enabled" );
    _LIT( KGlxContextValueDisabled, "Disabled" );
    
        {
        // make a query to see that the context is there
        CCFContextQuery* query = CCFContextQuery::NewLC();
        query->SetSourceL( KGlxContextSource );
        query->SetTypeL( KGlxContextType );
        RContextObjectArray result;
        TInt err = client->RequestContext( *query, result );
        EUNIT_ASSERT_EQUALS_DESC( KErrNone, err, "no error" );
        EUNIT_ASSERT_EQUALS_DESC( 1, result.Count(), "1 result" );
        EUNIT_ASSERT_EQUALS_DESC( KGlxContextValueDisabled(), result[ 0 ]->Value(), "value is disabled" );
        result.ResetAndDestroy();
        CleanupStack::PopAndDestroy( query );
        }

    // publish state, zoom keys on
    NGlxZoomStatePublisher::PublishStateL( ETrue );

        {
        // make a query to see that the context is there
        CCFContextQuery* query = CCFContextQuery::NewLC();
        query->SetSourceL( KGlxContextSource );
        query->SetTypeL( KGlxContextType );
        RContextObjectArray result;
        TInt err = client->RequestContext( *query, result );
        EUNIT_ASSERT_EQUALS_DESC( KErrNone, err, "no error" );
        EUNIT_ASSERT_EQUALS_DESC( 1, result.Count(), "1 result" );
        EUNIT_ASSERT_EQUALS_DESC( KGlxContextValueEnabled(), result[ 0 ]->Value(), "value is enabled" );
        result.ResetAndDestroy();
        CleanupStack::PopAndDestroy( query );
        }

    CleanupStack::PopAndDestroy( client );
    }
    

// - EUnit test table -------------------------------------------------------

EUNIT_BEGIN_TEST_TABLE(
    T_GlxZoomStatePublisher,
    "Test suite for NGlxZoomStatePublisher",
    "UNIT" )

EUNIT_TEST(
    "PublishStateL test",
    "NGlxZoomStatePublisher",
    "PublishStateL",
    "FUNCTIONALITY",
    SetupL, T_TestL, Teardown )

EUNIT_END_TEST_TABLE
