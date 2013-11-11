/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Base class for all table classes.
*
*/


#ifndef MPXDBTABLE_H
#define MPXDBTABLE_H

// INCLUDES
#include <e32base.h>
#include <mpxattribute.h>
#include "mpxtable.h"

// CLASS FORWARDS
class CMPXDbManager;
class RSqlStatement;
class CMPXMedia;
class CMPXMediaArray;

// CONSTANTS
const TInt KMPXTableDefaultIndex = 0;

// CLASS DECLARATION

/**
* Base class for all table classes
*
* @lib MPXDbPlugin.lib
*/
class CMPXDbTable :
    public CBase,
    public MMPXTable
    {
    protected:

        /**
        * C++ constructor
        * @param aDbManager database manager instance
        */
        IMPORT_C CMPXDbTable(CMPXDbManager& aDbManager);

        /**
        * Safely construct things that can leave
        */
        IMPORT_C void BaseConstructL();

        /**
        * Destructor
        */
        IMPORT_C virtual ~CMPXDbTable();

    protected:

        /**
        * Update the media from the table. To be implemeted by derived classes, the default
        * implemetation does nothing.
        * @param aRecord table view
        * @param aAttrs the attributes to return
        * @param aMedia updated with the table info
        */
        IMPORT_C virtual void UpdateMediaL(RSqlStatement& aRecord, const TArray<TMPXAttribute>& aAttrs,
            CMPXMedia& aMedia);

        /**
        * Executes a query that retrieves information in a media array.
        * @param aAttrs the attributes to return
        * @param aMediaArray on return contains the results
        * @param aQuery query to be executed
        */
        IMPORT_C void ExecuteMediaQueryL(const TArray<TMPXAttribute>& aAttrs,
            CMPXMediaArray& aMediaArray, const TDesC& aQuery);

        /**
        * Executes a query that retrieves information in a media array.
        * @param aAttrs the attributes to return
        * @param aMediaArray on return contains the results
        * @param aAsc if the block is in ascending order
        * @param aQuery query to be executed
        */
        IMPORT_C void ExecuteMediaQueryL(const TArray<TMPXAttribute>& aAttrs,
            CMPXMediaArray& aMediaArray, const TBool aAsc, const TDesC& aQuery);

        /**
        * Executes a query with an int parameter that retrieves information in a media array.
        * @param aAttrs the attributes to return
        * @param aMediaArray on return contains the results
        * @param aQuery query to be executed
        * @param aValue integer parameter
        */
        IMPORT_C void ExecuteMediaQueryL(const TArray<TMPXAttribute>& aAttrs,
            CMPXMediaArray& aMediaArray, const TDesC& aQuery, TInt aValue);

        /**
        * Executes a query with one parameter that retrieves information in a media array.
        * @param aAttrs the attributes to return
        * @param aMediaArray on return contains the results
        * @param aQuery query to be executed
        * @param aValue1 string parameter
        * @param aValue2 integer parameter
        */
        IMPORT_C void ExecuteMediaQueryL(const TArray<TMPXAttribute>& aAttrs,
            CMPXMediaArray& aMediaArray, const TDesC& aQuery, const TDesC& aValue);

        /**
        * Executes a query with two int parameters that retrieves information in a media array.
        * @param aAttrs the attributes to return
        * @param aMediaArray on return contains the results
        * @param aQuery query to be executed
        * @param aValue1 integer parameter
        * @param aValue2 integer parameter
        */
        IMPORT_C void ExecuteMediaQueryL(const TArray<TMPXAttribute>& aAttrs,
            CMPXMediaArray& aMediaArray, const TDesC& aQuery, TInt aValue1, TInt aValue2);

        /**
        * Executes a query with two parameters that retrieves information in a media array.
        * @param aAttrs the attributes to return
        * @param aMediaArray on return contains the results
        * @param aQuery query to be executed
        * @param aValue1 string parameter
        * @param aValue2 integer parameter
        */
        IMPORT_C void ExecuteMediaQueryL(const TArray<TMPXAttribute>& aAttrs,
            CMPXMediaArray& aMediaArray, const TDesC& aQuery, const TDesC& aValue1, TInt aValue2);

        /**
        * Executes a query with two parameters that retrieves information in a media array.
        * @param aAttrs the attributes to return
        * @param aMediaArray on return contains the results
        * @param aQuery query to be executed
        * @param aValue1 string parameter
        * @param aValue2 string parameter
        */
        IMPORT_C void ExecuteMediaQueryL(const TArray<TMPXAttribute>& aAttrs,
            CMPXMediaArray& aMediaArray, const TDesC& aQuery, const TDesC& aValue1, const TDesC& aValue2);

        /**
        * Executes a query with four parameters that retrieves information in a media array.
        * @param aAttrs the attributes to return
        * @param aMediaArray on return contains the results
        * @param aQuery query to be executed
        * @param aValue1 string parameter
        * @param aValue2 integer parameter
        * @param aValue3 string parameter
        * @param aValue4 integer parameter
        */
        IMPORT_C void ExecuteMediaQueryL(const TArray<TMPXAttribute>& aAttrs,
            CMPXMediaArray& aMediaArray, const TDesC& aQuery,
                const TDesC& aValue1, TInt aValue2,
                const TDesC& aValue3, TInt aValue4);

        /**
        * Executes a query that retrieves information in a media item.
        * @param aAttrs the attributes to return
        * @param aMedia on return contains the result
        * @param aQuery query to be executed
        */
        IMPORT_C void ExecuteMediaQueryL(const TArray<TMPXAttribute>& aAttrs, CMPXMedia& aMedia,
            const TDesC& aQuery);

        /**
        * Executes a query with an int that retrieves information in a media item.
        * @param aAttrs the attributes to return
        * @param aMedia on return contains the result
        * @param aQuery query to be executed
        * @param aValue integer parameter
        */
        IMPORT_C void ExecuteMediaQueryL(const TArray<TMPXAttribute>& aAttrs, CMPXMedia& aMedia,
            const TDesC& aQuery, TInt aValue);

        /**
        * Executes a query that returns an int field.
        * @param aQuery query to be executed
        * @return returned field value
        */
        IMPORT_C TUint32 ExecuteIntQueryL(const TDesC& aQuery);

        /**
        * Executes a query with an int parameter that returns an int field.
        * @param aQuery query to be executed
        * @param aValue integer parameter
        * @return returned field value
        */
        IMPORT_C TUint32 ExecuteIntQueryL(const TDesC& aQuery, TUint32 aValue);

        /**
        * Executes a query with a string parameter that returns an int field.
        * @param aQuery query to be executed
        * @param aValue string parameter
        * @return returned field value
        */
        IMPORT_C TUint32 ExecuteIntQueryL(const TDesC& aQuery, const TDesC& aValue);

        /**
        * Executes a query with a string and an int parameter that returns an int field.
        * @param aQuery query to be executed
        * @param aValue1 string parameter
        * @param aValue2 int parameter
        * @return returned field value
        */
        IMPORT_C TUint32 ExecuteIntQueryL(const TDesC& aQuery, const TDesC& aValue1, TUint32 aValue2);

        /**
        * Executes a query with two int parameters that returns an int field.
        * @param aQuery query to be executed
        * @param aValue1 int parameter
        * @param aValue2 int parameter
        * @return returned field value
        */
        IMPORT_C TUint32 ExecuteIntQueryL(const TDesC& aQuery, TUint32 aValue1, TUint32 aValue2);

        /**
         * Executes a query that returns an int field.
         * @param aDriveID drive identifier
         * @param aQuery query to be executed
         * @return returned field value
         */
         IMPORT_C TUint32 ExecuteIntQueryL(TInt aDriveID,const TDesC& aQuery);

        /**
        * Executes a query that returns one or multiple records with one int field each.
        * The int values are summed and the result returned.
        * @param aQuery query to be executed
        * @return sum of int values
        */
        IMPORT_C TInt ExecuteSumQueryL(const TDesC& aQuery);

        /**
        * Executes a sum query with an int parameter.
        * @param aQuery query to be executed
        * @param aValue int parameter
        * @return sum of int values
        */
        IMPORT_C TInt ExecuteSumQueryL(const TDesC& aQuery, TUint32 aValue);

        /**
        * Executes a sum query with two int parameters.
        * @param aQuery query to be executed
        * @param aValue1 int parameter
        * @param aValue2 int parameter
        * @return sum of int values
        */
        IMPORT_C TInt ExecuteSumQueryL(const TDesC& aQuery,
            TUint32 aValue1, TUint32 aValue2);

        /**
        * Executes a sum query with a string and an int parameter.
        * @param aQuery query to be executed
        * @param aValue1 string parameter
        * @param aValue2 int parameter
        * @return sum of int values
        */
        IMPORT_C TUint32 ExecuteSumQueryL(const TDesC& aQuery,
            const TDesC& aValue1, TUint32 aValue2);

        /**
        * Executes a sum query with three int parameters.
        * @param aQuery query to be executed
        * @param aValue1 int parameter
        * @param aValue2 int parameter
        * @param aValue3 int parameter
        * @return sum of int values
        */
        IMPORT_C TInt ExecuteSumQueryL(const TDesC& aQuery,
            TUint32 aValue1, TUint32 aValue2, TUint32 aValue3);

        /**
        * Executes a sum query with a string and an int parameter.
        * @param aQuery query to be executed
        * @param aValue1 string parameter
        * @param aValue2 string parameter
        * @return sum of int values
        */
        IMPORT_C TInt ExecuteSumQueryL(const TDesC& aQuery,
            const TDesC& aValue1, const TDesC& aValue2);

        /**
        * Executes a sum query with a string and an int parameter.
        * @param aQuery query to be executed
        * @param aValue string parameter
        * @return sum of int values
        */
        IMPORT_C TInt ExecuteSumQueryL(const TDesC& aQuery, const TDesC& aValue);

        /**
        * Executes a sum query with a string and an int parameter.
        * @param aQuery query to be executed
        * @param aValue1 string parameter
        * @param aValue2 int parameter
        * @param aValue3 string parameter
        * @param aValue4 int parameter
        * @return sum of int values
        */
        IMPORT_C TInt ExecuteSumQueryL(const TDesC& aQuery,
            const TDesC& aValue1, TUint32 aValue2, const TDesC& aValue3, TUint32 aValue4);


        /**
        * Executes a media query that is bound to a specific query id.
        * If the query has not been prepared it will be prepared
        * @param aAttrs the attributes to return
        * @param aMediaArray on return contains the results
        * @param aQuery query to be executed
        * @param aValue1 TInt value to be bound to the query
        * @param aValue2 TInt value to be bound to the query
        * @param aQueryId unique ID to identify the query
        */
        IMPORT_C void ExecuteMediaQueryL( const TArray<TMPXAttribute>& aAttrs,
                                          CMPXMediaArray& aMediaArray,
                                          const TDesC& aQuery,
                                          TInt aValue1,
                                          TInt aValue2,
                                          TUint aQueryId );

        /**
        * Executes a media query that is bound to a specific query id.
        * If the query has not been prepared it will be prepared
        * @param aAttrs the attributes to return
        * @param aMediaArray on return contains the results
        * @param aQuery query to be executed
        * @param aValue1 value to be bound to the query
        * @param aValue2 value to be bound to the query
        * @param aAsc ascending or descending data retrival
        * @param aQueryId unique ID to identify the query
        */
        IMPORT_C void ExecuteMediaQueryL( const TArray<TMPXAttribute>& aAttrs,
                                          CMPXMediaArray& aMediaArray,
                                          const TDesC& aQuery,
                                          const TDesC& aTitle,
                                          TInt aCount,
                                          TBool aAsc,
                                          TUint aQueryId );

        /**
        * Checks that the specified query can be executed.
        * @param aDatabase database to execute the query on
        * @param aQuery query to execute
        * @return ETrue if the query can be executed, EFalse otherwise
        */
        IMPORT_C TBool DoCheckTable(RSqlDatabase& aDatabase, const TDesC& aQuery);

        /**
        * Executes a query with four parameters that retrieves information in a media array.
        * @param aDrive integer parameter
        * @param aAttrs the attributes to return
        * @param aMediaArray on return contains the results
        * @param aQuery query to be executed
        * @param aPlaylistId string parameter
        */
        IMPORT_C void ExecuteMediaQueryL(TInt aDrive, const TArray<TMPXAttribute>& aAttrs,
        	CMPXMediaArray& aMediaArray,const TDesC& aQuery,TInt aPlaylistId);
       
        /**
        * Executes a query with an int parameter that retrieves information in a media array.
        * @param aDrive integer parameter
        * @param aAttrs the attributes to return
        * @param aMediaArray on return contains the results
        * @param aQuery query to be executed
        */
        IMPORT_C void ExecuteMediaQueryOnDriveL(TInt aDrive,const TArray<TMPXAttribute>& aAttrs,
        	    CMPXMediaArray& aMediaArray, const TDesC& aQuery);
            
        /**
        * Executes a sum query with a string and two int parameters.
        * @param aQuery query to be executed
        * @param aPlaylistId string parameter
        * @param aDrive integer parameter
        */
        IMPORT_C TInt ExecuteSumExQueryL(const TDesC& aQuery, TUint32 aPlaylistId, TInt aDrive);        
    protected:    // Data

        CMPXDbManager& iDbManager;
    };

#endif // MPXDBTABLE_H

// End of File
