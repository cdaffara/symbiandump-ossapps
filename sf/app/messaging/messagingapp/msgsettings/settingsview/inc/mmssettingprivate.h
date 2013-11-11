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
 * Description:  This provides the messaging mw interface for MMS
 *
 */

#ifndef MMSSETTINS_PRIVATE_H
#define MMSSETTINS_PRIVATE_H

#include <smutset.h>
#include <mmsclient.h>

#include "msgsettingengine.h"

/**
 * MMS setting class implements the MMSC settings
 * The settings will be stored in the central repository.
 */

class MmsSettingsPrivate : public CBase
{
public:

    /**
     * 2 phase constructor
     */
    static MmsSettingsPrivate* NewL();

    /*
     * Destructor
     */
    ~MmsSettingsPrivate();

    void setMMSRetrievalL(MsgSettingEngine::MmsRetrieval aRetrieval);

    void setAnonymousMessagesL(TBool aAnonymous);

    void setReceiveMMSAdvertsL(TBool aReceiveAdvert);

    void advanceMmsSettingsL(MsgSettingEngine::MmsRetrieval& aRetrieval,
                            TBool& aAnonymousStatus, TBool& aMmsAdvertsStatus);

    void getAllAccessPointsL(RPointerArray<HBufC>& aAccessPoints,
                            TInt& aDefaultIndex);

    void setMMSAccesspointL(TInt& aDefaultIndex);

private:

    /*
     * 1st phase Constructor 
     */
    MmsSettingsPrivate();

    /**
     * 2nd phase constructor
     */
    void ConstructL();

    /*
     * Creates the repository
     */
    void createRepositoryL();

private:
    //nothing private data
};

#endif // MMSSETTINS_PRIVATE_H

