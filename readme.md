# pdoner

## Running

````shell
$ phpize
$ ./configure --with-php-config=/path/php/bin/php-config
$ make && sudo make install
````

## Function

| name | explain
|--- |---
| `int pd_random_id([int $salt = 0])` | a random id based unix timestamp.
| `string pd_implode_json(array $arr [, string $glue = ','])` | array [1=>123,2=>456] to string [123,456] <br> glue can also be other character.

## Class

Error define; usage:
````php
$Errs = new Errs;  
echo Errs::get(Errs::FAIL) . "\n";  
Errs::set(6, '测试');  
var_dump(Errs::get());  
````
reference:
````php
class Err 
{
    const SUCC = 0;
    const FAIL = 1;  
    const EXCEP = 2;
    const UNKNOW = 3;

    public static $msg = NULL;

    public function __construct() {
        self::$msg[self::SUCC] = '成功';
        self::$msg[self::FAIL] = '失败';
        self::$msg[self::EXCEP] = '异常';
        self::$msg[self::UNKNOW] = '未知';
    }   

    public static function get($code = "") 
    {   
		if (! self::$msg) {  
			trigger_error("please new Errs instance before use, for property will initialized in construct!\n", E_USER_WARNING);  
			return NULL;  
		}  

        return $code ? self::$msg[$code] : self::$msg;  
    }   

    public static function set($code, $value)
    {   
		if (! self::$msg) {  
			trigger_error("please new Errs instance before use, for property will initialized in construct!\n", E_USER_WARNING);  
			return false;  
		}

        if (isset(self::$msg[$code])) {  
			trigger_error("the code has exists in using " . __CLASS__ . "::" . __FUNCTION__ . "()!\n" , E_USER_WARNING);  
		} else if (self::$msg[$code] = $value) {  
			return true;  
		}  

        return false;  
    }   
}
````
