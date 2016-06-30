# pdoner

## Running

````shell
$ phpize
$ ./configure --with-php-config=/path/php/bin/php-config
$ make && sudo make install
````

## Functions
A random id based unix timestamp.

 * `int random_id([int salt = 0])`

## Class
Error define

```
class Errs
{
    const SUCC = 0;
    const FAIL = -1; 
    const EXCEP = 1;
    const UNKNOW = 2;

    public static $msg = [ 
        self::SUCC => '成功',
        self::FAIL => '失败',
        self::EXCEP => '异常',
        self::UNKNOW => '未知',
    ];  

    public static function get($name)
    {   
        return self::$msg[$name];
    }   

    public static function set($name, $value)
    {   
        self::$msg[$name] = $value;
    }   
}
```
