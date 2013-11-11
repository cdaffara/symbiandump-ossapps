/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:     Shared data keys for operator AP settings
*
*/





#ifndef WPAPSHAREDDATAKEYS_H
#define WPAPSHAREDDATAKEYS_H

// CONSTANTS

// -----------------------------------------------------------------------------
// AP Operator setting 0x101F466B
// -----------------------------------------------------------------------------

const TUid KSDUidOperatorSettings = {0x101F466B};

/**
* Modification of locked APs via OTA allowed 
*
* Possible values:
* 0 (no)
* 1 (yes)
*
* Default value: 0
**/
_LIT( KOsOtaModificationAllowed, "A" );

#endif // WPAPSharedDataKeys_H

// End of File