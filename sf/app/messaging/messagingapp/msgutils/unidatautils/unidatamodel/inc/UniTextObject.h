/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
*       CUniTextObject, Storage for single text attachment in presentation.
*
*
*/


#ifndef __UNITEXTOBJECT_H
#define __UNITEXTOBJECT_H

// INCLUDES

#include <e32std.h>
#include <cmsvattachment.h>
#include <UniObject.h>

// CONSTANTS

// MACROS

// FORWARD DECLARATIONS
class CUniMimeInfo;
class CMsgTextInfo;
class CBaseMtm;
class CUniDataUtils;
class CEikRichTextEditor;

// DATA TYPES

// FUNCTION PROTOTYPES

// CLASS DECLARATION

/**
* CUniTextObject, Storage for single text attachment in presentation.
*
* @lib UniDataModel.lib
* @since 3.1
*/
class CUniTextObject : public CUniObject
    {
    public:  // New methods

        /**
        * Factory method that creates this object
        * from an attachment.
        *
        * @since    3.1
        * @param    aMtm    IN reference to client mtm
        * @param    aMedia  IN initialized media info object - takes ownership!
        * @param    aManager        IN reference to attachment manager
        * @param    aAttachmentId   IN id of the attachment from
        *                           which to create the object
        * @return   Pointer to instance in cleanup stack
        */
        IMPORT_C static CUniTextObject* NewLC(
            RFs& aFs,
            CBaseMtm& aMtm,
            CUniDataUtils& aData,
            CMsgTextInfo* aMedia,
            MMsvAttachmentManager& aManager,
            CMsvAttachment& aAttachment );

        /**
        * Factory method that creates this object
        * from an attachment.
        *
        * @since    3.1
        * @param    aMtm    IN reference to client mtm
        * @param    aMedia  IN initialized media info object - takes ownership!
        * @param    aManager        IN reference to attachment manager
        * @param    aAttachmentId   IN id of the attachment from
        *                           which to create the object
        * @return   Pointer to instance
        */
        IMPORT_C static CUniTextObject* NewL(
            RFs& aFs,
            CBaseMtm& aMtm,
            CUniDataUtils& aData,
            CMsgTextInfo* aMedia,
            MMsvAttachmentManager& aManager,
            CMsvAttachment& aAttachment );

        /**
        * Factory method that creates this object
        * from a file.
        *
        * @since    3.1
        * @param    aData   IN reference to MsgData object
        * @param    aMedia  IN initialized media info object - takes ownership!
        * @return   Pointer to instance in cleanup stack
        */
        IMPORT_C static CUniTextObject* NewLC(
            RFs& aFs,
            CBaseMtm& aMtm,
            CUniDataUtils& aData,
            CMsgTextInfo* aMedia );

        /**
        * Factory method that creates this object
        * from a file.
        *
        * @since    3.1
        * @param    aData   IN reference to MsgData object
        * @param    aMedia  IN initialized media info object - takes ownership!
        * @return   Pointer to instance
        */
        IMPORT_C static CUniTextObject* NewL(
            RFs& aFs,
            CBaseMtm& aMtm,
            CUniDataUtils& aData,
            CMsgTextInfo* aMedia );

        /**
        * Factory method that creates this object
        * from a text buffer.
        *
        * @since    3.1
        * @param    aData   IN reference to MsgData object
        * @param    aTextEditor IN text buffer from
        *                       which to create the object
        * @return   Pointer to instance in cleanup stack
        */
        IMPORT_C static CUniTextObject* NewLC(
            RFs& aFs,
            CBaseMtm& aMtm,
            CUniDataUtils& aData,
            CEikRichTextEditor* aTextEditor );

        /**
        * Factory method that creates this object
        * from a text buffer.
        *
        * @since    3.1
        * @param    aData   IN reference to MsgData object
        * @param    aTextEditor IN text buffer from
        *                       which to create the object
        * @return   Pointer to instance
        */
        IMPORT_C static CUniTextObject* NewL(
            RFs& aFs,
            CBaseMtm& aMtm,
            CUniDataUtils& aData,
            CEikRichTextEditor* aTextEditor );

        /**
        * Destructor
        *
        * @since    3.1
        */
        virtual ~CUniTextObject();

        /*
        * Size
        * Inherited from CUniObject
        *
        * @since    3.1
        * @param    aWithoutHeaders IN if ETrue include MIME headers' sizes
        *                           if EFalse count only file sizes
        * @return   Size of the object in bytes
        */
        virtual TInt Size( TBool aWithoutHeaders = ETrue );

        /*
        * SynchronizeSize
        * Count the accurate size of the text object if it would
        * be saved as a file.
        * Useful for UTF-8 text
        *
        *
        * @since    3.1
        * @return   Size of the object in bytes
        */
        IMPORT_C TInt SynchronizeSize();

        /*
        * Text
        *
        * @since    3.1
        * @return   Pointer to text editor.
        */
        inline CEikRichTextEditor* Text() const;

        /*
        * SetText
        * Set text to save
        *
        * @since    3.1
        * @param    aTextEditor     IN pointer to text editor
        */
        inline void SetText( CEikRichTextEditor* aTextEditor );

    protected:

        /**
        * Constructor.
        *
        * @since    3.1
        */
        CUniTextObject(
            RFs& aFs,
            CBaseMtm& aMtm,
            CUniDataUtils& aData,
            CMsgTextInfo* aMedia );

        /**
        * 2nd phase constructor.
        *
        * @since    3.1
        */
        void ConstructL();

        /**
        * 2nd phase constructor.
        *
        * @since    3.1
        * @param    aData   IN reference to MsgData object
        * @param    aTextEditor IN text buffer from
        *                       which to create the object
        */
        void ConstructFromTextL( CEikRichTextEditor* aTextEditor );

        /**
        * Create file name for the object
        *
        * @since    3.1
        * @param    aBuffer IN text buffer from which to create file name
        * @param    aFileName   OUT created file name (without path)
        * @param    aData   IN reference to MsgData object
        */
        void CreateFileName( TFileName& aFileName );

        void InitializeTextSaveL();
        void CreateTextAttachmentL();
        void WriteToFileL();

        /**
        * Save
        * Inherited from CUniObject
        */
        virtual void Save(
            MUniObjectSaveObserver& aObserver,
            CMsvAttachment::TMsvAttachmentType aSaveType );

        /**
        * DoSaveL
        * Inherited from CUniObject
        */
        virtual void DoSaveL();

    protected: // data

        enum TMsgTextObjectSaveState
            {
            ETextSavingIdle,
            ETextCreatingAttachment,
            ETextWritingToFile
            };

        CEikRichTextEditor* iTextEditor;
        TInt iSyncCounter;
        TInt iSyncSize;
        TInt iSyncLength;
        TInt iPrevLength;

        TInt iTextSaveState;
        RFile* iEditFile;
    };

#include <UniTextObject.inl>

#endif // __UNITEXTOBJECT_H
