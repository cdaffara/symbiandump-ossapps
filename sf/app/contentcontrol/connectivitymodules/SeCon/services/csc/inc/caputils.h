/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CapUtil header file
*
*/


// INCLUDES

#ifndef _CAPUTILS_H_
#define _CAPUTILS_H_

#include "capability.h"

class RFs;

//
// TMemoryInfo
//
NONSHARABLE_CLASS( TMemoryInfo )
    {
public:
    TChar    iDriveLetter;
    TInt     iDriveNum;
    TBuf<16> iLocation;
    TInt64   iFree;
    TInt64   iUsed;
    TBool    iShared;
    TInt64   iFileSize;
    TInt64   iFolderSize;
    TInt     iFileNameSize;
    TInt     iFolderNameSize;
    TBool    iCaseSensitivity;
    TUint    iDriveStatus;
    TInt     iMemNr;
    };

//
// CapUtil 
//
NONSHARABLE_CLASS( CapUtil )
    {
public:
    /**
     * Gets default root path from pathinfo. 
     * @param aText
     * @param aType
     * @return none
     */
    static void GetDefaultRootPathL( RFs& aFs, TDes& aRootPath );
    
    /**
     * Gets memory type. 
     * @param aFs
     * @param aMemoryType
     * @param aDrive
     * @return none
     */
    static void GetMemoryType( RFs& aFs, TDes& aMemoryType, const TInt aDrive );
    
    /**
     * Gets language
     * @param aText
     * @return none
     */
    static void GetLanguage( TDes& aText );
    
    /**
     * Gets SW version, SW version date  and device model from SysUtil. 
     * @param aVersion SW version
     * @param aDate SW date
     * @param aModel Device model
     * @return none
     */
    static void GetSWVersionL( TDes& aVersion, TDes& aDate, TDes& aModel );
    
    /**
     * Gets phone manufacturer from HAL. In case manufacturer is not known,
     * empty value is used.
     * @param aText Manufacturer
     * @return none
     */
    static void GetManufacturer( TDes& aText );
    
    /**
     * Get memory information for one drive.
     * @param aFs RFs handle
     * @param aDriveNumber drive number
     * @param aInfo Memory information
     * @return none
     */
    static void GetMemoryInfoL( const RFs& aFs, const TInt aDriveNumber, TMemoryInfo& aInfo );
    
    /**
     * Initializes TMemoryInfo.
     * @param aInfo TMemoryInfo to be initialized
     * @return none
     */
    static void InitMemoryInfo( TMemoryInfo& aInfo );
    
    /**
     * Get language string for aId.
     * @param aId Language id
     * @param aText Language text
     * @return none
     */
    static void GetLanguageString( TLanguage aId, TDes& aText );
    
    /**
     * Finds all files in aDir.
     * @param aFs RFs handle
     * @param aDir Directory
     * @param aList List of files in directory
     * @return none
     */
    static void GetFileListL( const RFs& aFs, const TDesC& aDir, 
                            RArray<TFileName>& aList );
    
    
    /**
     * Gets operator name, country code, network ID
     * @param aLongName
     * @param aCountryCode
     * @param aNetworkID
     * @return none
     */
    static void GetOperatorNameL( TDes& aLongName, TDes& aCountryCode, TDes& aNetworkID );
    
    /**
     * Creates a panic
     * @param aReason Panic code
     * @return none
     */
    static void Panic( TInt aReason );
    
    /**
     * String copy with lenght check.
     * @param aTarget Target string
     * @param aSource Source string
     * @return none
     */
    static void StrCopy( TDes& aTarget, const TDesC& aSource );
    
    /**
     * Function converts ínteger to string.
     * @param aText string
     * @param aNum integer
     * @return none
     */
    static void IntToStr( TDes& aText, TInt64 aNum );
    
    /**
     * Function converts string to integer. If string cannot be converted,
     * error code is returned.
     * @param aText string
     * @param aNum integer
     * @return error code
     */
    static TInt StrToInt( const TDesC& aText, TInt& aNum );
    
    /**
     * Function splits string (eg "name1, name2, name3") into substrings.
     * @param aText string
     * @param aSeparator separator character
     * @param aArray substrings
     * @return none
     */
    static void SplitL( const TDesC& aText, const TChar aSeparator, 
                        RArray<TPtrC>& aArray );
    
    /**
     * Constructs capability date as string
     * @param aText string
     * @param aTime time object
     * @return none
     */
    static void CapabilityDate( TDes& aText, const TTime aTime );
    
    /**
     * Function parses date string of the format "dd-mm-yy".
     * @param aText string
     * @return time object
     */
    static TTime ParseDateL( const TDesC& aText );
    
    /**
     * Function return TMonth presentation of integer
     * @param aNum
     * @return TMonth object
     */
    static TMonth Month( TInt aNum );
    
    /**
     * Function checks file extension.
     * @param aFile File name
     * @param aExt extension
     * @return boolean
     */
    static TBool CheckFileType( const TDesC& aFile, const TDesC& aExt );
    };

//
//  TIdStack
//
NONSHARABLE_CLASS( TIdStack )
    {
public:
    
    /**
     * Push id to the stack
     * @param aId Id number
     * @return none
     */
    void Push( TInt aId );
    
    /**
     * Pop id from the stack
     * @return id
     */
    TInt Pop();
    
    /**
     * Size of the stack
     * @return size
     */
    TInt Size() const;
    
    /**
     * Reset the stack.
     * @return none
     */
    void Reset();

private:
    TInt iPos;
    TFixedArray<TInt, KNestingLimit> iArray;
    };

#endif // CapUtils.h

// End of file
