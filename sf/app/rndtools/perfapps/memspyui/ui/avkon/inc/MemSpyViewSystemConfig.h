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
* Description:
*
*/

#ifndef MEMSPYVIEWSYSTEMCONFIG_H
#define MEMSPYVIEWSYSTEMCONFIG_H

// System includes
#include <hal.h>
#include <f32file.h>
#include <coecntrl.h>
#include <aknlists.h>

// User includes
#include "MemSpyViewBase.h"

// Classes referenced


class CMemSpyViewSystemConfig : public CMemSpyViewBase
    {
public:
    CMemSpyViewSystemConfig( CMemSpyEngine& aEngine, MMemSpyViewObserver& aObserver );
    ~CMemSpyViewSystemConfig();
    void ConstructL( const TRect& aRect, CCoeControl& aContainer, TAny* aSelectionRune = NULL );

public: // From CMemSpyViewBase
    void RefreshL();
    TMemSpyViewType ViewType() const;
    CMemSpyViewBase* PrepareParentViewL();
    CMemSpyViewBase* PrepareChildViewL();

private: // From CMemSpyViewBase
    void SetListBoxModelL();

private: // Internal enumerations 

    enum TMemSpyDisplayMode
	    {
	    ENone,
	    EGray2,
	    EGray4,
	    EGray16,
	    EGray256,
	    EColor16,
	    EColor256,
	    EColor64K,
	    EColor16M,
	    ERgb,
	    EColor4K,
	    EColor16MU,
	    EColor16MA,
	    EColor16MAP,
	    EColorLast
	    };

private: // Internal methods
    static void GetManufacturer( TDes& aBuf );
    static void GetDeviceFamily( TDes& aBuf );
    static void GetCPU( TDes& aBuf );
    static void GetCPUABI( TDes& aBuf );
    static void GetStartupReason( TDes& aBuf );
    static void GetKeyboard( TDes& aBuf );
    static void GetMachineUid( TDes& aBuf );
    static void GetDisplayType( TDes& aBuf );
    static void GetDisplayMode( TDes& aBuf, TMemSpyDisplayMode aMode );
    //
    static TInt GetHALValue( HALData::TAttribute aAttribute, TInt& aValue );
    TInt GetHALValueAsStringL( HALData::TAttribute aAttribute, const TDesC& aCaption, const TDesC* aSuffix = NULL );
    TInt GetHALValueAsNumericL( HALData::TAttribute aAttribute, const TDesC& aCaption, const TDesC* aSuffix = NULL, TInt aWidth = -1 );
    TInt GetHALValueAsHexL( HALData::TAttribute aAttribute, const TDesC& aCaption, const TDesC* aSuffix = NULL );
    TInt GetHALValueAsYesNoL( HALData::TAttribute aAttribute, const TDesC& aCaption, const TDesC* aSuffix = NULL );
    TInt GetHALValueAsDriveLetterL( HALData::TAttribute aAttribute, const TDesC& aCaption, const TDesC* aSuffix = NULL );

private:
    void AddItemL( const TDesC& aCaption, const TDesC& aValue, const TDesC* aSuffix = NULL );
    void AddItemL( TDriveNumber aDrive, const TDesC& aCaption, const TDesC* aSuffix = NULL );

private: // Data members
    CDesCArrayFlat* iModel;
    };


#endif
