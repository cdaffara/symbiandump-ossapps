/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  calss which directly deal with notepad database.
*
*/


#ifndef NSMLNOTEPADDATABASE_H
#define NSMLNOTEPADDATABASE_H


//INCLUDES
#include <d32dbms.h>
#include <badesca.h>

//CLASS Forward declarations
class CNpdItem;
class CCalSession;
class CCalInstanceView;
class CCalEntryView;

const TInt KMaxFileLength(255);

//Class declarations

// ------------------------------------------------------------------------------------------------
// CNSmlNotepadDatabase
// ------------------------------------------------------------------------------------------------

class CNSmlNotepadDatabase : public CBase
    {
    public:
        /**
        * Creates new instance of CNpdSmlDbModel based class.
        * @return CNpdSmlDbModel* Newly created instance.
        */
        static CNSmlNotepadDatabase* NewL();

        virtual ~CNSmlNotepadDatabase();

    private:        
		// Constructor
        CNSmlNotepadDatabase();

        /**
        * 2-phase constructor
        */
        void ConstructL();

    public: 

       	/**
        * Opens notepad database file. If file does not exist, or is corrupted,
        * a new, empty file is created.
        * @param aStoreName, name of the notepad database file name
        */
        void OpenL(const TDesC& aStoreName);
		
        /**
        * Adds a note to the database.
        * @param aNote the note to be added.
        * @param aKey database index of the newly added note.
        */
        void AddNoteL(CNpdItem& aNote, TInt &aKey);

        /**
        * Deletes a note from the database.
        * @param aKey database index of the note to be deleted.
        */
        void DeleteNoteL(const TInt aKey);

	
        /**
        * Closes the database.
        */
        void Close();

        /**
        * Reads a note from the database.
        * @param aKey database index of the desired note.
        * @param aNote reference to note object, used to return note data.
        */
        void GetNoteL(const TInt aKey, CNpdItem& aNote);

        /**
        * Updates a note in the database.
        * @param aKey database index of the note to be updated.
        * @param aItem note data. This data overrides existing note.
        */
        void UpdateNoteL(const TInt aKey, CNpdItem& aItem);

        
        /**
        * Empties the database. All notes are deleted.
        * @return KErrNone if successful, a system wide error code otherwise.
        */
        TInt ResetL();

        /**
        * Checks if the database has been succesfully openend
        * @return ETrue if database has been succesfully opened, EFalse otherwise
        */
        TBool IsOpen();
        
            
        /**
        * Fetch all the note items if aKey is not present. Else item with the aKey.
        * @returns array of NpdItem.
        */
        CArrayPtr<CNpdItem>* FetchItemsLC(TBool aIncludeText = EFalse, TInt* aKey = 0);
        
        /**
        * Close all the CalSession related sessions 
        * viz., instance view, entry view and calsession itself
        */
        void CloseAgendaSessions();

   private: // Data
        
        // Flag for whether database has been opened
        TBool iOpened;       
        CCalSession*        iSession;
        CCalInstanceView*   iInstanceView;
        CCalEntryView*      iEntryView;
 
    enum TNotepadColumnType
        {
        ENotepadKey = 1, // this must be 1 (DBMS/SQL restriction)
        ENotepadUpdateTime,
        ENotepadMemo
        };
    };

class CNpdItem : public CBase
    {
    public:
        // Destructor
        virtual ~CNpdItem();

        /**
        * Creates new instance of CNpdItem based class.
        * @return CNpdItem* Newly created instance.
        */
        static CNpdItem* NewLC();

        /**
        * Creates new instance of CNpdItem based class.
        * @param aKey database index key
        * @param aLastModified timestamp
        * @param aContent content (text) of the note
        * @return CNpdItem* Newly created instance.
        */
        static CNpdItem* NewLC(TInt aKey, const TTime& aLastModified, HBufC* aContent);

    private:
		// Constructor
        CNpdItem();
    
        //Overloaded Constructor
        CNpdItem(TInt aKey, const TTime& aLastModified, HBufC *content);
        
	public:
        /**
        * Returns database key.
        * @return key value.
        */
        TInt Key() const;

        /**
        * Returns the time stamp of the note.
        * @return when note was last modified.
        */
        TTime LastModified() const;

        /**
        * Returns content of the note
        * @return text of the note.
        */
        HBufC* Content();

        /**
        * Set note data.
        * @param aKey database index
        * @param aModTime time note was last modified
        * @param aContent content of the note
        */
        void Set(TInt aKey, const TTime& aModTime, HBufC* aContent);

        /**
        * Set key of the note
        * @param aKey key of the note.
        */
        void SetKey(TInt aKey);
        
        /**
        * Set content of the note
        * @param aContent text of the note.
        */
        void SetContent(HBufC* aContent);

        /**
        * Returns timestamp of the note
        * @param aLastModified time note was last modified.
        */
        void SetModified(const TTime& aLastModified);

   private:
        TInt iKey;
        TTime  iLastModified;
        HBufC* iContent; // owned
    };

#endif // NSMLNOTEPADDATABASE_H
