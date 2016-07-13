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

#define PDONER_ERRS_CONSTANT_NAME_SUCC "SUCC"
#define PDONER_ERRS_CONSTANT_NAME_FAIL "FAIL"
#define PDONER_ERRS_CONSTANT_NAME_EXCEP "EXCEP"
#define PDONER_ERRS_CONSTANT_NAME_UNKNOW "UNKNOW"

#define PDONER_ERRS_SUCC 0
#define PDONER_ERRS_FAIL 1
#define PDONER_ERRS_EXCEP 2
#define PDONER_ERRS_UNKNOW 3

#define PDONER_ERRS_PROPERTY_NAME_MSG "msg"

extern zend_module_entry pdoner_module_entry;
#define phpext_pdoner_ptr &pdoner_module_entry

#define PHP_PDONER_VERSION "0.1.0"

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
