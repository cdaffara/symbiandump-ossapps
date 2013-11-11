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
* Description:  CSConCSC header file
*
*/


// INCLUDES

#ifndef _SCONCSC_H_
#define _SCONCSC_H_

#include <e32base.h>

class CCapInfo;

//============================================================
// Class CSConCSC declaration
//============================================================
class CSConCSC : public CBase
{
public:
    static CSConCSC* NewL();        
    ~CSConCSC();

public:
    // Creates capability object and places it into aBuf.
    // Returns system error code
	virtual TInt CapabilityObject( CBufFlat& aBuf );

private:	
	void CapabilityObjectL( CBufFlat& aBuf );
	
	CSConCSC();
    virtual void ConstructL();
    
private: // data
    CCapInfo*   iCapInfo;
    
};

IMPORT_C CSConCSC* CreateCSConCSCL();
typedef CSConCSC* (*TSConCreateCSConCSCFunc) ();   


#endif // SCONCSC

// End of file


