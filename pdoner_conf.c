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
#include "main/php_scandir.h"
#include "php_pdoner.h"
#include "pdoner_conf.h"

zend_class_entry *pdoner_conf_ce;

zend_function_entry pdoner_conf_methods[] = {
	{NULL, NULL, NULL}
};

/*
static void pdoner_conf_hash_init(zval *zv, size_t size)
{
	HashTable *hTable;
	zend_hash_init(hTable, size, NULL, NULL, 1);
}
*/

PDONER_STARTUP_FUNCTION(conf)
{
	zend_class_entry ce;

	/* init class entry */
	INIT_CLASS_ENTRY(ce, "Pdconf", pdoner_conf_methods);

	/* register */
	pdoner_conf_ce = zend_register_internal_class(&ce TSRMLS_CC);

	/* flags */
	pdoner_conf_ce->ce_flags |= ZEND_ACC_FINAL_CLASS;

	const char *path;
	struct stat filestat, sb;
	struct dirent **namelist;
	int num, i;
	char *p, ini_file[MAXPATHLEN];
	zend_file_handle fh; // zend_parse_ini_file
	zval result;

	path = PDONER_G(directory);

	// path stat
	if ( path && (stat(path, &filestat) == 0) && S_ISDIR(filestat.st_mode) ) {

		// scandir
		if ( (num = php_scandir(path, &namelist, NULL, php_alphasort)) > 0 ) {

			for (i = 0; i < num; i++) {
				if ( (p = strrchr(namelist[i]->d_name, '.')) && strcmp(p, ".ini") ) {
					continue;
				}

				// file stat
				snprintf(ini_file, MAXPATHLEN, "%s%c%s", path, DEFAULT_SLASH, namelist[i]->d_name);

				if ( (stat(ini_file, &sb) == 0) && S_ISREG(sb.st_mode) ) {
					fh.handle.fp = fopen(ini_file, "r");
					fh.filename = ini_file;
					fh.type = ZEND_HANDLE_FP;


					HashTable *hTable;
					zend_hash_init(hTable, 128, NULL, NULL, 1);
					Z_ARRVAL(result, hTable);

					
				}
			}
		} else {
			zend_error(E_WARNING, "could not scandir %s !\n", path);
		}
	}

	return SUCCESS;
}

PDONER_RINIT_FUNCTION(conf)
{
	return SUCCESS;
}

PDONER_SHUTDOWN_FUNCTION(conf)
{
	return SUCCESS;
}
