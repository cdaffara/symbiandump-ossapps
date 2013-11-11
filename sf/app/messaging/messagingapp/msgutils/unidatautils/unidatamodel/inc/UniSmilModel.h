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
*       CUniSmilModel, Storage for Smil presentation data.
*
*
*/


#ifndef __UNISMILMODEL_H
#define __UNISMILMODEL_H

// INCLUDES
#include <cmsvattachment.h>

#include <UniModelConst.h>
#include <UniObjectList.h>
#include <UniSmilUtils.h>

// CONSTANTS

// MACROS

// FORWARD DECLARATIONS

class RResourceFile;

class CBaseMtm;
class CUniSmilSlide;
class CSmilPresentation;
class TDataType;
class CMsvEntrySelection;
class CUniDataUtils;
class CUniObject;
class CUniTextObject;
class CEikRichTextEditor;

class CMDXMLNode;
class CMDXMLElement;
class CUniSmilParams;

// DATA TYPES

typedef CArrayPtrFlat<CUniSmilSlide> CSmilSlideArray;

// FUNCTION PROTOTYPES

// CLASS DECLARATION

/**
* CUniSmilModel - Representation of SMIL presentation data.
* Hides SMIL syntax from client.
*
* @lib UniDataModel.lib
* @since 3.1
*/
class CUniSmilModel : public CBase
    {
    public:  // New methods

        /**
        * NewLC
        * Factory method that creates this object.
        *
        * @since    3.1
        * @param    aFs     IN reference to file session
        * @param    aMtm    IN reference to client mtm
        * @param    aData   IN reference to MsgData object
        * @param    aObjectList IN reference to object list
        * @return   Pointer to instance in cleanup stack
        */
        IMPORT_C static CUniSmilModel* NewLC(
            RFs& aFs,
            CBaseMtm& aMtm,
            CUniDataUtils& aData,
            CUniObjectList& aObjectList );

        /**
        * NewL
        * Factory method that creates this object.
        *
        * @since    3.1
        * @param    aFs     IN reference to file session
        * @param    aMtm    IN reference to client mtm
        * @param    aData   IN reference to MsgData object
        * @param    aObjectList IN reference to object list
        * @return   Pointer to instance
        */
        IMPORT_C static CUniSmilModel* NewL(
            RFs& aFs,
            CBaseMtm& aMtm,
            CUniDataUtils& aData,
            CUniObjectList& aObjectList );

        /**
        * Destructor
        *
        * @since    3.1
        */
        virtual ~CUniSmilModel();

        /**
        * ParseL
        * Creates presentation from either from DOM (if set) or
        * from object list (no SMIL used).
        * Set various flags according to parse results.
        *
        * This function should only leave if model cannot be made
        * because some basic Symbian OS error for example OOM.
        * This never leaves if there is some kind of mitchmatch between
        * presentation and actual message ie smil refers to object
        * that does not exist. This kind of leaves are TRAPPED here
        * and if they are not the leave should be considered as an
        * implementation bug in this function.
        *
        * @since    3.1
        * @return   Parse result flags
        */
        IMPORT_C TInt ParseL( CMDXMLDocument* aDom );

        /**
        * SetScreenSize
        * Sets size of physical dislplay in pixels
        *
        * @since    3.1
        * @param    aScreenSize     IN screen size in pixels
        */
        inline void SetScreenSize( const TSize& aScreenSize );

        /*
        * AddSlideL
        * Adds slide to model. If aSlideNum is -1 or greater than
        * current max -> appends. Otherwise adds to given pos.
        * Zero means first.
        *
        * @since    3.1
        * @param    aSlideNum   IN insert position
        */
        IMPORT_C void AddSlideL( TInt aSlideNum = -1 );

        /**
        * RemoveSlide
        * Removes slide. Slide must be empty!
        *
        * @since    3.1
        * @param    aSlideNum   IN position of slide to be removed
        */
        IMPORT_C void RemoveSlide( TInt aSlideNum );

        /**
        * MoveSlideL
        * Moves slide.
        *
        * @since    3.1
        * @param    aSlideToMove IN the original position of slide to be moved
        * @param    aPlaceToMove IN place where slide is moved to
        */
        IMPORT_C void MoveSlideL( TInt aSlideToMove, TInt aPlaceToMove );

        /**
        * SortSlidesL
        * Sorts slides accoring to a separate comparison function
        * called SortSlidesCompare().
        * Insertion sort algorithm.
        *
        * Notice! Intended to be used with SendAs when there
        * are multiple slides - each slide containing only
        * one object. (Sorts slides accoring to first object in
        * the slide.)
        *
        * @since    3.1
        */
        IMPORT_C void SortSlidesL();

        /*
        * IsSlide
        * Check if model contains given slide
        *
        * @since    3.1
        * @param    aSlideNum   IN slide to check
        * @return   ETrue, if slide exists,
        *           EFalse, otherwise
        */
        IMPORT_C TBool IsSlide( TInt aSlideNum ) const;

        /**
        * Adds text buffer to model.
        *
        * @since    3.1
        * @param    aSlideNum   IN slide number where text is added
        * @param    aText   IN text buffer to be added
        * @return   Pointer to the added text object
        *           Ownership NOT transferred.
        */
        IMPORT_C CUniTextObject* AddTextObjectL(
            TInt aSlideNum, 
            CEikRichTextEditor* aText );

        /**
        * AddObjectL
        * Adds an object into msg store and smil slide
        *
        * @since    3.1
        * @param    aSlideNum   IN slide number where text is added
        * @param    aPathToObject   IN file to be added
        * @return   Pointer to the added object
        *           Ownership NOT transferred.
        */
        IMPORT_C CUniObject* AddObjectL(
            MUniObjectSaveObserver& aObserver,
            TInt aSlideNum,
            CMsgMediaInfo* aMedia );

        /**
        * AddStoredObjectL
        * Adds an object that is already in msg store to smil slide
        *
        * @since    3.1
        * @param    aSlideNum   IN slide number where text is added
        * @param    aAttachmentId   IN file to be added
        * @param    aMedia      IN media info - ownership transferred to MsgObject
        * @return   Pointer to the added object
        *           Ownership NOT transferred.
        */
        IMPORT_C CUniObject* AddStoredObjectL(
            TInt aSlideNum,
            TMsvAttachmentId aAttachmentId,
            CMsgMediaInfo* aMedia );

        /**
        * RemoveObjectL
        * Deletes an object of certain type from a slide and msg store
        * if needed (last reference to the object removed).
        *
        * @since    3.1
        * @param    aSlideNum   IN slide number where text is added
        * @param    aObject     IN the object to be removed
        * @param    aLeaveToStore IN Do not remove attachment from store
        *                            USE ONLY AS SECOND ATTEMPT IF REMOVING NORMALLY FAILS
        */
        IMPORT_C void RemoveObjectL(
            TInt aSlideNum,
            CUniObject* aObject,
            TBool aLeaveToStore = EFalse);

        /**
        * RemoveObjectByIndexL
        * Deletes the N'th object of from a slide and msg store if
        * needed (last reference to the object removed).
        *
        * @since    3.1
        * @param    aSlideNum   IN slide number
        * @param    aObjectNum  IN number of the object to be removed
        */
        IMPORT_C void RemoveObjectByIndexL(
            TInt aSlideNum,
            TInt aObjectNum );

        /**
        * GetObjectByIndex
        * Gets object by it's layout position in slide.
        * E.g. object number "0" is the first in layout.
        *
        * @since    3.1
        * @param    aSlideNum   IN slide number
        * @param    aObjectNum  IN number of the object
        * @return   Pointer to the object,
        *           NULL if not found
        */
        IMPORT_C CUniObject* GetObjectByIndex(
            TInt aSlideNum,
            TInt aObjectNum ) const;

        /**
        * GetObject
        * Gets object of certain type from a slide
        *
        * @since    3.1
        * @param    aSlideNum   IN slide number
        * @param    aObjectType IN type of the object
        * @return   Pointer to the object,
        *           NULL if not found
        */
        IMPORT_C CUniObject* GetObject(
            TInt aSlideNum,
            TUniRegion aRegion ) const;

        /**
        * IsObject
        * Checks if given slide has object of certain type.
        *
        * @since    3.1
        * @param    aSlideNum   IN slide number
        * @param    aObjectType IN type of the object
        * @param    aNum    IN the ordinal number of the object
        * @return   ETrue, slide has object of asked type,
        *           EFalse, otherwise.
        */
        //TBool IsObject(
        //    TInt aSlideNum,
        //    TMsgMediaType aObjType ) const;

        /**
        * SlideCount
        * Count of slides in SMIL presentation.
        *
        * @since    3.1
        * @return   Number of slides
        */
        inline TInt SlideCount() const;

        /**
        * ObjectCount
        * Count of media objects in SMIL presentation.
        * NOTE: Smil attachment is _not_ counted as object.
        *
        * @since    3.1
        * @return   Number of objects
        */
        IMPORT_C TInt ObjectCount() const;

        /**
        * SlideObjectCount
        * Count of media objects in given slide
        *
        * @since    3.1
        * @param    aSlideNum   IN slide number
        * @return   Number of objects on the slide
        */
        IMPORT_C TInt SlideObjectCount( TInt aSlideNum ) const;

        /**
        * Layout
        * Accessor. Queries current layout. Layout is same for all slides.
        *
        * @since    3.1
        * @return   Layout
        */
        inline TUniLayout Layout() const;

        /**
        * SetLayoutL
        * Mutator. All slides have same layout.
        * NOTE: This should only be called from editor objectsview
        * ChangeOrder command. Layout is automatically determined
        * at parse time.
        *
        * @since    3.1
        * @param    aLayout IN new layout to be set
        */
        IMPORT_C void SetLayoutL( TUniLayout aNewLayout ); 

        /**
        * Returns the (estimate) total size of SMIL.
        *
        * @since    3.1
        * @return   Total size in bytes
        */
        IMPORT_C TInt SmilComposeSize();

        /**
        * SmilComposeSize
        * Returns the (estimate) size of SMIL from beginning to the
        * object given as parameter.
        *
        * @since    3.1
        * @param    aToSlideNum     IN calculate till this slide
        *                              (From "1" to "SlideCount")
        * @param    aToObjectNum    IN calculate till this object
        *                              (From "1" to "SlideObjectCount")
        * @return   Size in bytes
        */
        IMPORT_C TInt SmilComposeSize(
            TInt aToSlideNum,
            TInt aToObjectNum );

        /**
        * RemoveEmptySlides
        * Removes empty all slides from the presentation
        *
        * @since    3.1
        */
        IMPORT_C void RemoveEmptySlides();

        /**
        * ComposeL
        * Composes new SMIL from current model content. SMIL is created as
        * an attachment to message.
        * Old SMIL should be deleted before calling this.
        *
        * @since    3.1
        * return    Pointer to new DOM
        */
        IMPORT_C CMDXMLDocument* ComposeL();

        /**
        * ChangeLayoutL
        * Changes layout of SMIL to match Series 60 created SMIL
        *
        * @since 3.1
        * @param aDom   SMIL DOM to be changed
        * @return ETrue if layout could be changed
        *         EFalse if layout couldn't be changed (e.g. missing head or root-region)
        */
        IMPORT_C TBool ChangeLayoutL( CMDXMLDocument* aDom );
        
        /**
        * CorrectTimingL
        * Correct timing of media objects on SMIL if it is invalid.
        *
        * @since  3.1
        * @param  aDom   SMIL DOM
        * @return ETrue if timing is valid.
        *         EFalse if timing couldn't be corrected (e.g. missing body)
        */
        IMPORT_C TBool CorrectTimingL( CMDXMLDocument* aDom );
        
        /**
        * Determines if given media type can be added to the SMIL model.
        *
        * @since 3.1
        * @param aMediaType Tested media type
        * @return ETrue if media type can be added to the SMIL model.
        *         EFalse if media type cannot be added
        */
        IMPORT_C TBool IsSupportedMediaType( TMsgMediaType aMediaType ) const;
        
        /**
        * Maximum number of slide in MMS message.
        *
        * @return 0 or less if not specified. Otherwise maximum slide limit.
        */
        inline TInt MaxSlideCount() const;

        /**
        * Sets maximum slide limit.
        *
        * @param aSlideCount Maximum slide count.
        */
        inline void SetMaxSlideCount( TInt aSlideCount );

        /**
        * Returns whether maximum slide count has been reached.
        *
        * @return ETrue if maximum slide count is set and has been reached.
        *         EFalse otherwise.
        */
        inline TBool MaxSlideCountReached() const;

    private:

        /**
        * Constructor.
        *
        * @since    3.1
        * @param    aFs     IN reference to file session
        * @param    aMtm    IN reference to client mtm
        * @param    aData   IN reference to MsgData object
        * @param    aObjectList IN reference to object list
        */
        CUniSmilModel(
            RFs& aFs,
            CBaseMtm& aMtm,
            CUniDataUtils& aData,
            CUniObjectList& aObjectList );

        /**
        * 2nd phase constructor.
        *
        * @since    3.1
        */
        void ConstructL();

        /**
        * DoPresentationFromDomL
        * Makes a presentation (model content) from DOM.
        *
        * @since    3.1
        * @return   ETrue, if created successfully,
        *           EFalse, otherwise
        */
        void DoPresentationFromDomL( CMDXMLDocument* aDom );

        /**
        * IsMessageSlide
        * Check if messages attachments fit into slide (no smil case)
        *
        * @since    3.1
        * @return   ETrue, if all objects fit into slide,
        *           EFalse, otherwise
        */
        TBool IsMessageSlide();

        /**
        * DoNoSmilOneSlide
        * Makes presentation (max one slide) from object list. Message
        * may contain SMIL, but when calling this function it should be
        * clear that model cannot be made as SMIL describes.
        *
        * @since    3.1
        */
        void DoNoSmilOneSlideL();

        /**
        * DoNoSmilManySlide
        * Makes presentation from object list. Message may contain SMIL,
        * but when calling this function it should be clear that model cannot
        * be made as SMIL describes.
        *
        * @since    3.1
        */
        void DoNoSmilManySlideL();

        /**
        * AddSlideFromDomL
        * Used when creating presentation from SMIL
        *
        * @since    3.1
        * @param    aSlideNum   IN slide number
        * @param    aObj    IN object to be added
        * @return   ETrue, object added successfully,
        *           EFalse, otherwise
        */
        void AddSlideFromDomL( CMDXMLNode* aPage );

        /**
        * AddObjectFromDomL
        * Used when creating presentation from SMIL
        *
        * @since    3.1
        * @param    aSlideNum   IN slide number
        * @param    aObj    IN object to be added
        */
        void AddObjectFromDomL(
            CUniSmilSlide* aSlide,
            CUniObject* aObj,
            TUniRegion aRegion );

        /**
        * Reset
        * Clears flag & empties slide & object arrays.
        * NOTE: Mtm pointer is not reseted.
        *
        * @since    3.1
        */
        void Reset();

        /**
        * CreateItemL
        * Sub function for ComposeL
        *
        * @since    3.1
        * @param    aRoot   IN parent element for the new element(s)
        * @param    aObj    IN object to be added
        */
        TInt CreateItemL( CMDXMLDocument* aDom, CMDXMLElement* aRoot, CUniObject* aObj );

        /**
        * CreateParL
        * Sub function for ComposeL
        *
        * @since    3.1
        * @param    aRoot   IN parent element for the new element(s)
        * @param    aSlide  IN slide number
        */
        void CreateParL( CMDXMLDocument* aDom, CMDXMLElement* aRoot, TInt aSlide );

        /**
        * CreateBodyL
        * Sub function for ComposeL
        *
        * @since    3.1
        * @param    aRoot   IN parent element for the new element(s)
        */
        void CreateBodyL( CMDXMLDocument* aDom, CMDXMLElement* aRoot );

        /**
        * CreateLayoutL
        * Sub function for ComposeL
        *
        * @since    3.1
        * @param    aRoot        IN parent element for the new element(s)
        * @param    aRootLayout  IN root-layout element
        * @param    aLayout      IN layout element
        * @param    aTextRegion  IN text region element
        * @param    aImageRegion IN image region element
        */
        void CreateLayoutL(
            CMDXMLDocument* aDom,
            CMDXMLElement* aRoot,
            CMDXMLElement* aRootLayout = NULL,
            CMDXMLElement* aLayout = NULL,
            CMDXMLElement* aTextRegion = NULL,
            CMDXMLElement* aImageRegion = NULL );

        /**
        * CreateHeadL
        * Sub function for ComposeL
        *
        * @since    3.1
        * @param    aRoot   IN parent element for the new element(s)
        */
        void CreateHeadL( CMDXMLDocument* aDom, CMDXMLElement* aRoot );

        /**
        * DoRemoveObjectL
        * Deletes object from slide and msg store if needed.
        *
        * @since    3.1
        * @param    aSlideNum   IN slide number where text is added
        * @param    aObjectType IN type of the object to be removed
        */
        void DoRemoveObjectL( TInt aSlideNum,
                              CUniObject* aObject,
                              TBool aLeaveToStore = EFalse );

        /**
        * Adds slide. Appends if slidenum greater than current max,
        * otherwise inserts.
        *
        * @since    3.1
        * @param    aSlideNum   IN insert position
        */
        CUniSmilSlide* DoAddSlideL( TInt aSlideNum = -1 );

        /**
        * SortSlidesCompare
        * Compares slides according to time. Most recent is greatest.
        * Empty slides are greater than non-empty slides.
        *
        * @since    3.1
        * @param    aSlide1     IN slide to compare
        * @param    aSlide2     IN slide to compare
        * @return If aSlide1 "is greater than" aSlide2 returns positive value.
        *         If aSlide1 "is smaller than" aSlide2 returns negative value.
        *         If aSlide1 and aSlide2 "are equal" returns zero.
        */
        TInt SortSlidesCompare( CUniSmilSlide* aSlide1, CUniSmilSlide* aSlide2 );

        /**
        * Gets the layout of MMS SMIL document.
        * Assumes that the document is valid MMS SMIL.

        * @since 3.1
        * @param    aDom    Root node of the document tree.
        * @return           Layout of the document.
        */
        TUniLayout GetMmsSmilLayout( CMDXMLDocument* aDom );

        /**
        * Gets the first element of given name from the SMIL document.
        *
        * @since 3.1
        * @param    aDom    Root node of the document tree.
        * @param    aName   The name of the node to be searched.
        * @return           Pointer to the element node.
        *                   If element not found returns NULL.
        */
        CMDXMLNode* GetNode( CMDXMLDocument* aDom, const TDesC& aName );

        void GetMediaElemRegionIDs( CMDXMLNode* aPage );

        void GetAllMediaElemRegionIDs( CMDXMLNode* aPage );

        void GetLayoutElements(
            CMDXMLNode* aLayout,
            CMDXMLElement*& aRlEle,
            CMDXMLElement*& aTxtEle,
            CMDXMLElement*& aImgEle );

        void GetRootLayoutSize( CMDXMLElement* aRl, TSize& aSize );

        void GetRegionRect( CMDXMLElement* aRegion, TSize aRoot, TRect& aRect );

        TUniRegion GetRegionType( CMDXMLNode* aNodePtr );

        void RemoveMetaTagsAndComments( CMDXMLDocument* aDom );

        /**
        * CorrectSlideTimingL
        *
        * @since    3.1
        */
        void CorrectSlideTimingL( CMDXMLElement* aPage );

        /**
        * TimingSpecified
        *
        * @since    3.1
        */
        TBool TimingSpecified( CMDXMLElement* aNodePrt );

    private: // data

        RFs&                iFs;
        CBaseMtm&      iMtm;
        CUniDataUtils&           iData;
        CUniObjectList&     iObjectList;

        TUniLayout iSlideLayout; // Defines order of objects in display for slides.

        CSmilSlideArray*    iSlideArray;    // Holds slides
        TInt                iParseResult;
        CUniSmilParams*     iSmilParams;
        TSize               iScreenSize;
        TBool               iSizeEstimateUpToDate;
        TInt                iSizeEstimate;

        TPtrC               iTextRegionId;
        TPtrC               iImageRegionId;
        TInt                iMaxSlideCount;
    };


#include <UniSmilModel.inl>

#endif // __UNISMILMODEL_H
