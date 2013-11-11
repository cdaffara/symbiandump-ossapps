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


#ifndef CAIWIMAGEPRINTIF_H
#define CAIWIMAGEPRINTIF_H

#include <AiwServiceIfMenu.h>
#include <badesca.h>
#include <eikenv.h>

class CAiwImagePrintIf : public CAiwServiceIfMenu
{
    public:

        ~CAiwImagePrintIf();

    public:

        CDesCArrayFlat* GetImageArrayL( const CAiwGenericParamList& aInParamList );

    protected:

        CAiwImagePrintIf();

    protected: // From CAiwServiceIfMenu

        TBool IsPrintingSupportedL( const CAiwGenericParamList& aInParamList );
        
        TBool IsPrintingSupportedL( const TDesC& aFileName );
        
        TBool IsProtectedL( const TDesC& aFileName );

        TBool CheckMIMETypeL( const TDesC8& aMimeType, const TDesC& aFileName );
        
    protected: // Implementation
        void ConstructL();

    protected: // data
    
        CEikonEnv& iEikEnv; //Allow CAiwPrintingProvider and CAiwPreviewProvider
                            //to use Eikon Environment without CEikonEnv::Static()

    private: //data
    
        TInt iResourceOffset;
      
};

#endif // CAIWIMAGEPRINTIF_H

//  End of File
