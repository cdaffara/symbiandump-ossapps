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
 * Description:  Interface that enables initializing call provider
 *               for testing purposes.    
 *
 */

#ifndef MCSPTESTCALLPROVIDER_H_
#define MCSPTESTCALLPROVIDER_H_

class CSPAudioHandlerBase;

/**
 *  Interface that enables initializing call provider
 *  with dummy audio handler for testing purposes.   
 *
 */
class MCSPTestCallProvider
    {
public:
    
    /**
     * Initializes call provider for testing purposes. 
     */
    virtual void InitializeL( const MCCPObserver& aObserver,
                              const MCCPSsObserver& aSsObserver,
                              CSPAudioHandlerBase* aAudioHandler ) = 0;
    };



#endif /* MCSPTESTCALLPROVIDER_H_ */
