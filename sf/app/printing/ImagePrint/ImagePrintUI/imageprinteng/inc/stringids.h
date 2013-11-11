/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef STRINGIDS_H
#define STRINGIDS_H

#include <imageprintengine.rsg>

#include "printcapabilitycodes.h"
#include "printmessagecodes.h"
#include "imageprintvariant.h"

const TInt KEmptyString( 0 );

// This table combines the capability IDs with strings so that they
// can be fetched from the resources

#ifdef IMG_PRINT_DYNAMIC_PRINT_SETTINGS    
const TInt KConverterTableCount( 28 );
const TInt KConverterTableIndexCount( 2 );
const TInt KIDConverterTable[KConverterTableCount][KConverterTableIndexCount] =
    {
    { EPrintCapabLayout, R_QTN_STR_TEMPLATE } ,
    { EPrintCapabPaperSize, R_QTN_STR_PAPER_SIZE },
    { EPrintCapabPaperSizeAuto, R_QTN_STR_SIZE_AUTO },
    { EPrintCapabPaperSizeLetter, R_QTN_STR_SIZE_LETTER },
    { EPrintCapabPaperSizeA4, R_QTN_STR_SIZE_A4 },
    { EPrintCapabPaperSizeA6, R_QTN_STR_SIZE_A6 },
    { EPrintCapabPaperSize4x6, R_QTN_STR_SIZE_4X6 },
    { EPrintCapabPaperSize5x7, R_QTN_STR_SIZE_5X7 },
    { EPrintCapabPaperSizeSticker, R_QTN_STR_SIZE_STICKER },
    { EPrintCapabQuality, R_QTN_STR_QUALITY },
    { EPrintCapabQualityDefault, R_QTN_STR_QUAL_AUTO },
    { EPrintCapabQualityNormal, R_QTN_STR_QUAL_NORMAL },
    { EPrintCapabQualityDraft, R_QTN_STR_QUAL_DRAFT },
    { EPrintCapabQualityFine, R_QTN_STR_QUAL_FINE },
    { EPrintCapabLayout1UpBorderless, R_QTN_STR_TEMPLATE_BLESS },
    { EPrintCapabLayout1UpMedium, R_QTN_STR_TEMPLATE_BLESS },
    { EPrintCapabLayout1UpBorder, R_QTN_STR_TEMPLATE_ONE },
    { EPrintCapabLayout1Up, R_QTN_STR_TEMPLATE_ONE },
    { EPrintCapabLayout1UpStick, R_QTN_STR_TEMPLATE_ONE },
    { EPrintCapabLayout2Up, R_QTN_STR_TEMPLATE_TWO },
    { EPrintCapabLayout4Up, R_QTN_STR_TEMPLATE_FOUR },
    { EPrintCapabLayout4UpStick, R_QTN_STR_TEMPLATE_FOUR },
    { EPrintCapabLayout6Up, R_QTN_STR_TEMPLATE_SIX },
    { EPrintCapabLayout9Up, R_QTN_STR_TEMPLATE_NINE },
    { EPrintCapabLayout12Up, R_QTN_STR_TEMPLATE_TWELVE },
    { EPrintCapabLayout16UpStick, R_QTN_STR_TEMPLATE_SIXTEEN },
    { EPrintCapabLayout16Up, R_QTN_STR_TEMPLATE_SIXTEEN },
    { KEmptyString, R_EMPTY  }
    };
#else
const TInt KConverterTableCount( 15 );
const TInt KConverterTableIndexCount( 2 );
const TInt KIDConverterTable[KConverterTableCount][KConverterTableIndexCount] =
    {
    { EPrintCapabLayout, R_EMPTY } ,
    { EPrintCapabPaperSize, R_QTN_STR_PAPER_SIZE },
    { EPrintCapabPaperSizeAuto, R_QTN_STR_SIZE_AUTO },
    { EPrintCapabPaperSizeLetter, R_QTN_STR_SIZE_LETTER },
    { EPrintCapabPaperSizeA4, R_QTN_STR_SIZE_A4 },
    { EPrintCapabPaperSizeA6, R_QTN_STR_SIZE_A6 },
    { EPrintCapabPaperSize4x6, R_QTN_STR_SIZE_4X6 },
    { EPrintCapabPaperSize5x7, R_QTN_STR_SIZE_5X7 },
    { EPrintCapabPaperSizeSticker, R_QTN_STR_SIZE_STICKER },
    { EPrintCapabQuality, R_QTN_STR_QUALITY },
    { EPrintCapabQualityDefault, R_QTN_STR_QUAL_AUTO },
    { EPrintCapabQualityNormal, R_QTN_STR_QUAL_NORMAL },
    { EPrintCapabQualityDraft, R_QTN_STR_QUAL_DRAFT },
    { EPrintCapabQualityFine, R_QTN_STR_QUAL_FINE },
    { KEmptyString, R_EMPTY  }
    };
#endif
#endif  //  STRINGIDS_H

//  End of File
