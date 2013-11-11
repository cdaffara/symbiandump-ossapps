/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef MSGATTACHMENTUTILS_H
#define MSGATTACHMENTUTILS_H

// ========== INCLUDE FILES ================================

#include <e32std.h>

// ========== CONSTANTS ====================================

// ========== MACROS =======================================

// ========== DATA TYPES ===================================

// ========== FUNCTION PROTOTYPES ==========================

// ========== FORWARD DECLARATIONS =========================

// ========== CLASS DECLARATION ============================

/**
 * Attachment utilities.
 *
 */
class MsgAttachmentUtils
    {
    public:
        /**
         * Generates a legal filename from given text buffer.
         * @param aFileName  OUT: generated filename.
         * @param aBuffer    text buffer.
         * @param aMaxLength maximum length for filename.
         * @param aExt       optional file extension.
         */
        static void GetFileNameFromBuffer(
            TFileName&      aFileName,
            const TDesC&    aBuffer,
            TInt            aMaxLength,
            const TDesC*    aExt = NULL );

    };

#endif // MSGATTACHMENTUTILS_H

// End of File
