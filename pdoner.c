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
  | Author:                                                              |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_pdoner.h"

#include <time.h>

static int le_pdoner;


/* {{{ proto int random_id([int salt = 0])
   a random id based unix timestamp */
PHP_FUNCTION(random_id)
{
	time_t t;
	long *salt = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|l", &salt) == FAILURE) {
		return;
	}

	t = (long)time(NULL) + (long)salt;

	RETURN_LONG(t);
}
/* }}} */


PHP_MINIT_FUNCTION(pdoner)
{
	return SUCCESS;
}


PHP_MSHUTDOWN_FUNCTION(pdoner)
{
	return SUCCESS;
}



PHP_RINIT_FUNCTION(pdoner)
{
	return SUCCESS;
}



PHP_RSHUTDOWN_FUNCTION(pdoner)
{
	return SUCCESS;
}


PHP_MINFO_FUNCTION(pdoner)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "pdoner support", "enabled");
	php_info_print_table_row(2, "pdoner version", "beta");
	php_info_print_table_end();

}


const zend_function_entry pdoner_functions[] = {
	PHP_FE(random_id, NULL)
	PHP_FE_END
};


zend_module_entry pdoner_module_entry = {
	STANDARD_MODULE_HEADER,
	"pdoner",
	pdoner_functions,
	PHP_MINIT(pdoner),
	PHP_MSHUTDOWN(pdoner),
	PHP_RINIT(pdoner),	
	PHP_RSHUTDOWN(pdoner),
	PHP_MINFO(pdoner),
	PHP_PDONER_VERSION,
	STANDARD_MODULE_PROPERTIES
};


#ifdef COMPILE_DL_PDONER
ZEND_GET_MODULE(pdoner)
#endif

