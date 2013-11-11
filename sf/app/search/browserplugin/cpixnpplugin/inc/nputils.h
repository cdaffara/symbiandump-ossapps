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

#ifndef NPUTILS_H_
#define NPUTILS_H_

#include <e32cmn.h>

#include <npapi.h>
#include <npscript.h>
#include <npupp.h>

namespace nputils {

	TInt VariantToInt( NPVariant& v );
	TReal VariantToDouble( NPVariant& v );
	TUint VariantToUnsigned( NPVariant& v );
	void DescriptorToVariantL(const TDesC16& aString, NPVariant& aVariant);
	void DescriptorToVariant(const TDesC8& aString, NPVariant& aVariant);
	void VariantToDescriptor(NPVariant& aVariant, TDes8& aString);
	void VariantToDescriptorL(NPVariant& aVariant, TDes16& aString);
	HBufC16* VariantToHBufC16LC(NPVariant& aVariant);
	HBufC8* VariantToHBufC8LC(NPVariant& aVariant);
	HBufC8* GetLocationUrlLC();
}


#endif /* NPUTILS */
