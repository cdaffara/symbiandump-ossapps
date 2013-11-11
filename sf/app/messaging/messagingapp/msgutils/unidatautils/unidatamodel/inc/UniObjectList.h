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
*       CUniObjectList, List of attachments.
*
*
*/


#ifndef __UNIOBJECTLIST_H
#define __UNIOBJECTLIST_H

// INCLUDES

#include <e32std.h>
#include <f32file.h>
#include <apmstd.h>

#include <cmsvattachment.h>
#include <UniObject.h>


// CONSTANTS

// MACROS

// FORWARD DECLARATIONS
class CUniDataUtils;
class CMDXMLDocument;
class CMDXMLNode;
class CUniObjectList;

// DATA TYPES

// FUNCTION PROTOTYPES

// CLASS DECLARATION

/**
* MUniObjectListObserver - Callback API for object list operations.
*
* @since 3.1
*/
class MUniObjectListObserver
    {
    public:

        /**
        * Called when object is added to the list.
        *
        * @since 3.1
        * @param aList IN List where object is added.
        * @param aObject IN Added object.
        * @param aIndex IN Index number of added object.
        */
        virtual void ObjectAddedL( CUniObjectList* aList,
                                   CUniObject* aObject,
                                   TInt aIndex ) = 0;

        /**
        * Called when object is removed from the list.
        *
        * @since 3.1
        * @param aList IN List where object is added.
        * @param aObject IN Removed object.
        * @param aIndex IN Index number of removed object.
        */
        virtual void ObjectRemovedL( CUniObjectList* aList,
                                     CUniObject* aObject,
                                     TInt aIndex ) = 0;
    };

/**
* CUniObjectList - List of attachments.
*
* @lib UniDataModel.lib
* @since 3.1
*/
class CUniObjectList : public CBase,
                       public MUniObjectSaveObserver
    {

    public:  // New methods

        /**
        * Factory method that creates this object.
        *
        * @since    3.1
        * @param    aMtm    IN reference to client mtm
        * @param    aData   IN reference to MsgData object
        * @return   Pointer to instance
        */
        IMPORT_C static CUniObjectList* NewL( CBaseMtm& aMtm, CUniDataUtils& aData );

        /**
        * Destructor
        *
        * @since    3.1
        */
        virtual ~CUniObjectList();

        /**
        * AddObjectL
        * Appends object to object array.
        *
        * @since    3.1
        * @param    aObject     IN object to be added
        */
        IMPORT_C void AddObjectL( CUniObject* aObject );

        /**
        * SpaceNeededForSaveAll
        * Calculates disk space needed to save all objects
        * that are not yet saved into message store.
        *
        * @since    3.1
        * @return   Disk space needed in bytes
        */
        IMPORT_C TInt SpaceNeededForSaveAll();

        /**
        * SaveAll
        * Calls SaveL for each object in the object array
        *
        * @since    3.1
        * @param    aObserver   IN operation observer
        * @param    aSaveType   IN save type
        */
        IMPORT_C void SaveAll( MUniObjectSaveObserver& aObserver, CMsvAttachment::TMsvAttachmentType aSaveType );

        /**
        * RemoveAllObjectsL
        * Removes all objects from message store and object
        * array. Deletes all objects.
        *
        * @since    3.1
        */
        IMPORT_C void RemoveAllObjectsL();

        /**
        * RemoveObjectL
        * Removes an object from object array and also from
        * message store if requested.
        * The caller must delete the object!!!
        *
        * @since    3.1
        * @param    aObject IN object to be removed
        * @param    aRemoveFromStore    IN if EFalse remove only from object array,
        *                               if ETrue remove also from message store
        */
        IMPORT_C void RemoveObjectL( CUniObject* aObject, TBool aRemoveFromStore = ETrue );

        /**
        * GetByIndex
        * Get object by index
        *
        * @since    3.1
        * @param    aIndex  IN index
        * @return   Pointer to object
        */
        IMPORT_C CUniObject* GetByIndex( TInt aIndex );

        /**
        * GetByAttachmentId
        * Get object by attachment id
        *
        * @since    3.1
        * @param    aAttachmentId   IN attachment id
        * @return   Pointer to object
        */
        IMPORT_C CUniObject* GetByAttachmentId( TMsvAttachmentId aAttachmentId );

        /**
        * GetByNode
        * Get object by DOM node
        *
        * @since    3.1
        * @param    aNode   IN DOM node
        * @return   Pointer to object
        */
        IMPORT_C CUniObject* GetByNodeL( CMDXMLNode* aNode );

        /**
        * GetByUrl
        * Get object by URL
        *
        * @since    3.1
        * @param    aUrl    IN URL
        * @return   Pointer to object
        */
        IMPORT_C CUniObject* GetByUrlL( const TDesC& aUrl );

        /**
        * ObjectByteSize
        * Calculate size of all the objects in bytes
        *
        * @since    3.1
        * @return   Size in bytes
        */
        IMPORT_C TInt ObjectByteSize() const;

        /**
        * EnsureAllObjectsHaveContentLocationL
        * Makes sure that all objects have ContentLocation
        * MIME header defined. All objects must have ContentLocation
        * when SMIL is composed.
        *
        * @since    3.1
        */
        IMPORT_C void EnsureAllObjectsHaveContentLocationL();
        
        /**
        * SetListObserver
        * Sets observer to the list that changes to list are notified.
        *
        * @since    3.1
        * @param aObserver IN New object list observer. Setting observer
        *                     to NULL resets observer.
        */
        IMPORT_C void SetListObserver( MUniObjectListObserver* aObserver );
        
        /**
        * Returns object based on unique handle.
        *
        * @param  aUniqueControlHandle Control handle that uniquely identified the control
        * @return Pointer to matched uniobject or NULL if not found.
        */
        IMPORT_C CUniObject* GetObjectByUniqueHandle( TInt aUniqueControlHandle );
        
        /**
        * Count
        *
        * @since    3.1
        * @return   Number of objects
        */
        inline TInt Count() const;

    public:

        /**
        * From MUniObjectSaveObserver
        */
        void ObjectSaveReady( TInt aError );

    protected:

        /**
        * Constructor.
        *
        * @since    3.1
        * @param    aMtm    IN reference to client mtm
        * @param    aData   IN reference to MsgData object
        */
        CUniObjectList( CBaseMtm& aMtm, CUniDataUtils& aData );

        /**
        * 2nd phase constructor.
        *
        * @since    3.1
        */
        void ConstructL();

    protected: // data

        CArrayPtrFlat<CUniObject>* iObjectArray;
        CUniDataUtils& iData;
        CBaseMtm& iMtm;

        MUniObjectSaveObserver* iSaveObserver;
        CMsvAttachment::TMsvAttachmentType iSaveType;
        TInt iSaveCount;

        MUniObjectListObserver* iListObserver;
    };

#include <UniObjectList.inl>

#endif // __UNIOBJECTLIST_H
