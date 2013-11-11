/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
*       CUniDrmInfo, Storage for single attachment in presentation. 
*
*
*/

                    
// ---------------------------------------------------------
// CUniDrmInfo::ConsumeIntent
//
// ---------------------------------------------------------
//
inline ContentAccess::TIntent CUniDrmInfo::ConsumeIntent()
    {
    switch ( iMediaInfo.MediaType() )
        {
        case EMsgMediaAudio:
        case EMsgMediaVideo:
            {
            return ContentAccess::EPlay;
            break;
            }
        case EMsgMediaImage:
        case EMsgMediaText:
        case EMsgMediaSvg:
            {
            return ContentAccess::EView;
            break;
            }
        default:
            {
            return ContentAccess::EUnknown;
            break;
            }
        }
    }
