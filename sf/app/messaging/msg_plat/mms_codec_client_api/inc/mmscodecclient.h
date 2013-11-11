/*
* Copyright (c) 2005 - 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   MMS Codec Client offers API to multimedia message handling
*
*/




#ifndef MMSCODECCLIENT_H
#define MMSCODECCLIENT_H

//  INCLUDES
#include <e32base.h>        // CActive
#include <msvstd.h>         // TMsvId


// CONSTANTS
 

// MACROS


// DATA TYPES
 

// DATA TYPES
 


// FUNCTION PROTOTYPES
//
// FORWARD DECLARATIONS
class CClientMtmRegistry;
class CMsvOperation;
class CMsvSession;
class CMmsSettings;
class CMmsClientEntry;
class CMmsClientMtm;
class CMmsHeaders;
class CMmsDecode;
class CMmsEncode;

// CLASS DECLARATION
/**
*  This is a class declaration of CMmsCodecClient. It is a part of MMS Engine
*  and it used for adding, fetching and sending multimedia messages from and into
*  the message store in binary format. Also move, replace and some flag modifications
*  provided.
*  
*  The exported functions may be called in any order EXCEPT 
*  CreateNewMessageEntryL() MUST be called BEFORE AddMML().
*  If this rule is violated the operation may not be continued.
*  The chunked transfer also must be done in order as described below.
*  One message must be complete before another one is transferred.
*
*  Chunked transfer added to version 3.2.
*  To support synchronizing large messages without the need of large linear
*  buffer, chunked mode of data transfer has been added to the API
*
*  Chunked transfer from phone to PC:
*
*  1) Initialize retrieve (InitializeChunkedRetrievingL)
*  2) get a data chunk    (GetNextDataPart)
*  3) release data chunk  (ReleaseData)
*  repeat steps 2 and 3 until all data has been processed
*
*  Chunked transfer from PC to phone:
*  The same procedure can be used both for creating new entries
*  or replacing existing entries.
*
*  1) Create an entry or specify an entry to be replaced
*     (InitializeChunkedAddingL or InitializeChunkedReplacingL)
*  2) Give next data chunk until last one has been reached (NextDataPart)
*
*  If a chunked transfer to either direction is not totally completed 
*  before a new operation starts, the new operation will reset the state
*  of the previous operation and delete possible incomplete entry.
*  The next operation will proceed normally.
*    
*  Setting the flags:
*  Some information is not transferred inside the binary data in message headers.
*  That's why some data is transferred in flags. The flag info is moved
*  to the codec client in AddMML, ReplaceMML and SetFlags functions. To the caller
*  the info is transferred in RetrieveMML function.
*  The unread flag is starightforward, it is just a boolean which tells if the 
*  message has been read or not. 
*  The other flags (TUint32) is just like TMsvEntry.iMtmData1 (See mmsconst.h). 
*  but only the following bits are used, others are zero:
*  EMmsDrmCorruptedAttachment  , if the corrupted DRM attachments has been removed
*  KMmsMessageMobileTerminated , modbile terminated message
*  KMmsMessageEditorOriented   , mobile originated message
*  The last two must be set exclusive e.g the may not be set at the same time
*  
*  Folders: 
*  User of this class may only access messages in the standard folders:
*  KMsvGlobalInBoxIndexEntryId = 0x1002
*  KMsvGlobalOutBoxIndexEntryId=0x1003
*  KMsvDraftEntryId= 0x1004
*  KMsvSentEntryId=0x1005
*
*  @lib mmscodecclient.lib
*/
class CMmsCodecClient : public CActive
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * 
        * @param aMsvSession    Message Server Session handle   
        */
        IMPORT_C static CMmsCodecClient* NewL( CMsvSession& aMsvSession );

                
        /**
        * Destructor.
        */
        virtual ~CMmsCodecClient();

    public: // New functions
        
        /**
        * Add a multimedia message. The given multimedia message is stored to
        * the target folder.  The flags are set correctly.
        * CreateNewMessageEntryL must be called before this function.
        * 
        * 
        * @param aMm       the MM in binary format, which will be added to 
        *                  the messaging store
        * @param aFolder   the folder in which the MM will be stored 
        * @param aFlags    the flags of the MM, which go to iMtmData1
        * @param aUnread   if the MM is unread or not
        * @param aMmId     the index, which was created for the added MM by 
        *                  the caller.
        * @param aStatus   request status of the calling active object 
        * 
        * Return values: 
        * @return KErrNone, if everything proceeds fine
        * @return KErrArgument, if aMm or aFolder is invalid.
        */
        IMPORT_C void AddMML( 
            CBufFlat& aMm,
            TMsvId aFolder,
            TUint32 aFlags,                                                                                                                           
            TBool aUnread,                                                                                   
            TMsvId& aMmId,
            TRequestStatus& aStatus );

        /**
        * Delete a multimedia message. 
        * 
        * @param aMmId      The index to the MM, which will be deleted
        * @return KErrNone, if everything proceeds fine or if the multimedia is
        *                   not found.
        * @return Error value in exceptional cases                     
        * 
        */
        IMPORT_C TInt DeleteMM( TMsvId aMmId );

        /**
        * Retrieves a multimedia message. This function provides an MM in the 
        * binary format, the folder where the MM is stored and its flags.
        *
        * 
        * @param aMm       The MM
        * @param aFolder   The folder, in which the message is stored.
        * @param aFlags    the flags of the MM, from the iMtmData1
        * @param aUnread   if the MM is unread or not
        * @param aMmId     Index to the MM, which will be retrieved            
        * @param aStatus   request status of the calling active object 
        *
        * @return KErrNone, if everything proceeds fine.
        * @return Error value in exceptional cases  
        */
        IMPORT_C void RetrieveMML(
            TMsvId aMmId,
            CBufFlat& aMM,
            TMsvId& aFolder,
            TUint32& aFlags,                   
            TBool& aUnread,
            TRequestStatus& aStatus );

        /**
        * Sends the specified MM.The message is moved to the Outbox before 
        * sending. The caller of the method should not delete CMsvOperation, 
        * which is returned from the function until aStatus has completed. 
        * If CMsvOperation is destroyed too early, the sending is canceled.
        *  
        * 
        * @param aMmId     Index to the MM, which will be sent.           
        * @param aStatus   request status of the calling active object 

        * @return   pointer to CMsvOperation object. Caller must take care of
        *           deleting the pointer after the operation completes.
        */
        IMPORT_C CMsvOperation* SendMML( 
            TMsvId aMmId, 
            TRequestStatus& aStatus );

        /**
        * A multimedia message is replaced. 
        * First the attachments of the original message is destroyed. 
        * Then the new message is stored to the same entry. Flags are set as 
        * defined. If the message is in the Outbox, it is not allowed to be 
        * replaced.
        * 
        * @param aMmId     the index to the replaceable MM  
        * @param aMm       The new MMS
        * @param aFlags    the flags of the MM, which go to iMtmData1
        * @param aUnread   if the MM is unread or not         
        * @param aStatus   request status of the calling active object 
        * 
        */
        IMPORT_C void ReplaceMML (
            TMsvId& aMmId, 
            CBufFlat& aMm,
            TUint32 aFlags,
            TBool aUnread,
            TRequestStatus& aStatus );
        
        
        /**
        * A multimedia message is moved from a standard folder to another.
        * Moving to outbox is not allowed.
        * @since 3.0
        * 
        * @param aMmId     the id to the moved MM  
        * @param aParentId New folder
        * 
        */
        IMPORT_C void MoveMML( 
                      TMsvId aMmId, 
                      TMsvId aParentId, 
                      TRequestStatus& aStatus );
        
        
        /**
        * This function sets flags of the MM.
        * Note that it is not allowed to change the flags,
        * if the MM is in outbox.
        * 
        * @param aMmId     Index to the MM          
        * @param aFlags    the flags of the MM, which go to iMtmData1
        * @param aUnread   if the MM is unread or not
        *     
        * @return KErrNone, if everything proceeds fine.
        * @return KErrNotSupported, if the message is in Outbox. 
        *
        */
        IMPORT_C TInt SetFlags( 
                TMsvId aMmId,
                TUint32 aFlags,
                TBool aUnread );
            
        /*
        * Create a message entry into the target folder. This function must be
        * called
        * before the AddMML function.
        * 
        * @since 3.0
        * @param aFolder    the folder under which the entry is created. 
        * @param aCreatedId The new creared entry ID.
        *
        * @return KErrNone the function succeeded.
        * @return Error values in exceptional cases. 
        */
        IMPORT_C TInt CreateNewMessageEntryL( 
                TMsvId aFolder, 
                TMsvId& aCreatedId );


        /**
        * Check if disk space is below critical level when storing data.
        * This is a wrapper function.
        * @since 3.0
        *
        * @param aDataSize amount of data to be added. If aDataSize == 0,
        *     returns info if disk space is already below critical level.
        * @return error code KErrDiskFull if data does not fit,
        *     KErrNone if data fits.
        *     May return some other error code if appropriate.
        *     KErrNone always means it is all right to save the data.
        */
        IMPORT_C TInt  DiskSpaceBelowCriticalLevelL( TInt aSize );
        
    // New methods to support chunked transfer.
        
        /**
         * Initialize retrieving a message in chunked mode.
         * Data will be obtained by repeatedly calling GetNextDataPart()
         * and ReleaseData().
         * Function encodes the first data part into its internal buffer
         * ready to be fetched by calling GetNextDataPart()
         *
         * @since S60 v3.1
         * @param aMessageId Id of the message to be retrieved
         * @param aFolder The folder where the message is stored
         * @param aFlags  The flags of the message, from the iMtmData1
         * @param aUnread If the message is unread or not
         * @param aOverallDataSize The total size of the encoded data
         *    if the size can be determined. -1 means the size could not be
         *    determined, and the return value of GetNextDataPart() function
         *    should be used to determine when the last data part has been
         *    reached. The buffer will be smaller than the total data size
         *    except in the case of small messages that may fit in the buffer
         *    in one chunk.
         * @param aStatus   request status of the calling active object 
         * Leaves with some system wide error code or completes with error
         *     if something goes wrong. If the function leaves, the caller
         *     must not set itself active as it was not set pending and will
         *     not be completed.
         */
         
        IMPORT_C void InitializeChunkedRetrievingL(
            TMsvId aMessageId,
            TMsvId& aFolder,
            TUint32& aFlags,
            TBool& aUnread,
            TInt& aOverallDataSize,
            TRequestStatus& aStatus );

        /**
         * Obtain a data part from the supplier.
         * The data is guaranteed to survive until a call is made to ReleaseData().
         * If the component that is receiving the data cannot receive all data
         * in one chunk, data can be given forward in smaller chunks. The data will
         * will remain in the internal buffer of CodecClient until it has been
         * released by calling ReleaseData().
         *
         * @since S60 v3.1
         * @param aDataPart Pointer to the data that has been encoded into internal buffer
         *     The length of the pointer is set to reflect the amount of data.
         * @param aLastDataChunk 
         *     ETrue if this is the last data chunk
         *     EFalse if this is not the last data chunk
         * @return error code, KErrNone if all is well.
         *     If returns some other error, transaction should be ended.
         */
        IMPORT_C TInt GetNextDataPart( TPtrC8& aDataPart, TBool& aLastDataChunk );
            
        /**
         * Release the current data part being held at the data supplier.
         * This call indicates to the supplier that the part
         * is no longer needed, and another one can be provided.
         * When this function is called, the next data part will be encoded
         * into the buffer. When this function returns, a new data part
         * can be fetched with GetNextDataPart.
         * This function must always be called after the data obtained by
         * GetNextDataPart() call has been processed.
         * This function must also be called after last data chunk has 
         * been processed so that the state of the data supplier can be reset.
         *
         * @since S60 v3.1
         * @return error code, KErrNone if all is well.
         *     If returns some other error, transaction must be ended.
         */
        IMPORT_C TInt ReleaseData();

        /** 
         * Initialize adding a message in chunked mode.
         * An empty invisible message entry is created into the folder specified.
         *
         * @since S60 v3.1
         * @param aFolder The folder where the message will be added
         * @param aMessageId The ID of the message that has been created
         * @param aFlags The flags of the MM to be stored in iMtmData1
         * @param aUnread Specifies if the message is unread or not 
         * Leaves with KErrArgument if aFolder is invalid or if called 
         *    in the middle of some other operation
         * Leaves with some system wide error code if something goes wrong.
         */
        IMPORT_C void InitializeChunkedAddingL(
            TMsvId aFolder,
            TMsvId& aMessageId,
            TUint32 aFlags,
            TBool aUnread );

        /** 
         * Initialize replacing a message in chunked mode.
         * The present content of the message entry is deleted.
         * The data is added by calling NextDataPart()
         *
         * @since S60 v3.1
         * @param aMessageId The ID of the message that will be replaced
         * @param aFlags The flags of the MM to be stored in iMtmData1
         * @param aUnread Specifies if the message is unread or not 
         * Leaves with KErrArgument if aFolder is invalid or if called 
         *    in the middle of some other operation.
         * Leaves with some system wide error code if something goes wrong.
         */
        IMPORT_C void InitializeChunkedReplacingL(
            TMsvId aMessageId,
            TUint32 aFlags,
            TBool aUnread );

        /**
         * Gives the next data part to be decoded.
         * Decoding is synchronous. When the code returns, data can be released.
         * MMS Decode may not be able to handle incomplete headers. MMS Codec Client
         * will take care of buffering the incoming data and saving possible
         * uncoded data in a flat buffer the same way CMmsTransaction does.
         * The data should be provided in chunks that are small enough to be
         * copied to an internal buffer without fear of running out of memory.
         * HTTP stack provides data in chunks that are less than 10 kB.
         * A similar size would be recommended but there is no absolute 
         * requirement as to the size of the data chunk. A compromise between
         * performance and memory consumption may be used to determine an appropriate
         * buffer size.
         *
         * @since S60 v3.1
         * @param aDataPart the data to be decoded.
         * @param aLastChunk ETrue if the data chunk in the buffer is the last one.
         *     When the last chunk has been decoded, the message entry is finalized.
         *     The flags are put into the entry, and the entry is made visible.
         * @return error code, KErrNone if decoding succeeds
         *     If returns some other error, the entry that was created in InitializeChunkedAddingL
         *     will be deleted.
         */
        IMPORT_C TInt NextDataPart(
            TPtrC8& aDataPart,
            TBool aLastDataChunk );
        


    public: // Functions from base classes


        
    protected:  // New functions
        

    protected:  // Functions from base classes
        
        
        /**
        * From CActive         
        */
        virtual void DoCancel();


        /**
        * From CActive.
        */
        virtual void RunL();
        
        /**
        * From CActive.
        */
        TInt RunError(TInt aError);

    private:

        /**
        * C++ default constructor.
        * C++ default constructor can NOT contain any code, that
        * might leave.
        */
        CMmsCodecClient();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( CMsvSession& aMsvSession );
        
        /*
        * This function finalizes an entry, which has a decoded MM.
        */
        void FinalizeDecodedMML( );

        /*
        * This function generates iDetails for TMsvEntry::iMtmData1
        * in case of outgoing messages 
        */
        void GenerateRecipientsL( TDes& aDetails );
       
        /*
        * This function generates iDetails for TMsvEntry::iMtmData1
        * in case of incoming messages 
        */
        void GenerateSenderL ( TDes& aDetails );
        
        /*
        * Retrieve flags of the MM
        *
        * @param aEntry    The entry, which flags are given.          
        * @param aFlags    the flags, from the iMtmData1
        * @param aUnread   if the MM is unread or not
        */
        void RetrieveFlags( 
            TMsvEntry aEntry,
            TUint32 &aFlags,
            TBool &aUnread );

        /*
        * Set flags to the TMsvEntry
        *
        * @param aEntry    The entry, which flags are set.          
        *
        */

        void SetFlagsToTMsvEntry(
            TMsvEntry& aEntry );

        /*
        * Check if the entry's parent is Outbox.
        *
        * @param aMmId     Index to the MM
        *
        * @return ETrue when the parent is Outbox.
        * @return EFalse when the parent is Outbox.
        */

        TBool ParentOutbox(
            TMsvId aMmId );
            
        /*
        * Deletes the current entry.
        */
        void DeleteCurrentEntryL();
        
        /*
        * If a chunked operation has not completed properly, reset the state
        */
        void ResetChunkedMode();    

        /*
        * After last chunk has been fetched, an error encountered during
        * retrieve or if caller starts a new operation without completing
        * previous fetch, the entry being handled is put back to read only
        * state if needed.
        */
        void ReleaseRetrievedEntry();

    public:     // Data
        
    
    protected:  // Data
        

    private:    // Data
        
        CMsvSession* iMsvSession; 
        CMsvEntry* iClientEntry;
        
        RFs iFs;  // file system
        CMmsClientMtm* iMmsClient;
        CMmsClientEntry* iClientEntryWrapper;
        CClientMtmRegistry* iClientMtmRegistry;

        CMmsHeaders* iMmsHeaders;
        CMmsDecode* iDecoder;
        CMmsEncode* iEncoder;
        TMsvId iEntryBeingHandled;

        TMsvId iFolder;
        
        TUint32 iFlags;                                                                                                                           
        TBool iUnread;    

        TRequestStatus*  iClientStatus;     // status of the client
        
        // Private encode buffer for copying incoming data as partial headers
        // cannot be decoded, and it is possible that partial data will be left
        // in the buffer and more data appended to the end.
        // The procedure is the same as in CMmsTransaction.
        CBufFlat*           iEncodeBuffer;
        // Position where the data will be written into buffer. There may be some old
        // unhandled data left.
        TInt                iPosition;
        // keep track if we already got last chunk
        TBool               iLastChunk;
        
        // MMS encapsulation versio to be used if the version is not defined
        // in the messages
        TInt16 iMmsVersion;
        
        
        enum TState
            {
            EIdle = 0,
            EFinalizeDecodedMM,
            EFinalizeEncodedMM,
            ESendMM,
            EEntryCreated,
            EChunkedRetrieve, // not an active state
            EChunkedAdd,      // not an active state
            EChunkedReplace   // not an active state
            }; 
        TState iState;    // The state of the active object
        
    };

#endif      // MMSCODECCLIENT_H   
            
// End of File
