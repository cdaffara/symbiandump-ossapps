rem Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
rem All rights reserved.
rem This component and the accompanying materials are made available
rem under the terms of "Eclipse Public License v1.0"
rem which accompanies this distribution, and is available
rem at the URL "http://www.eclipse.org/legal/epl-v10.html".
rem
rem Initial Contributors:
rem Nokia Corporation - initial contribution.
rem
rem Contributors:
rem
rem Description:
rem

REM NOTE! Do not use tabs here!
md c:\MsgTest
md c:\MsgTest\smts
md c:\MsgTest\smts\checkagainst
md c:\MsgTest\smts\rfc822
md c:\MsgTest\smts\scripts

copy z:\System\MsgTest\msgtest\smts\checkagainst\imsk25.out c:\msgtest\smts\checkagainst\imsk25.out
attrib c:\msgtest\smts\checkagainst\*.* -r

copy z:\System\MsgTest\msgtest\smts\rfc822\RFC82201.TXT c:\msgtest\smts\rfc822\RFC82201.TXT
copy z:\System\MsgTest\msgtest\smts\rfc822\RFC82202.TXT c:\msgtest\smts\rfc822\RFC82202.TXT
copy z:\System\MsgTest\msgtest\smts\rfc822\RFC82203.TXT c:\msgtest\smts\rfc822\RFC82203.TXT
copy z:\System\MsgTest\msgtest\smts\rfc822\RFC82204.TXT c:\msgtest\smts\rfc822\RFC82204.TXT
copy z:\System\MsgTest\msgtest\smts\rfc822\RFC82205.TXT c:\msgtest\smts\rfc822\RFC82205.TXT
copy z:\System\MsgTest\msgtest\smts\rfc822\RFC82206.TXT c:\msgtest\smts\rfc822\RFC82206.TXT
copy z:\System\MsgTest\msgtest\smts\rfc822\RFC82207.TXT c:\msgtest\smts\rfc822\RFC82207.TXT
copy z:\System\MsgTest\msgtest\smts\rfc822\RFC82208.TXT c:\msgtest\smts\rfc822\RFC82208.TXT
copy z:\System\MsgTest\msgtest\smts\rfc822\RFC82209.TXT c:\msgtest\smts\rfc822\RFC82209.TXT
attrib c:\msgtest\smts\rfc822\*.* -r

copy z:\System\MsgTest\msgtest\smts\scripts\smtslog.scr c:\msgtest\smts\scripts\smtslog.scr
copy z:\System\MsgTest\msgtest\smts\scripts\t_smtsTlsNewSession.scr c:\msgtest\smts\scripts\t_smtsTlsNewSession.scr
copy z:\System\MsgTest\msgtest\smts\scripts\t_smtsTlsNoServices.scr c:\msgtest\smts\scripts\t_smtsTlsNoServices.scr
copy z:\System\MsgTest\msgtest\smts\scripts\t_smtsTlsNotSupported.scr c:\msgtest\smts\scripts\t_smtsTlsNotSupported.scr
copy z:\System\MsgTest\msgtest\smts\scripts\t_smtsTlsOk.scr c:\msgtest\smts\scripts\t_smtsTlsOk.scr
copy z:\System\MsgTest\msgtest\smts\scripts\t_smtsTlsRefused.scr c:\msgtest\smts\scripts\t_smtsTlsRefused.scr
copy z:\System\MsgTest\msgtest\smts\scripts\t_smtsTlsStopResponding.scr c:\msgtest\smts\scripts\t_smtsTlsStopResponding.scr
copy z:\System\MsgTest\msgtest\smts\scripts\t_smtsTlsSyntaxError.scr c:\msgtest\smts\scripts\t_smtsTlsSyntaxError.scr
copy z:\System\MsgTest\msgtest\smts\scripts\t_smtsTlsTempFail.scr c:\msgtest\smts\scripts\t_smtsTlsTempFail.scr
copy z:\System\MsgTest\msgtest\smts\scripts\imsk25_tls_no_smtp_auth_on_server.scr c:\msgtest\smts\scripts\imsk25_tls_no_smtp_auth_on_server.scr
copy z:\System\MsgTest\msgtest\smts\scripts\imsk25_tls_no_smtp_auth.scr c:\msgtest\smts\scripts\imsk25_tls_no_smtp_auth.scr
copy z:\System\MsgTest\msgtest\smts\scripts\imsk25_tls_login.scr c:\msgtest\smts\scripts\imsk25_tls_login.scr
copy z:\System\MsgTest\msgtest\smts\scripts\imsk25_no_tls_wrong_username.scr c:\msgtest\smts\scripts\imsk25_no_tls_wrong_username.scr
copy z:\System\MsgTest\msgtest\smts\scripts\imsk25_no_tls_wrong_password.scr c:\msgtest\smts\scripts\imsk25_no_tls_wrong_password.scr
copy z:\System\MsgTest\msgtest\smts\scripts\imsk25_no_tls_plain.scr c:\msgtest\smts\scripts\imsk25_no_tls_plain.scr
copy z:\System\MsgTest\msgtest\smts\scripts\imsk25_no_tls_no_smtp_auth_on_server.scr c:\msgtest\smts\scripts\imsk25_no_tls_no_smtp_auth_on_server.scr
copy z:\System\MsgTest\msgtest\smts\scripts\imsk25_no_tls_login.scr c:\msgtest\smts\scripts\imsk25_no_tls_login.scr
copy z:\System\MsgTest\msgtest\smts\scripts\imsk25_no_tls_cram_md5.scr c:\msgtest\smts\scripts\imsk25_no_tls_cram_md5.scr
copy z:\System\MsgTest\msgtest\smts\scripts\imsk25_ehlo_fail_conitnue_with_helo.scr c:\msgtest\smts\scripts\imsk25_ehlo_fail_conitnue_with_helo.scr
copy z:\System\MsgTest\msgtest\smts\scripts\imsk25_ehlo_fail_quit_if_tls_on.scr c:\msgtest\smts\scripts\imsk25_ehlo_fail_quit_if_tls_on.scr
attrib c:\msgtest\smts\scripts\*.* -r





