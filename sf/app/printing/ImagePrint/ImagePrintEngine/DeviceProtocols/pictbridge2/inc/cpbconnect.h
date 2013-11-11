/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Contains the CPBConnect class definition.
*
*/


#ifndef CPBCONNECT_H
#define CPBCONNECT_H

class MPBObserver;
class CPBEvent;
class CDpsEngine;


class CPBConnect : public CActive
	{
public:
    static CPBConnect* NewL( CDpsEngine& aEngine, MPBObserver& aObserver );
    CPBConnect( CDpsEngine& aEngine, MPBObserver& aObserver );
    ~CPBConnect();    
    void ConstructL();
    

protected: // from CActive
    void RunL();
    void DoCancel();

private:
	CDpsEngine& iDpsEngine;
    MPBObserver& iObserver;    
    CPBEvent* iEvent;
	};

#endif // CPBCONNECT_H

//  End of File
