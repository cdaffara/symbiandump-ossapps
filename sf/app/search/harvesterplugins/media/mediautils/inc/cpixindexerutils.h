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
* Description:  Indexer utility create indexes for each drive
 *
*/


#ifndef CPIXINDERUTILS_H_
#define CPIXINDERUTILS_H_
#include <e32base.h>
#include <f32file.h>
#include <rsearchserversession.h>

//Forward declaration
class CCPixIndexer;

NONSHARABLE_CLASS(CCPixIndexerUtils) : public CBase
    {
public:
    /*
     * Constructrion
     * @param RSearchServerSession& aSession valid search session
     * @return instance of Indexer util 
     */
    static CCPixIndexerUtils* NewL(RSearchServerSession& aSession);
    /*
     * Constructrion
     * @param RSearchServerSession& aSession valid search session
     * @return instance of Indexer util 
     */
    static CCPixIndexerUtils* NewLC(RSearchServerSession& aSession);
    /*
     * Destruction
     */
    virtual ~CCPixIndexerUtils();
    /*
     * Mount all available drives 
     * TDesC& aBaseAppClassGeneric generic base app class
     * TDesC& aPath path of database e.g. \\root\\media\\audio
     */
    void MountAllAvailableDriveL(const TDesC& aBaseAppClassGeneric,const TDesC& aPath);
    /*
     * Unmount all available drives 
     * TDesC& aBaseAppClassGeneric generic base app class
     * TBool aUndefine needs to be undefined or not
     */    
    void UnMountAllDrivesL(const TDesC& aBaseAppClassGeneric,TBool aUndefine = EFalse);
    /*
     * Mount drive
     * TDriveNumber aMedia drive to be mounted
     * TDesC& aBaseAppClassGeneric generic base app class
     * TDesC& aPath path of database
     */
    void MountDriveL(TDriveNumber aMedia,const TDesC& aBaseAppClassGeneric,const TDesC& aPath);
    /*
     * UnMount drive
     * TDriveNumber aMedia drive to be mounted
     * TDesC& aBaseAppClassGeneric generic base app class
     * TBool aUndefine needs to be undefined or not
     */
    void UnMountDriveL(TDriveNumber aMedia,const TDesC& aBaseAppClassGeneric,TBool aUndefine = EFalse);
    /*
     * Checks Media is removable or not
     * TDriveNumber& aDrive drive to check
     * @return ETrue if removable else EFalse
     */
    TBool IsMediaRemovableL(const TDriveNumber& aDrive);
    /*
     * Checks drive can be mounted not
     * TDriveNumber aDrive drive to check
     * @return ETrue if mountable else EFalse
     */
    TBool IsDriveCanbeMountedL(TDriveNumber aDrive);
    /*
     * Get Indexer for drive
     * TDriveNumber aDrive drive to check
     * @return instance of CCPixIndexer* ownership not passed
     */
    CCPixIndexer* GetIndexerFromDrive(TDriveNumber aMedia);
    /*
     * Get Indexer From media Id
     * TUint aUniqueID media Id 
     * @return instance of CCPixIndexer* ownership not passed
     */
    CCPixIndexer* GetIndexerFromMediaId(TUint aUniqueID);
    /*
     * Checks if Drive already mounted
     * TDriveNumber aMedia 
     * @return ETrue if mounted else EFalse
     */
    TBool IsAlreadyAvilable(TDriveNumber aMedia);
    /*
     * Reset all indexes
     */
    void ResetAllL();
    /*
     * Get array of all mounted drives
     * @return array of mounted drives
     */
    RArray<TDriveNumber>& GetMountedDriveList();
    /* 
     * Internale method GetDrive from media Id
     * @param TUint aUniqueID media Id
     * TDriveNumber& drive number in return
     */
    void GetDriveFromMediaId(TUint aUniqueID,TDriveNumber&);
private:
    /*
     * Default constructor
     */
    CCPixIndexerUtils();
    /*
     * Second phase constructor
     */
    void ConstructL(RSearchServerSession& aSession);
    /*
     * Form base app class for media
     * @param TDriveNumber aMedia media drive
     * @param TDesC& aBaseAppClassGeneric base app class
     * @param TDes& aBaseAppClass generated value of base app class
     * @return KErrNone on success else error 
     */
    TInt FormBaseAppClass(TDriveNumber aMedia, const TDesC& aBaseAppClassGeneric, TDes& aBaseAppClass);
    /*
     * Database path for drive
     * @param TDriveNumber aMedia media drive
     * @param TDesC& aPath path of database
     * @return database path generated
     */
    HBufC* DatabasePathLC(TDriveNumber aMedia,const TDesC& aPath);
    /*
     * Remove unmounted drive from Mounted list
     * @param TDriveNumber aMedia drive
     */
    void RemoveUnmountedDrive(TDriveNumber aMedia);
    
    /*
    * Remove unmounted drive database
    * @param TDriveNumber aMedia drive
    * @param TDesC& aPath path of database
    */
   void RemoveUnmountedDatabaseL(TDriveNumber aMedia, const TDesC& aPath);
       
private:
    RArray<TDriveNumber>  iMountedDrives; //Array of mounted drives
    CCPixIndexer *iIndexer[EDriveZ + 1];//Array of all Indexers
    RFs           iFs; //File session
    RSearchServerSession iSearchSession; //Search session
    
    //for helping with testing.
    #ifdef HARVESTERPLUGINTESTER_FRIEND
        friend class CHarvesterPluginTester;
    #endif
    };

#endif /* CPIXINDERUTILS_H_ */
