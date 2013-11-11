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
* Description:  Videoteleng Record to file extension API
*
*/


#ifndef MVTENGRECORD2FILE_H
#define MVTENGRECORD2FILE_H

//	INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS

// CLASS DECLARATION

// CONSTANTS
const TUid KVtEngExtensionRecord2File = {0x10283156}; 

/**
*  Record to file extension API class.
*
*  @lib videoteleng.lib
*  @since S60 v5.0
*/

class MVtEngRecord2File
    {
    public:
 
        /**
        * For testin puposes only REMOVE
        * @param  aSupported boolean for tone play support
        * @return Symbian error code  
        */
        virtual TInt IsTonePlaySupported( TBool& aSupported) const = 0;
    };    
#endif //MVTENGRECORD2FILE_H    
    
