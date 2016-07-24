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

/* $Id$ */

#ifndef PHP_PDONER_H
#define PHP_PDONER_H

/* {{{ global const */
#define PD_ONE_MINUTE 60
#define PD_ONE_HOUR 3600
#define PD_BY_DAY (3600 * 12)
#define PD_ONE_DAY (3600 * 24)
/* }}} */

/* {{{ Rp const */
#define PDONER_RP_CONSTANT_NAME_SUCC "SUCC"
#define PDONER_RP_CONSTANT_NAME_FAIL "FAIL"
#define PDONER_RP_CONSTANT_NAME_EXCEP "EXCEP"
#define PDONER_RP_CONSTANT_NAME_UNKNOW "UNKNOW"

#define PDONER_RP_SUCC 0
#define PDONER_RP_FAIL 1
#define PDONER_RP_EXCEP 2
#define PDONER_RP_UNKNOW 3

#define PDONER_RP_PROPERTY_NAME_MSG "msg"
/* }}} */

extern zend_module_entry pdoner_module_entry;
#define phpext_pdoner_ptr &pdoner_module_entry

#define PHP_PDONER_VERSION "0.1.1"

PHP_MINIT_FUNCTION(pdoner);
PHP_RINIT_FUNCTION(pdoner);
PHP_RSHUTDOWN_FUNCTION(pdoner);
PHP_MSHUTDOWN_FUNCTION(pdoner);
PHP_MINFO_FUNCTION(pdoner);

/* {{{ custom startup funcs */
#define PDONER_STARTUP_FUNCTION(module)    ZEND_MINIT_FUNCTION(pdoner_##module)
#define PDONER_STARTUP(module)             ZEND_MODULE_STARTUP_N(pdoner_##module)(INIT_FUNC_ARGS_PASSTHRU)
#define PDONER_SHUTDOWN_FUNCTION(module)   ZEND_MSHUTDOWN_FUNCTION(pdoner_##module)
#define PDONER_SHUTDOWN(module)            ZEND_MODULE_SHUTDOWN_N(pdoner_##module)(SHUTDOWN_FUNC_ARGS_PASSTHRU)
/* }}} */

#ifdef PHP_WIN32
#	define PHP_PDONER_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#	define PHP_PDONER_API __attribute__ ((visibility("default")))
#else
#	define PHP_PDONER_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

#ifdef ZTS
#define PDONER_G(v) TSRMG(pdoner_globals_id, zend_pdoner_globals *, v)
#else
#define PDONER_G(v) (pdoner_globals.v)
#endif

#endif


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
