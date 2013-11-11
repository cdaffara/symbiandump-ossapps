/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  ?Description
*
*/


#ifndef IRHTTPPOST_H
#define IRHTTPPOST_H

#include <f32file.h>
#include <mhttpdatasupplier.h>
#include <rhttptransaction.h>

class CIRSettings;

/**
 * CIRHttpPost class for posting the data
 * Reading from the file as chunks of data
 * using MHTTPDataSupplier - Data supplier 
 */
NONSHARABLE_CLASS ( CIRHttpPost ) : public CBase, public MHTTPDataSupplier
    {
public:

    /** 
     * CIRHttpPost::NewL
     * 1st phase constructor
     */
    static CIRHttpPost* NewL( const RHTTPTransaction& aHttpTransaction );

    /** 
     * CIRHttpPost::~CIRHttpPost
     * C++ default destructor
     */
    virtual ~CIRHttpPost();

    /** 
     * CIRHttpPost::GetNextDataPart
     * Obtain a data part from the supplier. 
     * The data is guaranteed to survive until a call
     * is made to ReleaseData(). 
     */
    virtual TBool GetNextDataPart( TPtrC8& aDataPart );

    /** 
     * ReleaseData()
     * Release the current data part being held at the data supplier
     */
    virtual void ReleaseData();

    /** 
     * OverallDataSize()
     * Obtain the overall size of the data being supplied, 
     * if known to the supplier
     */
    virtual TInt OverallDataSize();

    /** 
     * Reset()
     * Reset the data supplier
     * otherwise returns KErrNone
     */
    virtual TInt Reset();

    /** 
     * GetFileToPostL()
     * Prepares the data to be posted from the log file
     */
    void GetFileToPostL( TInt *aFileSize );

    /** 
     * CloseLogFile ()
     * Used to Close the log file LogUsage.gz externally from other sources (bug-fixing)
     */
    void CloseLogFile();

    /** 
     * SetTransaction().
     * Sets transaction.
     *
     * @param aHttpTransaction Reference to RHTTPTransaction.
     */
    void SetTransaction( const RHTTPTransaction& aHttpTransaction );

private:

    /** 
     * CIRHttpPost::CIRHttpPost
     * C++ default constructor
     * @param aHttpTransaction Reference to RHTTPTransaction
     */
    CIRHttpPost( const RHTTPTransaction& aHttpTransaction );

    /** 
     * CIRHttpPost::ConstructL
     * 2nd phase constructor
     */
    void ConstructL();

    /** 
     * CIRHttpPost::GetDataPartFromFileL
     * Obtain a data part from the browse log file.
     *
     * @param aDataPart Data from browse log to be returned.
     */
    void GetDataPartFromFileL( TDes8& aDataPart );

private:

    /**
     * Browse log file.
     */
    RFile iLogFile;

    /**
     * Total size of the post data file
     */
    TInt iTotalsize;

    /**
     * set the initial data sent
     */
    TInt iStartPos;

    /**
     * remaining size of data to be sent
     */
    TInt iRemainingLength;

    /**
     * object to HTTP Transaction 
     */
    RHTTPTransaction iTransaction;

    /**
     * Data part buffer.
     */
    RBuf8 iPartPostBuffer;

    /**
     *central repository settings handle
     */
    CIRSettings* iSettings;
    };

#endif //IRHTTPPOST_H

