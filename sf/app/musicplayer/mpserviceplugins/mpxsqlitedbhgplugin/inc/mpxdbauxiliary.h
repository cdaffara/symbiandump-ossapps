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
* Description:  This class is responsible for interacting with the Auxiliary
*                table.
*
*/


#ifndef MPXDBAUXILIARY_H
#define MPXDBAUXILIARY_H

// INCLUDES
#include "mpxdbtable.h"

// CLASS DECLARATION

/**
* Responsible for managing the playlist tables
*
* @lib MPXDbPlugin.lib
*/
class CMPXDbAuxiliary :
    public CMPXDbTable
    {
    public:

        /**
        * Two-phased constructor.
        * @param aDbManager database manager to use for database interactions
        * @return New CMPXDbManager instance.
        */
        static CMPXDbAuxiliary* NewL(CMPXDbManager& aDbManager);

        /**
        * Two-phased constructor.
        * @param aDbManager database manager to use for database interactions
        * @return New CMPXDbManager instance on the cleanup stack.
        */
        static CMPXDbAuxiliary* NewLC(CMPXDbManager& aDbManager);

        /**
        * Destructor
        */
        virtual ~CMPXDbAuxiliary();

    public:

        /**
        * Sets the last refreshed time in the auxiliary table.
        * @param aTime the last refreshed time value
        */
        void SetLastRefreshedTimeL(TTime aTime);

        /**
        * Gets the last refreshed time from the auxiliary table.
        * @return last refreshed time value
        */
        TTime LastRefreshedTimeL();

        /**
        * Sets the db corrupted flag in the auxiliary table.
        * @param aCorrupted db corrupted flag value
        */
        void SetDBCorruptedL(TBool aCorrupted);

        /**
        * Gets the db corrupted flag value from the auxiliary table.
        * @return db corrupted flag value
        */
        TBool DBCorruptedL();

        /**
        * Sets the save deleted record count value.
        * @param aDrive drive identifier
        * @param aValue new field value
        *
        */
        void SetSaveDeletedRecordCountL(TInt aDrive,TUint32 aValue);

        /**
        * Gets the save deleted record count field value.
        * @return field value
        */
        TUint32 SaveDeletedRecordCountL();

        /**
         * Gets the save deleted record count field value.
         * @param aDrive drive identifier
         * @return field value
         */
         TUint32 SaveDeletedRecordCountL(TInt aDrive);

        /**
        * Checks if all databases have been refreshed.
        * @return EFalse if at least one database has not been refreshed.
        */
        TBool IsRefreshedL();

        /**
        * Get the auxilary id for a drive
        * @param aDrive drive to get the volume id
        * @return TInt id for the aux database
        */
        TInt IdL( TInt aDrive );

        /**
        * Set the auxilary id for a drive
        * @param aDrive,
        * @param aId
        */
        void SetIdL( TInt aDrive, TInt aId );

    private:    // from MMPXTable

        /**
        * @see MMPXTable
        */
        virtual void CreateTableL(RSqlDatabase& aDatabase, TBool aCorruptTable);

        /**
        * @see MMPXTable
        */
        virtual void DropTableL(RSqlDatabase& aDatabase);

        /**
        * @see MMPXTable
        */
        virtual TBool CheckTableL(RSqlDatabase& aDatabase);

    private:

        /**
        * C++ constructor
        * @param aDbManager database manager to use for database interactions
        */
        CMPXDbAuxiliary(CMPXDbManager& aDbManager);

        /**
        * Safely construct things that can leave
        */
        void ConstructL();

    private:

        /**
        * Column indexes in the auxiliary table
        */
        enum TAuxiliaryColumns
            {
            EAuxiliaryId = KMPXTableDefaultIndex,
            EAuxiliaryVersion,
            EAuxiliaryTimeRefreshed,
            EAuxiliaryTimeSynced,
            EAuxiliaryCorrupt,
            EAuxiliarySavedDeletedRecordCount
            };

    };

#endif // MPXDBAUXILIARY_H

// End of File
