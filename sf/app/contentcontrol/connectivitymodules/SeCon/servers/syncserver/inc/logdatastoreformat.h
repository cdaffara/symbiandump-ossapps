/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Simple class for tracing datastore format
*
*/

#ifndef LOGDATASTOREFORMAT_H_
#define LOGDATASTOREFORMAT_H_

//
#include "debug.h"

NONSHARABLE_CLASS(TLogDataStoreFormat)
    {
    public:
#ifndef _DEBUG
// does nothing on release version
static void TLogDataStoreFormat::LogDataStoreFormatL( const TDesC& /*aLogInfo*/, const CSmlDataStoreFormat& /*aDataStoreFormat*/ )
    {
    }
#else
static void TLogDataStoreFormat::LogDataStoreFormatL( const TDesC& aLogInfo, const CSmlDataStoreFormat& aDataStoreFormat )
    {
    LOGGER_WRITE_1("LogDataStoreFormatL, aLogInfo: %S", &aLogInfo);
    LOGGER_WRITE_1("DisplayName: %S", &aDataStoreFormat.DisplayName());
    TBool hasMaxSize = aDataStoreFormat.IsSupported( CSmlDataStoreFormat::EOptionHasMaxSize );
    TBool hasMaxItems = aDataStoreFormat.IsSupported( CSmlDataStoreFormat::EOptionHasMaxItems );
    TBool hierarcial = aDataStoreFormat.IsSupported( CSmlDataStoreFormat::EOptionHierarchial );
    LOGGER_WRITE_1("hasMaxSize: %d", (TInt)hasMaxSize);
    LOGGER_WRITE_1("hasMaxItems: %d", (TInt)hasMaxItems);
    LOGGER_WRITE_1("hierarcial: %d", (TInt)hierarcial);
    LOGGER_WRITE_1("MimeFormatCount: %d", aDataStoreFormat.MimeFormatCount());
    for ( TInt i = 0; i < aDataStoreFormat.MimeFormatCount(); i++)
        {
        LOGGER_WRITE_1("  MimeFormat(%d)",i);
        const CSmlMimeFormat& mimeformat = aDataStoreFormat.MimeFormat(i);
        LOGGER_WRITE8_1("  MimeType: %S", &mimeformat.MimeType().DesC());
        LOGGER_WRITE8_1("  MimeVersion: %S", &mimeformat.MimeVersion().DesC());
        LOGGER_WRITE_1( "  FieldLevel, bool: %d", (TInt) mimeformat.FieldLevel());
        LOGGER_WRITE_1( "  PropertyCount: %d", mimeformat.PropertyCount());
        for ( TInt j=0; j<mimeformat.PropertyCount(); j++ )
            {
            LOGGER_WRITE_1("    MimeFormat property(%d)",j);
            const CSmlDataProperty& dataProp = mimeformat.Property(j);
            const CSmlDataField& dataField = dataProp.Field();
            LOGGER_WRITE8_1("    dataField: %S", &dataField.Name().DesC());
            LOGGER_WRITE_1( "    DisplayName: %S", &dataField.DisplayName());
            LOGGER_WRITE_1( "    HasMaxSize: %d", (TInt)dataProp.HasMaxSize());
            LOGGER_WRITE_1( "    MaxSize: %d", dataProp.MaxSize());
            LOGGER_WRITE_1( "    HasMaxOccur: %d", (TInt)dataProp.HasMaxOccur());
            LOGGER_WRITE_1( "    MaxOccur: %d", dataProp.MaxOccur());
            TBool OptionNoTruncate = dataProp.IsSupported(CSmlDataProperty::EOptionNoTruncate);
            LOGGER_WRITE_1( "    EOptionNoTruncate: %d", (TInt)OptionNoTruncate);
            LOGGER_WRITE8_1("    DataType(): %S", &dataField.DataType().DesC());
            LOGGER_WRITE_1( "    EnumValueCount(): %d", dataField.EnumValueCount());
            for ( TInt k=0; k<dataField.EnumValueCount(); k++)
                {
                LOGGER_WRITE8_1("      EnumValue(): %S", &dataField.EnumValue(k).DesC());
                }
            LOGGER_WRITE_1("    ParamCount: %d", dataProp.ParamCount());
            for ( TInt k=0; k<dataProp.ParamCount(); k++)
                {
                LOGGER_WRITE_1( "      dataParamField(%d)", k);
                const CSmlDataPropertyParam& param = dataProp.Param(k);
                const CSmlDataField& field = param.Field();
                LOGGER_WRITE8_1("      dataParamField: %S", &field.Name().DesC());
                LOGGER_WRITE_1( "      dataParamDisplayName: %S", &field.DisplayName());
                LOGGER_WRITE8_1("      dataParamDataType(): %S", &field.DataType().DesC());
                LOGGER_WRITE_1( "      dataParamEnumValueCount(): %d", field.EnumValueCount());
                for ( TInt l=0; l<field.EnumValueCount(); l++)
                    {
                    LOGGER_WRITE8_1("        dataParamEnumValue(): %S", &field.EnumValue(l).DesC());
                    }
                
                LOGGER_WRITE("      ---------------");
                }
            LOGGER_WRITE(   "    -----------------");
            }
        
        LOGGER_WRITE(   "  -----------------");
        }
    }
#endif
    };


#endif /* LOGDATASTOREFORMAT_H_ */
