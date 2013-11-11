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
* Description:  MDE session utility class
 *
*/

#ifndef MDSSESSIONOBJECTUTILS_H_
#define MDSSESSIONOBJECTUTILS_H_

#include <mdccommon.h>
#include <mdesession.h>
#include <mdequery.h>

//Forward declaration
class CMdeObject;

class CMdsSessionObjectUtils : public CBase,
                               public MMdESessionObserver,
                               public MMdEQueryObserver
    {
public:
    /*
     * NewL construction
     * @return instance of mds session utils
     */
    static CMdsSessionObjectUtils* NewL();
    /*
     * NewLC construction
     * @return instance of mds session utils
     */
    static CMdsSessionObjectUtils* NewLC();
    /*
     * Destructor
     */
    virtual ~CMdsSessionObjectUtils();
    /*
     * InitializeL 
     */
    void InitializeL(); //Open valid mde Sessions

public:
    //From MMdESessionObserver
    void HandleSessionOpened(CMdESession& aSession, TInt aError);
    void HandleSessionError(CMdESession& aSession, TInt aError);
    //From MMdEQueryObserver
    void HandleQueryNewResults(CMdEQuery& aQuery,
                               TInt aFirstNewItemIndex,
                               TInt aNewItemCount);
    void HandleQueryCompleted(CMdEQuery& aQuery, TInt aError);
    
private:
    /*
     * Second phase constructor
     */
    void ConstructL();
    /*
     * default constructor
     */
    CMdsSessionObjectUtils();
public:
    /*
     * GetSession
     * @return instance of valid mde session
     */
    CMdESession* GetSession(); //get valid session
    /*
     * GetObjectL
     * @param TItemId& aItemId mde item id to get
     * @param TDesC& aObjectDefName object definition
     * @return reference of object 
     */
    CMdEObject& GetObjectL(const TItemId& aItemId,const TDesC& aObjectDefName);
private:
    CMdESession*     iMdeSession; //Valid mde session
    CMdEObjectQuery* iQuery; // metadata query
    CActiveSchedulerWait* iWait; //Nested AO wait
    };

#endif /* MDSSESSIONOBJECTUTILS_H_ */
