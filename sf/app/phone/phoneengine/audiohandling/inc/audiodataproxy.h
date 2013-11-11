/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This file contains the header file of the class CPEAudioDataProxy.
*
*/


#ifndef AUDIODATAPROXY_H
#define AUDIODATAPROXY_H

//  INCLUDES
#include "mpeaudiodata.h"

// CONSTANTS
// None.

// MACROS
// None.

// DATA TYPES
// None.

// FUNCTION PROTOTYPES
IMPORT_C MPEAudioData* CreateL( MPEPhoneModelInternal& aPhoneModel, 
    RMobilePhone& aPhone, RDosServer& aDosServer, RMmCustomAPI& aCustomPhone ); 

// FORWARD DECLARATIONS
// None.

// CLASS DECLARATION
// None.

#endif      // AUDIODATAPROXY_H   
            
// End of File
