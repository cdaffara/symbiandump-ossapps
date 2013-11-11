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
*       Static utility class for manipulating SMIL DOM
*
*
*/


#ifndef __UNISMILUTILS_H
#define __UNISMILUTILS_H

// ========== INCLUDE FILES ================================
#include <gdi.h>

#include <MsgMedia.hrh>

#include <UniModelConst.h>

// ========== CONSTANTS ====================================

// SMIL ELEMENTS
_LIT(KSMILWREleBody,        "body");
_LIT(KSMILWREleHead,        "head");
_LIT(KSMILWREleLayout,      "layout");
_LIT(KSMILWRElePar,         "par");
_LIT(KSMILWREleRegion,      "region");
_LIT(KSMILWREleRoot_layout, "root-layout");
_LIT(KSMILWREleSeq,         "seq");
_LIT(KSMILWREleSmil,        "smil");
_LIT(KSMILWREleMeta,        "meta");
_LIT(KSMILWREleTrans,       "transition");

// Media types
_LIT(KSMILWREleAnimation,   "animation");
_LIT(KSMILWREleAudio,       "audio");
_LIT(KSMILWREleImg,         "img");
_LIT(KSMILWREleRef,         "ref");
_LIT(KSMILWREleText,        "text");
_LIT(KSMILWREleTextstream,  "textstream");
_LIT(KSMILWREleVideo,       "video");

// SMIL PARAMS
_LIT(KSMILWRParaId,         "id" );
_LIT(KSMILWRParaName,       "name");
_LIT(KSMILWRParaContent,    "content");
_LIT(KSMILWRParaRegion,     "region");
_LIT(KSMILWRParaBgColor,    "backgroundColor");
_LIT(KSMILWRParaSrc,        "src");
_LIT(KSMILWRParaFit,        "fit");
_LIT(KSMILWRParaDur,        "dur");
_LIT(KSMILWRParaType,       "type");      // Transition
_LIT(KSMILWRParaSubType,    "subtype");   // Transition
_LIT(KSMILWRParaTransIn,    "transIn" );
_LIT(KSMILWRParaTransOut,   "transOut" );
_LIT(KSMILWRParaBegin,      "begin" );
_LIT(KSMILWRParaEnd,        "end" );

// Text color mediaparam format:
// <param name="foreground-color" value="#000000"/>
_LIT ( KSMILWREleParam,     "param" );
// _LIT ( KSMILWRParaName,      "name" );  Already defined
_LIT ( KSMILWRParaValue,    "value" );

// Region dimensions
_LIT(KSMILWRParaWidth,      "width");
_LIT(KSMILWRParaHeight,     "height");
_LIT(KSMILWRParaTop,        "top");
_LIT(KSMILWRParaLeft,       "left");
_LIT(KSMILWRParaBottom,     "bottom");
_LIT(KSMILWRParaRight,      "right");

// Region ID's
_LIT(KSMILWRRegIDImage,     "Image");
_LIT(KSMILWRRegIDVideo,     "Video");
_LIT(KSMILWRRegIDText,      "Text");

// TEMPORARY - To recognise template
_LIT(KSMILMetaSmilProfile,       "SMILProfile");
_LIT(KSMILMetaSmilProfileValue,  "3GPP-SMIL-Rel-5");


// ========== MACROS =======================================

// ========== DATA TYPES ===================================

// ========== FUNCTION PROTOTYPES ==========================

// ========== FORWARD DECLARATIONS =========================
class CMDXMLNode;
class CMDXMLDocument;

// ========== CLASS DECLARATION ============================

/**
* Static utility class for manipulating SMIL DOM
*
* @lib UniDataModel.lib
* @since 3.1
*/
class UniSmilUtils
    {

    public:

    /**
    * StringToIntValue
    * Convert a descriptor to TInt
    *
    * @since 3.1
    * @param aString  IN: to be converted
    * @param aDefault IN: default value to return if conversion fails
    * @return converted value
    */
    IMPORT_C static TInt StringToIntValue( const TDesC& aString, TInt aDefault );

    /**
    * IsMediaElem
    * Checks if node is mediaobject
    *
    * @since 3.1
    * @param aNodePtr  IN: node to check
    * @return ETrue node is media objects, EFalse otherwise
    */
    IMPORT_C static TBool IsMediaElem( CMDXMLNode *aNodePtr );

    /**
    * NextNode
    * Finds the next node in DOM in order
    * 1. Child nodes of the ref node first
    * 2. Then siblings of the ref node
    * 3. If ref node has no children or siblings moves up to
    *     parent node and continues there
    * 4. Returns NULL at the end of the document.
    *
    * @since 3.1
    * @param aRefNode  IN: current node
    * @return next node in presentation
    */
    IMPORT_C static CMDXMLNode* NextNode( CMDXMLNode* aRefNode );

    /**
    * Removechildren
    * Removes all children and children of children etc. from given node
    *
    * @since 3.1
    * @param IN aParent, node whos children are to be removed
    */
    IMPORT_C static void RemoveChildren( CMDXMLNode* aParent ); 

    /**
    * MediaElemType
    * Gets the media type of the object (text, img, video etc)
    *
    * @since 3.1
    * @param IN aNodePtr, media object node
    * @return mediatype. EMsgMediaUnknown if node is not media object.
    */
    IMPORT_C static TMsgMediaType MediaElemType( CMDXMLNode* aNodePtr ); 

    /**
    * Gets the id value of the node.
    *
    * @since 3.1
    * @param IN  aNodePtr, element
    * @param OUT aId, id of the element if found, empty otherwise.
    * @return KErrNone if found. KErrNotFound if element but no id,
    *         KErrNotSupported if not element.
    */
    IMPORT_C static TInt GetId( CMDXMLNode* aNodePtr, TPtrC& aId );

    /**
    * Gets the name and content of the metatag i.e. <meta name="x1" content="x2" />
    *
    * @since 3.1
    * @param IN  aNodePtr, must be element
    * @param OUT aName,  name of the metatag. Value is garbage if function
    *                    returns KErrNotFound
    * @param OUT aContent, content of the metatag. Value is garbage if function
    *                    returns KErrNotFound
    * @return KErrNone if both ok. KErrNotFound otherwise.
    */
    IMPORT_C static TInt GetMetaTag(
        CMDXMLNode* aNodePtr,
        TPtrC& aName,
        TPtrC& aContent );

    /**
    * Gets the region id i.e. region="xx" value of the node.
    * Node must be media element node.
    *
    * @since 3.1
    * @param IN  aNodePtr, media element
    * @param OUT aId, regoin id of the element if found, empty otherwise.
    * @return KErrNone if found. KErrNotSupported if not media element,
    *         KErrNotFound if no region specified.
    */
    IMPORT_C static TInt GetRegionId( CMDXMLNode* aNodePtr, TPtrC& aId );

    /**
    * Gets the location string from nodes src parameter.
    *
    * @since 3.1
    * @param    aNodePtr, media element
    * @return   value of the location string.
    */
    IMPORT_C static TPtrC GetSrcL( CMDXMLNode* aNodePtr );

    /**
    * Gets SMIL type of the document.
    * Tries to validate document according to conformance document requirements
    * and SmilEditor template requirements.
    *
    * @since 3.1
    * @param    aDom    Root node of the document tree.
    * @return           Smil type of the document.
    */
    IMPORT_C static TUniSmilType GetSmilTypeL( CMDXMLDocument* aDom );

    /**
    * Gets SMIL type of the document.
    * Tries to validate document according to SmilEditor template requirements.
    *
    * @since 3.1
    * @param    aDom    Root node of the document tree.
    * @return           ETrue if document is template SMIL.
    */
    IMPORT_C static TBool IsTemplateSmil( CMDXMLDocument* aDom );
    
    };

#endif // __UNISMILUTILS_H
