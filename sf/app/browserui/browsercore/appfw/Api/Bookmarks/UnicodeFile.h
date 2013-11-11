/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation, version 2.1 of the License.
* 
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this program.  If not, 
* see "http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html/".
*
* Description: 
*      Declaration of RUnicodeFile.
*      Part of  : Bookmark Engine - Bookmark Import Utility
*
*/

#ifndef UNICODE_FILE_H
#define UNICODE_FILE_H

#include "bookmarks.h"
#include "xbel.h"

#ifdef Q_OS_SYMBIAN
// INCLUDE FILES

#include <e32base.h>
#include <f32file.h>

// CLASS DECLARATION

/**
* Read characters from a Unicode text file.
*/
	
class RUnicodeFile
    {
    public:     // New methods

        /**
        * Open Unicode file for input (read). Write access not supported.
        * @param aFs File Server Session handle.
        * @param aName File name.
        * @param aFileMode File opening mode.
        * @return Error code.
        */
        TInt Open( RFs& aFs, const TDesC& aName, TUint aFileMode );

        /**
        * Create/replace Unicode file for output (write). Existing content of
        * file is lost.
        * Read access not supported.
        * @param aFs File Server Session handle.
        * @param aName File name.
        * @param aFileMode File opening mode.
        * @return Error code.
        */
        TInt ReplaceL( RFs& aFs, const TDesC& aName, TUint aFileMode );

        /**
        * Close file.
        */
        void Close();

        /**
        * Get a character.
        * @return Character got.
        */
        TUint GetCharL();

        /**
        * Write Unicode descriptor to file.
        * @param aDes Descriptor to write.
        * @return Error code.
        */
        TInt Write( const TDesC16& aDes );

    private:    // data

        RFile iFile;    ///< File handle.
    };
#endif
#endif
            
// End of File
