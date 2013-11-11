// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#include <s32strm.h>
#include <s32buf.h>
#include <s32file.h>
#include <badesca.h>

#include "agmcalendarinfo.h"
#include "agmcalendarinfoproperty.h"
#include "calsessionimpl.h"

EXPORT_C CAgnCalendarInfo* CAgnCalendarInfo::NewL() 
    {
    CAgnCalendarInfo* self = new(ELeave) CAgnCalendarInfo();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

EXPORT_C CAgnCalendarInfo::CAgnCalendarInfo()
    :iLinearOrder(CAgnCalendarInfoProperty::Compare)
    {
    }

EXPORT_C void CAgnCalendarInfo::ConstructL()
    {
    iFileName = KNullDesC().AllocL();
    iName = KNullDesC().AllocL();
    iDescription = KNullDesC().AllocL();
    }

EXPORT_C CAgnCalendarInfo::~CAgnCalendarInfo()   
    {
    delete iFileName;
    delete iName;
    delete iDescription;
    
    iProperties.ResetAndDestroy();
    }

EXPORT_C void CAgnCalendarInfo::SetIsValid(TBool aIsValid)
    {
    iIsValid = aIsValid;
    }

EXPORT_C TBool CAgnCalendarInfo::IsValid() const
    {
    return iIsValid;
    }

EXPORT_C void CAgnCalendarInfo::SetFileNameL(const TDesC& aFileName)
    {
    HBufC* tempFileName = aFileName.AllocL();
    delete iFileName;
    iFileName = tempFileName;
    }

EXPORT_C const TDesC& CAgnCalendarInfo::FileNameL() const
    {
    return *iFileName;
    }

EXPORT_C void CAgnCalendarInfo::SetNameL(const TDesC& aName)
    {
    HBufC* tempName = aName.AllocL();
    delete iName;
    iName = tempName;
    }

EXPORT_C const TDesC& CAgnCalendarInfo::NameL() const 
    {
    return *iName;
    }

EXPORT_C void CAgnCalendarInfo::SetDescriptionL(const TDesC& aDescription)
    {
    HBufC* tempDescription = aDescription.AllocL();
    delete iDescription;
    iDescription = tempDescription;
    }

EXPORT_C const TDesC& CAgnCalendarInfo::DescriptionL() const 
    {
    return *iDescription;
    }

EXPORT_C void CAgnCalendarInfo::SetColor(TRgb aColor)  
    {
    iColor = aColor;
    }

EXPORT_C TRgb CAgnCalendarInfo::Color() const   
    {
    return iColor;
    }

EXPORT_C void CAgnCalendarInfo::SetEnabled(TBool aEnabled) 
    {
    iEnabled = aEnabled;
    }

EXPORT_C TBool CAgnCalendarInfo::Enabled() const
    {
    return iEnabled;
    }

EXPORT_C CDesC8Array* CAgnCalendarInfo::PropertyKeysL() const
    {
    const TInt KPropertyCount(iProperties.Count());
    const TInt propertyCount = KPropertyCount==0 ? 1 : KPropertyCount;
    
    CDesC8Array* propertyKeys = new(ELeave) CDesC8ArrayFlat(propertyCount);
    CleanupStack::PushL(propertyKeys);
    
    for (TInt i(0) ; i < KPropertyCount ; ++i)
        {
        if (iProperties[i]->State() != CAgnCalendarInfoProperty::EDeleted)
            {
            propertyKeys->AppendL(iProperties[i]->Key());
            }
        }
    
    CleanupStack::Pop(propertyKeys);
    return propertyKeys;
    }

EXPORT_C const TDesC8& CAgnCalendarInfo::PropertyValueL(const TDesC8& /*aKey*/) const
    {
    // This is a virtual function overridden in CCalCalendarInfoImpl
    // This should never be called
    __ASSERT_DEBUG(EFalse, User::Invariant());
    
    return KNullDesC8;
    }

// Adds or updates a property by key and value
EXPORT_C void CAgnCalendarInfo::SetPropertyL(const TDesC8& aKey, const TDesC8& aValue)
    {
    if (aKey.Compare(KNullDesC8) == 0
            || aValue.Compare(KNullDesC8) == 0)
        {
        // Either the key ot the value is NULL so leave
        User::Leave(KErrArgument);
        }
    
    TInt position = iProperties.FindInOrder(aKey, CAgnCalendarInfoProperty::Compare);
    
    if (position != KErrNotFound)
        {
        // This property already exists so set the value
        
        iProperties[position]->SetValue(aValue.AllocL());
        }
    else
        {
        // This property does not exist yet so insert it in key order
        
        CAgnCalendarInfoProperty* property = CAgnCalendarInfoProperty::NewLC(aKey);
        
        property->SetValue(aValue.AllocL());
        
        iProperties.InsertInOrderL(property, iLinearOrder);
        
        CleanupStack::Pop(property);
        }
    }

EXPORT_C void CAgnCalendarInfo::RemovePropertyL(const TDesC8& aKey)
    {
    TInt position = iProperties.FindInOrder(aKey, CAgnCalendarInfoProperty::Compare);
    
    if (position != KErrNotFound)
        {
        iProperties[position]->SetState(CAgnCalendarInfoProperty::EDeleted);
        }
    }

EXPORT_C void CAgnCalendarInfo::FileInternalizeL(RReadStream& aReadStream)
    {
    delete iName;
    delete iDescription;
    iName = NULL;
    iDescription = NULL;
    
    iName = HBufC::NewL(aReadStream, KMaxTInt);
    iDescription = HBufC::NewL(aReadStream, KMaxTInt);
    
    iColor = TRgb(aReadStream.ReadUint32L());
    iEnabled = aReadStream.ReadInt8L();
    
    // Remove any existing properties
    iProperties.ResetAndDestroy();
    
    const TInt KNewPropertyCount(aReadStream.ReadUint32L());
    
    for (TInt i(0) ; i < KNewPropertyCount ; ++i)
        {
        HBufC8* key = HBufC8::NewLC(aReadStream, KMaxTInt);
        CAgnCalendarInfoProperty* property = CAgnCalendarInfoProperty::NewLC(*key);
        
        TStreamId streamId;
        aReadStream >> streamId;
        property->SetStreamId(streamId);
        
        iProperties.InsertInOrderL(property, iLinearOrder);
        CleanupStack::Pop(property);
        CleanupStack::PopAndDestroy(key);
        }
    }

EXPORT_C void CAgnCalendarInfo::FileExternalizeL(RWriteStream& aWriteStream) const
    {
    aWriteStream << *iName;
    aWriteStream << *iDescription;
    aWriteStream.WriteUint32L(iColor.Value());
    aWriteStream.WriteInt8L(iEnabled);
    
    const TInt KPropertyCount(iProperties.Count());
    
    TStreamPos streamCountPos = aWriteStream.Sink()->TellL(MStreamBuf::EWrite);
    
    aWriteStream.WriteUint32L(KPropertyCount);
    
    TInt actualPropertyCount(0);
    
    for (TInt i(0) ; i < KPropertyCount ; ++i)
        {
        if (iProperties[i]->State() != CAgnCalendarInfoProperty::EDeleted)
            {
            aWriteStream << iProperties[i]->Key();
            aWriteStream << iProperties[i]->StreamId();
            
            ++actualPropertyCount;
            }
        }
    
    if (actualPropertyCount != KPropertyCount)
        {
        // We have deleted some properties so write the actual count
        
        aWriteStream.Sink()->SeekL(MStreamBuf::EWrite, streamCountPos);
        aWriteStream.WriteUint32L(actualPropertyCount);
        aWriteStream.Sink()->SeekL(MStreamBuf::EWrite, EStreamEnd);
        }
    }

EXPORT_C void CAgnCalendarInfo::IpcInternalizeL(RReadStream& aReadStream)
    {
    delete iFileName;
    delete iName;
    delete iDescription;
    iFileName = NULL;
    iName = NULL;
    iDescription = NULL;
    
    iIsValid = aReadStream.ReadInt8L();
    iFileName = HBufC::NewL(aReadStream, KMaxTInt);
    iName = HBufC::NewL(aReadStream, KMaxTInt);
    iDescription = HBufC::NewL(aReadStream, KMaxTInt);
    iColor = TRgb(aReadStream.ReadUint32L());
    iEnabled = aReadStream.ReadInt8L();
    
    // Remove any existing properties
    iProperties.ResetAndDestroy();
    
    const TInt KNewPropertyCount(aReadStream.ReadUint32L());
    
    for (TInt i(0) ; i < KNewPropertyCount ; ++i)
        {
        HBufC8* key = HBufC8::NewLC(aReadStream, KMaxTInt);
        CAgnCalendarInfoProperty* property = CAgnCalendarInfoProperty::NewLC(*key);
        
        TStreamId streamId;
        aReadStream >> streamId;
        property->SetStreamId(streamId);
        
        property->SetState(static_cast<CAgnCalendarInfoProperty::TState>(aReadStream.ReadUint8L()));
        
        iProperties.InsertInOrderL(property, iLinearOrder);
        CleanupStack::Pop(property);
        CleanupStack::PopAndDestroy(key);
        
        if (aReadStream.ReadUint8L())
            {
            // There was a value, so read it in
            HBufC8* value = HBufC8::NewL(aReadStream, KMaxTInt);
            property->SetValue(value);
            }
        }
    }

EXPORT_C void CAgnCalendarInfo::IpcExternalizeL(RWriteStream& aWriteStream) const
    {
    aWriteStream.WriteInt8L(iIsValid);
    aWriteStream << *iFileName;
    aWriteStream << *iName;
    aWriteStream << *iDescription;
    aWriteStream.WriteUint32L(iColor.Value());
    aWriteStream.WriteInt8L(iEnabled);
    
    const TInt KPropertyCount(iProperties.Count());
    
    aWriteStream.WriteUint32L(KPropertyCount);
    
    for (TInt i(0) ; i < KPropertyCount ; ++i)
        {
        aWriteStream << iProperties[i]->Key();
        aWriteStream << iProperties[i]->StreamId();
        aWriteStream.WriteUint8L(iProperties[i]->State());
        
        if (iProperties[i]->Value().Compare(KNullDesC8) == 0)
            {
            // This is empty so don't write the value
            aWriteStream.WriteUint8L(EFalse);
            }
        else
            {
            aWriteStream.WriteUint8L(ETrue);
            aWriteStream << iProperties[i]->Value();
            }
        }
    }

EXPORT_C void CAgnCalendarInfo::ExternalizePropertiesL(CFileStore& iFileStore, const CAgnCalendarInfo* aOldCalendarInfo) const
    {
    if (aOldCalendarInfo)
        {
        // We are updating the metadata so we should first delete any of the
        // old properties that cannot be found in the new metadata
    
        const TInt KOldPropertyCount(aOldCalendarInfo->iProperties.Count());
        for (TInt i(0) ; i < KOldPropertyCount ; ++i)
            {
            TInt position(iProperties.FindInOrder(aOldCalendarInfo->iProperties[i], CAgnCalendarInfoProperty::Compare));
            
            if (position == KErrNotFound)
                {
                // This property was not found in the new list so delete it
                iFileStore.DeleteL(aOldCalendarInfo->iProperties[i]->StreamId());
                }
            }
        }
    
    // Now update or delete properties that are in the new list 
    const TInt KPropertyCount(iProperties.Count());
    for (TInt i(0) ; i < KPropertyCount ; ++i)
        {
        // Find this property in the old metadata
        TInt position(KErrNotFound);
        if (aOldCalendarInfo)
            {
            position = aOldCalendarInfo->iProperties.FindInOrder(iProperties[i], CAgnCalendarInfoProperty::Compare);
            }

        switch (iProperties[i]->State())
            {
            case CAgnCalendarInfoProperty::EDeleted:
                {
                // The property has been marked for deleting so delete the stream
                if (aOldCalendarInfo && position != KErrNotFound)
                    {
                    // Use the old property list's stream id
                    // Note: if it wasn't found then I can't have been stored before
                    // so we don't need to delete it
                    iFileStore.DeleteL(aOldCalendarInfo->iProperties[position]->StreamId());
                    }
                }
                break;
            case CAgnCalendarInfoProperty::EModified:
                {
                RStoreWriteStream writeStream;
                
                if (aOldCalendarInfo && position != KErrNotFound)
                    {
                    // The property has been edited so replace the stream
                    // Use the old property list's stream id
                    writeStream.ReplaceLC(iFileStore, aOldCalendarInfo->iProperties[position]->StreamId());
                    iProperties[i]->SetStreamId(aOldCalendarInfo->iProperties[position]->StreamId());
                    }
                else if (iProperties[i]->StreamId() == KNullStreamId)
                    {
                    // This is a new property for this metadata
                    TStreamId newStreamId = writeStream.CreateLC(iFileStore);
                    iProperties[i]->SetStreamId(newStreamId);
                    }
                else
                    {
                    // The property has been edited so replace the stream
                    writeStream.ReplaceLC(iFileStore, iProperties[i]->StreamId());
                    }
                
                writeStream << iProperties[i]->Value();
                
                writeStream.CommitL();
                CleanupStack::PopAndDestroy(&writeStream);
                }
                break;
            default:
                break;
            }
        }
    }


