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
* Description: 
*
*/

#ifndef MDSITEMENTITY_H_
#define MDSITEMENTITY_H_

#include <e32base.h>
#include <f32file.h>

class CMDSEntity : public CBase
    {
public:
    /**
    * Creates new instance of CMDSEntity based class.
    * @return CMDSEntity* Newly created instance.
    */
    static CMDSEntity* NewLC();

    /**
    * Creates new instance of CMDSEntity based class.
    * @return CMDSEntity* Newly created instance.
    */
    static CMDSEntity* NewL();

    /**
    * Creates new instance of CMDSEntity based class.
    * @param aKey database index key
    * @param aUri aUri
    * @param aDrive drive number
    * @return CMDSEntity* Newly created instance.
    */
    static CMDSEntity* NewLC(TInt aKey, const TDesC& aUri, TDriveNumber& aDrive);

    /**
    *Destructor
    */
    virtual ~CMDSEntity();
    
private:

    /**
    * Two-Phase Constructor 
    */  
    CMDSEntity();
    
    /**
    * Over-loaded Constructor
    */  
    CMDSEntity(TInt aKey, const TDesC& aUri, TDriveNumber& aDrive);

public:
    /**
    * Returns database key.
    * @return TInt key value.
    */
    TInt Key() const;

    /**
    * Returns the Uri of Entity.
    * @return TDesC .
    */
    const TDesC& Uri() const;

    /**
     * Returns the Drive number of Entity. 
     * @return TDriveNumber reference of the Segmneted Content 
     */
    TDriveNumber DriveNumber();

    /**
    * Set Entity data.
    * @param aKey database index
    * @param aUri URI of entity
    * @param aDrive drive
    */
    void Set(TInt aKey, const TDesC& aUri, TDriveNumber& aDrive);

    /**
     * Set the Key of entity. 
     */
    void Setkey(TInt aKey);
    
    /**
    * Set Uri of entity
    * @param aUritext of the note.
    */
    void SetUri(const TDesC& aUri);

    /**
    * Set Drive number of entity
    * @param aDrive of entity
    */
    void SetDrive(TDriveNumber& aDrive);
    
    /*
     * Reset the state of entity variables
     */
    void Reset();
    
    /**
    * Returns the Path of Entity with root drive.
    * @return TDesC .
    */
    const TDesC& GetPath();

private:
    
    // The Unique Identifier
    TInt iKey; //Mapped to ObjectId of MDS item
    
    //URI Can never exceed System Limit
    TBuf<KMaxFileName> iUri;  
    
    //Drive number on which items stored
    TDriveNumber  iDriveNumber;
    
    //Absolute path with root drive 
    TBuf<KMaxFileName> iPath;
    };

#endif /* MDSITEMENTITY_H_ */
