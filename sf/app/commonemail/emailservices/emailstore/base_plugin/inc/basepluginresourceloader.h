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
* Description: Library to handle resource loading when 
*              CCoeEnv is not available.
*/

#ifndef __BASEPLUGINRESOURCELOADER_H__
#define __BASEPLUGINRESOURCELOADER_H__

//  INCLUDES
#include <e32base.h>
#include <f32file.h>
#include <barsc.h>
#include <biditext.h>

/**
 * Library to handle resource loading when CCoeEnv is not available.
 */
class CResourceLoader : public CBase
    {
public:

    /**
     * A constructor.
     * @param aName a resource name
     * @return created object.
     */
    IMPORT_C static CResourceLoader* NewL( const TDesC& aName );

    /**
     * Destructor
     */
    virtual ~CResourceLoader();

    /**
    * @return reference to opened file session
    */
    IMPORT_C RFs& Fs();
    
    /**
     * @param aResourceId a resource key of a resource defined as LBUF
     * @return resource value from resource, instance ownership is transferred
     */
    IMPORT_C HBufC* LoadLC(TInt aResourceId );

    /**
     * @param aResourceId a resource key of a resource defined as TBUF
     * @return resource value from resource, instance ownership is transferred
     */
    IMPORT_C HBufC* Load2L( TInt aResourceId );

    /**
     * @param aResourceId a resource key of a resource defined as TBUF
     * @return resource value from resource, instance ownership is transferred
     */
    IMPORT_C HBufC* Load2LC( TInt aResourceId );
    
    /**
     * @param aResourceId a resource key of a resource defined as TBUF
     * @param aString a string value used as parameter in formatting
     * @return resource value from resource, instance ownership is transferred
     */
    IMPORT_C HBufC* Load2LC( TInt aResourceId, const TDesC& aString );

    /**
     * @param aResourceId a resource key of a resource defined as TBUF
     * @param aString a string value used as parameter in formatting
     * @return resource value from resource, instance ownership is transferred
     */
    IMPORT_C HBufC* Load2L( TInt aResourceId, const TDesC& aString );

    /**
     * @param aResourceId a resource key of a resource defined as TBUF
     * @param aInt a number to be inserted into the string
     * @return resource value from resource, instance ownership is transferred
     */
    IMPORT_C HBufC* Load2LC( TInt aResourceId, TInt aInt );

    /**
     * @param aResourceId a resource key of a resource defined as TBUF
     * @param aInt a number to be inserted into the string
     * @return resource value from resource, instance ownership is transferred
     */
    IMPORT_C HBufC* Load2L( TInt aResourceId, TInt aInt );

    /**
     * @param aReader a reader
     * @param aResourceId a resource key
     * @return pointer to resource buffer placed on leave stack
     */
    IMPORT_C HBufC8* CreateResourceReaderLC( TResourceReader& aReader, TInt aResourceId ) const;
    
private:

    /**
    * A constructor
    */
    CResourceLoader();

    /**
    * A 2nd phase constructor
    * @param aName a HTML macro, e.g. lt
    */
    void ConstructL( const TDesC& aName );

    TInt GetSubStringCount( const TDesC& aText );
    HBufC* ResolveSubStringDirsL( TDes& aText, TInt aCount, TBool* aMarker );
    void RemoveNoDirMarkers(TDes& aText);
    TBidiText::TDirectionality DirectionalityL(const TDesC& aText, TBool* aFound);
    TBidiText::TDirectionality ResolveDirectionality(TDes& aText, TBool* aFound);
    HBufC* FormatStringL(const TDesC& aSource, const TDesC& aKey, const TDesC& aSubs, TBidiText::TDirectionality aDir);
    HBufC* FormatStringL( const TDesC& aSource, const TDesC& aKey, const TDesC& aSubs,
                          TBidiText::TDirectionality aDirectionality, TInt& aParamCount, TInt aSubCount );
    TInt Formater( TDes& aDest, const TDesC& aSource, const TDesC& aKey,
                   const TDesC& aSubs, TBidiText::TDirectionality aDirectionality );
    HBufC* ResolveSubStringL(TDes& aText, TBool* aMarker);
    TInt GetParamCount(const TDesC& aText, TInt aIndex = -1);

private:

class LanguageSpecificNumberConverter
    {
public:
    static void Convert(TDes &aDes);
    static void ConvertToWesternNumbers(TDes &aDes);
    static TBool IsConversionNeeded();
    };

private:
    RFs iFs;
    RResourceFile iResFile;
    };


#endif  // __BASEPLUGINRESOURCELOADER_H__
