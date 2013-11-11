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
* Description:  COMADSFolderObject class implementation.
*
*/


#include <utf.h>
#include "omadsfolderobject.h"
#include "logger.h"

_LIT(KFolderBeginTag, "<Folder>");
_LIT(KFolderEndTag, "</Folder>");
_LIT(KNameBeginTag, "<name>");
_LIT(KNameEndTag, "</name>");
_LIT(KCreatedBeginTag, "<created>");
_LIT(KCreatedEndTag, "</created>");
_LIT(KModifiedBeginTag, "<modified>");
_LIT(KModifiedEndTag, "</modified>");
_LIT(KDateFormat, "%04d%02d%02dT%02d%02d%02d");
const TInt KDateFormatLength( 15 );


COMADSFolderObject* COMADSFolderObject::NewLC()
    {
    COMADSFolderObject* self = new (ELeave) COMADSFolderObject;
    CleanupStack::PushL( self );
    return self;
    }

COMADSFolderObject::~COMADSFolderObject()
    {
    }

void COMADSFolderObject::ExportFolderXmlL( CBufBase& aBuffer )
    {
    iBuffer = &aBuffer;
    iWriteBufPosition = 0;
    iWriteBufSize = aBuffer.Size();     

    ExportL();
    }
    
void COMADSFolderObject::ExportL()
    {
    // Folder
    WriteL( KFolderBeginTag );
    
    // Name
    WriteL( KNameBeginTag );
    WriteL( iName );
    WriteL( KNameEndTag );
    
    // Created Date
    TBuf<KDateFormatLength> tempdate;
    tempdate.Format( KDateFormat, iCreatedDate.Year(), iCreatedDate.Month() + 1,
        iCreatedDate.Day(), iCreatedDate.Hour(), iCreatedDate.Minute(), iCreatedDate.Second() );
    
    WriteL( KCreatedBeginTag );
    WriteL( tempdate );
    WriteL( KCreatedEndTag );
    
    // Modified Date
    tempdate.Format( KDateFormat, iModifiedDate.Year(), iModifiedDate.Month() + 1,
        iModifiedDate.Day(), iModifiedDate.Hour(), iModifiedDate.Minute(), iModifiedDate.Second() );
    
    WriteL( KModifiedBeginTag );
    WriteL( tempdate );
    WriteL( KModifiedEndTag );
    
    // Folder end
    WriteL(KFolderEndTag);
    }
    
void COMADSFolderObject::WriteL( const TDesC& aData )
    {   
    User::LeaveIfError( CnvUtfConverter::ConvertFromUnicodeToUtf8( iTemp, aData ) );
    
    TInt newPosition = iWriteBufPosition + iTemp.Length();

    if ( newPosition > iWriteBufSize )
        {
        TInt expand = newPosition - iWriteBufSize;
        iBuffer->ExpandL( iWriteBufSize, expand );
        iWriteBufSize += expand;
        }

    iBuffer->Write( iWriteBufPosition, iTemp );
    iWriteBufPosition = newPosition;
    }
