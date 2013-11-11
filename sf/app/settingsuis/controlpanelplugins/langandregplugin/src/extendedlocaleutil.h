/*
 * Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 * This component and the accompanying materials are made available
 * under the terms of "Eclipse Public License v1.0""
 * which accompanies this distribution, and is available
 * at the URL "http://www.eclipse.org/legal/epl-v10.html".
 *
 * Initial Contributors:
 * Nokia Corporation - initial contribution.
 *
 * Contributors:
 *
 * Description:  Language plugin view class
 *
 */

/*
 * extendedlocaleutil.h
 *
 *  Created on: 24.8.2010
 *      Author: mhujanen
 */

#ifndef EXTENDEDLOCALEUTIL_H_
#define EXTENDEDLOCALEUTIL_H_

#include <hblocaleutil.h>
#include <hbinputlanguage.h>

/*
 *  Temporary class. 
 *  will be replaced by function added in HbLocaleUtil in future.
 *  
 *  */

struct LanguageRegionMapping
    {
    QString languageId;
    QString regionId;
    QString collationId;
    HbInputLanguage primaryWritingLan;
    HbInputLanguage secondaryWritingLan;
    };

class ExtendedLocaleUtil : public HbLocaleUtil
    {
   public:
        static QList<LanguageRegionMapping> localeMappings(bool onlySupported);
   private:
        static QList<LanguageRegionMapping> allLocaleMappings();
    };

#endif /* EXTENDEDLOCALEUTIL_H_ */
