/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Represents one media group (e.g. EMsgMediaImage), and
*                contains a list of the MIME types that are of this media type
*                (read from the resource file). Wildcards may be used in the
*                resource, e.g. "image*".
*
*/




#ifndef __MSGMEDIATYPEGROUP_H
#define __MSGMEDIATYPEGROUP_H

//  INCLUDES
#include <e32base.h>
#include <badesca.h>
#include "MsgMedia.hrh"

// FORWARD DECLARATIONS
class TResourceReader;

// CLASS DECLARATION

/**
*  Represents one media group (e.g. EMsgMediaImage), and contains a list
*  of the MIME types that are of this media type (read from the resource file).
*  Wildcards may be used in the resource, e.g. "image*".
*
*  @lib MsgMedia.lib
*  @since 3.1
*/
NONSHARABLE_CLASS( CMsgMediaTypeGroup ) : public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CMsgMediaTypeGroup* NewLC( TResourceReader& aReader );

        /**
        * Destructor.
        */
        virtual ~CMsgMediaTypeGroup();

    public: // New functions

        /**
        * The media type id of this group.
        * @since 3.1
        * @return The media type id.
        */
        TMsgMediaType MediaType() const;

        /**
        * Ask if the given MIME type is part of this group.
        * @since 3.1
        * @param aMimeType A MIME type, e.g. "audio/wav".
        * @return ETrue, if this group contains the given MIME type.
        */
        TBool HasMimeType( const TDesC8& aMimeType ) const;

    private:

        /**
        * Symbian 2nd phase constructor.
        */
        void ConstructL( TResourceReader& aReader );

    private:    // Data

         // Descriptor array containing the mime types. Owned.
        CDesC8Array* iMimeTypeList;

		// The media type id of this group
		TMsgMediaType iMediaType;

    };

#endif      // __MSGMEDIATYPEGROUP_H

// End of File
