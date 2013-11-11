/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Meeting Request Info object processor
*
*/


#ifndef M_MRATTACHMENT_H
#define M_MRATTACHMENT_H

#include <e32base.h>
#include <f32file.h>

// Forward declaration
class MMRInfoObject;

/**
 *  MMRAttachment defines interface for meeting request attachments.
 *
 *  @lib FSFWCommonLib
 *  @since S60 v3.1
 */
class MMRAttachment
    {
public:
    
    /**
     * Virtual destructor.
     */
    virtual ~MMRAttachment() { }

public: // Interface

    /**
     * Fetches attachment file name.
     *
     * @return attachment file name.
     *
     * @exception System wide error code.
     */
    virtual const TDesC& AttachmentFileNameL() const = 0;
    
    /**
     * Fetches attachment size in bytes.
     *
     * @return Attachment size in bytes.
     *
     * @exception System wide error code.
     */
    virtual TInt AttachmentSizeL() const = 0;
    
    /**
     * Fetches handle to attachment. Handle can be used for reading
     * the attachment contents.
     *
     * @return Reference to attachment file handle.
     *
     * @exception System wide error codes.
     */
    virtual const RFile& AttachmentHandleL() const = 0;
    };

#endif // M_MRATTACHMENT_H
