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
 * Description: Message Indicator private class. 
 * Gets the indicaton information from conversation server.
 *
 */

#ifndef MSGINDICATOR_PRIVATE_H
#define MSGINDICATOR_PRIVATE_H

#include <e32base.h>
#include "msginfodefs.h"

// Forward Declarations
class MsgIndicator;
class CCSRequestHandler;
class IndicatorData;

/**
 * Message indicator class. 
 * Handles client request and showing the indications. 
 */
class MsgIndicatorPrivate : public CBase
{
public:
    /**
     * Constructor
     */
    MsgIndicatorPrivate(MsgIndicator* indicator);

    /**
     * Destructor
     */
    ~MsgIndicatorPrivate();

   /**
    * Get the information required to show the inidctaions
    * @pamam indicatorData indicator data which will be filled
    */
    void getIndicatorInfo(MsgInfo& indicatorData);

    /**
     * Get the information required to show the inidctaions. 
     * Calls conversation sever to fetch the information.
     * @param indicatorData indicator data which will be filled.
     */
    void getIndicatorInfoL(MsgInfo& indicatorData);
    
private : 
    /**
     * Does the initializations.
     */
    void initL();
		
private:
    /**
     * Pointer to msgnotifier
     */
    MsgIndicator* q_ptr;  

    /**
     * Conversation server object
     */
    CCSRequestHandler* mCvServer;
};

#endif // MSGINDICATOR_H

