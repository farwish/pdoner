# pdoner

## Running

````shell
$ phpize
$ ./configure --with-php-config=/path/php/bin/php-config
$ make && sudo make install
````

## Function

 * `int pd_random_id([int $salt = 0])`	// A random id based unix timestamp.

 * `string pd_implode_json(array $arr [, string $glue = ','])`	// array [1=>123,2=>456] to string [123,456]

## Class
Error define.

```
class Errs
{
    const SUCC = 0;
    const FAIL = 1; 
    const EXCEP = 2;
    const UNKNOW = 3;

	public static $msg;

	pubic function __construct() {
		self::$msg[self::SUCC] = '成功';
		self::$msg[self::FAIL] = '失败';
		self::$msg[self::EXCEP] = '异常';
		self::$msg[self::UNKNOW] = '未知';
	}

    public static function get($code)
    {   
        return self::$msg[$code];
    }

    public static function set($code, $value)
    {   
        self::$msg[$code] = $value;
    }
}
```
