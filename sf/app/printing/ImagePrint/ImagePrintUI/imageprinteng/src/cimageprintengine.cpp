/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies).
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


#include <centralrepository.h>

#include "imageprintprivatecrkeys.h"
#include "cimageprintengine.h"
#include "ciffactory.h"
#include "crealfactory.h"
#include "clog.h"
#include "mprintsettings.h"

// CONSTRUCTION
EXPORT_C CImagePrintEngine* CImagePrintEngine::NewL(
    CDesCArrayFlat* aImageFiles )
    {
    CImagePrintEngine* self = CImagePrintEngine::NewLC( aImageFiles );
    CleanupStack::Pop();	// self

    return self;
    }

EXPORT_C CImagePrintEngine* CImagePrintEngine::NewLC(
    CDesCArrayFlat* aImageFiles )
    {
    CImagePrintEngine* self = new ( ELeave ) CImagePrintEngine;
    CleanupStack::PushL( self );
    self->ConstructL( aImageFiles );
    return self;
    }

// Destructor (virtual by CBase)
CImagePrintEngine::~CImagePrintEngine()
    {
    delete iIFFactory;
    delete iFiles;   
    iFileArray.ResetAndDestroy();
    iFileArray.Close();
    delete iCRSession;
    }

// Default constructor
CImagePrintEngine::CImagePrintEngine()
    {
    }

// Second phase construct
void CImagePrintEngine::ConstructL(
    CDesCArrayFlat* aImageFiles )
    {
    CleanupStack::PushL( aImageFiles );
    iIFFactory = CRealFactory::NewL( this );
    CleanupStack::Pop( aImageFiles );
    SetImageArrayL( aImageFiles );

    iCRSession  = CRepository::NewL( KCRUidImagePrint );
    }

// Returns the reference to interface factory
EXPORT_C CIFFactory& CImagePrintEngine::InterfaceFactory()
    {
    return *iIFFactory;
    }

// Initializes the image array
EXPORT_C void CImagePrintEngine::SetImageArrayL(
    CDesCArrayFlat* aArray )
    {
    delete iFiles;
    iFiles = aArray;

    if ( iFiles )
        {
        iFileArray.ResetAndDestroy();
        TInt i( 0 );
        TInt num( iFiles->Count() );
        for ( i = 0; i < num; i++ )
            {
            TFileName* tf = CreateFileNameL();
            CleanupStack::PushL( tf );
            *tf = iFiles->MdcaPoint( i );
            User::LeaveIfError( iFileArray.Append( tf ) );
            CleanupStack::Pop( tf );
            }
        
        MPrintSettings* settings =
            static_cast<CRealFactory*>( iIFFactory )->SettingsIF();
        RArray<TInt> numberOfCopiesArray;
        CleanupClosePushL( numberOfCopiesArray );
        for( TInt i=0; i < iFiles->Count(); ++i )
            {
            numberOfCopiesArray.Append( 1 );
            }
        if( settings )
            {
            settings->SetNumberOfCopiesL( numberOfCopiesArray );
            }
        CleanupStack::PopAndDestroy();	// numberOfCopiesArray
        }
    }

// Returns file array to be passed to server
EXPORT_C RPointerArray<TDesC>& CImagePrintEngine::FileArray()
    {
    return iFileArray;
    }

// Creates new TFileName for file array
TFileName* CImagePrintEngine::CreateFileNameL()
    {
    TFileName* fileName = 0;
    fileName = new (ELeave) TFileName;
    User::LeaveIfNull( fileName );
    return fileName;
    }

// Restarts Image Print engine
EXPORT_C void CImagePrintEngine::RestartEngine()
    {
    iIFFactory->RestartEngine();
    }

//  End of File
