/*
 * Copyright (c) 2000 Nokia Corporation and/or its subsidiary(-ies). 
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
 * Description: Implementation of applicationmanagement components
 *
 */

#ifndef __AMDEPLOYMENTCOMPONENTDATA_H__
#define __AMDEPLOYMENTCOMPONENTDATA_H__

#include <e32base.h>
#include <f32file.h> 
#include <s32strm.h>
#include "ApplicationManagementCommon.h"
const TInt KMaxUrlLength = 2048;
namespace NApplicationManagement
    {

    /**
     *  Data class to Data access
     */
    class CDeploymentComponentData : public CBase
        {
public:
        enum TType
            {
            EData,
            EInstallOptions,
            EMetaData
            };
private:
        CDeploymentComponentData(TType aType, const TDesC8 &aDataFile);
        CDeploymentComponentData();

        void ConstructL(const TDesC8 &aData, const TDesC8 &aMime);
        void ConstructLoadL(RReadStream &aStream);
        /**
         Possibly changes mutable iData
         */
        void LoadDataL() const;
        
        TBool IsSISInstallFile(const TDesC8 &aMimeType);

public:
        static CDeploymentComponentData* NewL(TType aType,
                const TDesC8 &aData, const TDesC8 &aMime,
                const TDesC8 &aDataFile);

        static CDeploymentComponentData* NewLC(TType aType,
                const TDesC8 &aData, const TDesC8 &aMime,
                const TDesC8 &aDataFile);

        static CDeploymentComponentData* LoadL(RReadStream &aStream);

        static CDeploymentComponentData* LoadLC(RReadStream &aStream);

        virtual ~CDeploymentComponentData();

        void SerializedFormL(RWriteStream &aBuffer) const;
        TInt SerializedFormLength() const;

        const TDesC8 &Data() const;
        const TDesC8 &DataFileName() const;
        void ResetData(RFs &aFs);
        TInt DataLengthL() const;
        inline TType Type() const;

        TUid SetDataL(const TDesC8& aMimeType);
        TUid SetDataL(const TDesC8 &aData, const TDesC8& aMimeType);
        TUid SetDataL(const TFileName &aData, const TDesC8& aMimeType);
        void SetDataFileNameL(const TDesC8 &aNewFileName);
        void SetMimeTypeL(const TDesC8 &aMimeType);
        inline const TDesC8 &MimeType() const;

        void PersistL(RFs &aFs);
        void DestroyL(RFs &aFs) const;

        CDeploymentComponentData &operator=(
                const CDeploymentComponentData &aData);

        TUid ResolveUidL(RFs &aFs);

        TUid ParseUidFromSisFileL(RFile& aHandleToFile);

private:
        mutable HBufC8 *iData;
        TBuf8<KMaxFileName> iDataFileName;
        TMimeType iMimeType;
        TType iType;
        TBuf<15> iExtn;
        };

    inline CDeploymentComponentData::TType CDeploymentComponentData::Type() const
        {
        return iType;
        }

    inline const TDesC8 &CDeploymentComponentData::MimeType() const
        {
        return iMimeType;
        }
    }

#endif

//  End of File
