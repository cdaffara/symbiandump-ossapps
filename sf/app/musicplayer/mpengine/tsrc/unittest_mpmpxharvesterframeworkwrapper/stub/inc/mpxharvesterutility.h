/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Harvester Utility stub for testing mpmpxframeworkwrapper
*
*/


#ifndef MMPXHARVESTERUTILITY_H
#define MMPXHARVESTERUTILITY_H

#include <bamdesca.h>


class MMPXHarvesterUtility
{

public:
    
    //Stub Functions
    MMPXHarvesterUtility();
    ~MMPXHarvesterUtility();
    void ScanL();
    void CancelScanL();
    void CheckForSystemEventsL();
    void Close();
    
public:
    TBool   iScan;
    TBool   iCheckSystemEvents;
    TBool   iLeave;
  
};

//  Utility Factory class to create the utility

class CMPXHarvesterFactory : CBase
{
public:
    
    //Factory stub function to create the utility

    static MMPXHarvesterUtility* NewL();
};


#endif // MMPXHARVESTERUTILITY_H
