/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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


// INCLUDE FILES
#include <e32math.h> 
#include <u32hal.h>
#include <hal.h> 

#include "loadgen_utils.h"
#include "loadgen.hrh"

inline TInt64 INIT_SEED()
{
    TTime now; 
    now.HomeTime(); 
    return now.Int64();
}

TInt64 CLoadGenUtils::iRandomNumberSeed = INIT_SEED();

TThreadPriority CLoadGenUtils::SettingItemToThreadPriority(TInt aIndex)
    {
    TThreadPriority threadPriority = EPriorityNull;
    
    switch ( aIndex )
        {
        case EThreadPriorityTypeMuchLess:
            {
            threadPriority = EPriorityMuchLess; break;
            }
        case EThreadPriorityTypeLess:
            {
            threadPriority = EPriorityLess; break;
            }
        case EThreadPriorityTypeNormal:
            {
            threadPriority = EPriorityNormal; break;
            }
        case EThreadPriorityTypeMore:
            {
            threadPriority = EPriorityMore; break;
            }
        case EThreadPriorityTypeMuchMore:
            {
            threadPriority = EPriorityMuchMore; break;
            }
        case EThreadPriorityTypeRealTime:
            {
            threadPriority = EPriorityRealTime; break;
            }
        case EThreadPriorityTypeAbsoluteVeryLow:
            {
            threadPriority = EPriorityAbsoluteVeryLow; break;
            }
        case EThreadPriorityTypeAbsoluteLow:
            {
            threadPriority = EPriorityAbsoluteLow; break;
            }
        case EThreadPriorityTypeAbsoluteBackground:
            {
            threadPriority = EPriorityAbsoluteBackground; break;
            }
        case EThreadPriorityTypeAbsoluteForeground:
            {
            threadPriority = EPriorityAbsoluteForeground; break;
            }
        case EThreadPriorityTypeAbsoluteHigh:
            {
            threadPriority = EPriorityAbsoluteHigh; break;
            }

        default:
            {
            User::Panic(_L("Wrong tp index"), 276);
            break;
            }
        }

    return threadPriority;
    }

// --------------------------------------------------------------------------------------------

void CLoadGenUtils::SettingItemToThreadDescription(TInt aIndex, TDes& aBuf)
    {
    switch ( aIndex )
        {
        case EThreadPriorityTypeMuchLess:
            {
            aBuf.Append(_L("ML(-20)")); break;
            }
        case EThreadPriorityTypeLess:
            {
            aBuf.Append(_L("L(-10)")); break;
            }
        case EThreadPriorityTypeNormal:
            {
            aBuf.Append(_L("NO(0)")); break;
            }
        case EThreadPriorityTypeMore:
            {
            aBuf.Append(_L("M(10)")); break;
            }
        case EThreadPriorityTypeMuchMore:
            {
            aBuf.Append(_L("MM(20)")); break;
            }
        case EThreadPriorityTypeRealTime:
            {
            aBuf.Append(_L("RL(30)")); break;
            }
        case EThreadPriorityTypeAbsoluteVeryLow:
            {
            aBuf.Append(_L("AVL(100)")); break;
            }
        case EThreadPriorityTypeAbsoluteLow:
            {
            aBuf.Append(_L("AL(200)")); break;
            }
        case EThreadPriorityTypeAbsoluteBackground:
            {
            aBuf.Append(_L("ABG(300)")); break;
            }
        case EThreadPriorityTypeAbsoluteForeground:
            {
            aBuf.Append(_L("AFG(400)")); break;
            }
        case EThreadPriorityTypeAbsoluteHigh:
            {
            aBuf.Append(_L("AH(500)")); break;
            }

        default:
            {
            User::Panic(_L("Wrong td index"), 277);
            break;
            }
        }
    }

// --------------------------------------------------------------------------------------------

void CLoadGenUtils::SettingItemToSourceDescription(TInt aIndex, TDes& aBuf)
    {
    switch ( aIndex )
        {
        case EMemoryEatSourceTypeRAM:
            {
            aBuf.Append(_L("RAM")); break;
            }
        case EMemoryEatSourceTypeDriveC:
            {
            aBuf.Append(_L("C:")); break;
            }
        case EMemoryEatSourceTypeDriveD:
            {
            aBuf.Append(_L("D:")); break;
            }            
        case EMemoryEatSourceTypeDriveE:
            {
            aBuf.Append(_L("E:")); break;
            }            
        case EMemoryEatSourceTypeDriveF:
            {
            aBuf.Append(_L("F:")); break;
            }            
        case EMemoryEatSourceTypeDriveG:
            {
            aBuf.Append(_L("G:")); break;
            }            
        case EMemoryEatSourceTypeDriveH:
            {
            aBuf.Append(_L("H:")); break;
            }            
        case EMemoryEatSourceTypeDriveI:
            {
            aBuf.Append(_L("I:")); break;
            }            
        case EMemoryEatSourceTypeDriveJ:
            {
            aBuf.Append(_L("J:")); break;
            }            
        case EMemoryEatSourceTypeDriveK:
            {
            aBuf.Append(_L("K:")); break;
            }            
        case EMemoryEatSourceTypeDriveL:
            {
            aBuf.Append(_L("L:")); break;
            }            
        case EMemoryEatSourceTypeDriveM:
            {
            aBuf.Append(_L("M:")); break;
            }            
        case EMemoryEatSourceTypeDriveN:
            {
            aBuf.Append(_L("N:")); break;
            }            

        default:
            {
            User::Panic(_L("Wrong src index"), 278);
            break;
            }
        }
    }
        
// --------------------------------------------------------------------------------------------

TInt CLoadGenUtils::MilliSecondsToMicroSeconds(TInt aMilliSeconds, TInt aRandomVariance)
    {
    __ASSERT_ALWAYS(aRandomVariance >= 0 && aRandomVariance <= 100, User::Panic(_L("Inv.var."), 180));
    __ASSERT_ALWAYS(aMilliSeconds >=0, User::Panic(_L("Inv.ms."), 181));
    
    // if no variance defined, just do a quick conversion
    if (aRandomVariance == 0 || aMilliSeconds == 0)
        return aMilliSeconds * 1000;
    
    // otherwise calculate difference
    TInt diff = 1000 * aMilliSeconds / 2 * aRandomVariance / 100;  
    
    return RandomNumber(aMilliSeconds*1000-diff, aMilliSeconds*1000+diff);
    }

// --------------------------------------------------------------------------------------------

TInt CLoadGenUtils::RandomNumber(TInt aMin, TInt aMax)
    {
    TInt range = (aMax > aMin ? aMax - aMin : aMin - aMax) + 1;
    TInt random = (TInt) (range * Math::FRand(iRandomNumberSeed));
    
    random = (random % range) + aMin;

    if (random > aMax)
        random = aMax;
    else if (random < aMin)
        random = aMin;
    
    return random;
    }

// --------------------------------------------------------------------------------------------

TSize CLoadGenUtils::ScreenSize()
    {    
    TInt x = 0;
    TInt y = 0;
    HAL::Get(HALData::EDisplayXPixels, x);
    HAL::Get(HALData::EDisplayYPixels, y);
    return TSize(x, y);
    }
