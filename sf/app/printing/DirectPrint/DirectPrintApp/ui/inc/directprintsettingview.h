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
#ifndef __PRINTERSETTINGVIEW_H__
#define __PRINTERSETTINGVIEW_H__

// System includes
#include <aknlists.h>
#include <aknsettingitemlist.h>

// User includes
#include "DirectPrintViewBase.h"
#include "directprintsettinglistbox.h"
#include "directprintselectitem.h"

// Forward declarations
class CDirectPrintBearerMgr;

// Class declaration
/**
 *  Printer setting view class
 *  more_complete_description
 */
class CDirectPrintPrinterSettingView : public CDirectPrintViewBase
	{
public:
	enum TListItemIndex
		{
		ESettingViewItemPrinterName = 1,
		ESettingViewItemDriver,
		ESettingViewItemType,
		ESettingViewItemBearer,
		ESettingViewItemExtent
		};

public:
	/** Constructors */
	static CDirectPrintPrinterSettingView* NewL();
	static CDirectPrintPrinterSettingView* NewLC();
	/** Destructor */
	~CDirectPrintPrinterSettingView();

public:
	/**
	 * From CAknView.
	 * Handles user commands.
	 *
	 * @param aCommand The id of commands.
	 */
	void HandleCommandL(TInt aCommand);

public:
	/**
	 * Handles index.
	 *
	 * @param aIndex The index of selected item.
	 * @param aChanged The flag of change data.
	 */
	void HandleEditItemL(TInt aIndex, TBool aChanged);

protected: // from CAknView
	void DoActivateL(const TVwsViewId& aPrevViewId, TUid aCustomMessageId, const TDesC8& aCustomMessage);
	void DoDeactivate();

private:
	CDirectPrintPrinterSettingView();
	void ConstructL();

	void LoadListL();
	void LoadExtendListL();
	void ReloadListL();
	void SetBearerListL(CAknEnumeratedTextPopupSettingItem* item);

	void ReadDataL();
	void WriteDataL();
	void NewDataL();

	void LoadDriverInfoL();
	void LoadTypeInfoL();
	void LoadBearerInfoL();
	void LoadItemInfoL(TInt aResouceId, const TDesC& aInfo, TInt& aIndex);
	void LoadExtendSettingsL();
	void StoreDriverInfoL();
	void StoreTypeInfoL();
	void StoreBearerInfoL();
	void StoreItemInfoL(TInt aResouceId, TDes& aInfo);
	void StoreExtendSettingsL();

	CAknSettingItem* GetSettingItemL(const TDesC& aItemTitle);

	CDirectPrintBearerMgr* GetBearerMgr();

private:
	/** Pointer to listbox. */
	CDirectPrintSettingListBox* iListBox;
	/** Printer id. */
	TInt iPrinterId;
	/** Printer name. */
	TBuf<50> iPrinterName;
	/** Index of driver setting. */
	TInt iDriver;
	/** Driver uid. */
	TUid iDriverUid;
	/** Driver name. */
	TBuf<50> iDriverName;
	/** Index of bearer setting. */
	TInt iBearer;
	/** Bearer name. */
	TBuf<50> iBearerName;
	/** Index of access point setting. */
	TInt iAccessPoint;
	/** Access point name. */
	TBuf<50> iAccessPointName;
	/** Host name. */
	TBuf<50> iHost;
	/** Ip address. */
	TInetAddr iHostIp;
	/** Port number. */
	TInt iPort;
	/** User name. */
	TBuf<50> iUserName;
	/** Que name. */
	TBuf<50> iQue;
	/** Printer type name. */
	TBuf<50> iTypeName;
	/** Index of printer type setting. */
	TInt iType;

	/** Array of driver. */
	RSelectItemArray iDriverList;
	/** Array of bearer. */
	RSelectItemArray iBearerList;
	/** Array of type. */
	RSelectItemArray iTypeList;
	};

#endif // __PRINTERSETTINGVIEW_H__
