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


#include <badesca.h>
#include <bautils.h>
#include <e32std.h>
#include <f32file.h>
#include <caf/content.h>
#include <imageconversion.h>

#include "caiwimageprintif.h"
#include "cimgpprintutils.h"

using namespace ContentAccess;

#ifndef __WINS__
_LIT( KResourceFile, "aiwprintingprovider.rsc" );
_LIT(KResourceFilePath, "z:\\resource\\");
#else
_LIT(KResourceFile, "z:\\resource\\aiwprintingprovider.rsc");
#endif

_LIT8( KJpegFileType, "image/jpeg" );
_LIT( KJpegFileType16, "image/jpeg" );

CAiwImagePrintIf::~CAiwImagePrintIf()
    {
    iEikEnv.DeleteResourceFile( iResourceOffset );
    }

CAiwImagePrintIf::CAiwImagePrintIf():iEikEnv( *CEikonEnv::Static() )
    {
    }

void CAiwImagePrintIf::ConstructL()
    {
#ifndef __WINS__
    TFileName file;
    TParse parse;
    parse.Set( file, NULL, NULL );
    file.Append(KResourceFilePath);
    file.Append( KResourceFile );
#else
    TFileName file( KResourceFile );
#endif
    BaflUtils::NearestLanguageFile( iEikEnv.FsSession(), file );
    iResourceOffset = iEikEnv.AddResourceFileL( file );
    }

// Get image array
CDesCArrayFlat* CAiwImagePrintIf::GetImageArrayL( const CAiwGenericParamList& aInParamList )
    {
    CDesCArrayFlat* imageFiles = new (ELeave) CDesCArrayFlat(5); //image list for the application
    CleanupStack::PushL( imageFiles );
    TInt index = 0;
	const TAiwGenericParam* param = aInParamList.FindFirst(index, 
			EGenericParamFile, 
            EVariantTypeDesC);
    TFileName filename;

	while ( index != KErrNotFound )
		{
		filename.Copy( param->Value().AsDes() );

		if ( IsPrintingSupportedL( filename ) )
			{
			imageFiles->AppendL( filename );
			}
		param = aInParamList.FindNext(index, 
			EGenericParamFile, 
            EVariantTypeDesC);
		}	

    CleanupStack::Pop( imageFiles );
	return imageFiles;
    }

TBool CAiwImagePrintIf::IsPrintingSupportedL( const CAiwGenericParamList& aInParamList )
    {  
    TInt index(0);
    const TAiwGenericParam* param = aInParamList.FindFirst( index, EGenericParamMIMEType );
	TBool printable( EFalse );
	
	// Currently Media Gallery does not offer MIME-parameter in aInParamList (Fix exists).
	// Because of that there is checked directly from files if printing is supported
	// This is slow and because of this should be deleted when MG offers MIMEs as parameter.
    if ( index == KErrNotFound )	
        {        
        index = 0;
   	    const TAiwGenericParam* param = aInParamList.FindFirst(index, EGenericParamFile, EVariantTypeDesC);

        //Check if there is any printable images available. At least image must be
        //JPEG and it also must be unprotected to be printable.
        while ( index != KErrNotFound && !printable)
            {
            TRAP_IGNORE( printable = IsPrintingSupportedL( param->Value().AsDes() ));
   	        param = aInParamList.FindNext(index, EGenericParamFile, EVariantTypeDesC);        
            }
        return printable;    
        }
	//End of "To be removed" section
	
	while ( index != KErrNotFound && !printable )
		{
	   	if ( param->Value().TypeId() == EVariantTypeDesC &&
			 param->Value().AsDes() == KJpegFileType16 )
	    	{
	    	// MIME-type parameter follows filename parameter in parameter list.
	    	// Because of that previous item in list is used.
	    	if (index > 0)
	    	    {    	    
    	      	printable = !IsProtectedL( aInParamList[index-1].Value().AsDes() );
	    	    }
	    	}

	    if ( !printable )
	        {        
    	   	param = aInParamList.FindNext(index, EGenericParamMIMEType);
	        }
       	}    
	     
    return printable;	 
    }

TBool CAiwImagePrintIf::IsPrintingSupportedL( const TDesC& aFileName )
    {       
    //1. check: Mimetype should be correct (JPEG)
    TBool printable = CheckMIMETypeL( KJpegFileType, aFileName ); 
       
    if (printable)
        {
        //2. check: file should not be protected
        printable = !IsProtectedL( aFileName ); 
        }
        
    return printable;                                      
    }

TBool CAiwImagePrintIf::IsProtectedL( const TDesC& aFileName )
    {
    CContent* content = NULL;
		content = CContent::NewLC( aFileName );
		TInt isDrmProtected( 0 );
		TInt err = content->GetAttribute( EIsProtected, isDrmProtected );
		CleanupStack::PopAndDestroy(); // content
    return err ? ETrue : isDrmProtected;  
    }
    
    
TBool CAiwImagePrintIf::CheckMIMETypeL( const TDesC8& aMimeType,
											   const TDesC& aFileName )
    {
    TBuf8<128> mimeType(0);
    CImageDecoder::GetMimeTypeFileL(iEikEnv.FsSession(), aFileName, mimeType);

    // Check that MIME-type matches (compare returns 0), otherwise return EFalse
    return mimeType.CompareF( aMimeType ) ? EFalse : ETrue;
    }

//  End of File
