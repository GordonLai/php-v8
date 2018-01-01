--TEST--
V8\ExceptionManager::createGetStackTrace()
--SKIPIF--
<?php if (!extension_loaded("v8")) print "skip"; ?>
--FILE--
<?php
/** @var \Phpv8Testsuite $helper */
$helper = require '.testsuite.php';

require '.v8-helpers.php';
$v8_helper = new PhpV8Helpers($helper);
$isolate = new \V8\Isolate();
$context = new \V8\Context($isolate);


try {
    $stack_trace = V8\ExceptionManager::getStackTrace($context, new \V8\StringValue($isolate, 'test'));
    $helper->assert('Can get stack trace when out of context', true);
} catch (\Exception $e) {
    $helper->exception_export($e);
}

$helper->line();

//$stack_trace_generation_allowed = false;
//$isolate->SetCaptureStackTraceForUncaughtExceptions($stack_trace_generation_allowed); // actually, this is default behavior

$func_test_tpl = new \V8\FunctionTemplate($isolate, function (\V8\FunctionCallbackInfo $info) use ($helper, $v8_helper, &$stack_trace_generation_allowed) {
    $isolate = $info->getIsolate();

    $helper->assert('Exception passed', count($info->arguments()) == 1);
    $helper->line();

    $exception = $info->arguments()[0];


    if (!$stack_trace_generation_allowed) {
        $stack_trace = V8\ExceptionManager::getStackTrace($info->getContext(), $exception);
        $helper->assert('Stack trace created from thrown value is null when capturing stack trace disabled', $stack_trace === null);
        $helper->line();

        return;
    }

    $stack_trace = V8\ExceptionManager::getStackTrace($info->getContext(), $exception);
    $helper->header('Stack trace created from thrown value');
    $helper->dump_object_methods($stack_trace, [], new ArrayListFilter(['getFrame'], true, ReflectionMethod::IS_PUBLIC));
    $helper->line();


    $exception = new \V8\StringValue($info->getIsolate(), 'test');
    $stack_trace = V8\ExceptionManager::getStackTrace($info->getContext(), $exception);
    $helper->assert('Stack trace created from manually created value is null', null === $stack_trace);
    $helper->line();
});

$global_tpl = new \V8\ObjectTemplate($isolate);
$global_tpl->set(new \V8\StringValue($isolate, 'test'), $func_test_tpl);

$context = new \V8\Context($isolate, $global_tpl);
$v8_helper->injectConsoleLog($context);


$source = '
    var ex;

    try {
        throw new Error("test");
    } catch (exception) {
        console.log("exception: ", "\'", exception, "\'");
        console.log("exception.stack: ", exception.stack);
        console.log();

        test(exception);

        ex = exception;
    }

    ex
';

$res = $v8_helper->CompileRun($context, $source);
$helper->line();

$stack_trace_generation_allowed = true;
$isolate->setCaptureStackTraceForUncaughtExceptions($stack_trace_generation_allowed);

$res = $v8_helper->CompileRun($context, $source);


// EXPECTF: ---/\["script_id":"V8\\StackFrame":private\]=>\n        int\(\d+\)/
// EXPECTF: +++["script_id":"V8\StackFrame":private]=>\n        int(%d)
?>
--EXPECTF--
Can get stack trace when out of context: ok

exception: 'Error: test'
exception.stack: Error: test
    at test.js:5:15

Exception passed: ok

Stack trace created from thrown value is null when capturing stack trace disabled: ok


exception: 'Error: test'
exception.stack: Error: test
    at test.js:5:15

Exception passed: ok

Stack trace created from thrown value:
--------------------------------------
V8\StackTrace->getFrames():
    array(1) {
      [0]=>
      object(V8\StackFrame)#14 (9) {
        ["line_number":"V8\StackFrame":private]=>
        int(5)
        ["column":"V8\StackFrame":private]=>
        int(15)
        ["script_id":"V8\StackFrame":private]=>
        int(%d)
        ["script_name":"V8\StackFrame":private]=>
        string(7) "test.js"
        ["script_name_or_source_url":"V8\StackFrame":private]=>
        string(7) "test.js"
        ["function_name":"V8\StackFrame":private]=>
        string(0) ""
        ["is_eval":"V8\StackFrame":private]=>
        bool(false)
        ["is_constructor":"V8\StackFrame":private]=>
        bool(false)
        ["is_wasm":"V8\StackFrame":private]=>
        bool(false)
      }
    }
V8\StackTrace->getFrameCount(): int(1)

Stack trace created from manually created value is null: ok
