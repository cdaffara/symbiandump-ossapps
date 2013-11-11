#
# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
#
# Initial Contributors:
# Nokia Corporation - initial contribution.
#
# Contributors:
#
# Description:
#

TEMPLATE = subdirs

SUBDIRS = cxeunitrunner \
          unittest_cxestatemachine \
          unittest_cxestate \
          unittest_cxevideocapturecontrolsymbian \
          unittest_cxefilenamegeneratorsymbian \
          unittest_cxecameradevicecontrolsymbian \
          unittest_cxeautofocuscontrolsymbian \
          unittest_cxesettmappersymbian \
          unittest_cxeviewfindercontrolsymbian \
          unittest_cxetestutils \
          unittest_cxecameradevice \
          unittest_cxeimagedataqueuesymbian \
          unittest_cxestillimagesymbian \
          unittest_cxeimagedataitemsymbian \
          unittest_cxeerrormappersymbian \
          unittest_cxefilesavethreadsymbian \
          unittest_cxesettingscenrepstore \
          unittest_cxestillcapturecontrolsymbian \
          unittest_cxezoomcontrolsymbian \
          unittest_cxefeaturemanagerimp \
          unittest_cxeenginesymbian \
          unittest_cxesettingsimp \
	  unittest_cxescenemodestore \
          unittest_cxequalitypresetssymbian \
          unittest_cxethumbnailmanagersymbian \
          unittest_cxeharvestercontrolsymbian \
          unittest_cxesettingscontrolsymbian \
          unittest_cxesnapshotcontrol \
          unittest_cxegeotaggingtrail \
          unittest_cxediskmonitor \
          unittest_cxememorymonitor

CONFIG *= ordered
