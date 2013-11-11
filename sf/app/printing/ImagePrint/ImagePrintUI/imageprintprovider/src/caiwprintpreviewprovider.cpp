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


#include <AiwMenu.h>
#include <AiwCommon.h>
#include <AiwCommon.hrh>
#include <coemain.h>
#include <barsread.h>
#include <aiwprintingprovider.rsg>
#include <e32property.h>
#include <s32strm.h>
#include <badesca.h>
#include <s32file.h>
#include <f32file.h>
#include <apacmdln.h>
#include <apgtask.h>
#include <apaid.h>
#include <apgcli.h>
#include <w32std.h>
#include <bautils.h>
#include <eikenv.h>

#include "clog.h"
#include "caiwprintpreviewprovider.h"
#include "aiwprintingprovider.hrh"
#include "caiwimageprintif.h"
#include "imageprint.h"
#include "cimgpprintutils.h"

CAiwPrintPreviewProvider* CAiwPrintPreviewProvider::NewL()
    {

    CAiwPrintPreviewProvider* self = new( ELeave ) CAiwPrintPreviewProvider;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

CAiwPrintPreviewProvider::~CAiwPrintPreviewProvider()
    {
    delete iService;
    }

void CAiwPrintPreviewProvider::InitialiseL( MAiwNotifyCallback& /*aFrameworkCallback*/,
                                       const RCriteriaArray& /*aInterest*/ )
    {
    }


void CAiwPrintPreviewProvider::HandleServiceCmdL(const TInt& aCmdId,
		                       const CAiwGenericParamList& aInParamList,
		                       CAiwGenericParamList& aOutParamList,
		                       TUint /*aCmdOptions*/,
		                       const MAiwNotifyCallback* aCallback)

    {
    if ( aCmdId == KAiwCmdPrintPreview || aCmdId == KAiwCmdPrint )
        {
        HandleLaunchRequestL( aInParamList, aOutParamList, aCallback );
        LaunchImagePrintApplicationL();
        }
    }



void CAiwPrintPreviewProvider::InitializeMenuPaneL
        ( CAiwMenuPane& aMenuPane, TInt aIndex,
         TInt /*aCascadeId*/, const CAiwGenericParamList& aInParamList )
    {

    if ( IsPrintingSupportedL( aInParamList ) )
        {
        LOG("CAiwPrintPreviewProvider::InitializeMenuPaneL 1");
        TResourceReader reader;
        iEikEnv.CreateResourceReaderLC
            ( reader, R_AIW_PREVIEW_PRINTING_PROVIDER_MENU );
        aMenuPane.AddMenuItemsL( reader, KAiwCmdPrint, aIndex );
        CleanupStack::PopAndDestroy(); // reader
        LOG("CAiwPrintPreviewProvider::InitializeMenuPaneL 2");
        }

    }

void CAiwPrintPreviewProvider::HandleMenuCmdL
			( TInt aMenuCmdId, const CAiwGenericParamList& aInParamList,
			 CAiwGenericParamList& aOutParamList, TUint /*aCmdOptions*/,
			 const MAiwNotifyCallback* aCallback)
    {
    if ( aMenuCmdId == KAiwCmdPrintPreview || aMenuCmdId == KAiwCmdPrint )
        {
        HandleLaunchRequestL( aInParamList, aOutParamList, aCallback );
        LaunchImagePrintApplicationL();
        }
    }

void CAiwPrintPreviewProvider::HandleLaunchRequestL
					(const CAiwGenericParamList& aInParamList,
					 CAiwGenericParamList& aOutParamList,
					 const MAiwNotifyCallback* aCallback)
	{
    iConsumerInParamList = &aInParamList;
    iConsumerOutParamList = &aOutParamList;
    iConsumerCallback = aCallback;
      
    TFileName paramFile;
    CIMGPPrintUtils::GetDataFilePathL( paramFile );
    
    RFileWriteStream writeStream;
    User::LeaveIfError( writeStream.Replace( iEikEnv.FsSession(), paramFile, EFileWrite) );
    writeStream.PushL();
    aInParamList.ExternalizeL( writeStream );
    writeStream.CommitL();
    CleanupStack::PopAndDestroy( &writeStream );

    TInt err = RProperty::Define( KPrintPropertyCat, EPrintProperty, RProperty::EInt );

    if ( err != KErrAlreadyExists )
        {
        User::LeaveIfError( err );
        }
    RProperty::Set( KPrintPropertyCat,EPrintProperty,EAiwPrintCall );
	}

void CAiwPrintPreviewProvider::LaunchImagePrintApplicationL()
    {
    TUid appuid;
    appuid.iUid = 0x101FFA84;
    TRAPD( err, iService = CAknLaunchAppService::NewL( appuid, this, NULL ) );
	if ( err != KErrNone )
		{
		LOG1("CAiwPrintPreviewProvider::LaunchImagePrintApplicationL failed with: %d", err);
		}
    }

void CAiwPrintPreviewProvider::HandleServerAppExit( TInt aReason )
    {
    delete iService;
    iService = NULL;

    LOG1("CAiwPrintPreviewProvider::HandleServerAppExit: %d", aReason);

    // In the case that we want to exit also the parent application,
    // ImagePrint is exited with User::Exit() which is seen here as
    // KErrServerTerminated (-15).
    if( iConsumerInParamList && iConsumerOutParamList && iConsumerCallback && iConsumerCallback )
        {
        // Const cast is used to fix weirdness in AIW header files. MWaiNotifyCallback
        // does not define any const function but CAiwServiceIfMenu interface provides
        // const callback object.
        TRAP_IGNORE( const_cast<MAiwNotifyCallback*>(iConsumerCallback)->HandleNotifyL(
            KAiwCmdPrintPreview, KAiwEventCompleted,
            *iConsumerOutParamList, *iConsumerInParamList ));
        iConsumerInParamList = NULL;
        iConsumerOutParamList = NULL;
        iConsumerCallback = NULL;
        }
    if ( aReason == KErrServerTerminated )
        {
        User::Exit( 0 );
        }
    }

//  End of File
