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


#include <e32std.h>
#include <gdi.h>

#include <gmxmldocument.h>
#include <gmxmlnode.h>
#include <gmxmlelement.h>

#include <ConformanceChecker.h>
#include "UniSmilUtils.h"


// ---------------------------------------------------------
// UniSmilUtils::StringToIntValue
// Convert a descriptor to TInt
// ---------------------------------------------------------
//
EXPORT_C TInt UniSmilUtils::StringToIntValue( const TDesC& aString, 
                                                    TInt aDefault )
    {
    TInt value = aDefault;
    if( aString.Length() != 0 )
        {
        TChar c = aString[0];
        if( c.IsDigit() )
            {
            TLex luthor( aString );
            luthor.Val( value );
            }
        else
            {
            value = aDefault;
            }
        }
    return value;
    }


// ---------------------------------------------------------
// UniSmilUtils::IsMediaElem
// ---------------------------------------------------------

EXPORT_C TBool UniSmilUtils::IsMediaElem( CMDXMLNode* aNodePtr )
{
    TBool ret = EFalse;

    if ( aNodePtr->NodeType() == CMDXMLNode::EElementNode &&
         UniSmilUtils::MediaElemType( aNodePtr) != EMsgMediaUnknown )
        {
        ret = ETrue;
        }

    return ret;
}

// ---------------------------------------------------------
// UniSmilUtils::NextNode
// ---------------------------------------------------------

EXPORT_C CMDXMLNode* UniSmilUtils::NextNode(CMDXMLNode* aRefNode)
{
    if( aRefNode->FirstChild() )
        {
        aRefNode = aRefNode->FirstChild();
        }
    else
        {
        if( aRefNode->NextSibling() )
            {
            // move to the next sibling if exists
            aRefNode = aRefNode->NextSibling();
            }
        else
            {
            // No siblings at this level. Go up to parent.
            // might need to go up several layers so WHILE rather than IF
            while ( aRefNode && !aRefNode->NextSibling() )
                {
                // no siblings, move back to parent.
                aRefNode = aRefNode->ParentNode();
                }

            // Check if we stopped because node has siblings.
            if ( aRefNode && aRefNode->NextSibling() )
                {
                aRefNode = aRefNode->NextSibling();
                }
            }
        }

    return aRefNode;
}

// ---------------------------------------------------------
// UniSmilUtils::RemoveChildren
// ---------------------------------------------------------

EXPORT_C void UniSmilUtils::RemoveChildren(
    CMDXMLNode* aParent )
    {
    CMDXMLNode* child = NULL;

    if ( aParent )
        {
        child = aParent->FirstChild();
        }

    while ( child )
        {
        if ( child->HasChildNodes() )
            {
            UniSmilUtils::RemoveChildren( child );
            }
        else
            {
            CMDXMLNode* n = child;
            child = child->NextSibling();
            aParent->RemoveChild( n );
            delete n;
            }
        }
    }


// ---------------------------------------------------------
// UniSmilUtils::ElemType
// ---------------------------------------------------------

EXPORT_C TMsgMediaType UniSmilUtils::MediaElemType(CMDXMLNode* aNodePtr) 
{
    TMsgMediaType type(EMsgMediaUnknown);

    if ( aNodePtr )
        {
        TPtrC nName;
        nName.Set(aNodePtr->NodeName());

        if ( nName.Compare(KSMILWREleAnimation) == 0 )
            {
            type = EMsgMediaAnimation;
            }
        else if (nName.Compare(KSMILWREleAudio) == 0 )
            {
            type = EMsgMediaAudio;
            }
        // SVG included. No special SVG element tag exist.
        else if (nName.Compare(KSMILWREleImg) == 0 )
            {
            type = EMsgMediaImage;
            }
        else if (nName.Compare(KSMILWREleRef) == 0 )
            {
            type = EMsgMediaRef;
            }
        else if ( nName.Compare(KSMILWREleText) == 0 )
            {
            type = EMsgMediaText;
            }
        else if ( nName.Compare(KSMILWREleTextstream) == 0 )
            {
            type = EMsgMediaTextStream;
            }
        else if ( nName.Compare(KSMILWREleVideo) == 0 )
            {
            type = EMsgMediaVideo;
            }
        }

    return type;
}

// ---------------------------------------------------------
// UniSmilUtils::GetId
// ---------------------------------------------------------

EXPORT_C TInt UniSmilUtils::GetId( CMDXMLNode* aNodePtr, TPtrC& aId )
    {
    TInt ret = KErrNotSupported;

    if( aNodePtr && aNodePtr->NodeType() == CMDXMLNode::EElementNode )
        {
        CMDXMLElement* e =(CMDXMLElement*) aNodePtr;
        if ( e->IsAttributeSpecified( KSMILWRParaId ))
            {
            e->GetAttribute(KSMILWRParaId, aId);
            ret = KErrNone;
            }
        else
            {
            ret = KErrNotFound;
            }
        }
    return ret;
    }

// ---------------------------------------------------------
// UniSmilUtils::GetMetaTag
// ---------------------------------------------------------

EXPORT_C TInt UniSmilUtils::GetMetaTag( CMDXMLNode* aNodePtr, 
                                   TPtrC& aName, 
                                   TPtrC& aContent )
    {
    TInt ret = KErrNotFound;

    if( aNodePtr &&
        aNodePtr->NodeType() == CMDXMLNode::EElementNode &&
        aNodePtr->NodeName().Compare(KSMILWREleMeta) == 0)
        {
        CMDXMLElement* e = (CMDXMLElement*) aNodePtr;

        if ( e->IsAttributeSpecified( KSMILWRParaName ) &&
             e->IsAttributeSpecified( KSMILWRParaContent ))
            {
            if (!e->GetAttribute(KSMILWRParaName, aName) &&
                !e->GetAttribute(KSMILWRParaContent, aContent))
                {
                ret = KErrNone;
                }
            }
        }
    return ret;
    }

// ---------------------------------------------------------
// UniSmilUtils::RegionId
// ---------------------------------------------------------

EXPORT_C TInt UniSmilUtils::GetRegionId( CMDXMLNode* aNodePtr, TPtrC& aId )
    {
    TInt ret = KErrNotSupported;

    if ( aNodePtr )
        {
        if ( UniSmilUtils::IsMediaElem( aNodePtr ) )
            {
            ret = ((CMDXMLElement*)aNodePtr)->GetAttribute(KSMILWRParaRegion, aId);
            }
        }
    return ret;
    }

// ---------------------------------------------------------
// UniSmilUtils::GetSrc
// ---------------------------------------------------------

EXPORT_C TPtrC UniSmilUtils::GetSrcL( CMDXMLNode* aNodePtr )
{
    TPtrC src = TPtrC();
    if ( aNodePtr )
            {
        CMDXMLElement* e = (CMDXMLElement*) aNodePtr;

        if ( e && e->IsAttributeSpecified(KSMILWRParaSrc) )
            {
            User::LeaveIfError( e->GetAttribute(KSMILWRParaSrc, src) );
            }
        else
            {
            // Ok not to have src i.e. no default content.
            }
        }

    return src;
}


// ---------------------------------------------------------
// UniSmilUtils::GetSmilTypeL
//
// ---------------------------------------------------------
EXPORT_C TUniSmilType UniSmilUtils::GetSmilTypeL( CMDXMLDocument* aDom )
    {
	if (!aDom)
		{
		return ENoSmil;
		}

    if ( UniSmilUtils::IsTemplateSmil( aDom ) )
        {
        return ETemplateSmil;
        }
    else if ( !aDom->DocumentElement() ||
              !aDom->DocumentElement()->FirstChild() )
        {
        return ENoSmil;
        }
    else
        {
        CConformanceChecker* checker = CConformanceChecker::NewL();
        CleanupStack::PushL( checker );
        TBool isConf( EFalse );

        isConf = checker->Check( aDom, EMmsSmil_v2_0,
            EAllowVideoTag |
            EAllowNonMilliseconds |
            EAllowAllAttributes |
            EAllowMixedRegionDimensions |
            EAllowAnyRegionNames );

        CleanupStack::PopAndDestroy( checker );

        if( isConf )
            {
            return EMmsSmil;
            }

        }
    return E3GPPSmil;
    }


// ---------------------------------------------------------
// UniSmilUtils::IsTemplateSmil
//
// ---------------------------------------------------------

EXPORT_C TBool UniSmilUtils::IsTemplateSmil( CMDXMLDocument* aDom )
    {
    TBool ret( EFalse );

    CMDXMLNode* n = aDom->DocumentElement()->FirstChild();

    while ( n )
        {
        if ( n->NodeName().Compare( KSMILWREleMeta ) == 0)
            {
            TPtrC v1, v2;
            if ( !UniSmilUtils::GetMetaTag( n, v1, v2 ) )
                {
                if ( v1.Compare( KSMILMetaSmilProfile ) == 0 &&
                     v2.Compare( KSMILMetaSmilProfileValue ) == 0)
                    {
                    ret = ETrue;
                    break;
                    }
                }
            }
        else if ( n->NodeName().Compare( KSMILWREleBody ) == 0 )
            {
            // Cannot anymore have metatags
            break;
            }
        n = UniSmilUtils::NextNode( n );
        }
    return ret;
    }


// EOF
