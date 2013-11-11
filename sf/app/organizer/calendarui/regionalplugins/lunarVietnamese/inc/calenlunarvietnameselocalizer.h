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
* Description: Calendar Lunar Plugin 
*
*/

#ifndef __CALENLUNARVIETNAMESELOCALIZER_H__
#define __CALENLUNARVIETNAMESELOCALIZER_H__

#include <e32base.h>
#include "CalenLunarLocalizer.h"



class CCalenLunarVietnameseLocalizer : public CCalenLunarLocalizer
    {
public: // public API 
    static CCalenLunarVietnameseLocalizer* NewL();

    virtual ~CCalenLunarVietnameseLocalizer();

    virtual CCalenLunarLocalizedInfo* LocalizeL( TCalenLunarInfo& aInfo );
    
    virtual TPtrC GetExtraRowTextL( CCalenLunarLocalizedInfo& aLocInfo, 
                                    TInt aMaxWidth, 
                                    const CFont& aFont

                                    , TBool aTwoLines

                                    );

protected: // from base classes
    void LocalizeMonthAndDayL(CCalenLunarLocalizedInfo* aLocInfo,
                              TCalenLunarInfo& aInfo);

private: // own methods
    CCalenLunarVietnameseLocalizer();

    void ConstructL();


private: // data
    // own.
    HBufC* iYearOfCat;
    };

#endif // __CALENLUNARVIETNAMESELOCALIZER_H__
