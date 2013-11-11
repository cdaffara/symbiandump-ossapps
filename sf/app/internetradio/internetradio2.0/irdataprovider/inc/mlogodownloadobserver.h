/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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


#ifndef MLOGODOWNLOADOBSERVER_H
#define MLOGODOWNLOADOBSERVER_H

#include <e32def.h>

#include "irisdspreset.h"

class CIRIsdsPreset;

/**
 * MLogoDownloadObserver
 * passes id of a preset which has downloaded the logo.
 * An instance of this class must be provided while calling 
 * the API SendRequestL() of CIRLogoDownloadEngine.  
 * The call of the API SendRequestL() is from NowPlayingView.
 */
class MLogoDownloadObserver
    {
public:
    
    /**
     * MLogoDownloadObserver::PresetLogoDownloadedL()
     * called back when a preset's logo has downloaded
     * @param CIRIsdsPreset*, preset with downloaded logo
     */
    virtual void PresetLogoDownloadedL( CIRIsdsPreset* aPreset )=0;

    /**
     * MLogoDownloadObserver::PresetLogoDownloadError()
     * called back when a preset's logo has not downloaded
     * @param CIRIsdsPreset*, preset with no logo data
     */
    virtual void PresetLogoDownloadError( CIRIsdsPreset* aPreset )=0;
    };

#endif // MLOGODOWNLOADOBSERVER_H
