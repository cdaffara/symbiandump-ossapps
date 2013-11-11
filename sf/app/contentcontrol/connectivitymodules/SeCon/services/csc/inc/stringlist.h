/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CStringList header file
*
*/


#ifndef _STRINGLIST_H_
#define _STRINGLIST_H_

// INCLUDES

#include <e32base.h>
#include <badesca.h>  // for CDesCArrayFlat
#include <f32file.h>

//
// CStringList is used for storing lines of text.
//
NONSHARABLE_CLASS( CStringList ) : public CBase
{
public:
    /**
     * Two-phase constructor. The created instance is placed to cleanup stack
     * @param aFileName Default KNullDesC
     * @return CStringList instance
     */
    static CStringList* NewLC();
    /**
     * Two-phase constructor.
     * @param aFileName Default KNullDesC
     * @return CStringList instance
     */
    static CStringList* NewL();
    /**
     * Destructor
     * @return none
     */
    ~CStringList();
    
public:
    /**
     * Returns the count of lines
     * @return The count
     */
    TInt Count() const;
    /**
     * Returns pointer to the string
     * @param aIndex The position
     * @return String pointer
     */
    TPtrC16 ReadPtr(TInt aIndex);
    /**
     * Copies a string / strings
     * @param aSource Source buffer
     * @param aStart A start position
     * @param aStop A stop position
     * @return none
     */
    void CopyL(CStringList* aSource, TInt aStart, TInt aStop);
    /**
     * Resets iLines
     * @return none
     */
    void Reset();
    /**
     * Returns the mark
     * @return Mark value
     */
    TInt Mark() const;
    /**
     * Sets mark
     * @param aMark The mark value
     * @return none
     */
    void SetMark(TInt aMark);
    /**
     * Read strings from file.
     * @param aText The tag buffer
     * @return none
     */
    void ReadFromFileL( RFs& aFs, const TDesC& aName );

private:
    /**
     * Default constuctor
     * @return none
     */
    CStringList();
    /**
     * Initializes member data
     * @param aFileName The file name
     * @return none
     */
    void ConstructL();
    /**
     * Creates a panic
     * @param aPanic Panic code
     * @return none
     */
    void Panic(TInt aPanic) const;
    /**
     * Copies the string
     * @param aTarget Target string
     * @param aSource Source string
     * @return ETrue if successful
     */
    TBool StrCopy(TDes& aTarget, const TDesC& aSource) const;
    /**
     * Writes a string
     * @param aText The string
     * @return none
     */
    void WriteL(const TDesC& aText);
    /**
     * Internalizes from file
     * @param aFileName The name of the file
     * @return none
     */
    void InternalizeL( RFs& aFs, const TDesC& aFileName );

private:
    TInt            iMark; 
    CDesCArrayFlat* iLines;
};

#endif

// End of file


