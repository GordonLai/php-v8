--TEST--
V8\Uint32Value
--SKIPIF--
<?php if (!extension_loaded("v8")) print "skip"; ?>
--FILE--
<?php

/** @var \Phpv8Testsuite $helper */
$helper = require '.testsuite.php';

require '.v8-helpers.php';
$v8_helper = new PhpV8Helpers($helper);

// Tests:


$isolate = new V8\Isolate();
$value = new V8\Uint32Value($isolate, 2147483647+1);

$helper->header('Object representation');
$helper->dump($value);
$helper->space();

$helper->assert('Uint32Value extends IntegerValue', $value instanceof \V8\IntegerValue);
$helper->line();

$helper->header('Accessors');
$helper->method_matches($value, 'getIsolate', $isolate);
$helper->method_export($value, 'value');
$helper->space();


$v8_helper->run_checks($value, 'Checkers for negative');

$global_template = new \V8\ObjectTemplate($isolate);
$context = new \V8\Context($isolate, $global_template);


$string = $value->toString($context);

$helper->header(get_class($value) .'::toString() converting');
$helper->dump($string);
$helper->dump($string->value());
$helper->space();



$helper->header('Primitive converters');
$helper->method_export($value, 'booleanValue', [$context]);
$helper->method_export($value, 'numberValue', [$context]);
$helper->space();


$helper->header('Uint32 is unsingned int32 value, so test for out-of-range (0-UINT32_MAX)');


foreach ([-1, PHP_INT_MAX, -PHP_INT_MAX, NAN, INF, -INF] as $val) {
  $helper->value_export($val);
  try {
    $value = new V8\Uint32Value($isolate, $val);
    $helper->method_export($value, 'value');
  } catch (Throwable $e) {
    $helper->exception_export($e);
  }
  $helper->space();
}


?>
--EXPECT--
Object representation:
----------------------
object(V8\Uint32Value)#4 (1) {
  ["isolate":"V8\Value":private]=>
  object(V8\Isolate)#3 (0) {
  }
}


Uint32Value extends IntegerValue: ok

Accessors:
----------
V8\Uint32Value::getIsolate() matches expected value
V8\Uint32Value->value(): int(2147483648)


Checkers for negative:
----------------------
V8\Uint32Value(V8\Value)->typeOf(): V8\StringValue->value(): string(6) "number"

V8\Uint32Value(V8\Value)->isUndefined(): bool(false)
V8\Uint32Value(V8\Value)->isNull(): bool(false)
V8\Uint32Value(V8\Value)->isNullOrUndefined(): bool(false)
V8\Uint32Value(V8\Value)->isTrue(): bool(false)
V8\Uint32Value(V8\Value)->isFalse(): bool(false)
V8\Uint32Value(V8\Value)->isName(): bool(false)
V8\Uint32Value(V8\Value)->isString(): bool(false)
V8\Uint32Value(V8\Value)->isSymbol(): bool(false)
V8\Uint32Value(V8\Value)->isFunction(): bool(false)
V8\Uint32Value(V8\Value)->isArray(): bool(false)
V8\Uint32Value(V8\Value)->isObject(): bool(false)
V8\Uint32Value(V8\Value)->isBoolean(): bool(false)
V8\Uint32Value(V8\Value)->isNumber(): bool(true)
V8\Uint32Value(V8\Value)->isInt32(): bool(false)
V8\Uint32Value(V8\Value)->isUint32(): bool(true)
V8\Uint32Value(V8\Value)->isDate(): bool(false)
V8\Uint32Value(V8\Value)->isArgumentsObject(): bool(false)
V8\Uint32Value(V8\Value)->isBooleanObject(): bool(false)
V8\Uint32Value(V8\Value)->isNumberObject(): bool(false)
V8\Uint32Value(V8\Value)->isStringObject(): bool(false)
V8\Uint32Value(V8\Value)->isSymbolObject(): bool(false)
V8\Uint32Value(V8\Value)->isNativeError(): bool(false)
V8\Uint32Value(V8\Value)->isRegExp(): bool(false)
V8\Uint32Value(V8\Value)->isAsyncFunction(): bool(false)
V8\Uint32Value(V8\Value)->isGeneratorFunction(): bool(false)
V8\Uint32Value(V8\Value)->isGeneratorObject(): bool(false)
V8\Uint32Value(V8\Value)->isPromise(): bool(false)
V8\Uint32Value(V8\Value)->isMap(): bool(false)
V8\Uint32Value(V8\Value)->isSet(): bool(false)
V8\Uint32Value(V8\Value)->isMapIterator(): bool(false)
V8\Uint32Value(V8\Value)->isSetIterator(): bool(false)
V8\Uint32Value(V8\Value)->isWeakMap(): bool(false)
V8\Uint32Value(V8\Value)->isWeakSet(): bool(false)
V8\Uint32Value(V8\Value)->isArrayBuffer(): bool(false)
V8\Uint32Value(V8\Value)->isArrayBufferView(): bool(false)
V8\Uint32Value(V8\Value)->isTypedArray(): bool(false)
V8\Uint32Value(V8\Value)->isUint8Array(): bool(false)
V8\Uint32Value(V8\Value)->isUint8ClampedArray(): bool(false)
V8\Uint32Value(V8\Value)->isInt8Array(): bool(false)
V8\Uint32Value(V8\Value)->isUint16Array(): bool(false)
V8\Uint32Value(V8\Value)->isInt16Array(): bool(false)
V8\Uint32Value(V8\Value)->isUint32Array(): bool(false)
V8\Uint32Value(V8\Value)->isInt32Array(): bool(false)
V8\Uint32Value(V8\Value)->isFloat32Array(): bool(false)
V8\Uint32Value(V8\Value)->isFloat64Array(): bool(false)
V8\Uint32Value(V8\Value)->isDataView(): bool(false)
V8\Uint32Value(V8\Value)->isSharedArrayBuffer(): bool(false)
V8\Uint32Value(V8\Value)->isProxy(): bool(false)


V8\Uint32Value::toString() converting:
--------------------------------------
object(V8\StringValue)#79 (1) {
  ["isolate":"V8\Value":private]=>
  object(V8\Isolate)#3 (0) {
  }
}
string(10) "2147483648"


Primitive converters:
---------------------
V8\Uint32Value(V8\Value)->booleanValue(): bool(true)
V8\Uint32Value(V8\Value)->numberValue(): float(2147483648)


Uint32 is unsingned int32 value, so test for out-of-range (0-UINT32_MAX):
-------------------------------------------------------------------------
integer: -1
V8\Exceptions\ValueException: Uint32 value to set is out of range


integer: 9223372036854775807
V8\Exceptions\ValueException: Uint32 value to set is out of range


integer: -9223372036854775807
V8\Exceptions\ValueException: Uint32 value to set is out of range


double: NAN
TypeError: Argument 2 passed to V8\Uint32Value::__construct() must be of the type integer, float given


double: INF
TypeError: Argument 2 passed to V8\Uint32Value::__construct() must be of the type integer, float given


double: -INF
TypeError: Argument 2 passed to V8\Uint32Value::__construct() must be of the type integer, float given
