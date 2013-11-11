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
 * Description:  common header for contact history model.
 *
 */

#ifndef NMCONTACTHISTORYMODELCOMMONHEADERS_H_
#define NMCONTACTHISTORYMODELCOMMONHEADERS_H_

    enum NmContactHistoryModelType {
        EmailAddressModel,
        PhoneNumberModel
    };

#if defined(NMCHMAPI_LIBRARY)
#  define  NMCHMAPI_EXPORT Q_DECL_EXPORT
#else
#  if defined(NMCHMAPI_LIBRARY_NO_LIBRARY)
#    define NMCHMAPI_EXPORT
#  else
#    define NMCHMAPI_EXPORT Q_DECL_IMPORT
#  endif
#endif

#endif /* NMCONTACTHISTORYMODELCOMMONHEADERS_H_ */
