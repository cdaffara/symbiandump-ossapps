/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Definition file for Location Trail PS keys.
*
*/

#ifndef __LOCATIONTRAILPSKEYS_H__
#define __LOCATIONTRAILPSKEYS_H__

const TUid KPSUidLocationTrail = { 0x200071BE };

/**
 * Used by location trail, indicating location trail state changes.
 *
 * Possible values:
 * 0: Location trail stopped.
 * 1: Location trail started, waiting for valid GPS data.
 * 2: Location trail started, searching GPS device.
 * 3: Location trail started.
 */
const TUint32 KLocationTrailState = 0x00000001;


#endif // __LOCATIONTRAILPSKEYS_H__

// End of file.

