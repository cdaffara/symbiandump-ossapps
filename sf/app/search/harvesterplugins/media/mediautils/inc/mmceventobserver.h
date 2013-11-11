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
* Description:  mmc instertion removal event observer callback
 *
*/


#ifndef MMCEVENTOBSERVER_H_
#define MMCEVENTOBSERVER_H_

class MMMCEventObserver
    {
public:
    //Observer implement this to get MMC events on mounted drive
    //aMMCInsertRemoval is ETrue when MMC inserted EFalse otherwise
    /*
     * HandleMMCEventL
     * @param TDriveNumber aDrive drive number of MMC
     * @param TBool aMMCInsertRemoval ETrue if inserted else EFalse
     */
    virtual void HandleMMCEventL(const TDriveNumber aDrive,const TBool aMMCInsertRemoval) = 0;
    };
#endif /* MMCEVENTOBSERVER_H_ */
