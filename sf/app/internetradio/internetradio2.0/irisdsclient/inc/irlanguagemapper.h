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
* Description:  The implementation for presentation elements.
*
*/


#ifndef IRLANGUAGEMAPPER_H
#define IRLANGUAGEMAPPER_H

#include <e32cmn.h>

const TInt KIRLanguageCodeLength = 2;

/**
 * A utility class to translate TLanguage into a country code http headers 
 * know something about
 *
 * @ingroup Utils
 */
NONSHARABLE_CLASS(IRLanguageMapper)
    {
public:
    /**
     * Translate TLanguage into language code representation 
     * (such as "en", "fi" etc).
     * @param aLanguage the language to be translated
     * @return Language code representation of the given input.
     */
    static TBufC8<KIRLanguageCodeLength> MapLanguage(TLanguage aLanguage);
    };

#endif // IRLANGUAGEMAPPER_H
