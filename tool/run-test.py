#!/usr/bin/env python3

import os
import re
import subprocess
import sys

# This python script was ported from the Crafting Interpreters' test runner written dart.
# The original test runner code can be found at:
# https://github.com/munificent/craftinginterpreters/blob/9f7cb4dbce88b1904d8067586abe04045383468c/tool/bin/test.dart

_executable = 'build/cloxx'

_expectedOutputPattern = re.compile(r"// expect: ?(.*)")
_expectedErrorPattern = re.compile(r"// (Error.*)")
_errorLinePattern = re.compile(r"// \[((java|c) )?line (\d+)\] (Error.*)")
_expectedRuntimeErrorPattern = re.compile(r"// expect runtime error: (.+)")
_syntaxErrorPattern = re.compile(r"\[.*line (\d+)\] (Error.+)")
_nonTestPattern = re.compile(r"// nontest")

_passed = 0
_failed = 0
_skipped = 0
_expectations = 0

_suite = None # Suite

_allSuites = {} # str -> Suite

# Rudimentary ANSI terminal suppoer
class term:
    @staticmethod
    def red(text):
        return "\033[31m{}\033[0m".format(text)
    @staticmethod
    def green(text):
        return "\033[92m{}\033[0m".format(text)
    @staticmethod
    def gray(text):
        return "\033[30;1m{}\033[0m".format(text)
    @staticmethod
    def pink(text):
        return "\033[31;1m{}\033[0m".format(text)
    @staticmethod
    def yellow(text):
        return "\033[33;1m{}\033[0m".format(text)
    @staticmethod
    def clearLine():
        print("\033[1000D\033[0K", end='')
    @staticmethod
    def updateLine(text):
        print("\033[1000D\033[0K" + text, end='', flush=True)

class Suite:
  def __init__(this, name, tests):
      this.name = name
      this.tests = tests

def _runSuite(name):
    global _suite, _passed, _failed, _skipped, _expectations
    _suite = _allSuites[name]

    _passed = 0
    _failed = 0
    _skipped = 0
    _expectations = 0

    print("=== Suite: {} ===".format(name))

    for (dirpath, _, filenames) in os.walk("test"):
        for filename in filenames:
            filepath = os.path.join(dirpath, filename)
            _runTest(filepath)

    term.clearLine()
    if _failed == 0:
        print("All {} tests passed ({} expectations).".format(term.green(_passed), _expectations))
    else:
        print("{} tests passed. {} tests failed.".format(term.green(_passed), term.red(_failed)))

    return _failed == 0

def _runTest(path):
    if "benchmark" in path:
        return

    global _passed, _failed

    # Update the status line.
    term.updateLine("Passed: {} Failed: {} Skipped: {} {}".format(term.green(_passed), term.red(_failed), term.yellow(_skipped), term.gray(path)))
    
    # Read the test and parse out the expectations.
    test = Test(path)

    # See if it's a skipped or non-test file.
    if not test.parse():
        return

    failures = test.run()

    # Display the results.
    if len(failures) == 0:
        _passed += 1
    else:
        _failed += 1
        term.clearLine()
        print("{} {}".format(term.red("FAIL"), path))
        for failure in failures:
            print("     {}".format(term.pink(failure)))
        print("")

class ExpectedOutput:
    def __init__(this, line, output):
        this.line = line
        this.output = output

class Test:
    def __init__(this, path):
        this._path = path
        this._expectedOutputs = []

        # The set of expected compile error messages.
        this._expectedErrors = set()

        # The expected runtime error message or `null` if there should not be one.
        this._expectedRuntimeError = None

        this._expectedExitCode = 0

        # The list of failure message lines.
        this._failures = []

    def parse(this):
        # Get the path components.
        parts = this._path.split('/')
        state = None # "pass" or "skip"

        # Figure out the state of the test. We don't break out of this loop because
        # we want lines for more specific paths to override more general ones.
        subpath = ""
        for part in parts:
            if len(subpath) > 0:
                subpath += "/"
            subpath += part
            if subpath in _suite.tests:
                state = _suite.tests[subpath]

        if state is None:
            raise RuntimeError("Unknown test state for '{}'.".format(this._path))
        elif state == "skip":
            global _skipped
            _skipped += 1
            return False

        global _expectations

        lines = []
        with open(this._path, 'r') as file:
            lines = file.readlines()
        lineNum = 0
        for line in lines:
            lineNum += 1
            # Not a test file at all, so ignore it.
            match = _nonTestPattern.search(line)
            if match is not None:
                return False

            match = _expectedOutputPattern.search(line)
            if match is not None:
                this._expectedOutputs.append(ExpectedOutput(lineNum, match[1]))
                _expectations += 1
                continue

            match = _expectedErrorPattern.search(line)
            if match is not None:
                this._expectedErrors.add("[line {}] {}".format(lineNum, match[1]))

                # If we expect a compile error, it should exit with EX_DATAERR.
                this._expectedExitCode = 65
                _expectations += 1
                continue

            match = _errorLinePattern.search(line)
            if match is not None:
                language = match[2]
                if language is not None and language == "c":
                    # cloxx works more like jlox then clox.
                    # if the error is intended for clox then let's skip it
                    continue

                this._expectedErrors.add("[line {}] {}".format(match[3], match[4]))

                # If we expect a compile error, it should exit with EX_DATAERR.
                this._expectedExitCode = 65
                _expectations += 1
                continue

            match = _expectedRuntimeErrorPattern.search(line)
            if match is not None:
                this._expectedRuntimeError = "[line {}] {}".format(lineNum, match[1])
                # If we expect a runtime error, it should exit with EX_SOFTWARE.
                this._expectedExitCode = 70
                _expectations += 1
    
        if len(this._expectedErrors) > 0 and this._expectedRuntimeError is not None:
            print("${term.magenta('TEST ERROR')} $_path")
            print("     Cannot expect both compile and runtime errors.")
            print("")
            return False

        # If we got here, it's a valid test.
        return True

    # Invoke the interpreter and run the test.
    def run(this):
        global _executable

        result = subprocess.run([_executable, this._path], stdout=subprocess.PIPE, stderr=subprocess.PIPE)

        # Normalize Windows line endings.
        outputLines = result.stdout.decode('utf-8').split('\n')
        errorLines = result.stderr.decode('utf-8').split('\n')

        # Validate that an expected runtime error occurred.
        if this._expectedRuntimeError is not None:
            this._validateRuntimeError(errorLines)
        else:
            this._validateCompileErrors(errorLines)

        this._validateExitCode(result.returncode, errorLines)
        this._validateOutput(outputLines)
        return this._failures
    

    def _validateRuntimeError(this, errorLines):
        if len(errorLines) < 2:
            this._fail("Expected runtime error '{}' and got none.".format(this._expectedRuntimeError))
            return
        if errorLines[0] != this._expectedRuntimeError:
            this._fail("Expected runtime error '{}' and got:".format(this._expectedRuntimeError))
            this._fail(errorLines[0])

    def _validateCompileErrors(this, errorLines):
        # Validate that every compile error was expected.
        foundErrors = set()
        unexpectedCount = 0
        for line in errorLines:
            match = _syntaxErrorPattern.search(line)
            if match is not None:
                error = "[line {}] {}".format(match[1], match[2])
                if error in this._expectedErrors:
                    foundErrors.add(error)
                else:
                    if unexpectedCount < 10:
                        this._fail("Unexpected error:")
                        this._fail(line)
                    unexpectedCount += 1
            elif line != "":
                if unexpectedCount < 10:
                    this._fail("Unexpected output on stderr:")
                    this._fail(line)
                unexpectedCount += 1
        if unexpectedCount > 10:
            this._fail("(truncated {} more...)".format(unexpectedCount - 10))

        # Validate that every expected error occurred.
        for error in this._expectedErrors.difference(foundErrors):
            this._fail("Missing expected error: {}".format(error))

    def _validateExitCode(this, exitCode, errorLines):
        if exitCode == this._expectedExitCode:
            return
        if len(errorLines) > 10:
            errorLines = errorLines[:10]
            errorLines.append("(truncated...)")
            this._fail("Expected return code {} and got {}. Stderr:".format(this._expectedExitCode, exitCode),
                    errorLines)

    def _validateOutput(this, outputLines):
        # Remove the trailing last empty line.
        if len(outputLines) > 0 and outputLines[-1] == "":
            del outputLines[-1]

        index = 0
        for outputLine in outputLines:
            if index >= len(this._expectedOutputs):
                this._fail("Got output '{}' when none was expected.".format(outputLine))
            else:
                expected = this._expectedOutputs[index]
                if expected.output != outputLine:
                    this._fail("Expected output '{}' on line {} and got '{}'.".format(expected.output, expected.line, outputLine))
            index += 1

        while index < len(this._expectedOutputs):
            expected = this._expectedOutputs[index]
            this._fail("Missing expected output '{}' on line {}.".format(expected.output, expected.line))
            index += 1

    def _fail(this, message, lines = None):
        this._failures.append(message)
        if lines is not None:
            this._failures.extend(lines)

def _merge_dicts(*dicts):
    result = {}
    for d in dicts:
        result.update(d)
    return result

def _defineTestSuites():
    # JVM doesn't correctly implement IEEE equality on boxed doubles.
    javaNaNEquality = {
        "test/number/nan_equality.lox": "skip",
    }

    # No hardcoded limits in jlox.
    noJavaLimits = {
        "test/limit/loop_too_large.lox": "skip",
        "test/limit/no_reuse_constants.lox": "skip",
        "test/limit/too_many_constants.lox": "skip",
        "test/limit/too_many_locals.lox": "skip",
        "test/limit/too_many_upvalues.lox": "skip",

        # Rely on JVM for stack overflow checking.
        "test/limit/stack_overflow.lox": "skip",
    }

    jloxTests = _merge_dicts(
        { "test": "pass" },
        javaNaNEquality,
        noJavaLimits
    )

    cloxxTests = _merge_dicts(
        jloxTests,
        { "test/print": "skip" },

        # now true & false are instance of Bool
        { "test/field/set_on_bool.lox": "skip" },
        { "test/field/get_on_bool.lox": "skip" },

        # now nil instance of Nil
        { "test/field/set_on_nil.lox": "skip" },
        { "test/field/get_on_nil.lox": "skip" },

        # now strings are instance of String
        { "test/field/set_on_string.lox": "skip" },
        { "test/field/get_on_string.lox": "skip" },

        # now numbers are instance of Number
        { "test/field/set_on_num.lox": "skip" },
        { "test/field/get_on_num.lox": "skip" },

        # now classes are instance of Class
        { "test/field/set_on_class.lox": "skip" },
        { "test/field/get_on_class.lox": "skip" },

        # now functions are instance of Function
        { "test/field/set_on_function.lox": "skip" },
        { "test/field/get_on_function.lox": "skip" },
    )

    # cloxx behaves like jlox
    _allSuites["cloxx"] = Suite("cloxx", cloxxTests)

    # more suites can go here...


def main():
    if len(sys.argv) > 2:
        sys.exit('Usage: {} [suite]'.format(sys.argv[0]))
    
    _defineTestSuites()
    if len(sys.argv) == 2:
        _runSuite(sys.argv[1])
    else:
        _runSuite('cloxx')

if __name__ == '__main__':
    main()
