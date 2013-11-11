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
 * 
 * Description:
 *      The source file of the file type recognizer
 */

#include "fmfiletyperecognizer.h"
#include "fmutils.h"
#include <QFileInfo>
#include <QStringList>

FmFileTypeRecognizer::FmFileTypeRecognizer()
{
    mFileExtensionMap.insert( QString( "bmp" ), FileTypeImage );
    mFileExtensionMap.insert( QString( "gif" ), FileTypeImage );
    mFileExtensionMap.insert( QString( "jpe" ), FileTypeImage );
    mFileExtensionMap.insert( QString( "jpeg" ),FileTypeImage );
    mFileExtensionMap.insert( QString( "jpg" ), FileTypeImage );
    mFileExtensionMap.insert( QString( "ota" ), FileTypeImage );
    mFileExtensionMap.insert( QString( "png" ), FileTypeImage );
    mFileExtensionMap.insert( QString( "tif" ), FileTypeImage );
    mFileExtensionMap.insert( QString( "tiff" ),FileTypeImage );
    mFileExtensionMap.insert( QString( "wbmp" ),FileTypeImage );
    mFileExtensionMap.insert( QString( "wmf" ), FileTypeImage );
    mFileExtensionMap.insert( QString( "jp2" ), FileTypeImage );
    mFileExtensionMap.insert( QString( "jpg2" ),FileTypeImage );
    mFileExtensionMap.insert( QString( "jp3" ), FileTypeImage );
    mFileExtensionMap.insert( QString( "ico" ), FileTypeImage );
    mFileExtensionMap.insert( QString( "vcf" ), FileTypeImage );

    mFileExtensionMap.insert( QString( "3gp" ), FileTypeVideo );
    mFileExtensionMap.insert( QString( "mp4" ), FileTypeVideo );
    mFileExtensionMap.insert( QString( "nim" ), FileTypeVideo );
    mFileExtensionMap.insert( QString( "rm" ),  FileTypeVideo );
    mFileExtensionMap.insert( QString( "rv" ),  FileTypeVideo );
    mFileExtensionMap.insert( QString( "wmv" ), FileTypeVideo );
    mFileExtensionMap.insert( QString( "3g2" ), FileTypeVideo );
    mFileExtensionMap.insert( QString( "rmvb" ),FileTypeVideo );
    mFileExtensionMap.insert( QString( "mkv" ), FileTypeVideo );
    mFileExtensionMap.insert( QString( "3gpp" ),FileTypeVideo );
    mFileExtensionMap.insert( QString( "mpeg4"),FileTypeVideo );
    

    mFileExtensionMap.insert( QString( "aac" ), FileTypeTone );
    mFileExtensionMap.insert( QString( "amr" ), FileTypeTone );
    mFileExtensionMap.insert( QString( "au" ),  FileTypeTone );
    mFileExtensionMap.insert( QString( "awb" ), FileTypeTone );
    mFileExtensionMap.insert( QString( "mid" ), FileTypeTone );
    mFileExtensionMap.insert( QString( "mp3" ), FileTypeTone );
    mFileExtensionMap.insert( QString( "ra" ),  FileTypeTone );
    mFileExtensionMap.insert( QString( "rmf" ), FileTypeTone );
    mFileExtensionMap.insert( QString( "rng" ), FileTypeTone );
    mFileExtensionMap.insert( QString( "snd" ), FileTypeTone );
    mFileExtensionMap.insert( QString( "wav" ), FileTypeTone );
    mFileExtensionMap.insert( QString( "wve" ), FileTypeTone );
    mFileExtensionMap.insert( QString( "wma" ), FileTypeTone );
    mFileExtensionMap.insert( QString( "m4a" ), FileTypeTone );
    mFileExtensionMap.insert( QString( "ott" ), FileTypeTone );
    mFileExtensionMap.insert( QString( "mxmf" ),FileTypeTone );
    
    mFileExtensionMap.insert( QString( "doc" ), FileTypeText );
    mFileExtensionMap.insert( QString( "pdf" ), FileTypeText );
    mFileExtensionMap.insert( QString( "pps" ), FileTypeText );
    mFileExtensionMap.insert( QString( "ppt" ), FileTypeText );
    mFileExtensionMap.insert( QString( "txt" ), FileTypeText );
    mFileExtensionMap.insert( QString( "xls" ), FileTypeText );
            
    mFileExtensionMap.insert( QString( "sis" ), FileTypeSisx );
    mFileExtensionMap.insert( QString( "sisx" ),FileTypeSisx );
        
    mFileExtensionMap.insert( QString( "jad" ), FileTypeJava );
    mFileExtensionMap.insert( QString( "jar" ), FileTypeJava );
    
    mFileExtensionMap.insert( QString( "swf" ), FileTypeFlash );

    // have not handle FileTypePlaylist
    // have not handle FileTypeWidget
    // have not handle FileTypeWebLink 
}

FmFileTypeRecognizer::~FmFileTypeRecognizer()
{
    
}

FmFileTypeRecognizer::FileType FmFileTypeRecognizer::getType( const QString& path ) const
{
    if( FmUtils::isDrive( path ) ) {
        return FileTypeDrive;
    }
    
    QFileInfo fileInfo(path);
    if( fileInfo.isDir() ) {
        return FileTypeFolder;
    }
    
    //if can not find key, return default value: FileTypeUnKnown
    return mFileExtensionMap.value( fileInfo.suffix().toLower(), FileTypeUnKnown );
}
