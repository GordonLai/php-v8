--TEST--
V8\TryCatch
--SKIPIF--
<?php if (!extension_loaded("v8")) print "skip"; ?>
--ENV--
HOME=/tmp/we-need-home-env-var-set-to-load-valgrindrc
--FILE--
<?php

require '.tracking_dtors.php';

/** @var \Phpv8Testsuite $helper */
$helper = require '.testsuite.php';


$isolate = new \v8Tests\TrackingDtors\Isolate();
$context = new v8Tests\TrackingDtors\Context($isolate);


$obj = new \V8\TryCatch($isolate, $context);

$helper->header('Object representation (default)');
$helper->dump($obj);
$helper->space();

$helper->header('Test getters (default)');
$helper->method_matches($obj, 'getIsolate', $isolate);
$helper->method_matches($obj, 'getContext', $context);
$helper->method_matches($obj, 'getException', null);
$helper->method_matches($obj, 'getMessage', null);
$helper->method_matches($obj, 'getStackTrace', null);

$helper->method_matches($obj, 'canContinue', false);
$helper->method_matches($obj, 'hasTerminated', false);
$helper->space();



$exception = new \V8\ObjectValue($context);
$message = new \V8\Message('message', 'line', new \V8\ScriptOrigin('resource_name'), 'resource_name', new \V8\StackTrace([]));
$trace = new \V8\StringValue($isolate, 'trace');

$obj = new \V8\TryCatch($isolate, $context, $exception, $trace, $message, true, true, $php_exception = new RuntimeException('test'));

$helper->header('Object representation');
$helper->dump($obj);
$helper->space();

$helper->header('Test getters');
$helper->method_matches($obj, 'getIsolate', $isolate);
$helper->method_matches($obj, 'getContext', $context);
$helper->method_matches($obj, 'getException', $exception);
$helper->method_matches($obj, 'getMessage', $message);
$helper->method_matches($obj, 'getStackTrace', $trace);

$helper->method_matches($obj, 'canContinue', true);
$helper->method_matches($obj, 'hasTerminated', true);

$helper->method_matches($obj, 'getExternalException', $php_exception);
$helper->space();


$exception = null;
$message = null;
$trace = null;

$obj = null;
$isolate = null;
$context = null;

echo 'END', PHP_EOL;
// EXPECTF: ---/string\(\d+\) ".+\/tests\/TryCatch.php"/
// EXPECTF: +++string(%d) "%s/tests/TryCatch.php"
?>
--EXPECTF--
Object representation (default):
--------------------------------
object(V8\TryCatch)#4 (8) {
  ["isolate":"V8\TryCatch":private]=>
  object(v8Tests\TrackingDtors\Isolate)#2 (0) {
  }
  ["context":"V8\TryCatch":private]=>
  object(v8Tests\TrackingDtors\Context)#3 (1) {
    ["isolate":"V8\Context":private]=>
    object(v8Tests\TrackingDtors\Isolate)#2 (0) {
    }
  }
  ["exception":"V8\TryCatch":private]=>
  NULL
  ["stack_trace":"V8\TryCatch":private]=>
  NULL
  ["message":"V8\TryCatch":private]=>
  NULL
  ["can_continue":"V8\TryCatch":private]=>
  bool(false)
  ["has_terminated":"V8\TryCatch":private]=>
  bool(false)
  ["external_exception":"V8\TryCatch":private]=>
  NULL
}


Test getters (default):
-----------------------
V8\TryCatch::getIsolate() matches expected value
V8\TryCatch::getContext() matches expected value
V8\TryCatch::getException() matches expected value
V8\TryCatch::getMessage() matches expected value
V8\TryCatch::getStackTrace() matches expected value
V8\TryCatch::canContinue() matches expected value
V8\TryCatch::hasTerminated() matches expected value


Object representation:
----------------------
object(V8\TryCatch)#11 (8) {
  ["isolate":"V8\TryCatch":private]=>
  object(v8Tests\TrackingDtors\Isolate)#2 (0) {
  }
  ["context":"V8\TryCatch":private]=>
  object(v8Tests\TrackingDtors\Context)#3 (1) {
    ["isolate":"V8\Context":private]=>
    object(v8Tests\TrackingDtors\Isolate)#2 (0) {
    }
  }
  ["exception":"V8\TryCatch":private]=>
  object(V8\ObjectValue)#5 (2) {
    ["isolate":"V8\Value":private]=>
    object(v8Tests\TrackingDtors\Isolate)#2 (0) {
    }
    ["context":"V8\ObjectValue":private]=>
    object(v8Tests\TrackingDtors\Context)#3 (1) {
      ["isolate":"V8\Context":private]=>
      object(v8Tests\TrackingDtors\Isolate)#2 (0) {
      }
    }
  }
  ["stack_trace":"V8\TryCatch":private]=>
  object(V8\StringValue)#10 (1) {
    ["isolate":"V8\Value":private]=>
    object(v8Tests\TrackingDtors\Isolate)#2 (0) {
    }
  }
  ["message":"V8\TryCatch":private]=>
  object(V8\Message)#6 (11) {
    ["message":"V8\Message":private]=>
    string(7) "message"
    ["script_origin":"V8\Message":private]=>
    object(V8\ScriptOrigin)#7 (6) {
      ["resource_name":"V8\ScriptOrigin":private]=>
      string(13) "resource_name"
      ["resource_line_offset":"V8\ScriptOrigin":private]=>
      NULL
      ["resource_column_offset":"V8\ScriptOrigin":private]=>
      NULL
      ["script_id":"V8\ScriptOrigin":private]=>
      NULL
      ["source_map_url":"V8\ScriptOrigin":private]=>
      string(0) ""
      ["options":"V8\ScriptOrigin":private]=>
      object(V8\ScriptOriginOptions)#8 (1) {
        ["flags":"V8\ScriptOriginOptions":private]=>
        int(0)
      }
    }
    ["source_line":"V8\Message":private]=>
    string(4) "line"
    ["resource_name":"V8\Message":private]=>
    string(13) "resource_name"
    ["stack_trace":"V8\Message":private]=>
    object(V8\StackTrace)#9 (1) {
      ["frames":"V8\StackTrace":private]=>
      array(0) {
      }
    }
    ["line_number":"V8\Message":private]=>
    NULL
    ["start_position":"V8\Message":private]=>
    NULL
    ["end_position":"V8\Message":private]=>
    NULL
    ["start_column":"V8\Message":private]=>
    NULL
    ["end_column":"V8\Message":private]=>
    NULL
    ["error_level":"V8\Message":private]=>
    NULL
  }
  ["can_continue":"V8\TryCatch":private]=>
  bool(true)
  ["has_terminated":"V8\TryCatch":private]=>
  bool(true)
  ["external_exception":"V8\TryCatch":private]=>
  object(RuntimeException)#12 (7) {
    ["message":protected]=>
    string(4) "test"
    ["string":"Exception":private]=>
    string(0) ""
    ["code":protected]=>
    int(0)
    ["file":protected]=>
    string(%d) "%s/tests/TryCatch.php"
    ["line":protected]=>
    int(36)
    ["trace":"Exception":private]=>
    array(0) {
    }
    ["previous":"Exception":private]=>
    NULL
  }
}


Test getters:
-------------
V8\TryCatch::getIsolate() matches expected value
V8\TryCatch::getContext() matches expected value
V8\TryCatch::getException() matches expected value
V8\TryCatch::getMessage() matches expected value
V8\TryCatch::getStackTrace() matches expected value
V8\TryCatch::canContinue() matches expected value
V8\TryCatch::hasTerminated() matches expected value
V8\TryCatch::getExternalException() matches expected value


Context dies now!
Isolate dies now!
END
