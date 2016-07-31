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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"

#include "php_pdoner.h"
#include "pdoner_conf.h"

ZEND_DECLARE_MODULE_GLOBALS(pdoner);

static int le_pdoner;


zend_class_entry *pdoner_rp_ce;

/* {{{ ARG_INFO
*/
ZEND_BEGIN_ARG_INFO_EX(pdoner_rp_get_arginfo, 0, 0, 1)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(pdoner_rp_has_arginfo, 0, 0, 1)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(pdoner_rp_set_arginfo, 0, 0, 2)
	ZEND_ARG_INFO(0, name)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()
/* }}} */

/* {{{ proto int pd_random_id([int $salt = 0])
   a random id based unix timestamp */
PHP_FUNCTION(pd_random_id)
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

/* {{{ proto string pd_implode_json(array $arr [, string $glue = ','])
* TODO malloc and strcat optimize.
*/
PHP_FUNCTION(pd_implode_json)
{
	zval *pieces, *glue = NULL;
	zval function_name, *params[2] = {0};
	zval *retval = NULL;

	if ( zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|z", &pieces, &glue) == FAILURE ) {
		RETURN_FALSE;
	}

	ZVAL_STRING(&function_name, "implode", 0);

	params[0] = pieces;

	if (glue == NULL) {
		// ZVAL_STRING() used when glue was a zval, not zval *	\
			so under line will cause segmentfalut.
		// ZVAL_STRING(glue, ",", 1);

		MAKE_STD_ZVAL(glue);
		Z_STRVAL_P(glue) = ",";
		Z_STRLEN_P(glue) = strlen(",");
		Z_TYPE_P(glue) = IS_STRING;
	}

	params[1] = glue;

	if ( call_user_function(EG(function_table), NULL, &function_name, return_value, 2, params TSRMLS_CC) == FAILURE ) {
		if (return_value) {
			zval_dtor(return_value);
		}
		RETURN_FALSE;
	}

	/*
	// this way will segmentfalut when pass the second param.
	if (glue != NULL) {
		ZVAL_NULL(glue);
		zval_ptr_dtor(&glue);
	}
	*/

	// when passed the second param glue, if you forget dtor, segmentfault.
	if (ZEND_NUM_ARGS() == 1) {
		ZVAL_NULL(glue);
		zval_ptr_dtor(&glue);
	}
	
	zval op1, op2;
	ZVAL_STRINGL(&op1, "[", 1, 1);
	ZVAL_STRINGL(&op2, "]", 1, 1);

	/*
	// concat; way1
	zval result;

	concat_function(&result, &op1, return_value TSRMLS_CC);
	concat_function(&result, &result, &op2 TSRMLS_CC);

 	// 1. zval_ptr_dtor(&return_value) was wrong.
	// 2. forget zval_dtor(return_value) will cause memory leaks.
	zval_dtor(return_value);

	// copy result to return_value;
	// if "zval result" is not zero-terminated, use ZVAL_ZVAL() instead.	\
		PHP Warning:  String is not zero-terminated.
	ZVAL_COPY_VALUE(return_value, &result);
	*/
	
	// concat; way2
	concat_function(&op1, &op1, return_value TSRMLS_CC);
	concat_function(&op1, &op1, &op2 TSRMLS_CC);
	zval_dtor(return_value);
	ZVAL_ZVAL(return_value, &op1, 0, 1);

	zval_dtor(&op1);
	zval_dtor(&op2);
}
/* }}} */

/* {{{ proto public Rp::__construct(void) 
*/
PHP_METHOD(rp, __construct)
{
	zval *msg, *pThis = getThis();
	MAKE_STD_ZVAL(msg);	
	array_init(msg);

	add_assoc_string_ex(msg, PDONER_RP_CONSTANT_NAME_SUCC, sizeof(PDONER_RP_CONSTANT_NAME_SUCC) + 1, "成功", 1);
	add_assoc_string_ex(msg, PDONER_RP_CONSTANT_NAME_FAIL, sizeof(PDONER_RP_CONSTANT_NAME_FAIL) + 1, "失败", 1);
	add_assoc_string_ex(msg, PDONER_RP_CONSTANT_NAME_EXCEP, sizeof(PDONER_RP_CONSTANT_NAME_EXCEP) + 1, "异常", 1);
	add_assoc_string_ex(msg, PDONER_RP_CONSTANT_NAME_UNKNOW, sizeof(PDONER_RP_CONSTANT_NAME_UNKNOW) + 1, "未知", 1);

	zend_update_property(pdoner_rp_ce, pThis, ZEND_STRL(PDONER_RP_PROPERTY_NAME_MSG), msg TSRMLS_CC);

	// if not declare msg in MINIT, you can add property like this:
	//add_property_zval_ex(getThis(), ZEND_STRL(PDONER_RP_PROPERTY_NAME_MSG), msg TSRMLS_CC);

	zval_ptr_dtor(&msg);
}
/* }}} */

/* {{{ proto void public Rp::get($name) 
*/
PHP_METHOD(rp, get)
{
	zval *name, *msg, *pThis = getThis();

	if ( zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &name) == FAILURE ) {
		return;
	}

	msg = zend_read_property(pdoner_rp_ce, pThis, ZEND_STRL(PDONER_RP_PROPERTY_NAME_MSG), 1 TSRMLS_CC);
	
	HashTable *hTable = Z_ARRVAL_P(msg);
	zval **pData;

	char *string;
	ulong index;

	for (zend_hash_internal_pointer_reset(hTable);
		zend_hash_has_more_elements(hTable) == SUCCESS;
		zend_hash_move_forward(hTable)
	) {
		switch (zend_hash_get_current_key(hTable, &string, &index, 0)) {
			case HASH_KEY_IS_LONG:
				break;
			case HASH_KEY_IS_STRING:
				if (strcasecmp(string, Z_STRVAL_P(name)) == 0) {
					if (zend_hash_get_current_data(hTable, (void **) &pData) == SUCCESS) {
						RETURN_STRING(Z_STRVAL_PP(pData), 1);
					}
				}
				break;
			default:
				RETURN_NULL();
		}
	}

	RETURN_NULL();
}
/* }}} */

/* {{{ proto mixed public Rp::has(string $name)
*/
PHP_METHOD(rp, has)
{
	char *name;
	uint len;

	if ( zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &name, &len) == FAILURE ) {
		return;
	}

	zval *msg = zend_read_property(pdoner_rp_ce, getThis(), ZEND_STRL(PDONER_RP_PROPERTY_NAME_MSG), 1 TSRMLS_CC);

	// below, can not get the correct results, dont know why!?
	// RETURN_LONG(zend_hash_exists(Z_ARRVAL_P(msg), name, len + 1));

	char *string;
	ulong index;

	HashTable *hTable = Z_ARRVAL_P(msg);
	for (zend_hash_internal_pointer_reset(hTable);
		zend_hash_has_more_elements(hTable) == SUCCESS;
		zend_hash_move_forward(hTable)
	) {
		switch (zend_hash_get_current_key(hTable, &string, &index, 0)) {
			case HASH_KEY_IS_LONG:
				RETURN_LONG(index);
				break;
			case HASH_KEY_IS_STRING:
				if (strcasecmp(name, string) == 0) {
					RETURN_STRING(string, 1);
				}
				break;
			default:
				RETURN_FALSE;
		}
	}
}
/* }}} */

/* {{{ proto bool public Rp::set(string $name, string $value) 
*/
PHP_METHOD(rp, set)
{
	char *name;
	uint name_len;
	char *value;
	uint value_len;

	if ( zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &name, &name_len, &value, &value_len) == FAILURE ) {
		return;
	}

	zval *msg = zend_read_property(pdoner_rp_ce, getThis(), ZEND_STRL(PDONER_RP_PROPERTY_NAME_MSG), 1 TSRMLS_CC);

	// check exists

	// add value
	add_assoc_stringl_ex(msg, name, name_len + 1, value, value_len, 1);

	// update property
	zend_update_property(pdoner_rp_ce, getThis(), ZEND_STRL(PDONER_RP_PROPERTY_NAME_MSG), msg TSRMLS_CC);

	RETURN_TRUE;
}
/* }}} */

/* {{{ Rp_methods 
*/
zend_function_entry rp_methods[] = {
	PHP_ME(rp, __construct, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
	PHP_ME(rp, get, pdoner_rp_get_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(rp, has, pdoner_rp_has_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(rp, set, pdoner_rp_set_arginfo, ZEND_ACC_PUBLIC)
	PHP_MALIAS(rp, __get, get, pdoner_rp_get_arginfo, ZEND_ACC_PUBLIC)
	PHP_MALIAS(rp, __set, set, pdoner_rp_set_arginfo, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};
/* }}} */

/* {{{ PHP_INI
*/
PHP_INI_BEGIN()
	STD_PHP_INI_ENTRY("pdoner.directory", "", PHP_INI_SYSTEM, OnUpdateString, directory, zend_pdoner_globals, pdoner_globals)
PHP_INI_END()
/* }}} */

/* {{{ PHP_GINIT_FUNCTION
*/
PHP_GINIT_FUNCTION(pdoner)
{
	pdoner_globals->directory = NULL;
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
*/
PHP_MINIT_FUNCTION(pdoner)
{
	REGISTER_INI_ENTRIES();

	zend_class_entry ce;
	INIT_CLASS_ENTRY(ce, "rp", rp_methods);	
	pdoner_rp_ce = zend_register_internal_class(&ce TSRMLS_CC);

	zend_declare_class_constant_long(pdoner_rp_ce, ZEND_STRL(PDONER_RP_CONSTANT_NAME_SUCC), PDONER_RP_SUCC TSRMLS_CC);
	zend_declare_class_constant_long(pdoner_rp_ce, ZEND_STRL(PDONER_RP_CONSTANT_NAME_FAIL), PDONER_RP_FAIL TSRMLS_CC);
	zend_declare_class_constant_long(pdoner_rp_ce, ZEND_STRL(PDONER_RP_CONSTANT_NAME_EXCEP), PDONER_RP_EXCEP TSRMLS_CC);
	zend_declare_class_constant_long(pdoner_rp_ce, ZEND_STRL(PDONER_RP_CONSTANT_NAME_UNKNOW), PDONER_RP_UNKNOW TSRMLS_CC);

	zend_declare_property_null(pdoner_rp_ce, ZEND_STRL(PDONER_RP_PROPERTY_NAME_MSG), ZEND_ACC_PUBLIC TSRMLS_CC);

	/* global const */
	REGISTER_LONG_CONSTANT("PD_ONE_MINUTE", PD_ONE_MINUTE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PD_ONE_HOUR", PD_ONE_HOUR, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PD_BY_DAY", PD_BY_DAY, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PD_ONE_DAY", PD_ONE_DAY, CONST_CS | CONST_PERSISTENT);

	/* startup component */
	PDONER_STARTUP(conf);

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
*/
PHP_MSHUTDOWN_FUNCTION(pdoner)
{
	UNREGISTER_INI_ENTRIES();
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_RINIT_FUNCTION
*/
PHP_RINIT_FUNCTION(pdoner)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_RSHUTDOWN_FUNCTION
*/
PHP_RSHUTDOWN_FUNCTION(pdoner)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
*/
PHP_MINFO_FUNCTION(pdoner)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "pdoner support", "enabled");
	php_info_print_table_row(2, "pdoner version", PHP_PDONER_VERSION);
	php_info_print_table_end();

	DISPLAY_INI_ENTRIES();
}
/* }}} */

/* {{{ pdoner_functions
*/
const zend_function_entry pdoner_functions[] = {
	PHP_FE(pd_random_id, NULL)
	PHP_FE(pd_implode_json, NULL)
	PHP_FE_END
};
/* }}} */

/* {{{ pdoner_module_entry
*/
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
	PHP_MODULE_GLOBALS(pdoner),
	PHP_GINIT(pdoner),
	NULL,
	NULL,
	STANDARD_MODULE_PROPERTIES_EX
};
/* }}} */

#ifdef COMPILE_DL_PDONER
ZEND_GET_MODULE(pdoner)
#endif

