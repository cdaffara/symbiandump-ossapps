/*
* Copyright (c) 2010 Kanrikogaku Kenkyusho, Ltd.
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Kanrikogaku Kenkyusho, Ltd. - Initial contribution
*
* Contributors:
*
* Description:
* {Description of the file}
*
*/

// Protection against nested includes
#ifndef _DIRECTPRINTUIPLUGIN_H_
#define _DIRECTPRINTUIPLUGIN_H_

// System includes
#include <e32base.h>
#include <coecntrl.h>
#include <ecom.h>

// User includes
#include "directprintpropertydata.h"

// Forward declarations
class CAknSettingItemArray;
class RReadStream;
class RWriteStream;

//Constants
/**  Interface UID */
const TUid KUidDirectPrintUiPluginInterface = { 0x2002BB33 };

// Class declaration
/**
 *  Ui plugin class
 *  more_complete_description
 */
class CDirectPrintUiPlugin : public CBase
	{
public:
	inline static CDirectPrintUiPlugin* NewL(TUid aUid);
	inline static CDirectPrintUiPlugin* NewL(const TDesC8& aKey);
	virtual ~CDirectPrintUiPlugin();

public:
	inline static void GetPluginListL(RImplInfoPtrArray& aArray);

public:
	virtual void InternalizePropertiesL(RReadStream& aStream) = 0;
	virtual void ExternalizePropertiesL(RWriteStream& aStream) = 0;

	virtual void LoadExtendItemsL(CAknSettingItemArray& aArray, TInt aId) = 0;
	virtual void ReadDataL(TDirectPrintPropertyData& aData) = 0;
	virtual void WriteDataL(RDPPropertyDataArray& aArray) = 0;
	virtual void NewDataL() = 0;

protected:
	inline CDirectPrintUiPlugin();

private:
	TUid iDtor_ID_Key;
	};


// Inline codepart
inline CDirectPrintUiPlugin::CDirectPrintUiPlugin()
	{
	}

inline CDirectPrintUiPlugin* CDirectPrintUiPlugin::NewL(TUid aUid)
	{
	TAny* any = REComSession::CreateImplementationL(aUid, _FOFF(CDirectPrintUiPlugin, iDtor_ID_Key));
	CDirectPrintUiPlugin* plugin = reinterpret_cast<CDirectPrintUiPlugin*>(any);
	return plugin;
	}

inline CDirectPrintUiPlugin* CDirectPrintUiPlugin::NewL(const TDesC8& aKey)
	{
	TEComResolverParams params;
	params.SetDataType(aKey);
	params.SetWildcardMatch(ETrue);

	TAny* any = REComSession::CreateImplementationL(KUidDirectPrintUiPluginInterface, _FOFF(CDirectPrintUiPlugin, iDtor_ID_Key), NULL, params);
	CDirectPrintUiPlugin* plugin = reinterpret_cast<CDirectPrintUiPlugin*>(any);
	return plugin;
	}

inline CDirectPrintUiPlugin::~CDirectPrintUiPlugin()
	{
	REComSession::DestroyedImplementation(iDtor_ID_Key);
	}

inline void CDirectPrintUiPlugin::GetPluginListL(RImplInfoPtrArray& aArray)
	{
	REComSession::ListImplementationsL(KUidDirectPrintUiPluginInterface, aArray);
	}

#endif // _DIRECTPRINTUIPLUGIN_H_
