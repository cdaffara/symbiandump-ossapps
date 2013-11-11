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




#ifndef NOTIFYCHANGE_H_
#define NOTIFYCHANGE_H_

//System Includes
#include <e32base.h>
#include <f32file.h>
#include <opensystemtrace.h>

/*
 * This Class raise a NotifyRequest for a particular path
 * & startes monitor file system with RFs::NotifyChange. 
 * It receives notifications when a change occurs,
 * it will try to list all the files from that path in ESortByDate order.
 *  
 */

class CNotifyChange : public CActive
    {
 
public:
    // Wrapper function to invoke the constructor
    static CNotifyChange* NewL(const TChar aDriveLetter);
    
    /*
     * wil issue a Notify Change request to FileSystem for particular path
     */
    
    void StartFilesystemMonitor();
    // Destructor;
    ~CNotifyChange();

protected:
    // Constructor
    CNotifyChange();
    // 2-Phase Constructor
    void ConstructL(const TChar aDriveLetter);
    /*
     *will List the  registration resources from a specified path.
     */
    void GetFilesFromDirL();
    /*
     * will register the resource with APPARC using SWI API's
     */
    void RegisterwithApparc(TDesC& aFileName);
   
    // Inherited from CActive.
    virtual void RunL();            
    virtual void DoCancel();
    virtual TInt RunError(TInt aError);
    
private:
    RFs iFs;
    HBufC* iPath;
    };
#endif /* NOTIFYCHANGE_H_ */
