/*
* Copyright (c) 2007-2010 Sebastian Brannstrom, Lars Persson, EmbedDev AB
*
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* EmbedDev AB - initial contribution.
*
* Contributors:
*
* Description:
*
*/

#ifndef __CONSTANTS_H__
#define __CONSTANTS_H__

const TUint KSizeKb = 1024;
const TUint KSizeMb = 1024000;

_LIT(KShowsSizeFormat,"%.1f");
_LIT(KShowsSizeUnit,"MB");
_LIT(KDateFormat,"%D%M%Y%/0%1%/1%2%/2%3%/3");
_LIT(KDateFormatShort,"%D%M%/0%4%/1%5");
// Date/time format string
_LIT(KTimeFormat, "%-B%:0%J%:1%T%+B");

#define KDefaultSQLDataBufferLength 2048
#define KDefaultURLBufferLength 2048
#endif
