#!/usr/bin/php
<?php

include('/m23/inc/db.php');
include('/m23/inc/vm.php');
include('/m23/inc/helper.php');
include("/m23/inc/i18n.php");
include('/m23/inc/client.php');
include('/m23/inc/checks.php');
include('/m23/inc/autoTest.php');
include('/m23/inc/CAutoTest.php');
include('/m23/inc/CMessageManager.php');
include('/m23/inc/CChecks.php');
include('/m23/inc/CSystemProxy.php');
include('/m23/inc/server.php');

// define('AT_OVA_EXPORT_DIR', '/crypto/iso');
define('TEST_VBOX_HOST', 'tuxedo');
define('TEST_VBOX_USER', 'dodger');

$r=AUTOTEST_VM_screenPixelDiff('seleniumvm2018');

print("\nMSG: $r\n");



?>