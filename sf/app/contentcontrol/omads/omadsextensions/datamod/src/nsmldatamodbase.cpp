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
* Description:  Versit data stripper and merger.
*
*/


// INCLUDES
#include <NSmlDataModBase.h>
#include "nsmldebug.h"
#include "nsmlconstants.h"

#include <versit.h>
#include <vcal.h>
#include <vcard.h>
#include <s32mem.h>
#include <stringpool.h>
#include <vtoken.h>
#include <e32property.h>
#include <DataSyncInternalPSKeys.h>

//Fix to Remove the Bad Compiler Warnings
#ifndef __WINS__
// This lowers the unnecessary compiler warning (armv5) to remark.
// "Warning:  #174-D: expression has no effect..." is caused by 
// DBG_ARGS8 macro in no-debug builds.
#pragma diag_remark 174
#endif

// ============================ MEMBER FUNCTIONS ===============================

// ------------------------------------------------------------------------------------------------
// CNSmlDataModBase::~CNSmlDataModBase
// Destructor.
// ------------------------------------------------------------------------------------------------
EXPORT_C CNSmlDataModBase::~CNSmlDataModBase()
	{
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDataModBase::SetOwnStoreFormat
// Sets own database format.
// ------------------------------------------------------------------------------------------------
EXPORT_C void CNSmlDataModBase::SetOwnStoreFormat( CSmlDataStoreFormat& aOwnStoreFormat )
	{
	_DBG_FILE("CNSmlDataModBase::SetOwnStoreFormat(): begin");
	iOwnStoreFormat = &aOwnStoreFormat;
	_DBG_FILE("CNSmlDataModBase::SetOwnStoreFormat(): end");
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDataModBase::SetPartnerStoreFormat
// Sets remote database format.
// ------------------------------------------------------------------------------------------------
EXPORT_C void CNSmlDataModBase::SetPartnerStoreFormat( CSmlDataStoreFormat& aRemoteStoreFormat )
	{
	_DBG_FILE("CNSmlDataModBase::SetPartnerStoreFormat(): begin");
	iRemoteStoreFormat = &aRemoteStoreFormat;
	_DBG_FILE("CNSmlDataModBase::SetPartnerStoreFormat(): end");
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDataModBase::SetUsedMimeType
// Sets mime type that is used for sending and receiving. 
// This method can be called separately before every item.
// ------------------------------------------------------------------------------------------------
EXPORT_C TInt CNSmlDataModBase::SetUsedMimeType( const RStringF aMimeType, const RStringF aMimeVersion )
	{
	_DBG_FILE("CNSmlDataModBase::SetUsedMimeTypeL(): begin");
	iUsedRemoteMimeType = -1;
	if ( !iRemoteStoreFormat )
		{
		_DBG_FILE("CNSmlDataModBase::SetUsedMimeTypeL() - Not Found: end");
		return KErrNotFound;
		}
	else
		{
		for ( TInt i = 0; i < iRemoteStoreFormat->MimeFormatCount(); i++ )
			{
			if ( iRemoteStoreFormat->MimeFormat( i ).MimeType().DesC().Compare( aMimeType.DesC() ) == 0 )
				{
				if ( iRemoteStoreFormat->MimeFormat( i ).MimeVersion().DesC().Compare( aMimeVersion.DesC() ) == 0 )
					{
					iUsedRemoteMimeType = i;
					iMimeType = iRemoteStoreFormat->MimeFormat( i ).MimeType();
					iMimeVersion = iRemoteStoreFormat->MimeFormat( i ).MimeVersion();
					_DBG_FILE("CNSmlDataModBase::SetUsedMimeTypeL() - Found: end");
					return KErrNone;
					}
				}
			}
		}
	_DBG_FILE("CNSmlDataModBase::SetUsedMimeTypeL() - Not Found: end");
	return KErrNotFound;
	}
	
// ------------------------------------------------------------------------------------------------
// CNSmlDataModBase::StripTxL
// Strips data that is to be transmitted to the sync partner.
// ------------------------------------------------------------------------------------------------
EXPORT_C void CNSmlDataModBase::StripTxL( CBufBase& aItem )
	{
	_DBG_FILE("CNSmlDataModBase::StripTxL(): begin");
	HBufC8* buf = HBufC8::NewLC(aItem.Size());
	*buf = aItem.Ptr(0);
	TPtr8 ptrBuf = buf->Des();

	StripL(ptrBuf);

	aItem.Reset();
	aItem.InsertL(0, ptrBuf);
	CleanupStack::PopAndDestroy(); // buf
	_DBG_FILE("CNSmlDataModBase::StripTxL(): end");
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDataModBase::MergeRxL
// Merges received item with item in exported from the local database.
// ------------------------------------------------------------------------------------------------
EXPORT_C void CNSmlDataModBase::MergeRxL( CBufBase& aNewItem, CBufBase& aOldItem, TBool aFieldLevel )
	{
	_DBG_FILE("CNSmlDataModBase::MergeRxL(): begin");
	if( NeedsMerge() )
		{
		HBufC8* b1 = HBufC8::NewLC(aNewItem.Size() + aOldItem.Size());
		*b1 = aNewItem.Ptr(0);
		TPtr8 ptrB1 = b1->Des();
		TPtr8 p2 = aOldItem.Ptr(0);

		MergeL( ptrB1, p2, aFieldLevel );

		aNewItem.Reset();
		aNewItem.InsertL(0, ptrB1);
		CleanupStack::PopAndDestroy(); // b1
		}
	else if ( iUsedRemoteMimeType == -1 )
		{
		User::Leave( KErrNotFound );
		}
	_DBG_FILE("CNSmlDataModBase::MergeRxL(): end");
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDataModBase::NeedsMerge
// Checks whether merging is needed or not.
// ------------------------------------------------------------------------------------------------
EXPORT_C TBool CNSmlDataModBase::NeedsMerge()
	{
	_DBG_FILE("CNSmlDataModBase::NeedsMerge(): begin");
	iUsedRemoteMimeType = -1;
	if ( !iRemoteStoreFormat )
		{
		_DBG_FILE("CNSmlDataModBase::NeedsMerge() - EFalse: end");
		return EFalse;
		}
	else
		{
		for ( TInt i = 0; i < iRemoteStoreFormat->MimeFormatCount(); i++ )
			{
			if ( iRemoteStoreFormat->MimeFormat( i ).MimeType().DesC().Compare( iMimeType.DesC() ) == 0 )
				{
				if ( iRemoteStoreFormat->MimeFormat( i ).MimeVersion().DesC().Compare( iMimeVersion.DesC() ) == 0 )
					{
					iUsedRemoteMimeType = i;
					if ( iRemoteStoreFormat->MimeFormat( i ).PropertyCount() > 0 )
						{
						_DBG_FILE("CNSmlDataModBase::NeedsMerge() - ETrue: end");
						return ETrue;
						}
					}
				}
			}
		}
	_DBG_FILE("CNSmlDataModBase::NeedsMerge() - EFalse: end");
	return EFalse;
	}
	
// ------------------------------------------------------------------------------------------------
// CNSmlDataModBase::CNSmlDataModBase
// Basic constructor of class.
// ------------------------------------------------------------------------------------------------
CNSmlDataModBase::CNSmlDataModBase()
	{
	iUsedOwnMimeType = -1;
	iUsedRemoteMimeType = -1;
	}
	
// ------------------------------------------------------------------------------------------------
// CNSmlDataModBase::StripL
// Strips data that is to be transmitted to the sync partner.
// ------------------------------------------------------------------------------------------------
void CNSmlDataModBase::StripL( TDes8& aItem )
	{
	_DBG_FILE("CNSmlDataModBase::StripL(): begin");
	if ( !NeedsMerge() )
		{
		if ( iUsedRemoteMimeType == -1 )
			{
			User::Leave( KErrNotFound );
			}
		return;
		}
	TBool modified( EFalse );
	CVersitParser* parser = ChildCreateParserLC();
	RDesReadStream drs( aItem );
	CleanupClosePushL( drs );
	parser->InternalizeL( drs );

	// Now we're ready to start analysis
	CArrayPtr<CVersitParser>* entities = parser->ArrayOfEntities( EFalse );
	if( entities )
		{
		for( TInt i = 0; i < entities->Count(); i++ )
			{
			StripEntityL( entities->At( i ), modified );
			}
		}
	else
		{
		StripEntityL( parser, modified );
		}

	// Only update if anything was modified in process
	if( modified )
		{
		aItem.Zero();
		RDesWriteStream dws( aItem );
		CleanupClosePushL( dws );
		parser->ExternalizeL( dws );
		dws.CommitL();
		CleanupStack::PopAndDestroy(); // dws
		}
	CleanupStack::PopAndDestroy( 2 ); // drs, parser
	_DBG_FILE("CNSmlDataModBase::StripL(): end");
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDataModBase::StripEntityL
// Strips data that is to be transmitted to the sync partner from entity.
// ------------------------------------------------------------------------------------------------
void CNSmlDataModBase::StripEntityL( CVersitParser* aEntity, TBool& aModified ) const
	{
	_DBG_FILE("CNSmlDataModBase::StripEntityL(): begin");
	StripAllNotOnPartnerListL( aEntity, aModified, ETrue );
	_DBG_FILE("CNSmlDataModBase::StripEntityL(): end");
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDataModBase::StripAllOnPartnerListL
// Strips all data from entity that is supported by remote server.
// ------------------------------------------------------------------------------------------------
void CNSmlDataModBase::StripAllOnPartnerListL( CVersitParser* aEntity, TBool& aModified, TBool aParamLevelCheck ) const
	{
    _DBG_FILE("CNSmlDataModBase::StripAllOnPartnerListL(): begin");

    if( iRemoteStoreFormat->MimeFormat( iUsedRemoteMimeType ).PropertyCount() )
        {
        
        // Check correct Data Sync protocol
        TInt value( EDataSyncNotRunning );
        TSmlProtocolVersion usedSyncProtocol( ESmlVersion1_2 );
        TInt error = RProperty::Get( KPSUidDataSynchronizationInternalKeys,
                                     KDataSyncStatus,
                                     value );
        if ( error == KErrNone &&
             value == EDataSyncRunning )
            {
            usedSyncProtocol = ESmlVersion1_1_2;
            }
        
        TBool wasModified( EFalse );

        CArrayPtr<CParserProperty>* allProps = aEntity->ArrayOfProperties( EFalse );
        for( TInt i = 0; i < allProps->Count(); ) // Variable i is not increased here because size of count might be changes during loop
            {
            const CParserProperty& ownProperty = *allProps->At( i );
           

            TBool removeMe( EFalse );
            
            for( TInt i2 = 0; i2 < iRemoteStoreFormat->MimeFormat( iUsedRemoteMimeType ).PropertyCount(); i2++ )
                {
                const CSmlDataProperty& remoteProperty = iRemoteStoreFormat->MimeFormat( iUsedRemoteMimeType ).Property( i2 );
                if( !ownProperty.Name().Compare( remoteProperty.Field().Name().DesC() ) )
                    {
                    if( remoteProperty.ParamCount() > 0 && aParamLevelCheck )
                        {
                        if ( usedSyncProtocol == ESmlVersion1_1_2 )
                            {
                            for( TInt i3 = 0; i3 < remoteProperty.ParamCount(); i3++ )
                                {
                                if( ownProperty.Param( remoteProperty.Param( i3 ).Field().Name().DesC() ) )
                                    {
                                    removeMe = ETrue;
                                    }
                                }                           
                            }
                        else // ESmlVersion1_2
                            {
                            CArrayPtr<CParserParam>* ownerparamarray = ownProperty.ParamArray();
                            if(ownerparamarray != NULL)
                                {
                                for(TInt ownerparam = 0; ownerparam < ownerparamarray->Count(); ownerparam++)
                                    {
                                    removeMe = EFalse;
                                    const CParserParam& ownParam = *ownerparamarray->At( ownerparam );
                                     TPtrC8 ownparamname = ownParam.Name();
                                     TPtrC8 ownparamvalue = ownParam.Value();
                                     if(ownparamvalue == _L8(""))
                                        {
                                         for(TInt remoteparam = 0; remoteparam < remoteProperty.ParamCount(); remoteparam++)
                                            {
                                            TDesC8 remoteparamname = remoteProperty.Param( remoteparam ).Field().Name().DesC();
                                            const CSmlDataField& field = remoteProperty.Param( remoteparam ).Field();
											if(field.EnumValueCount() > 0)
												{
	                                            for( TInt rmtenumcount = 0; rmtenumcount < field.EnumValueCount(); rmtenumcount++ )
													{
	                                                TPtrC8 rmtenumvalue = field.EnumValue( rmtenumcount ).DesC();
													if( rmtenumvalue.Compare(ownparamname)== 0 )
	                                                    {
	                                                    removeMe = ETrue;
														break;
	                                                    }  
	                                                } 
												}
											else
												{
												removeMe = ETrue;
												break;
												}
                                            }
                                        }
                                     else
                                        {
										//Handling when the device supports VersitTokenType as "Encoding"    
										if(ownparamname == KVersitTokenENCODING)
											{
											removeMe = ETrue;
											}
										else
											{
	                                         for(TInt remoteparam = 0; remoteparam < remoteProperty.ParamCount(); remoteparam++)
	                                            {
	                                            if(ownparamname.Compare(remoteProperty.Param( remoteparam ).Field().Name().DesC()) == 0)
	                                                {
	                                                const CSmlDataField& field = remoteProperty.Param( remoteparam ).Field();
													if( field.EnumValueCount() > 0 )
														{
		                                                for( TInt rmtenumcount = 0; rmtenumcount < field.EnumValueCount(); rmtenumcount++ )
		                                                    {
		                                                     TPtrC8 rmtenumvalue = field.EnumValue( rmtenumcount ).DesC();
		                                                     if( rmtenumvalue.Compare(ownparamvalue)== 0 )
		                                                         {
		                                                         removeMe = ETrue;
																 break;
		                                                         }  
		                                                    } 
														}
													else
														{
														removeMe = ETrue;
														break;
														}
	                                                }                                               
	                                            }
											}
                                        }
                                    	if( !removeMe )
                                        {
                                        	break;
                                        }	    
                                    } 
                                }
                            else
                                {
                                removeMe = ETrue;
                                }
                            }
                        }
                    else
                        {
                        removeMe = ETrue;
                        }

                    if( removeMe )
                        {
                        break;
                        }
                    }
                }
            if( removeMe )
                {
                #ifdef __NSML_DEBUG__
                    TPtrC8 pn( ownProperty.Name() );
                    DBG_ARGS8(_S8("CNSmlDataModBase::StripAllOnPartnerListL(): Dropping %S"), &pn);
                #endif // __NSML_DEBUG__
                delete allProps->At( i );
                allProps->Delete( i );
                wasModified = ETrue;
                aModified = ETrue;
                }
            else
                {
                #ifdef __NSML_DEBUG__
                    TPtrC8 pn( ownProperty.Name() );
                    DBG_ARGS8(_S8("CNSmlDataModBase::StripAllOnPartnerListL(): NOT dropping %S"), &pn);
                #endif // __NSML_DEBUG__
                i++;
                }
            }
        // can't use aModified as that may have been set earlier!
        if( wasModified )
            {
            allProps->Compress();
            }
        }
    _DBG_FILE("CNSmlDataModBase::StripAllOnPartnerListL(): end");
    }

// ------------------------------------------------------------------------------------------------
// CNSmlDataModBase::StripAllNotOnPartnerListL
// Strips all data from entity that is not supported by remote server.
// ------------------------------------------------------------------------------------------------
void CNSmlDataModBase::StripAllNotOnPartnerListL( CVersitParser* aEntity, TBool& aModified, TBool aParamLevelCheck ) const
	{
    _DBG_FILE("CNSmlDataModBase::StripAllNotOnPartnerListL(): begin");
    TInt remotepropertycount = iRemoteStoreFormat->MimeFormat( iUsedRemoteMimeType ).PropertyCount();
    if( iRemoteStoreFormat->MimeFormat( iUsedRemoteMimeType ).PropertyCount() )
        {
        
        // Check correct Data Sync protocol
        TInt value( EDataSyncNotRunning );
        TSmlProtocolVersion usedSyncProtocol( ESmlVersion1_2 );
        TInt error = RProperty::Get( KPSUidDataSynchronizationInternalKeys,
                                     KDataSyncStatus,
                                     value );
        if ( error == KErrNone &&
             value == EDataSyncRunning )
            {
            usedSyncProtocol = ESmlVersion1_1_2;
            }

        TBool wasModified( EFalse );

        CArrayPtr<CParserProperty>* allProps = aEntity->ArrayOfProperties( EFalse );
        for( TInt i = 0; i < allProps->Count(); ) // Variable i is not increased here because size of count might be changes during loop
            {
            const CParserProperty& ownProperty = *allProps->At( i );
             
            TBool removeMe( ETrue );
            
            for( TInt i2 = 0; i2 < iRemoteStoreFormat->MimeFormat( iUsedRemoteMimeType ).PropertyCount(); i2++ )
                {
                const CSmlDataProperty& remoteProperty = iRemoteStoreFormat->MimeFormat( iUsedRemoteMimeType ).Property( i2 );
                TPtrC8 remotename = remoteProperty.Field().Name().DesC();
                if( !ownProperty.Name().Compare( remoteProperty.Field().Name().DesC() ) )
                    {
                    TInt remoteparamcount = remoteProperty.ParamCount();
                    if( remoteProperty.ParamCount() > 0 && aParamLevelCheck )
                        {
                        if ( usedSyncProtocol == ESmlVersion1_1_2 )
                            {
                            const CParserProperty* p = allProps->At( i );
                            TInt entityParamCount = (( CNSmlProperty* )p)->ParamCount();
                            if( entityParamCount > 0)
                            {
                            for( TInt i3 = 0; i3 < remoteProperty.ParamCount(); i3++ )
                                {
                                TPtrC8 remoteparamname = remoteProperty.Param( i3 ).Field().Name().DesC();
                                if( ownProperty.Param( remoteProperty.Param( i3 ).Field().Name().DesC() ) )
                                    {
                                    removeMe = EFalse;
                                    }
                                }                           
                            }
                            else
                            {
                                removeMe = EFalse;
                            }
                            }
                        else // ESmlVersion1_2
                            {
                            CArrayPtr<CParserParam>* ownerparamarray = ownProperty.ParamArray();
                            if(ownerparamarray != NULL)
                                {
                                for(TInt ownerparam = 0; ownerparam < ownerparamarray->Count(); ownerparam++)
                                    {
									removeMe = ETrue;
                                    const CParserParam& ownParam = *ownerparamarray->At( ownerparam );
                                     TPtrC8 ownparamname = ownParam.Name();
                                     TPtrC8 ownparamvalue = ownParam.Value();
                                     if(ownparamvalue == _L8(""))
                                        {
                                         for(TInt remoteparam = 0; remoteparam < remoteProperty.ParamCount(); remoteparam++)
                                            {
                                            TDesC8 remoteparamname = remoteProperty.Param( remoteparam ).Field().Name().DesC();
                                            const CSmlDataField& field = remoteProperty.Param( remoteparam ).Field();
											if( field.EnumValueCount() > 0)
												{
												for( TInt rmtenumcount = 0; rmtenumcount < field.EnumValueCount(); rmtenumcount++ )
													{
													TPtrC8 rmtenumvalue = field.EnumValue( rmtenumcount ).DesC();
													if( rmtenumvalue.Compare(ownparamname)== 0 )
														{
														removeMe = EFalse;
														break;
														}
													} 
												}
											else
												{
												removeMe = EFalse;
												break;
												}											
                                            }
                                        }
                                     else
                                        {
										//Handling when the device supports VersitTokenType as "Encoding"    
										if(ownparamname == KVersitTokenENCODING)
											{
											removeMe = EFalse;
											}
										else
											{
	                                         for(TInt remoteparam = 0; remoteparam < remoteProperty.ParamCount(); remoteparam++)
	                                            {
	                                            TDesC8 remoteparamname = remoteProperty.Param( remoteparam ).Field().Name().DesC();
	                                            if(ownparamname.Compare(remoteProperty.Param( remoteparam ).Field().Name().DesC()) == 0)
	                                                {
	                                                const CSmlDataField& field = remoteProperty.Param( remoteparam ).Field();
													if( field.EnumValueCount() > 0)
														{
		                                                for( TInt rmtenumcount = 0; rmtenumcount < field.EnumValueCount(); rmtenumcount++ )
		                                                    {
		                                                     TPtrC8 rmtenumvalue = field.EnumValue( rmtenumcount ).DesC();
		                                                     if( rmtenumvalue.Compare(ownparamvalue)== 0 )
		                                                         {
		                                                         removeMe = EFalse;
																 break;
		                                                         }  
		                                                    }
														}
													else
														{
														removeMe = EFalse;
														break;
														}
	                                                }
												}
											}                                 
                                        }
                                        if( removeMe )
                                        {
                                        	break;
                                        }	
                                    } 
                                }
                            else
                                {
                                removeMe = EFalse;
                                }
                            }
                        }
                    else
                        {
                        removeMe = EFalse;
                        }

                    if( !removeMe )
                        {
                        break;
                        }
                    }
                }
            if( removeMe )
                {
                #ifdef __NSML_DEBUG__
                    TPtrC8 pn( ownProperty.Name() );
                    DBG_ARGS8(_S8("CNSmlDataModBase::StripAllNotOnPartnerListL(): Dropping %S"), &pn);
                #endif // __NSML_DEBUG__
                delete allProps->At( i );
                allProps->Delete( i );
                wasModified = ETrue;
                aModified = ETrue;
                }
            else
                {
                #ifdef __NSML_DEBUG__
                    TPtrC8 pn( ownProperty.Name() );
                    DBG_ARGS8(_S8("CNSmlDataModBase::StripAllNotOnPartnerListL(): NOT dropping %S"), &pn);
                #endif // __NSML_DEBUG__
                i++;
                }
            }
        // can't use aModified as that may have been set earlier!
        if( wasModified )
            {
            allProps->Compress();
            }
        }
    _DBG_FILE("CNSmlDataModBase::StripAllNotOnPartnerListL(): end");
    }

// ------------------------------------------------------------------------------------------------
// CNSmlDataModBase::MergeL
// Merges data from old item to new item.
// ------------------------------------------------------------------------------------------------
void CNSmlDataModBase::MergeL( TDes8& aNewItem, const TDesC8& aOldItem,TBool aFieldLevel )
	{
	TBool modified( EFalse );
	CVersitParser* newItemParser = ChildCreateParserLC();
	RDesReadStream newStream( aNewItem );
	CleanupClosePushL( newStream );
	newItemParser->InternalizeL( newStream );
	CVersitParser* oldItemParser = ChildCreateParserLC();
	RDesReadStream oldStream( aOldItem );
	CleanupClosePushL( oldStream );
	oldItemParser->InternalizeL( oldStream );

	// Now we're ready to start analysis
	CArrayPtr<CVersitParser>* newEntities = newItemParser->ArrayOfEntities( EFalse );
	CArrayPtr<CVersitParser>* oldEntities = oldItemParser->ArrayOfEntities( ETrue );

	if( newEntities && oldEntities )
		{
		CleanupPtrArrayPushL( oldEntities );
		for( TInt i = 0; ( i < newEntities->Count() ) && ( i < oldEntities->Count() ); i++ )
			{
			MergeEntityL( newEntities->At( i ), oldEntities->At( i ), modified, aFieldLevel );
			}
		CleanupStack::PopAndDestroy(); // oldEntities
		}
	else
		{
		MergeEntityL( newItemParser, oldItemParser, modified, aFieldLevel );
		}

	// Only update if anything was modified in process
	if ( modified )
		{
		aNewItem.Zero();
		RDesWriteStream dws( aNewItem );
		CleanupClosePushL( dws );
		newItemParser->ExternalizeL( dws );
		dws.CommitL();
		CleanupStack::PopAndDestroy(); // dws
		}

	CleanupStack::PopAndDestroy( 4 ); // oldStream, oldItemParser, newStream, newItemParser
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDataModBase::MergeEntityL
// Merges data from old entity to new entity.
// ------------------------------------------------------------------------------------------------
void CNSmlDataModBase::MergeEntityL( CVersitParser* aNewEntity, CVersitParser* aOldEntity, TBool& aModified, TBool aFieldLevel ) const
	{
	_DBG_FILE("CNSmlDataModBase::MergeEntityL(): begin");

	// Remove all data that was not supposed to be supported by the partner but
	// it was still sent to us.
	StripAllNotOnPartnerListL( aNewEntity, aModified );
	
	if( !aFieldLevel )
		{
		// Remove all properties from old item that are supported by remote server.
		// If it is field level then old this is not done.
		StripAllOnPartnerListL( aOldEntity, aModified, ETrue );
		CArrayPtr<CParserProperty>* mergeProps = aOldEntity->ArrayOfProperties( ETrue );
		if( mergeProps )
		    {
			CleanupStack::PushL( mergeProps );
			
			for( TInt i = 0; i < mergeProps->Count(); i++ )
				{
				aNewEntity->AddPropertyL( mergeProps->At( i ), ETrue );
				}
			
			CleanupStack::PopAndDestroy(); // mergeProps
		    }
		}
    else // Support for Field level merge
        {
        //Field level merge. Merge new item with old item. Properties of 
        //the old item are copied to new item if the new item entity does not 
        //contain certain property.
        //------------------------------------------------------------------------
        // Old                 New                          Merged                 
        //------------------------------------------------------------------------
        // BEGIN:VCARD       -> BEGIN:VCARD                 = BEGIN:VCARD 
        // VERSION:2.1       -> VERSION:2.1                 = VERSION:2.1
        // N:Smith;John      -> N:White;John                = N:White;John
        // ORG:Firm                                         = ORG:Firm
        // TITLE:Boss                                       = TITLE:Boss
        //                   -> TEL;CELL;VOICE:1234         = TEL;CELL;VOICE:1234
        // END:VCARD         -> END:VCARD                   = END:VCARD

        CArrayPtr<CParserProperty>* newProps = aNewEntity->ArrayOfProperties( EFalse );
        if( newProps )
            {
            CArrayPtr<CParserProperty>* oldProps = aOldEntity->ArrayOfProperties( EFalse );

            // Iterate through old list of properties. Add missing properties from old 
            // contact item, if some of the properties is not included in new item. 
            for( TInt i = 0; i < oldProps->Count(); ) 
                {
                CParserProperty* oldProperty = oldProps->At( i );
                
                //Check if the property is included in received vCard
                CArrayPtr<CParserProperty>* properties = aNewEntity->PropertyL( 
                    oldProperty->Name(), oldProperty->Uid(), EFalse );

                if ( !properties )
                    {
                    // New vCard does not include certain property. Copy all matching properties from 
                    // existing contact item.
                    CArrayPtr<CParserProperty>* oldProperties =
                        aOldEntity->PropertyL( oldProperty->Name(), oldProperty->Uid(), ETrue );
                    CleanupPtrArrayPushL( oldProperties );
                    
                    for ( TInt j = oldProperties->Count()-1; j >= 0; --j )
                        {
                        CParserProperty* property = oldProperties->At( j );
                        oldProperties->Delete( j );
                        CleanupStack::PushL( property );
                        aNewEntity->AddPropertyL( property, EFalse );
                        CleanupStack::Pop( property );
                        aModified = ETrue;
                        }       
                    CleanupStack::PopAndDestroy( oldProperties );
                    }
                else
                    {
                    // If new vCard includes at least one property with same name we will not copy 
                    // any any property with same name from existing contact item.
                    delete properties;
                     ++i;
                    }
                }
            }
		}
	
	#ifdef __NSML_DEBUG__
		CArrayPtr<CParserProperty>* props = aNewEntity->ArrayOfProperties( EFalse );
		for( TInt i = 0; i < props->Count(); i++ )
			{
			TBuf8<512> b;
			const CParserProperty* p = props->At( i );
			b = p->Name();
			const CArrayPtr<CParserParam>* pa = ( ( CNSmlProperty* )p )->Parameters();
			if( pa )
				{
				for( TInt i2 = 0; i2 < pa->Count(); i2++ )
					{
					b.Append( _L8(":") );
					b.Append( pa->At( i2 )->Name() );
					}
				}
			DBG_ARGS8(_S8("CNSmlDataModBase::MergeEntityL(): %S"), &b);
			}
	#endif // __NSML_DEBUG__
	_DBG_FILE("CNSmlDataModBase::MergeEntityL(): end");
	}

// End of file
