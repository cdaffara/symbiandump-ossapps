/*
* Copyright (c) 2010 Kanrikogaku Kenkyusho, Ltd.
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Kanrikogaku Kenkyusho, Ltd. - Initial contribution
*
* Contributors:
*
* Description:  
*
*/


#ifndef DIRECTPRINTSTRINGIDS_H
#define DIRECTPRINTSTRINGIDS_H

#include <DirectPrintApp.rsg>

#include "directprintcapabilitycodes.h"
#include "printmessagecodes.h"
#include "directprintvariant.h"

const TInt KEmptyString( 0 );

// This table combines the capability IDs with strings so that they
// can be fetched from the resources

const TInt KConverterTableCount( 18 );
const TInt KConverterTableIndexCount( 2 );
const TInt KIDConverterTable[KConverterTableCount][KConverterTableIndexCount] =
    {
    { EDirectPrintCapabLayout, R_QTN_STR_TEMPLATE } ,
    { EDirectPrintCapabQuality, R_QTN_STR_QUALITY },
    { EDirectPrintCapabQualityDefault, R_QTN_STR_QUAL_AUTO },
    { EDirectPrintCapabQualityNormal, R_QTN_STR_QUAL_NORMAL },
    { EDirectPrintCapabQualityDraft, R_QTN_STR_QUAL_DRAFT },
    { EDirectPrintCapabQualityFine, R_QTN_STR_QUAL_FINE },
    { EDirectPrintCapabAP, R_QTN_STR_ACCESS_POINT },
    { EDirectPrintCapabHost, R_QTN_STR_HOST },
    { EDirectPrintCapabPort, R_QTN_STR_PORT },
    { EDirectPrintCapabUser, R_QTN_STR_USER },
    { EDirectPrintCapabQue, R_QTN_STR_QUE },
    { EDirectPrintCapabExtent, R_QTN_STR_EXTENT },
    { EDirectPrintCapabExtent2, R_QTN_STR_EXTENT2 },
    { EDirectPrintCapabExtentSelect1, R_QTN_STR_EXTENT_SELECT1 },
    { EDirectPrintCapabExtentSelect2, R_QTN_STR_EXTENT_SELECT2 },
    { EDirectPrintCapabExtentSelect3, R_QTN_STR_EXTENT_SELECT3 },
    { EDirectPrintCapabExtentSelect4, R_QTN_STR_EXTENT_SELECT4 },
    { KEmptyString, R_EMPTY  }
    };
#endif  //  DIRECTPRINTSTRINGIDS_H
//  End of File
