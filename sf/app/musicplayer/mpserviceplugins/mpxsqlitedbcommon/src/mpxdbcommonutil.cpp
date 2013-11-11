/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  The class CMPXDbCommonUtil which contains utilities functions
*
*/


// INCLUDE FILES
#include <bautils.h>
#include <caf/data.h>
#include <caf/virtualpathptr.h>
#include <hash.h>
#include <apgcli.h>
#include <apmstd.h>
#include <sqldb.h>

#include <mpxmediageneraldefs.h>
#include <mpxmediamusicdefs.h>
#include <mpxmediaaudiodefs.h>
#include <mpxmediadrmdefs.h>
#include <mpxmediacollectiondetaildefs.h>
#include <mpxmedia.h>
#include <mpxmediaarray.h>
#include <mpxlog.h>

#include "mpxdbcommonstd.h"
#include "mpxdbcommondef.h"
#include "mpxdbcommonutil.h"

// CONSTANTS
_LIT(KPathStart, ":\\");

// ============================ FILE EXTENSION TO MIME MAP ==============================

// ----------------------------------------------------------------------------------------------------------
// Statically allocated Mime Map
// ----------------------------------------------------------------------------------------------------------
//

_LIT( KExtensionAac,    "aac" );
_LIT( KExtensionAif,    "aif"); 
_LIT( KExtensionAifc,   "aifc"); 
_LIT( KExtensionAiff,   "aiff");
_LIT( KExtensionAmr,    "amr" );
_LIT( KExtensionAu,     "au" );
_LIT( KExtensionAwb,    "awb" );
_LIT( KExtensionMid,    "mid" );
_LIT( KExtensionMidi,   "midi" );
_LIT( KExtensionMka,    "mka" );
_LIT( KExtensionMp3,    "mp3" );
_LIT( KExtensionOgg,    "ogg");
_LIT( KExtensionRa,     "ra"); 
_LIT( KExtensionRam,    "ram"); 
_LIT( KExtensionRmi,    "rmi"); 
_LIT( KExtensionSnd,    "snd" );
_LIT( KExtensionSpMid,  "spmid" );
_LIT( KExtensionWav,    "wav" );
_LIT( KExtensionWma,    "wma" );

_LIT8( KMimeTypeAac,      "audio/aac" );
_LIT8( KMimeTypeAiff,     "audio/x-aiff");
_LIT8( KMimeTypeAmr,      "audio/amr" );
_LIT8( KMimeTypeAu,       "audio/au" );
_LIT8( KMimeTypeAwb,      "audio/amr-wb" );
_LIT8( KMimeTypeBasic,    "audio/basic");
_LIT8( KMimeTypeMatroska, "audio/x-matroska");
_LIT8( KMimeTypeMid,      "audio/mid");
_LIT8( KMimeTypeMidi,     "audio/midi" );
_LIT8( KMimeTypeMpeg,     "audio/mpeg" );
_LIT8( KMimeTypeOgg,      "audio/ogg");
_LIT8( KMimeTypeReal,     "audio/x-pn-realaudio");
_LIT8( KMimeTypeSpMidi,   "audio/sp-midi" );
_LIT8( KMimeTypeWav,      "audio/wav" ); 
_LIT8( KMimeTypeWma,      "audio/x-ms-wma");

struct TMimeMapItem {
	const TDesC  * iExt;
	const TDesC8 * iType;
};

// We need to explicitly cast here as LitC::operator& requires writable DLL data (even though it is just a cast)
#define MIME_MAP_ITEM(ext,type) { &REINTERPRET_CAST(const TDesC&, ext), &REINTERPRET_CAST(const TDesC8&, type) }

// THIS ARRAY MUST BE SORTED BY EXTENSION
static const TMimeMapItem KMimeMap [] = {
    MIME_MAP_ITEM( KExtensionAac,   KMimeTypeAac),
    MIME_MAP_ITEM( KExtensionAif,   KMimeTypeAiff ),
    MIME_MAP_ITEM( KExtensionAifc,  KMimeTypeAiff ),
    MIME_MAP_ITEM( KExtensionAiff,  KMimeTypeAiff ),
    MIME_MAP_ITEM( KExtensionAmr,   KMimeTypeAmr ),
    MIME_MAP_ITEM( KExtensionAu,    KMimeTypeAu ), // KMimeTypeAudioBasic?  "audio/x-au"?
    MIME_MAP_ITEM( KExtensionAwb,   KMimeTypeAwb ),
    MIME_MAP_ITEM( KExtensionMid,   KMimeTypeMidi ),
    MIME_MAP_ITEM( KExtensionMidi,  KMimeTypeMidi ),
    MIME_MAP_ITEM( KExtensionMka,   KMimeTypeMatroska ),
    MIME_MAP_ITEM( KExtensionMp3,   KMimeTypeMpeg ),
    MIME_MAP_ITEM( KExtensionOgg,   KMimeTypeOgg ),
    MIME_MAP_ITEM( KExtensionRa,    KMimeTypeReal ),
    MIME_MAP_ITEM( KExtensionRam,   KMimeTypeReal ),
    MIME_MAP_ITEM( KExtensionRmi,   KMimeTypeMid ),
    MIME_MAP_ITEM( KExtensionSnd,   KMimeTypeBasic ),
    MIME_MAP_ITEM( KExtensionSpMid, KMimeTypeSpMidi ),
    MIME_MAP_ITEM( KExtensionWav,   KMimeTypeWav ), // "audio/x-wav"?
    MIME_MAP_ITEM( KExtensionWma,   KMimeTypeWma )
};

// ----------------------------------------------------------------------------------------------------------
// Look for Mime Type from map by file extension
// Returns NULL if file extension is not known
// ----------------------------------------------------------------------------------------------------------
//
static const TDesC8 * FindMimeTypeFromMap ( const TDesC& aFilename )
    {
    // extract extension
   	TPtrC extension;
    TInt pos = aFilename.LocateReverseF( '.' );
    if ( pos < 0  || ++pos >= aFilename.Length() )
        {
        return NULL;
        }
    extension.Set( aFilename.Mid( pos ) );

    // binary search from Mime Map
    TUint begin = 0;
    TUint end = sizeof KMimeMap / sizeof (TMimeMapItem);
    while (begin < end)
        {
        TUint at = (begin + end) / 2;
        const TMimeMapItem & item = KMimeMap[at];
        TInt r = item.iExt->CompareF(extension);
        if (r == 0)
            {
            return item.iType;
            }
        else if (r > 0)
            {
            end = at;
            }
        else
            {
            begin = at + 1;
            }
        }
    return NULL;
}

// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------------------------------------
// Set HBufC data member.
// ----------------------------------------------------------------------------------------------------------
//
EXPORT_C TInt MPXDbCommonUtil::SetHBuf(
    HBufC*& aBuf,
    const TDesC* aSource,
    TInt aMaxLen /*= -1*/)
    {
    TInt ret( KErrNone );
    delete aBuf;
    aBuf = NULL;

    if (aSource)
        {
        if (aMaxLen != -1 && aSource->Length() > aMaxLen)
            {
            aBuf = aSource->Left(aMaxLen).Alloc();
            ret = KErrOverflow;
            }
        else
            {
            aBuf = aSource->Alloc();
            }
        }
    else
        {
        aBuf = HBufC::New(0);
        }

    if (!aBuf)
        {
        ret = KErrNoMemory;
        }

    return ret;
    }

// ----------------------------------------------------------------------------------------------------------
// Replace single quotes `'` with two single quotes `''` as text queries are
// delimited by single quotes
// also replace 0x00 and '\t' as they will cause crash in SQLite and UI respectively
// ----------------------------------------------------------------------------------------------------------
//
EXPORT_C void MPXDbCommonUtil::FindAndReplaceSingleQuote(const TDesC& aSrc, TDes& aTrg)
    {
    TText ch;
    TInt srcLen(aSrc.Length());

    for (TInt i = 0; i < srcLen; ++i)
        {
        ch = aSrc[i];
        if((ch == 0) || (ch == TText('\t')))
            {
            aTrg.Append(TText(' '));
            }
        else
            {
            aTrg.Append(ch);
            if (ch == TText('\''))
                {
                aTrg.Append(ch);
                }
            }
        }
    }

// ----------------------------------------------------------------------------------------------------------
// Delete file from file system
// ----------------------------------------------------------------------------------------------------------
//
EXPORT_C TInt MPXDbCommonUtil::DeleteFile(
    RFs& aFs,
    const TDesC& aFile)
    {
    TInt ret(KErrNone);
    if(BaflUtils::FileExists(aFs, aFile))
        {
         ret = aFs.Delete(aFile);

        if (ret == KErrAccessDenied)
            {
            aFs.SetAtt(aFile,KEntryAttNormal,KEntryAttReadOnly);
            ret = aFs.Delete(aFile);
            }
        }
    return ret;
    }

// ----------------------------------------------------------------------------------------------------------
// Get the drive Id with a given volume unique Id
// ----------------------------------------------------------------------------------------------------------
//
EXPORT_C TInt MPXDbCommonUtil::GetDriveIdMatchVolIdL(
    RFs& aFs,
    TUint aVolumeId)
    {
    TDriveList driveList;
    TBool found(EFalse);
    TInt driveNum(EDriveA);

    User::LeaveIfError(aFs.DriveList(driveList));
    for (; driveNum <= EDriveZ; ++driveNum)
        {
        if (driveList[driveNum])
            {
            TVolumeInfo volInfo;

            if ((aFs.Volume(volInfo, driveNum) == KErrNone) &&
                (volInfo.iUniqueID == aVolumeId))
                {
                found = ETrue;
                break;
                }
            }
        }

    if (!found)
        {
        driveNum = KErrNotFound;
        }

    return driveNum;
    }

// ----------------------------------------------------------------------------------------------------------
// Get the volume Id with a given drive Id
// ----------------------------------------------------------------------------------------------------------
//
EXPORT_C TUint MPXDbCommonUtil::GetVolIdMatchDriveIdL(
    RFs& aFs,
    TInt aDriveId)
    {
    TUint volId (0);
    TVolumeInfo volInfo;
    TInt err = aFs.Volume(volInfo, aDriveId);
    if (err == KErrNone)
        {
        volId = volInfo.iUniqueID;
        }
    else if (err == KErrNotReady)
        {
        volId = 0;
        }
    else
        {
        User::Leave(err);
        }

    return volId;
    }

// ----------------------------------------------------------------------------
// Generate a 32bits Unique Id with MD5
// ----------------------------------------------------------------------------
//
EXPORT_C TUint32 MPXDbCommonUtil::GenerateUniqueIdL(
    RFs& aFs,
    TMPXGeneralCategory aTableId,
    const TDesC& aName,
    TBool aCaseSensitive)
    {
    MPX_FUNC("MPXDbCommonUtil::GenerateUniqueIdL");

    TInt extractPos(0);

    if( aName.Find(KPathStart) == KMCPathStartWithColon )
        {
        // aName is a filename
        extractPos = KMCPathStartPos;     // c:\..., include first '\' of the path
        }

    HBufC* fileNameBuf = HBufC::NewLC(aName.Length()+KMCIntegerLen);
    TPtr fileName(fileNameBuf->Des());
    if( extractPos )
        {
        // append volume's unique Id to path to maintain uniqueness
        TDriveUnit driveUnit( aName );
        TUint volId = MPXDbCommonUtil::GetVolIdMatchDriveIdL(aFs, driveUnit);
        if( volId )
            {
            fileName.AppendNum( volId );
            }
        else
            {
            // If media/drive doesn't exist, using whole path
            extractPos = 0;
            }
        }

    // append the path part
    fileName.Append( aName.Mid( extractPos ) );

    if( !aCaseSensitive )
        {
        fileName.LowerCase();
        }

    TInt narrowFileLen(0);
    TPtrC8 narrowFileName;
    #if defined(_UNICODE)
        narrowFileLen = fileNameBuf->Des().Length() * 2;
        narrowFileName.Set((TUint8*)fileName.Ptr(), narrowFileLen);
    #else
        narrowFileLen = fileName.Length();
        narrowFileName.Set(fileName.Ptr(), narrowFileLen);
    #endif

    CMD5* hasher = CMD5::NewL();
    CleanupStack::PushL( hasher );
    hasher->Reset();
    TBuf8<16> hash( hasher->Hash( narrowFileName ) );   //hashed to 128bits
    CleanupStack::PopAndDestroy( hasher );

    const TText8* ptr = hash.Ptr();

    TUint32 uniqueId(0);
    uniqueId = aTableId << 28;
    uniqueId |= (ptr[3]&0x0F)<<24 | (ptr[2]<<16) | (ptr[1]<<8) | ptr[0];
    
    CleanupStack::PopAndDestroy(fileNameBuf);
    
    return uniqueId;
    }

// ---------------------------------------------------------------------------
// Append an item into the media array
// ---------------------------------------------------------------------------
//
EXPORT_C void MPXDbCommonUtil::AppendMediaL(
    CMPXMediaArray& aArray,
    const TDesC& aTitle,
    TMPXGeneralType aType,
    TMPXGeneralCategory aCat,
    TMPXItemId aId,
    TInt aNonPermissibleActions,
    TUint aDbflag)
    {
    MPX_FUNC("MPXDbCommonUtil::AppendMediaL");

    CMPXMedia* entry = ConstructMediaLC(aTitle, aType, aCat, aId, aNonPermissibleActions, aDbflag);
    aArray.AppendL(*entry);
    CleanupStack::PopAndDestroy(entry);
    }

// ----------------------------------------------------------------------------
// Append an item into the media array
// ----------------------------------------------------------------------------
//
EXPORT_C void MPXDbCommonUtil::PrependMediaL(
    CMPXMediaArray& aArray,
    const TDesC& aTitle,
    TMPXGeneralType aType,
    TMPXGeneralCategory aCat,
    TMPXItemId aId /* = 0 */,
    TInt aNonPermissibleActions /* = 0 */,
    TUint aDbflag /* = 0 */,
    TInt aPos /* = 0 */)
    {
    MPX_FUNC("MPXDbCommonUtil::PrependMediaL");

    CMPXMedia* entry = ConstructMediaLC(aTitle, aType, aCat, aId, aNonPermissibleActions, aDbflag);
    User::LeaveIfError(aArray.Insert(*entry, aPos));
    CleanupStack::PopAndDestroy(entry);
    }

// ----------------------------------------------------------------------------
// Append an item into the media array
// ----------------------------------------------------------------------------
//
EXPORT_C void MPXDbCommonUtil::FillInSupportedUIDsL(
    const TArray<TMPXAttribute>& aAttrs,
    RArray<TInt>& aSupportedIds)
    {
    CleanupClosePushL(aSupportedIds);
    MPX_FUNC("MPXDbCommonUtil::FillInSupportedUIDs");

    TInt attrCount(aAttrs.Count());
    for (TInt i = 0; i < attrCount; ++i)
        {
        if (aAttrs[i].ContentId() == KMPXMediaIdGeneral)
            {
            aSupportedIds.AppendL(KMPXMediaIdGeneral);
            }
        else if (aAttrs[i].ContentId() == KMPXMediaIdMusic)
            {
            aSupportedIds.AppendL(KMPXMediaIdMusic);
            }
        else if (aAttrs[i].ContentId() == KMPXMediaIdAudio)
            {
            aSupportedIds.AppendL(KMPXMediaIdAudio);
            }
        else if (aAttrs[i].ContentId() == KMPXMediaIdDrm)
            {
            aSupportedIds.AppendL(KMPXMediaIdDrm);
            }
        else if (aAttrs[i].ContentId() == KMPXMediaIdCollectionDetails)
            {
            aSupportedIds.AppendL(KMPXMediaIdCollectionDetails);
            }
        else
            {
            // ignore attribute
            }
        }
    CleanupStack::Pop();
    }

// ----------------------------------------------------------------------------
// Fill in change event message with the given info
// ----------------------------------------------------------------------------
//
EXPORT_C void MPXDbCommonUtil::FillItemChangedMessageL(
    CMPXMessage& aMessage,
    TMPXItemId aId,
    TMPXChangeEventType aChangeType,
    TMPXGeneralCategory aCategory,
    TUint aUid,
    TMPXItemId aDeprecatedId)
    {
    MPX_FUNC("MPXDbCommonUtil::FillItemChangedMessageL");
    MPX_DEBUG5("MPXDbCommonUtil::FillItemChangedMessageL: type [%d], category [%d], id[0x%x], oldId[0x%x]",
               aChangeType, aCategory, aId.iId1, aDeprecatedId.iId1);

    aMessage.SetTObjectValueL<TMPXMessageId>(KMPXMessageGeneralId, KMPXMessageIdItemChanged);
    aMessage.SetTObjectValueL<TUid>(KMPXMessageCollectionId, TUid::Uid(aUid));
    aMessage.SetTObjectValueL<TMPXChangeEventType>(KMPXMessageChangeEventType, aChangeType);
    aMessage.SetTObjectValueL<TMPXGeneralCategory>(KMPXMessageMediaGeneralCategory, aCategory);
    aMessage.SetTObjectValueL<TMPXItemId>(KMPXMessageMediaGeneralId, aId);

    if ((aDeprecatedId != 0) && (aId != aDeprecatedId))
        {
        aMessage.SetTObjectValueL<TMPXItemId>(KMPXMessageMediaDeprecatedId, aDeprecatedId);
        }
    }

// ----------------------------------------------------------------------------
// Add an item changed message to the message array
// ----------------------------------------------------------------------------
//
EXPORT_C void MPXDbCommonUtil::AddItemChangedMessageL(
    CMPXMessageArray& aMessageArray,
    TMPXItemId aId,
    TMPXChangeEventType aChangeType,
    TMPXGeneralCategory aCategory,
    TUint aUid,
    TMPXItemId aDeprecatedId /* = 0 */)
    {
    MPX_FUNC("MPXDbCommonUtil::AddItemChangedMessageL");
    CMPXMessage* message = CMPXMedia::NewL();
    CleanupStack::PushL(message);

    FillItemChangedMessageL(*message, aId, aChangeType, aCategory, aUid, aDeprecatedId);
    if (FindItemChangedMessageL(aMessageArray, *message) == KErrNotFound)
        {
        aMessageArray.AppendL(*message); // ownership xfer
        }
    CleanupStack::PopAndDestroy(message);
    }

// ----------------------------------------------------------------------------
// Find the message in the array, if not KErrNotFound is returned; otherwise the
// index of the first matching message is returned
// ----------------------------------------------------------------------------
//
EXPORT_C TInt MPXDbCommonUtil::FindItemChangedMessageL(
    const CMPXMessageArray& aMessageArray,
    const CMPXMessage& aMessage)
    {
    MPX_FUNC("MPXDbCommonUtil::FindItemChangedMessageL");

    TInt index(KErrNotFound);
    TInt messageCount(aMessageArray.Count());

    for (TInt i = 0; i < messageCount; ++i)
        {
        CMPXMessage* message = aMessageArray.AtL(i);
        
        if (message->IsSupported(KMPXMessageGeneralId) &&
            message->IsSupported(KMPXMessageCollectionId) &&
            message->IsSupported(KMPXMessageChangeEventType) &&
            message->IsSupported(KMPXMessageMediaGeneralCategory) &&
            message->IsSupported(KMPXMessageMediaGeneralId) &&
            aMessage.IsSupported(KMPXMessageGeneralId) &&
            aMessage.IsSupported(KMPXMessageCollectionId) &&
            aMessage.IsSupported(KMPXMessageChangeEventType) &&
            aMessage.IsSupported(KMPXMessageMediaGeneralCategory) &&
            aMessage.IsSupported(KMPXMessageMediaGeneralId))
            {
            if (message->ValueTObjectL<TMPXMessageId>(KMPXMessageGeneralId) == aMessage.ValueTObjectL<TMPXMessageId>(KMPXMessageGeneralId) &&
                message->ValueTObjectL<TUid>(KMPXMessageCollectionId) == aMessage.ValueTObjectL<TUid>(KMPXMessageCollectionId) &&
                message->ValueTObjectL<TMPXChangeEventType>(KMPXMessageChangeEventType) == aMessage.ValueTObjectL<TMPXChangeEventType>(KMPXMessageChangeEventType) &&
                message->ValueTObjectL<TMPXGeneralCategory>(KMPXMessageMediaGeneralCategory) == aMessage.ValueTObjectL<TMPXGeneralCategory>(KMPXMessageMediaGeneralCategory) &&
                message->ValueTObjectL<TMPXItemId>(KMPXMessageMediaGeneralId) == aMessage.ValueTObjectL<TMPXItemId>(KMPXMessageMediaGeneralId))
                {
                if (!message->IsSupported(KMPXMessageMediaDeprecatedId) &&
                    !aMessage.IsSupported(KMPXMessageMediaDeprecatedId))
                    {
                    index = i;
                    break;
                    }
                else if (message->IsSupported(KMPXMessageMediaDeprecatedId) &&
                         aMessage.IsSupported(KMPXMessageMediaDeprecatedId))
                    {
                    if (message->ValueTObjectL<TMPXItemId>(KMPXMessageMediaDeprecatedId) ==
                        aMessage.ValueTObjectL<TMPXItemId>(KMPXMessageMediaDeprecatedId))
                        {
                        index = i;
                        break;
                       }
                    }
                else
                    {
                    // else do nothing
                    }
                }
            }
        }

    return index;
    }

// ----------------------------------------------------------------------------
// Get the DRM protection type of the file
// ----------------------------------------------------------------------------
//
EXPORT_C TMCDrmType MPXDbCommonUtil::GetDRMTypeL(const TDesC& aFile)
    {
    MPX_FUNC("MPXDbCommonUtil::GetDRMTypeL");

    using namespace ContentAccess;
    TInt drmProtected(0);
    TVirtualPathPtr virtualPath(aFile, KDefaultContentObject);
    CData* content = CData::NewL(virtualPath, EPeek, EContentShareReadOnly);
    CleanupStack::PushL(content);
    User::LeaveIfError(content->GetAttribute(EIsProtected, drmProtected));
    CleanupStack::PopAndDestroy( content );
    return drmProtected? EMCDrmOmaDrm : EMCDrmNone;
    }

// ----------------------------------------------------------------------------
// MPXDbUtil::ProcessSingleQuotesLC
// ----------------------------------------------------------------------------
//
EXPORT_C HBufC* MPXDbCommonUtil::ProcessSingleQuotesLC(
    const TDesC& aString)
    {
    MPX_FUNC("MPXDbCommonUtil::ProcessSingleQuotesLC");

    // reserve space for all single quotes (double the size)
    // coverity[incorrect_multiplication]
    // coverity[buffer_alloc]
    HBufC* value = HBufC::NewLC(aString.Length() * 2);
    TPtr valuePtr(value->Des());

    MPXDbCommonUtil::FindAndReplaceSingleQuote(aString, valuePtr);

    return value;
    }

// ----------------------------------------------------------------------------
// MPXDbUtil::ProcessPatternCharsLC
// 1) a percentage needs to be escaped for sqlite followed by
//    another percentage sign for escaping % for descriptor formatting, i.e.
//    % --> %% (this percentage will be treated as a percentage instead of
//    pattern matching)
// 2) since back slash is used for escaping the pattern characters, user
//    specified back slash should be escapped as well, i.e. \ --> \\
// ----------------------------------------------------------------------------
//
EXPORT_C HBufC* MPXDbCommonUtil::ProcessPatternCharsLC(
    const TDesC& aString)
    {
    MPX_FUNC("MPXDbCommonUtil::ProcessPatternCharsLC");

    // reserve space for all percentage signs (triple the size because % should
    // be replaced by %%)
    TInt srcLen(aString.Length());
    // coverity[incorrect_multiplication]
    // coverity[buffer_alloc]
    HBufC* targetString = HBufC::NewLC(aString.Length() * 3);
    TPtr targetStringPtr(targetString->Des());

    TPtrC ch;
    for (TInt i = 0; i < srcLen; ++i)
        {
        ch.Set(&aString[i], 1);
        if (ch.CompareF(KMCPercentage) == 0)
            {
            targetStringPtr.Append(KMCPercentage);
            }
           else if ( ch.CompareF( KMCBackSlash ) == 0 )
            {
            targetStringPtr.Append(KMCBackSlash);
            }
        targetStringPtr.Append(ch);            
        }

    MPX_DEBUG3("    original=%S, new=%S", &aString, targetString);
    
    return targetString;
    }

// ----------------------------------------------------------------------------
// Constructs an int SQL criterion
// ----------------------------------------------------------------------------
//
EXPORT_C HBufC* MPXDbCommonUtil::SqlCriterionLC(
    const TDesC& aCriterion,
    TInt aValue)
    {
    MPX_FUNC("MPXDbCommonUtil::SqlCriterionLC");

    HBufC* sqlCriterion = HBufC::NewLC(aCriterion.Length() + KMCIntegerLen);
    TPtr ptr = sqlCriterion->Des();
    ptr.Format(aCriterion, aValue);
    return sqlCriterion;
    }

// ----------------------------------------------------------------------------
// Constructs an int64 SQL criterion
// ----------------------------------------------------------------------------
//
EXPORT_C HBufC* MPXDbCommonUtil::SqlCriterion64LC(
    const TDesC& aCriterion,
    TInt64 aValue)
    {
    MPX_FUNC("MPXDbCommonUtil::SqlCriterion64LC");

    HBufC* sqlCriterion = HBufC::NewLC(aCriterion.Length() + KMCInt64Len);
    TPtr ptr = sqlCriterion->Des();
    ptr.Format(aCriterion, aValue);
    return sqlCriterion;
    }

// ----------------------------------------------------------------------------
// Constructs an SQL criterion using two int values
// ----------------------------------------------------------------------------
//
EXPORT_C HBufC* MPXDbCommonUtil::SqlCriterionLC(
    const TDesC& aCriterion,
    TInt aValue1,
    TInt aValue2)
    {
    MPX_FUNC("MPXDbCommonUtil::SqlCriterionLC 2");

    HBufC* sqlCriterion = HBufC::NewLC(aCriterion.Length() + 2 * KMCIntegerLen);
    TPtr ptr = sqlCriterion->Des();
    ptr.Format(aCriterion, aValue1, aValue2);
    return sqlCriterion;
    }

// ----------------------------------------------------------------------------
// Constructs an SQL criterion using two int64 values
// ----------------------------------------------------------------------------
//
EXPORT_C HBufC* MPXDbCommonUtil::SqlCriterion64LC(
    const TDesC& aCriterion,
    TInt64 aValue1,
    TInt64 aValue2)
    {
    MPX_FUNC("MPXDbCommonUtil::SqlCriterion64LC 2");

    HBufC* sqlCriterion = HBufC::NewLC(aCriterion.Length() + 2 * KMCInt64Len);
    TPtr ptr = sqlCriterion->Des();
    ptr.Format(aCriterion, aValue1, aValue2);
    return sqlCriterion;
    }

// ----------------------------------------------------------------------------
// Constructs a descriptor SQL criterion
// ----------------------------------------------------------------------------
//
EXPORT_C HBufC* MPXDbCommonUtil::SqlCriterionLC(
    const TDesC& aCriterion,
    const TDesC& aValue)
    {
    MPX_FUNC("MPXDbCommonUtil::SqlCriterionLC 3");

    HBufC* value = ProcessSingleQuotesLC(aValue);

    HBufC* sqlCriterion = HBufC::NewL(aCriterion.Length() + value->Length());
    TPtr ptr(sqlCriterion->Des());
    ptr.Format(aCriterion, value);

    CleanupStack::PopAndDestroy(value);
    CleanupStack::PushL(sqlCriterion);
    return sqlCriterion;
    }
    
// ----------------------------------------------------------------------------
// Constructs a descriptor SQL criterion
// ----------------------------------------------------------------------------
//
EXPORT_C HBufC* MPXDbCommonUtil::SqlCriterionLC(
    const TDesC& aCriterion,
    const TDesC& aValue1,
    const TDesC& aValue2)
    {
    MPX_FUNC("MPXDbCommonUtil::SqlCriterionLC 3");

    HBufC* value1 = ProcessSingleQuotesLC(aValue1);
    HBufC* value2 = ProcessSingleQuotesLC(aValue2);

    HBufC* sqlCriterion = HBufC::NewL(aCriterion.Length() + 
                                      value1->Length() + 
                                      value2->Length());
    TPtr ptr(sqlCriterion->Des());
    ptr.Format(aCriterion, value1, value2);

    CleanupStack::PopAndDestroy(value2);
    CleanupStack::PopAndDestroy(value1);
    CleanupStack::PushL(sqlCriterion);
    return sqlCriterion;
    }
    
// ----------------------------------------------------------------------------
// Constructs a descriptor SQL criterion
// ----------------------------------------------------------------------------
//
EXPORT_C HBufC* MPXDbCommonUtil::SqlCriterionLC(
    const TDesC& aCriterion,
    const TDesC& aValue1,
    TInt aValue2,
    const TDesC& aValue3,
    TInt aValue4)
    {
    MPX_FUNC("MPXDbCommonUtil::SqlCriterionLC 3");

    HBufC* value1 = ProcessSingleQuotesLC(aValue1);
    HBufC* value3 = ProcessSingleQuotesLC(aValue3);

    HBufC* sqlCriterion = HBufC::NewL(aCriterion.Length() + 
                                      value1->Length() + 
                                      value3->Length() + 
                                      2 * KMCIntegerLen);
    TPtr ptr(sqlCriterion->Des());
    ptr.Format(aCriterion, value1, aValue2, value3, aValue4);

    CleanupStack::PopAndDestroy(value3);
    CleanupStack::PopAndDestroy(value1);
    CleanupStack::PushL(sqlCriterion);
    return sqlCriterion;
    }    

// ----------------------------------------------------------------------------
// Constructs and adds an int SQL criterion to the criteria array
// ----------------------------------------------------------------------------
//
EXPORT_C TInt MPXDbCommonUtil::AddSqlCriterionL(
    CDesCArray& aSqlCriteria,
    const TDesC& aCriterion,
    TInt aValue)
    {
    MPX_FUNC("MPXDbCommonUtil::AddSqlCriterionL");

    HBufC* sqlCriterion = SqlCriterionLC(aCriterion, aValue);
    TInt length = sqlCriterion->Length();
    aSqlCriteria.AppendL(*sqlCriterion);
    CleanupStack::PopAndDestroy(sqlCriterion);
    return length;
    }

// ----------------------------------------------------------------------------
// Constructs and adds a string SQL criterion to the criteria array
// ----------------------------------------------------------------------------
//
EXPORT_C TInt MPXDbCommonUtil::AddSqlCriterionL(
    CDesCArray& aSqlCriteria,
    const TDesC& aCriterion,
    const TDesC& aValue)
    {
    MPX_FUNC("MPXDbCommonUtil::AddSqlCriterionL");

    HBufC* sqlCriterion = SqlCriterionLC(aCriterion, aValue);
    TInt length = sqlCriterion->Length();
    aSqlCriteria.AppendL(*sqlCriterion);
    CleanupStack::PopAndDestroy(sqlCriterion);
    return length;
    }


// ----------------------------------------------------------------------------
// Create a full path with input drive Id and path
// ----------------------------------------------------------------------------
//
EXPORT_C HBufC* MPXDbCommonUtil::CreateFullPathL(
    TInt aDriveId,
    const TDesC& aPath)
    {
    MPX_FUNC("MPXDbCommonUtil::CreateFullPathL");

    HBufC* hbuf = HBufC::NewLC(aPath.Length() + KMaxDriveName);
    TPtr fullPath(hbuf->Des());

    if (aDriveId != KErrNotFound)
        {
        TDriveUnit driveUnit(aDriveId);
        fullPath.Copy(driveUnit.Name());
        }

    fullPath.Append(aPath);
    CleanupStack::Pop(hbuf);
    return hbuf;
    }

// ----------------------------------------------------------------------------
// MPXDbCommonUtil::AppendValueL
// ----------------------------------------------------------------------------
//
EXPORT_C void MPXDbCommonUtil::AppendValueL(
    CDesCArray& aFields,
    CDesCArray& aValues,
    const TDesC& aField,
    const TDesC& aValue)
    {
    MPX_FUNC("MPXDbCommonUtil::AppendValueL");

    aFields.AppendL(aField);

    HBufC* value = HBufC::NewLC(aValue.Length() * 2 + 2);
    TPtr valuePtr(value->Des());

    MPXDbCommonUtil::FindAndReplaceSingleQuote(aValue, valuePtr);

    // use 'value' instead of value, 0 length strings should be ''
    //
    if( valuePtr.Length() )
        {
        valuePtr.Insert(0, KMCSingleQuote);
        valuePtr.Append(KMCSingleQuote);
        }
    else
        {
        valuePtr.Append(KMCSingleQuote);
        valuePtr.Append(KMCSingleQuote);
        }
    aValues.AppendL(valuePtr);

    CleanupStack::PopAndDestroy(value);
    }

// ----------------------------------------------------------------------------
// MPXDbCommonUtil::AppendValueL
// ----------------------------------------------------------------------------
//
EXPORT_C void MPXDbCommonUtil::AppendValueL(
    CDesCArray& aFields,
    CDesCArray& aValues,
    const TDesC& aField,
    TUint32 aValue)
    {
    MPX_FUNC("MPXDbCommonUtil::AppendValueL");

    aFields.AppendL(aField);
    TBuf<KMCIntegerLen> value;
    value.AppendNum(static_cast<TInt64>(aValue));
    aValues.AppendL(value);
    }

// ----------------------------------------------------------------------------
// Gets the MIME type for a specified URI
// ----------------------------------------------------------------------------
//
EXPORT_C TDataType MPXDbCommonUtil::GetMimeTypeForUriL(
    const TDesC& aUri)
    {
    MPX_FUNC("MPXDbUtil::GetMimeTypeForUriL");

    if ( const TDesC8 * type = FindMimeTypeFromMap (aUri) )
        {
        return TDataType(*type);
        }
    
    RApaLsSession appArc;
    User::LeaveIfError(appArc.Connect());
    CleanupClosePushL(appArc);
    TUid dummyUid(KNullUid);
    TDataType mimeType;
    appArc.AppForDocument(aUri, dummyUid, mimeType);
    CleanupStack::PopAndDestroy(&appArc);

    return mimeType;
    }

// ----------------------------------------------------------------------------
// MPXDbCommonUtil::StringFromArrayLC
// ----------------------------------------------------------------------------
//
EXPORT_C HBufC* MPXDbCommonUtil::StringFromArrayLC(
    const CDesCArray& aArray,
    const TDesC& aSeparator)
    {
    MPX_FUNC("MPXDbCommonUtil::StringFromArrayLC");

    HBufC* str(NULL);
    TInt count(aArray.Count());
    if (count)
        {
        TInt len(0);
        TInt index(0);
        for (index = 0; index < count; ++index)
            {
            len += aArray[index].Length();
            }

        str = HBufC::NewLC(len + (aSeparator.Length() * (count - 1)));
        TPtr ptr(str->Des());
        TPtrC16 item;
        for (index = 0; index < count; ++index)
            {
            item.Set(aArray[index]);
            ptr.Append(item);
            MPX_DEBUG2("aArray[index] %S", &item);
            if (index < (count - 1))
                {
                ptr.Append(aSeparator);
                }
            }
        }
    else
        {
        str = HBufC::NewLC(0);
        }

    return str;
    }

// ----------------------------------------------------------------------------
// MPXDbCommonUtil::StringFromArraysLC
// ----------------------------------------------------------------------------
//
EXPORT_C HBufC* MPXDbCommonUtil::StringFromArraysLC(
    const CDesCArray& aNameArray,
    const CDesCArray& aValueArray,
    const TDesC& aValueSeparator,
    const TDesC& aEntitySeparator)
    {
    MPX_FUNC("MPXDbCommonUtil::StringFromArraysLC");

    // calculate the length of the SET string
    TInt len(0);
    TInt index(0);
    TInt count(aNameArray.Count());
    for (index = 0; index < count; ++index)
        {
        len += aNameArray[index].Length() + aValueArray[index].Length();
        }

    // construct the result string
    HBufC* result = HBufC::NewLC(len +
        ((aValueSeparator.Length() + aEntitySeparator.Length()) * count));
    TPtr resultPtr(result->Des());
    for (index = 0; index < count; ++index)
        {
        resultPtr.Append(aNameArray[index]);
        resultPtr.Append(aValueSeparator);
        resultPtr.Append(aValueArray[index]);

        if (index < count - 1)
            {
            resultPtr.Append(aEntitySeparator);
            }
        }

    return result;
    }

// ----------------------------------------------------------------------------
// MPXDbCommonUtil::TTimeToDesLC
// Converts a TTime to the internal SQLite format (YYYY-MM-DD HH:MM:SS).
// ----------------------------------------------------------------------------
//
EXPORT_C HBufC* MPXDbCommonUtil::TTimeToDesLC(
    const TTime& aTime)
    {
    MPX_FUNC("MPXDbCommonUtil::TTimeToDesLC");

    HBufC* dateTime;
    if (aTime == Time::NullTTime())
        {
        dateTime = HBufC::NewLC(0);
        }
    else
        {
        _LIT(KDateTimeFormat, "%04d-%02d-%02d %02d:%02d:%02d");
        
        TTime time(0);

        // negative time means BC, but format string will be invalid with our format,
        // i.e. %04d in KDateTimeFormat when year is -1 will result in "00-1" and
        // the whole string becomes "00-1-01-01 00:00:00" which will result in error,
        // so set to 0 in such cases
        TDateTime dt = aTime>time ? aTime.DateTime() : time.DateTime();
        TInt dateTimeLen = KDateTimeFormat().Length();
        dateTime = HBufC::NewLC(dateTimeLen);
        TPtr dateTimePtr = dateTime->Des();
        dateTimePtr.Format(KDateTimeFormat,
                        dt.Year(),
                        dt.Month() + 1, // zero based
                        dt.Day() + 1,   // zero based
                        dt.Hour(),
                        dt.Minute(),
                        dt.Second());
        }
    return dateTime;
    }

// ----------------------------------------------------------------------------
// MPXDbCommonUtil::DesToTTimeL
// Converts a date time string in the internal SQLite format (YYYY-MM-DD HH:MM:SS)
// to a TTime. TTime can only parse in the DD-MM-YYYY HH:MM:SS format.
// ----------------------------------------------------------------------------
//
EXPORT_C TTime MPXDbCommonUtil::DesToTTimeL(
    const TDesC& aDateTime)
    {
    MPX_FUNC("MPXDbCommonUtil::DesToTTimeL");

    TTime time(0);
    if (aDateTime.Length() != 0)
        {
        _LIT(KDash, "-");
        _LIT(KSpace, " ");

        TLocale locale;
        TDateFormat iDateFormat = locale.DateFormat();

        HBufC* dateTime = HBufC::NewLC(aDateTime.Length());
        TPtr dateTimePtr = dateTime->Des();

        //as TTime::Parse is locale dependent, check it:
        if(iDateFormat==EDateEuropean)
            {
            dateTimePtr.Append(aDateTime.Mid(8, 2));//day DD
            dateTimePtr.Append(KDash);
            dateTimePtr.Append(aDateTime.Mid(5, 2));//month MM
            dateTimePtr.Append(KDash);
            dateTimePtr.Append(aDateTime.Left(4));//year YYYY
            dateTimePtr.Append(KSpace);
            }
        else if(iDateFormat==EDateJapanese)
            {
            dateTimePtr.Append(aDateTime.Left(4));//year YYYY
            dateTimePtr.Append(KDash);
            dateTimePtr.Append(aDateTime.Mid(5, 2));//month MM
            dateTimePtr.Append(KDash);
            dateTimePtr.Append(aDateTime.Mid(8, 2));//day DD
            dateTimePtr.Append(KSpace);
            }
        else //iDateFormat==EDateAmerican
            {
            dateTimePtr.Append(aDateTime.Mid(5, 2));//month MM
            dateTimePtr.Append(KDash);
            dateTimePtr.Append(aDateTime.Mid(8, 2));//day DD
            dateTimePtr.Append(KDash);
            dateTimePtr.Append(aDateTime.Left(4));//year YYYY
            dateTimePtr.Append(KSpace);            
            }

		    // When colon (:) is set as Date separator in Date and Time setting, 
		    // colon in Time descriptors is parsed as Date separator. 
        if ( locale.DateSeparator(1) == ':')
            {
            _LIT(KDot, ".");
        
        		// time HH.MM.SS
            dateTimePtr.Append( aDateTime.Mid(11, 2) );
            dateTimePtr.Append( KDot );
            dateTimePtr.Append( aDateTime.Mid(14, 2) );
            dateTimePtr.Append( KDot );
            dateTimePtr.Append( aDateTime.Mid(17, 2) );
            }
        else
            {
            dateTimePtr.Append(aDateTime.Right(8));//time HH:MM:SS
            }

        User::LeaveIfError(time.Parse(dateTimePtr));
        CleanupStack::PopAndDestroy(dateTime);
        }
    else
        {
        time = Time::NullTTime();
        }
    return time;
    }

// ----------------------------------------------------------------------------
// MPXDbCommonUtil::CurrentTimeDesLC
// ----------------------------------------------------------------------------
//
EXPORT_C HBufC* MPXDbCommonUtil::CurrentTimeDesLC()
    {
    MPX_FUNC("MPXDbCommonUtil::CurrentTimeDesLC");

    TTime time;
    time.HomeTime();

    return TTimeToDesLC(time);
    }

// ----------------------------------------------------------------------------
// MPXDbCommonUtil::CurrentDateDesLC
// ----------------------------------------------------------------------------
//
EXPORT_C HBufC* MPXDbCommonUtil::CurrentDateDesLC()
    {
    MPX_FUNC("MPXDbCommonUtil::CurrentDateDesLC");

    TTime time;
    time.HomeTime();

    // Round off to the nearest hour
    TDateTime date(time.DateTime());
    date.SetMinute(0);
    date.SetSecond(0);
    date.SetMicroSecond(0);

    return TTimeToDesLC(TTime(date));
    }

// ----------------------------------------------------------------------------
// MPXDbCommonUtil::GetDriveL
// ----------------------------------------------------------------------------
//
EXPORT_C TInt MPXDbCommonUtil::GetDriveL(
    const TDesC& aUri,
    TDriveUnit& aDrive)
    {
    MPX_FUNC("MPXDbCommonUtil::GetDriveL");

    TInt err(KErrNotFound);
    TParsePtrC parser(aUri);
    if (parser.Drive().Length())
        {
        aDrive = TDriveUnit(aUri);
        err = KErrNone;
        }

    return err;
    }

// ----------------------------------------------------------------------------
// MPXDbCommonUtil::AttributeExistsL
// ----------------------------------------------------------------------------
//
EXPORT_C TBool MPXDbCommonUtil::AttributeExists(
    const TArray<TMPXAttribute>& aAttrs,
    const TMPXAttribute& aAttribute)
    {
    MPX_FUNC("MPXDbCommonUtil::AttributeExists");

    TBool ret(EFalse);

    TUint content(aAttribute.ContentId());
    TUint attribute(aAttribute.AttributeId());

    TInt count(aAttrs.Count());
    for (TInt i = 0; i < count; ++i)
        {
        if ((aAttrs[i].ContentId() == content) &&
            (aAttrs[i].AttributeId() & attribute))
            {
            ret = ETrue;
            break;
            }
        }

    return ret;
    }

// ----------------------------------------------------------------------------
// MPXDbCommonUtil::ConstructMediaLC
// ----------------------------------------------------------------------------
//
CMPXMedia* MPXDbCommonUtil::ConstructMediaLC(
    const TDesC& aTitle,
    TMPXGeneralType aType,
    TMPXGeneralCategory aCat,
    TMPXItemId aId,
    TInt aNonPermissibleActions,
    TUint aDbflag)
    {
    MPX_FUNC("MPXDbCommonUtil::ConstructMediaLC");

    RArray<TInt> supportedIds;
    CleanupClosePushL(supportedIds);
    supportedIds.AppendL(KMPXMediaIdGeneral);
    CMPXMedia* entry = CMPXMedia::NewL(supportedIds.Array());
    CleanupStack::PopAndDestroy(&supportedIds);
    CleanupStack::PushL(entry);
    entry->SetTextValueL(KMPXMediaGeneralTitle, aTitle);
    entry->SetTObjectValueL<TMPXGeneralType>(KMPXMediaGeneralType, aType);
    entry->SetTObjectValueL<TMPXGeneralCategory>(KMPXMediaGeneralCategory, aCat);
    entry->SetTObjectValueL<TMPXItemId>(KMPXMediaGeneralId, aId);
    entry->SetTObjectValueL<TUint>(KMPXMediaGeneralFlags, aDbflag);

    if (aNonPermissibleActions)
        {
        // set non-permissible actions
        entry->SetTObjectValueL<TMPXGeneralNonPermissibleActions>(KMPXMediaGeneralNonPermissibleActions,
            static_cast<TMPXGeneralNonPermissibleActions>(aNonPermissibleActions));
        }

    return entry;
    }

// ----------------------------------------------------------------------------
// MPXDbCommonUtil::GetColumnTextL
// ----------------------------------------------------------------------------
//
EXPORT_C TPtrC MPXDbCommonUtil::GetColumnTextL(
	RSqlStatement& aStatement, 
	TInt aField)
	{
    MPX_FUNC("MPXDbCommonUtil::GetColumnTextL");
    
    TPtrC text;
	if (aStatement.ColumnSize(aField))
		{
	    text.Set(aStatement.ColumnTextL(aField));
		}
	else
		{
	    text.Set(KNullDesC);    	        		
		}	

    return text;
	}

// ----------------------------------------------------------------------------
// Add an album item changed message to the message array
// ----------------------------------------------------------------------------
//
EXPORT_C void MPXDbCommonUtil::AddItemAlbumChangedMessageL(
    CMPXMessageArray& aMessageArray,
    TMPXItemId aId,
    TMPXChangeEventType aChangeType,
    TMPXGeneralCategory aCategory,
    TUint aUid,
    TBool aAlbumArt,
    TMPXItemId aDeprecatedId)
    {
    MPX_FUNC("MPXDbCommonUtil::AddItemChangedMessageL");
    CMPXMessage* message = CMPXMedia::NewL();
    CleanupStack::PushL(message);

    FillItemChangedMessageL(*message, aId, aChangeType, aCategory, aUid,
    		aDeprecatedId );
    if ( aAlbumArt )
        {
        message->SetTObjectValueL<TMPXItemId>(KMPXMessageMediaDeprecatedId, aId);
        }
    if (FindItemChangedMessageL(aMessageArray, *message) == KErrNotFound)
        {
        aMessageArray.AppendL(*message); // ownership xfer
        }
    CleanupStack::PopAndDestroy(message);
    }
// End of File
