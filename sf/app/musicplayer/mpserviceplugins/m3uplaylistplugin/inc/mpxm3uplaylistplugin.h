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
*/


#ifndef MPXM3UPLAYLISTPLUGIN_H
#define MPXM3UPLAYLISTPLUGIN_H

// INCLUDES
#include <mpxplaylistplugin.h>

// FORWARD DECLARATIONS
class MMPXPlaylistPluginObserver;
class CMPXM3uPlaylistImporter;
class CMPXM3uPlaylistExporter;

// CONSTANTS

// CLASS DECLARATION

/**
*  CMPXM3uPlaylistPlugin implements CMPXPlaylistPlugin to provide m3u
*  playlist importing and exporting services. 
*/
NONSHARABLE_CLASS(CMPXM3uPlaylistPlugin) : public CMPXPlaylistPlugin
    {
public: // Constructors and destructor
    
    /**
    * Two-phased constructor
    *
    * @param aInitParams constructor parameters
    * @return object of constructed
    */
    static CMPXM3uPlaylistPlugin* NewL(TAny* aInitParams);

    /**
    * Destructor
    */
    ~CMPXM3uPlaylistPlugin();
    
public: // from base clase CMPXPlaylistPlugin

    /** 
    * Internalize a playlist
    *
    * @param aStatus caller's request status
    * @param aPlaylist a playlist
    */
    void InternalizePlaylistL(
        TRequestStatus& aStatus,
        const TDesC& aPlaylistUri);

    /** 
    * Externalize a playlist
    *
    * @param aStatus caller's request status
    * @param aPlaylist playlist to be externalized
    * @param aFilePath File path for where the playlist should be placed
    */
    void ExternalizePlaylistL(
        TRequestStatus& aStatus,
        const CMPXMedia& aPlaylist,
        const TDesC& aFilePath);

    /**
    * Required attributes for the medias in the playlist in order to
    * externalize them to a M3U playlist.
    *
    * @return an array of attributes required in order to externalize
    * a playlist media into a M3U playlist file.
    */
    const TArray<TMPXAttribute> RequiredAttributes() const;

    /**
    * Optional attributes for the medias in the playlist for externalizing
    * them to a M3U playlist
    *
    * @return an array of attributes which are optional when externalizing
    * a playlist media into a M3U playlist file
    */
    const TArray<TMPXAttribute> OptionalAttributes() const;

    /**
    * Returns the file extension the plugin handles
    *
    * @return file extension which includes the period.
    */
    const TDesC& FileExtension() const;

    /**
    * cancel a client request
    */
    void Cancel();
    
private:

    /**
    * C++ Constructor
    */
    CMPXM3uPlaylistPlugin();

    /**
    * 2nd phase constructor
    */    
    void ConstructL();
    
private:

    CMPXM3uPlaylistImporter* iImporter;
    CMPXM3uPlaylistExporter* iExporter;
    RArray<TMPXAttribute>    iRequiredAttributes;
    RArray<TMPXAttribute>    iOptionalAttributes;
    };

#endif   // MPXM3UPLAYLISTPLUGIN_H
            

