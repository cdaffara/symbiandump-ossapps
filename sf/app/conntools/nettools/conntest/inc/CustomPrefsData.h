/*
 * Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
 * Description: CSettingsData stores the settings for ConnTest
 *
 */

#ifndef CUSTOMPREFSDATA_H
#define CUSTOMPREFSDATA_H

// INCLUDES

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
 * Class for encapsulating Custom Preferences data.
 */
class CCustomPrefsData : public CBase
    {
public:
    static CCustomPrefsData* NewL();
    ~CCustomPrefsData();

private:
    CCustomPrefsData();
    void ConstructL();

public:
    /** SNAP purpose. */
    // CMManager::TSnapPurpose iSnapPurpose;
    TInt iSnapPurpose;
    /** SNAP id. */
    TInt iSnapId;
    /** IAP id. */
    TInt iIapId;
    /** Bearer set. */
    TInt iBearerSet;
    /** UI behaviour. */
    // TUint32 iUiBehaviour;
    TInt iNoteBehaviour;
    /** Indicates whether UI disconnect dialog is shown. */
    TBool iDisconnectDialog;
    /** Indicates whether UI connection selection dialog is shown. */
    TBool iConnSelectionDialog;
    /** Indicates whether forced roaming is enabled. */
    TBool iForcedRoaming;
    };

#endif

// End of File
