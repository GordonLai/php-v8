--TEST--
V8\ObjectTemplate::Set() - recursive tree
--SKIPIF--
<?php if (!extension_loaded("v8")) print "skip"; ?>
--FILE--
<?php
/** @var \Phpv8Testsuite $helper */
$helper = require '.testsuite.php';

require '.v8-helpers.php';
$v8_helper = new PhpV8Helpers($helper);

// Tests:

$isolate = new \V8\Isolate();

$t1 = new \V8\ObjectTemplate($isolate); $s1 = new \V8\StringValue($isolate, 't1');
$t2 = new \V8\ObjectTemplate($isolate); $s2 = new \V8\StringValue($isolate, 't2');
$t3 = new \V8\ObjectTemplate($isolate); $s3 = new \V8\StringValue($isolate, 't3');
$t4 = new \V8\ObjectTemplate($isolate); $s4 = new \V8\StringValue($isolate, 't4');
$t5 = new \V8\ObjectTemplate($isolate); $s5 = new \V8\StringValue($isolate, 't5');
$t6 = new \V8\ObjectTemplate($isolate); $s6 = new \V8\StringValue($isolate, 't6');
$t7 = new \V8\ObjectTemplate($isolate); $s7 = new \V8\StringValue($isolate, 't7');
$t8 = new \V8\ObjectTemplate($isolate); $s8 = new \V8\StringValue($isolate, 't8');
$t9 = new \V8\ObjectTemplate($isolate); $s9 = new \V8\StringValue($isolate, 't9');

//             1
//            / \
//          /     \
//         2       5
//        / \     / \
//       3   4   6   7
//                  / \
//                 8   9

$t1->Set($s2, $t2);

$t2->Set($s3, $t3);
$t2->Set($s4, $t4);

$t1->Set($s5, $t5);

$t5->Set($s6, $t6);
$t5->Set($s7, $t7);

$t7->Set($s8, $t8);
$t7->Set($s9, $t9);

try {
    $t9->Set($s1, $t1);
} catch (\V8\Exceptions\Exception $e) {
    $helper->exception_export($e);
}


try {
    $t7->Set($s1, $t1);
} catch (\V8\Exceptions\Exception $e) {
    $helper->exception_export($e);
}

$t4->Set($s6, $t6);

try {
    $t6->Set($s4, $t4);
} catch (\V8\Exceptions\Exception $e) {
    $helper->exception_export($e);
}

$context = new \V8\Context($isolate);
$context->GlobalObject()->Set($context, new \V8\StringValue($isolate, 'test'), $t1->NewInstance($context));

?>
--EXPECT--
V8\Exceptions\Exception: Can't set: recursion detected
V8\Exceptions\Exception: Can't set: recursion detected
V8\Exceptions\Exception: Can't set: recursion detected
