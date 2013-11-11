/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: msgunieditorimageprocessor.inl*
*/



// ========== INLINE METHODS ===============================

// ---------------------------------------------------------
// CUniImageProcessor::Error
// ---------------------------------------------------------
//
inline TInt CUniImageProcessor::Error() const
    {
    return iError;
    }
    
// ---------------------------------------------------------
// CUniImageProcessor::SetOnlyDecodePlaneScaling
// ---------------------------------------------------------
//
inline void CUniImageProcessor::SetOnlyDecodePlaneScaling( TBool aOnlyDecodeTimeScaling )
    {
    if ( aOnlyDecodeTimeScaling )
        {
        iFlags |= EOnlyDecodeTimeScaling;
        }
    else
        {
        iFlags &= ~EOnlyDecodeTimeScaling;
        }
    }
    
// ---------------------------------------------------------
// CUniImageProcessor::SetAllowScalingUp
// ---------------------------------------------------------
//  
inline void CUniImageProcessor::SetAllowScalingUp( TBool aAllowScalingUp )
    {
    if ( aAllowScalingUp )
        {
        iFlags |= EAllowScalingUp;
        }
    else
        {
        iFlags &= ~EAllowScalingUp;
        }
    }
               
//  End of File
