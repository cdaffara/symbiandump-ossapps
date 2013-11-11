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
* Description: audioequalizerutility stub for testing mpequalizerframeworkwrapper
*
*/


#include "mptrace.h"
//#include <e32debug.h>

#include "stub/inc/audioequalizerutility.h"


/*!
 Stub function.
*/
CAudioEqualizerUtility::CAudioEqualizerUtility()
{
}


/*!
 Stub function.
*/
CAudioEqualizerUtility::~CAudioEqualizerUtility()
{
    iPresetArray.Close();
}

/*!
 Stub function.
*/
void CAudioEqualizerUtility::ConstructL(CMdaAudioPlayerUtility& /*aUtility*/)
{
    UpdatePresetArray(iPresetArray);
}

/*!
 Stub function.
*/
CAudioEqualizerUtility* CAudioEqualizerUtility::NewL(CMdaAudioPlayerUtility& aUtility)
{
    CAudioEqualizerUtility* self = new( ELeave ) CAudioEqualizerUtility;
    CleanupStack::PushL( self );
    self->ConstructL(aUtility);
    CleanupStack::Pop(self);
    return self;
}

/*!
 Stub function.
*/
const TDesC& CAudioEqualizerUtility::GetPresetL( TInt aPresetIndex )
{
    TX_ENTRY

    if(aPresetIndex > (iPresetArray.Count() - 1) || aPresetIndex < 0 ) {
        User::Leave(KErrArgument);
    }

    TX_EXIT
    return iPresetArray[aPresetIndex].iPresetName;
}

/*!
 Stub function.
*/
TArray<TEfAudioEqualizerUtilityPreset> CAudioEqualizerUtility::Presets()
{
    return iPresetArray.Array();
}

/*!
 Stub function.
*/
void CAudioEqualizerUtility::UpdatePresetArray(RArray<TEfAudioEqualizerUtilityPreset>& aPresetArray)
{
    aPresetArray.Reset();
    
    TEfAudioEqualizerUtilityPreset  tempPreset1;        
    tempPreset1.iPresetDescriptionKey = 0x10;
    tempPreset1.iPresetName = _L("Bass");
    tempPreset1.iPresetNameKey = 0x0A;
    aPresetArray.Append(tempPreset1);
        
    TEfAudioEqualizerUtilityPreset  tempPreset2;        
    tempPreset2.iPresetDescriptionKey = 0x20;
    tempPreset2.iPresetName = _L("Classical");
    tempPreset2.iPresetNameKey = 0x0B;
    aPresetArray.Append(tempPreset2);

    TEfAudioEqualizerUtilityPreset  tempPreset3;        
    tempPreset3.iPresetDescriptionKey = 0x30;
    tempPreset3.iPresetName = _L("Pop");
    tempPreset3.iPresetNameKey = 0x0C;
    aPresetArray.Append(tempPreset3);
        
    TEfAudioEqualizerUtilityPreset  tempPreset4;        
    tempPreset4.iPresetDescriptionKey = 0x40;
    tempPreset4.iPresetName = _L("Jazz");
    tempPreset4.iPresetNameKey = 0x0D;
    aPresetArray.Append(tempPreset4);

    TEfAudioEqualizerUtilityPreset  tempPreset5;        
    tempPreset5.iPresetDescriptionKey = 0x50;
    tempPreset5.iPresetName = _L("Rock");
    tempPreset5.iPresetNameKey = 0x0E;
    aPresetArray.Append(tempPreset5);
}

//end of file
