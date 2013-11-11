/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Declaration for CMMCScBkupStateFactory
*     
*
*/

#ifndef __CMMCSCBKUPSTATEFACTORY_H__
#define __CMMCSCBKUPSTATEFACTORY_H__

// User includes
#include "MMCScBkupStateIds.h"
#include "MMCScBkupOperations.h"

// Classes referenced
class CMMCScBkupState;
class MMMCScBkupDriver;

// Type definitions
typedef RPointerArray<CMMCScBkupState> RMMCScBkupStateArray;



/**
*
*
* @since 3.0
*/
class CMMCScBkupStateFactory : public CBase
    {
    public:
        /**
        *
        */
        static CMMCScBkupStateFactory* FactoryByOperationTypeLC( TMMCScBkupOperationType aOperationType );

    protected:

        /**
        *
        */
        CMMCScBkupStateFactory();

    public: // From CMMCScBkupStateFactory

        /**
        * 
        */
        virtual CMMCScBkupState* GetStateLC(TMMCScBkupStateId aCurrentState, MMMCScBkupDriver& aDriver) = 0;
    };



/**
*
*
* @since 3.0
*/
NONSHARABLE_CLASS(CMMCScBkupStateFactoryBackup) : public CMMCScBkupStateFactory
    {
    public:
        static CMMCScBkupStateFactoryBackup* NewL();
    
    private:
        CMMCScBkupStateFactoryBackup();

    public: // From CMMCScBkupStateFactory
        CMMCScBkupState* GetStateLC(TMMCScBkupStateId aRequiredType, MMMCScBkupDriver& aDriver);
    };




/**
*
*
* @since 3.0
*/
NONSHARABLE_CLASS(CMMCScBkupStateFactoryRestore) : public CMMCScBkupStateFactory
    {
    public:
        static CMMCScBkupStateFactoryRestore* NewL();
    
    private:
        CMMCScBkupStateFactoryRestore();

    public: // From CMMCScBkupStateFactory
        CMMCScBkupState* GetStateLC(TMMCScBkupStateId aRequiredType, MMMCScBkupDriver& aDriver);
    };




#endif // __CMMCSCBKUPSTATEFACTORY_H__

//End of File
