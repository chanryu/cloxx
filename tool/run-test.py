#!/usr/bin/env python3

import os
import re
import subprocess

# Runs the tests.

_executable = 'build/cloxx'

_expectedOutputPattern = re.compile(r"// expect: ?(.*)")
_expectedErrorPattern = re.compile(r"// (Error.*)")
_errorLinePattern = re.compile(r"// \[((java|c) )?line (\d+)\] (Error.*)")
_expectedRuntimeErrorPattern = re.compile(r"// expect runtime error: (.+)")
_syntaxErrorPattern = re.compile(r"\[.*line (\d+)\] (Error.+)")
_stackTracePattern = re.compile(r"\[line (\d+)\]")
_nonTestPattern = re.compile(r"// nontest")

_passed = 0
_failed = 0
_skipped = 0
_expectations = 0

_suite = None # Suite

_allSuites = {} # str -> Suite

class Suite:
  def __init__(this, name, tests):
      this.name = name
      this.tests = tests

# def _usageError(parser, message):
#     print(message)
#     print("")
#     print("Usage: test.dart <suites> [filter] [custom interpreter...]")
#     print("")
#     print("Optional custom interpreter options:")
#     print(parser.usage)
#     exit(1)

def _runSuites(names):
  anyFailed = False
  for name in names:
    print("=== {} ===".format(name))
    if not _runSuite(name):
        anyFailed = True
  if anyFailed:
      exit(1)

def _runSuite(name):
    global _suite, _passed, _failed, _skipped, _expectations
    _suite = _allSuites[name]

    _passed = 0
    _failed = 0
    _skipped = 0
    _expectations = 0

    for (dirpath, _, filenames) in os.walk("test"):
        for filename in filenames:
            filepath = os.path.join(dirpath, filename)
            _runTest(filepath)

    #term.clearLine()
    print("")

    if _failed == 0:
        #print("All ${term.green(_passed)} tests passed ($_expectations expectations).")
        print("All {} tests passed ({} expectations).".format(_passed, _expectations))
    else:
        #print("${term.green(_passed)} tests passed. ${term.red(_failed)} tests failed.")
        print("{} tests passed. {} tests failed.".format(_passed, _failed))

    return _failed == 0

def _runTest(path):
    if "benchmark" in path:
        return

    global _passed, _failed

    # Make a nice short path relative to the working directory. Normalize it to
    # use "/" since the interpreters expect the argument to use that.
    #path = os.path.normalize(path)

    # Update the status line.
    #grayPath = term.gray("($path)")
    #term.writeLine("Passed: ${term.green(_passed)} Failed: ${term.red(_failed)} Skipped: ${term.yellow(_skipped)} $grayPath")
    #print("Passed: {} Failed: {} Skipped: {} {}".format(_passed, _failed, _skipped, path))

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
        #term.writeLine("${term.red("FAIL")} $path")
        print("FAIL {}".format(path))
        print("")
        for failure in failures:
            #print("     ${term.pink(failure)}")
            print("     {}".format(failure))
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

        # If there is an expected runtime error, the line it should occur on.
        this._runtimeErrorLine = 0

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
                this._expectedErrors.add("[{}] {}".format(lineNum, match[1]))

                # If we expect a compile error, it should exit with EX_DATAERR.
                this._expectedExitCode = 65
                _expectations += 1
                continue

            match = _errorLinePattern.search(line)
            if match is not None:
                this._expectedErrors.add("[{}] {}".format(match[3], match[4]))

                # If we expect a compile error, it should exit with EX_DATAERR.
                this._expectedExitCode = 65
                _expectations += 1
                continue

            match = _expectedRuntimeErrorPattern.search(line)
            if match is not None:
                this._runtimeErrorLine = lineNum
                this._expectedRuntimeError = match[1]
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

        result = subprocess.run([_executable, this._path], stdout=subprocess.PIPE)

        # Normalize Windows line endings.
        outputLines = [] if result.stdout is None else result.stdout.decode('utf-8').split('\n')
        errorLines = [] if result.stderr is None else result.stderr.decode('utf-8').split('\n')

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

        # Make sure the stack trace has the right line.
        match = None
        stackLines = errorLines.sublist(1)
        for line in stackLines:
            match = _stackTracePattern.search(line)
            if match is not None:
                break

        if match is None:
            this._fail("Expected stack trace and got:", stackLines)
        else:
            stackLine = int.parse(match[1])
            if stackLine != this._runtimeErrorLine:
                this._fail("Expected runtime error on line {} but was on line {}.".format(this._runtimeErrorLine, stackLine))

    def _validateCompileErrors(this, errorLines):
        # Validate that every compile error was expected.
        foundErrors = set()
        unexpectedCount = 0
        for line in errorLines:
            match = _syntaxErrorPattern.search(line)
            if match is not None:
                error = "[{}] {}".format(match[1], match[2])
                if error in this._expectedErrors:
                    foundErrors.append(error)
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
            errorLines = errorLines[0, 10]
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

  _allSuites["jlox"] = Suite("jlox", _merge_dicts({ "test": "pass" }, javaNaNEquality, noJavaLimits))
  _allSuites["test-test"] = Suite("assign-local", { "test" : "skip", "test/assignment/local.lox": "pass" })


def main():
    _defineTestSuites()
#   var parser = ArgParser()
#   var options = parser.parse(arguments)
#   if options.rest.isEmpty: {
#     _usageError(parser, "Missing suite name.")
#   } else if options.rest.length > 1: {
#     _usageError(
#         parser, "Unexpected arguments '${options.rest.skip(2).join(' ')}'.")
#   }
#   if suite == "all": {
#    _runSuites(_allSuites.keys.toList())
#   } else if !_runSuite(suite): {
#     exit(1)
#   }
    _runSuite("jlox")

if __name__ == '__main__':
    main()
