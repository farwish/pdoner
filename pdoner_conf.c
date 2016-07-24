/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2015 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Wei Chen <farwish@foxmail.com>                               |
  +----------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "php_pdoner.h"
#include "pdoner_conf.h"

zend_class_entry *pdoner_conf_ce;

zend_function_entry pdoner_conf_methods[] = {
	{NULL, NULL, NULL}
};

PDONER_STARTUP_FUNCTION(conf) {
	zend_class_entry ce;

	/* init class entry */
	INIT_CLASS_ENTRY(ce, "Pd_Conf", pdoner_conf_methods);

	/* register */
	pdoner_conf_ce = zend_register_internal_class(&ce TSRMLS_CC);

	/* flags */

	return SUCCESS;
}
