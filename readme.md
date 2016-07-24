# pdoner

## Running

````shell
$ phpize
$ ./configure --with-php-config=/path/php/bin/php-config
$ make && sudo make install
````

## global const

| name | explain
|--- |---
| PD_ONE_MINUTE | 60
| PD_ONE_HOUR | 3600
| PD_BY_DAY | 3600*12
| PD_ONE_DAY | 3600*24

## Function

| name | explain
|--- |---
| `int pd_random_id([int $salt = 0])` | a random id based unix timestamp.
| `string pd_implode_json(array $arr [, string $glue = ','])` | array [1=>123,2=>456] to string [123,456] <br> glue can also be other character.

## Class

respond defined;  
APIs and usage:  
````php
$Rp = new Rp;
echo Rp::SUCC . "\n";  // 0
echo $Rp->SUCC . "\n"; // 成功
echo $Rp->get('SUCC') . "\n"; // 成功

if (! $Rp->has('SUCC')) {
	define('SUCC', 0);
	$Rp->SUCC = '成功';
}

$Rp->TEST = '测试';
$Rp->set('TEST2', '测试2');

print_r($Rp);
````

how to extend:  
````php
define('TEST', 4);
$Rp->TEST = '测试';
````

you can reference this:  
````php
class Err 
{
    const SUCC = 0;
    const FAIL = 1;  
    const EXCEP = 2;
    const UNKNOW = 3;

    public $msg = NULL;

    public function __construct() {
        $this->msg['SUCC'] = '成功';
        $this->msg['FAIL'] = '失败';
        $this->msg['EXCEP'] = '异常';
        $this->msg['UNKNOW'] = '未知';
    }   

	public function __get($name) {
		return $this->msg[$name];
	}

	public function has($name) {
		return $this->msg[$name] ?: false;
	}

	public function __set($name, $val) {
		$this->msg[$name] = $val;

		return true;
	}
}
````
