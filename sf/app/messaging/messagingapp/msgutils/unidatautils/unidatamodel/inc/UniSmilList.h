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
*       CUniSmilList, List of Smil attachments.
*
*
*/


#ifndef __UNISMILLIST_H
#define __UNISMILLIST_H

// INCLUDES

#include <e32std.h>
#include <f32file.h>
#include <apmstd.h>
#include <cmsvattachment.h>

#include <gmxmlcomposer.h>

#include <UniModelConst.h>

// CONSTANTS

// MACROS

// FORWARD DECLARATIONS
class CUniDataUtils;
class CBaseMtm;
class CMDXMLDocument;
class CMDXMLNode;
class CMsvStore;
class MMsvAttachmentManager;

// DATA TYPES

// FUNCTION PROTOTYPES

// CLASS DECLARATION

/**
* MUniSmilComposeObserver - Callback API for SMIL composing.
*
* @since 3.1
*/
class MUniSmilComposeObserver
    {
    public:
        virtual void SmilComposeEvent( TInt aError ) = 0;
    };

/**
* CUniSmilList - List of SMIL attachments of a message.
*
* @lib UniDataModel.lib
* @since 3.1
*/
class CUniSmilList : public CActive,
                     public MMDXMLComposerObserver
    {

    public:  // New methods

        /**
        * Factory method that creates this object.
        *
        * @param    aMtm    IN reference to client mtm
        * @return   Pointer to instance
        */
        IMPORT_C static CUniSmilList* NewL( CBaseMtm& aMtm );

        /**
        * Destructor
        *
        */
        virtual ~CUniSmilList();

        /**
        * AddSmilAttachmentL
        * Adds SMIL attachment to smil array
        *
        * @since    3.1
        * @param    aAttachmentId   IN id of the SMIL attachment
        *                           to be added
        */
        IMPORT_C void AddSmilAttachmentL(
            MMsvAttachmentManager& aManager,
            TMsvAttachmentId aAttachmentId );

        /**
        * CreateSmilAttachmentL
        * Creates SMIL attachment from DOM
        *
        * @param    aAttachmentId   OUT id of the created SMIL attachment
        * @param    aDom    IN the DOM from which to create the SMIL
        * @param    aFs     IN reference to file session
        */
        IMPORT_C void CreateSmilAttachmentL(
            MUniSmilComposeObserver& aObserver,
            CMsvStore& aEditStore,
            CMDXMLDocument* aDom );

        /**
        * GetSmilAttachmentByIndex
        * Get SMIL attachment by index
        *
        * @param    aIndex  IN index
        * @return   Message id of the SMIL attachment
        */
        IMPORT_C TMsvAttachmentId GetSmilAttachmentByIndex( TInt aIndex );

        /**
        * GetSmilFullPathByIndexL
        * Get file handle of SMIL attachment by index
        *
        * @param    aIndex  IN index
        */
        IMPORT_C RFile GetSmilFileByIndexL( TInt aIndex );

        /**
        * SmilByteSize
        * Calculate size of all the SMIL attachments in bytes
        *
        * @param    aFs     IN reference to file session
        * @return   Size in bytes
        */
        inline TInt SmilByteSize() const;

        /**
        * Count
        *
        * @return   Number of objects
        */
        inline TInt Count() const;

        void ComposeFileCompleteL();

    protected:

        /**
        * From CActive
        */
        void DoCancel();

        /**
        * From CActive
        */
        void RunL();

        /**
        * From CActive
        */
        TInt RunError( TInt aError );


    protected:

        /**
        * Constructor.
        *
        * @param    aMtm    IN reference to client mtm
        */
        CUniSmilList( CBaseMtm& aMtm );

        /**
        * 2nd phase constructor.
        */
        void ConstructL();

    protected: // data

        CBaseMtm& iMtm;

        CArrayFixFlat<TMsvAttachmentId>* iSmilAttachmentArray;
        TInt iSmilSize;

        MMsvAttachmentManager* iManager;
        CMsvAttachment* iAttachment;
        CMsvStore* iEditStore;
        RFile* iEditFile;

        CMDXMLDocument* iDom;
        CMDXMLComposer* iComposer;
        MUniSmilComposeObserver* iObserver;
        CActiveSchedulerWait     iObjWait;
    };

#include <UniSmilList.inl>

#endif // __UNISMILLIST_H
