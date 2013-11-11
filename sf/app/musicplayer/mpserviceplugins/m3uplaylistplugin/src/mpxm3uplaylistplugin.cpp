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
* Description:  Implementation of m3u playlist plugin interface
*
*  CMPXM3uPlaylistPlugin delegates the responsibilities for m3u playlist file
*  import and export to CMPXM3uPlaylistImporter and CMPXM3uPlaylistExporter
*  respectively.
*
*
*/


// INCLUDE FILES
#include <e32cmn.h>
#include <implementationproxy.h>
#include <mpxlog.h>
#include <mpxmediageneraldefs.h>
#include <mpxplaylistpluginobserver.h>
#include "mpxm3uplaylistimporter.h"
#include "mpxm3uplaylistexporter.h"
#include "mpxm3uplaylistplugin.h"
#include "mpxm3uplaylistdefs.h"
#include "mpxm3uplaylistdefs.hrh"

    
// ============================ MEMBER FUNCTIONS ==============================
// ----------------------------------------------------------------------------
// Constructor. 
// ----------------------------------------------------------------------------
CMPXM3uPlaylistPlugin::CMPXM3uPlaylistPlugin()
    {
    }

// ----------------------------------------------------------------------------
// 2nd phase constructor
// ----------------------------------------------------------------------------
void CMPXM3uPlaylistPlugin::ConstructL()
    {
    iRequiredAttributes.AppendL(KMPXMediaGeneralUri);
    
    iOptionalAttributes.AppendL(KMPXMediaGeneralTitle);
    iOptionalAttributes.AppendL(KMPXMediaGeneralDuration);
    }
    
// ----------------------------------------------------------------------------
// Two-phased constructor. 
// ----------------------------------------------------------------------------
CMPXM3uPlaylistPlugin* CMPXM3uPlaylistPlugin::NewL(TAny* /*aInitParams*/)
    {
    CMPXM3uPlaylistPlugin* self=new(ELeave)CMPXM3uPlaylistPlugin();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// ----------------------------------------------------------------------------
// Destructor. 
// ----------------------------------------------------------------------------
CMPXM3uPlaylistPlugin::~CMPXM3uPlaylistPlugin()
    {
    delete iImporter;
    delete iExporter;
    iRequiredAttributes.Close();
    iOptionalAttributes.Close();
    }

// ----------------------------------------------------------------------------
// Internalize a playlist
// ----------------------------------------------------------------------------
void CMPXM3uPlaylistPlugin::InternalizePlaylistL(
    TRequestStatus& aStatus,
    const TDesC& aPlaylistUri)
    {
    MPX_DEBUG1("CMPXM3uPlaylistPlugin::InternalizePlaylist(aPlaylistUri)");

    // instantiate a new CMPXM3uPlaylistImporter to handle this request.
    delete iImporter;
    iImporter = NULL;

    // processing starts as soon as object is instantiated. When request completes,
    // client is notified through MMPXPlaylistPlugibObserver interface        
    iImporter =
        CMPXM3uPlaylistImporter::NewL(
            iFs, iObserver, aPlaylistUri, *iTopCharacterSet, *iAvailableCharacterSet, aStatus );
    }

// ----------------------------------------------------------------------------
// Externalize a playlist
// ----------------------------------------------------------------------------
void CMPXM3uPlaylistPlugin::ExternalizePlaylistL(
    TRequestStatus& aStatus,
    const CMPXMedia& aPlaylist,
    const TDesC& aFilePath)
    {
    MPX_DEBUG1("CMPXM3uPlaylistPlugin::ExternalizePlaylistL");    

    // instantiate a new CMPXM3uPlaylistExporter to handle this request
    delete iExporter;
    iExporter = NULL;
    
    // processing starts as soon as object is instantiated. When request completes,
    // client is notified through MMPXPlaylistPlugibObserver interface        
    iExporter =
        CMPXM3uPlaylistExporter::NewL(
            iFs, iObserver, aPlaylist, aFilePath, aStatus );
    }

// ----------------------------------------------------------------------------
// Required attributes for the medias in the playlist in order to
// externalize them to a playlist.
// ----------------------------------------------------------------------------
const TArray<TMPXAttribute> CMPXM3uPlaylistPlugin::RequiredAttributes() const
    {
    return iRequiredAttributes.Array();    
    }

// ----------------------------------------------------------------------------
// Optional attributes for the medias in the playlist for externalizing
// them to a playlist
// ----------------------------------------------------------------------------
const TArray<TMPXAttribute> CMPXM3uPlaylistPlugin::OptionalAttributes() const
    {
    return iOptionalAttributes.Array();    
    }

// ----------------------------------------------------------------------------
// Returns the file extension the plugin handles
// ----------------------------------------------------------------------------
const TDesC& CMPXM3uPlaylistPlugin::FileExtension() const
    {
    return KMPXM3UExtension();
    }

// ----------------------------------------------------------------------------
// cancel a client request
// ----------------------------------------------------------------------------
void CMPXM3uPlaylistPlugin::Cancel()
    {
    if ( iImporter )
        {
        iImporter->Cancel();
        }
        
    if ( iExporter )
        {
        iExporter->Cancel();
        }
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================
// ----------------------------------------------------------------------------
// The list of implementations
// ----------------------------------------------------------------------------
const TImplementationProxy ImplementationTable[] = 
    { IMPLEMENTATION_PROXY_ENTRY(KMPXM3uPlaylistImplUid, CMPXM3uPlaylistPlugin::NewL) };

// ----------------------------------------------------------------------------
// The proxy of implementations
// ----------------------------------------------------------------------------
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
    {
    aTableCount=sizeof(ImplementationTable)/sizeof(TImplementationProxy);
    return ImplementationTable;
    }
    
// End of file
