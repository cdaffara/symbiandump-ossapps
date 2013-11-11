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
*       Inlines for CUniDataModel
*
*
*/



// ---------------------------------------------------------
// CUniDataModel::ObjectList
// ---------------------------------------------------------
//
inline CUniObjectList& CUniDataModel::ObjectList() const
    {
    
    return *iObjectList;
    }

// ---------------------------------------------------------
// CUniDataModel::AttachmentList
// ---------------------------------------------------------
//
inline CUniObjectList& CUniDataModel::AttachmentList() const
    {
    return *iAttachmentList;
    }

// ---------------------------------------------------------
// CUniDataModel::SmilList
// ---------------------------------------------------------
//
inline CUniSmilList& CUniDataModel::SmilList() const
    {
    return *iSmilList;
    }

// ---------------------------------------------------------
// CUniDataModel::DataUtils
// ---------------------------------------------------------
//
inline CUniDataUtils& CUniDataModel::DataUtils() const
    {
    return *iDataUtils;
    }

// ----------------------------------------------------
// CUniDataModel::SetSmilType
// ----------------------------------------------------
//
inline TUniSmilType CUniDataModel::SmilType()
    {
    return iSmilType;
    }

// ----------------------------------------------------
// CUniDataModel::SetSmilType
// ----------------------------------------------------
//
inline void CUniDataModel::SetSmilType( TUniSmilType aSmilType )
    {
    iSmilType = aSmilType;
    }

// ---------------------------------------------------------
// CUniDataModel::SmilModel
// ---------------------------------------------------------
//
inline CUniSmilModel& CUniDataModel::SmilModel() const
    {
#ifdef _DEBUG_TRACES_
    qCritical() << " CUniDataModel SmilModel start";
#endif

    return *iSmilModel;
    }

// ---------------------------------------------------------
// CUniDataModel::MediaResolver
//
// ACCESSOR
// ---------------------------------------------------------
//
inline CMsgMediaResolver& CUniDataModel::MediaResolver() const
    {
    return *iMediaResolver;
    }

// ---------------------------------------------------------
// CUniDataModel::MmsConformance
//
// ACCESSOR
// ---------------------------------------------------------
//
inline CMmsConformance& CUniDataModel::MmsConformance() const
    {
    return *iMmsConformance;
    }

// ---------------------------------------------------------
// CUniDataModel::Dom
//
// Accessor.
// ---------------------------------------------------------
//
inline CMDXMLDocument* CUniDataModel::Dom()
    {
    return iDom;
    }

// ---------------------------------------------------------
// CUniDataModel::SetDom
//
// Mutator.
// ---------------------------------------------------------
//
inline void CUniDataModel::SetDom( CMDXMLDocument* aDom )
    {
    delete iDom;
    iDom = aDom;
    }

// ---------------------------------------------------------
// CUniDataModel::ParseResult
//
// ---------------------------------------------------------
//
inline TInt CUniDataModel::ParseResult() const
    {
    return iParseResult;
    }

