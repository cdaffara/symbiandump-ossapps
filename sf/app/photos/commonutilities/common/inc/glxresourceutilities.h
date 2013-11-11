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
* Description:   Resource loading utility
*
*/



#ifndef __C_GLXRESOURCELOADER_H__
#define __C_GLXRESOURCELOADER_H__

#include <e32def.h>
#include <e32cmn.h> // for TFileName
#include <e32base.h>
#include <f32file.h> // Rfs

/**
 * Class with a single static method which 
 * finds the resource file on either the c or z drive
 * class CGlxResourceUtil
 * @author Loughlin Spollen 
 */
NONSHARABLE_CLASS( CGlxResourceUtilities ) : CBase
    {
public:

    /** 
      * Retrieve the full resource file name and path for a give resource 
      * file name.
      * Note! this version connects a RFs session
      * @param The resource filename to search
      */
    IMPORT_C static void GetResourceFilenameL( TFileName& aResFile );

    /** 
      * Retrieve the full resource file name and path for a give resource 
      * file name.
      * @param The resource filename to search
      * @param The file server session, must be connected before the call
      */
    IMPORT_C static void GetResourceFilenameL( TFileName& aResFile, RFs& aFs );
    };
    

#endif // __C_GLXRESOURCELOADER_H__

