/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description: Implementation of Smil reader class.
 * This is a wrapper class on top of Symbian GMXML parser. This class
 * provides simple APIs for reading smil DOM generated by the parser.
 */

//USER INCLUDES
#include "smildtd.h"
#include "smilxmlreader.h"
#include "smilliterals.h"

#include "debugtraces.h"


//---------------------------------------------------------------
// SmilXmlReader::NewL
// @see header
//---------------------------------------------------------------
SmilXmlReader* SmilXmlReader::NewL( RFile& filehandle )
    {
    SmilXmlReader* self = new (ELeave) SmilXmlReader();
    CleanupStack::PushL(self);
    self->ConstructL( filehandle);
    CleanupStack::Pop();
    return self;
    }

//---------------------------------------------------------------
// SmilXmlReader::ConstructL
// @see header
//---------------------------------------------------------------
void SmilXmlReader::ConstructL( RFile& filehandle )
    {
    ParseL( filehandle );
    }

//---------------------------------------------------------------
// SmilXmlReader::SmilXmlReader
// @see header
//---------------------------------------------------------------
SmilXmlReader::SmilXmlReader()
    {
    }

//---------------------------------------------------------------
// SmilXmlReader::~SmilXmlReader
// @see header
//---------------------------------------------------------------
SmilXmlReader::~SmilXmlReader()
    {
    delete xmlparser;
    }

//---------------------------------------------------------------
// SmilXmlReader::ParseFileCompleteL
// @see header
//---------------------------------------------------------------
void SmilXmlReader::ParseFileCompleteL()
    {
    if ( iWaitScheduler.IsStarted() )
        {
        iWaitScheduler.AsyncStop();
        
        //Take ownership
        CMDXMLDocument* dom = xmlparser->DetachXMLDoc();
        CleanupStack::PushL( dom );

        //Initialize the reader
        iRootNode = dom->DocumentElement();
        iCurrentNode = iRootNode;
        iState = EInitial;

        CleanupStack::Pop( dom );
        }
    }

//---------------------------------------------------------------
// SmilXmlReader::nextNodeL
// @see header
//---------------------------------------------------------------
CMDXMLNode* SmilXmlReader::nextNodeL()
    {
    CMDXMLNode* node = iCurrentNode;
    openNodeL( node );
    if( node->FirstChild() )
        node = node->FirstChild();
    else if( node->NextSibling() )
        {
        closeNodeL( node );
        node = node->NextSibling();
        }
    else
        {
        while( node )
            {
            closeNodeL( node );
            if( node->NextSibling() )
                {
                node = node->NextSibling();
                break;
                }
            node = node->ParentNode();
            }
        }

    //update new traversed node
    iCurrentNode = node;
    return node;
    }

//---------------------------------------------------------------
// SmilXmlReader::atEndL
// @see header
//---------------------------------------------------------------
TBool SmilXmlReader::atEndL()
    {
    TBool atend = EFalse;
    CMDXMLNode* node = iCurrentNode;

    if( node->FirstChild() || node->NextSibling() )
        atend = EFalse;
    else
        {
        while( node )
            {
            if( node->NextSibling() )
                {
                node = node->NextSibling();
                break;
                }
            node = node->ParentNode();
            }
        if( node )
            atend = EFalse;
        else
            atend = ETrue;
        }

    return atend;
    }

//---------------------------------------------------------------
// SmilXmlReader::reset
// @see header
//---------------------------------------------------------------
void SmilXmlReader::reset()
    {
    iCurrentNode = iRootNode;
    iState = EInitial;
    }

//---------------------------------------------------------------
// SmilXmlReader::ParseL
// @see header
//---------------------------------------------------------------
void SmilXmlReader::ParseL( RFile& filehandle )
    {
    //Create GMXML parser's instance
    xmlparser = CMDXMLParser::NewL( this,
                                                      CSMILDtd::NewL() );

    // XML parser closes the given handle. So make a duplicate handle.
    RFile parsehandle;
    User::LeaveIfError( parsehandle.Duplicate( filehandle ) );
    User::LeaveIfError( xmlparser->ParseFile( parsehandle ) );

    iWaitScheduler.Start();
	
    return;
    }

//---------------------------------------------------------------
// SmilXmlReader::openNode
// @see header
//---------------------------------------------------------------
void SmilXmlReader::openNodeL( CMDXMLNode* node )
    {
    if ( node->NodeType()==CMDXMLNode::EElementNode )
        {
        setOpenStateL( node->NodeName() );
        }
    return;
    }

//---------------------------------------------------------------
// SmilXmlReader::closeNode
// @see header
//---------------------------------------------------------------
void SmilXmlReader::closeNodeL( CMDXMLNode* node )
    {
    if ( node->NodeType()==CMDXMLNode::EElementNode )
        {
            setCloseStateL( node->NodeName() );
        }
    return;
    }

//---------------------------------------------------------------
// SmilXmlReader::setOpenStateL
// @see header
//---------------------------------------------------------------
void SmilXmlReader::setOpenStateL( const TDesC& tagname )
    {
    if ( tagname.CompareF( KSmilTag ) == 0 )
        {
        if ( iState != EInitial )
            User::Leave( KErrCorrupt );
        iState = ESmil;
        }
    else if ( tagname.CompareF( KHeadTag ) == 0 )
        {
        if (iState != ESmil)
            User::Leave( KErrCorrupt );
        iState = EHead;
        }
    else if ( tagname.CompareF( KLayoutTag ) == 0 )
        {
        if ( iState != EHead )
            User::Leave( KErrCorrupt );
        iState = ELayout;
        }
    else if ( tagname.CompareF( KBodyTag ) == 0 )
        {
        if ( iState != ESmil &&
             iState != EHeadFinished )
            User::Leave( KErrCorrupt );
        iState = EBody;
        }
    else if ( tagname.CompareF( KImageTag ) == 0 ||
              tagname.CompareF( KTextTag ) == 0 ||
              tagname.CompareF( KAudioTag ) == 0 ||
              tagname.CompareF( KRefTag ) == 0 ||
              tagname.CompareF( KTextStreamTag ) == 0 ||
              tagname.CompareF( KAnimationTag ) == 0 ||
              tagname.CompareF( KVideoTag ) == 0 )
        {
        if ( iState != EBody )
            User::Leave(KErrCorrupt);
        }

    return;
    }

//---------------------------------------------------------------
// SmilXmlReader::setCloseStateL
// @see header
//---------------------------------------------------------------
void SmilXmlReader::setCloseStateL( const TDesC& tagname )
    {
    if ( tagname.CompareF( KSmilTag ) == 0 )
        {
        iState = ESmilFinished;
        }
    else if ( tagname.CompareF( KHeadTag ) == 0 )
        {
        iState = EHeadFinished;
        }
    else if ( tagname.CompareF( KLayoutTag ) == 0 )
        {
        iState = ELayoutFinished;
        }
    else if ( tagname.CompareF( KBodyTag ) == 0 )
        {
        iState = EBodyFinished;
        }

    return;
    }

//  End of File
