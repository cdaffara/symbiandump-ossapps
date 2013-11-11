/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   OMA MMS encapsulation version number
*
*/




#ifndef MMSVERSION_H
#define MMSVERSION_H

// CONSTANTS

// version 1.0, this is used, when versions are incompatible
// major version = 1, minor version = 0, binary 0001 0000
const TUint8 KMmsVersion1 = 0x10;

// version 1.1, used for checking header support
const TUint8 KMmsVersion11 = 0x11;

// version 1.2, used for checking header support
const TUint8 KMmsVersion12 = 0x12;

// version 1.3, used for checking header support
const TUint8 KMmsVersion13 = 0x13;

// mininum version we can fall back to
// Currently it makes no sense to fall back below 1.2
const TUint8 KMmsMinimumSupportedVersion = KMmsVersion12;

// maximum supported MMS version
// This is used to check that the caller does not try to set
// an unsupported version when changing the MMS version is allowed
// Currently the maximum supported version is 1.3
const TUint8 KMmsMaximumSupportedVersion = KMmsVersion13;

// Default version that will be used if version not defined in
// Central repository or if the value is not within the supported limits
// The value from central repository can be obtained from CMmsSettings by
// function MmsVersion() that returns the current version
const TUint8 KMmsDefaultVersion = KMmsVersion12;

#endif      // MMSVERSION_H   
            
// End of File
