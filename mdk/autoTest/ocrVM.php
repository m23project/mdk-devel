#!/usr/bin/php
<?php

include('/m23/inc/db.php');
include('/m23/inc/vm.php');
include('/m23/inc/helper.php');
include('/m23/inc/client.php');
include('/m23/inc/checks.php');
include('/m23/inc/autoTest.php');
include('/m23/inc/CAutoTest.php');

define('TEST_VBOX_HOST', 'lc');
define('TEST_VBOX_USER', 'dodger');
define('TEST_VBOX_NETDEV', 'eth0');
define('TEST_VBOX_IMAGE_DIR', '/home/'.TEST_VBOX_USER);


print(AUTOTEST_VM_ocrScreen($argv[1]));

?>