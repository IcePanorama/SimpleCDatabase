from behave import *
import subprocess


def run_script(commands: list[str]) -> list[str]:
    raw_output = None
    with subprocess.Popen(["./main"], stdin=subprocess.PIPE,
                          stdout=subprocess.PIPE, text=True) as process:
        for command in commands:
            try:
                process.stdin.write(command + "\n")
                process.stdin.flush()
            except BrokenPipeError:
                break
        process.stdin.close()
        process.wait()
        raw_output = str(process.stdout.read()).split("\n")

    return raw_output


def arrays_match(arr0, arr1) -> bool:
    if len(arr0) != len(arr1):
        return False

    for i in range(len(arr0)):
        if arr0[i] != arr1[i]:
            return False
    return True


@given('we insert a row into the database')
def step_impl(context):
    context.results = run_script([
        "insert 1 user1 person1@example.com",
        "select",
        ".exit"
        ])


@then('we should be able to retrieve that row from the database.')
def step_impl(context):
    expected_result = ["db > Executed.",
                       "db > (1, user1, person1@example.com)",
                       "Executed.",
                       "db > "]
    assert arrays_match(context.results, expected_result) is True


@given('we insert a row')
def step_impl(context):
    context.results = run_script([
        "insert 1 user1 person1@example.com",
        ".exit"
        ])
    expected_result = [
            "db > Executed.",
            "db > ",
    ]
    assert arrays_match(context.results, expected_result)


@then('we should still be able to see the row after closing the db.')
def step_impl(context):
    context.results.clear()
    context.results = run_script([
        "select",
        ".exit"
        ])
    expected_result = [
            "db > (1, user1, person1@example.com)",
            "Executed.",
            "db > "
    ]

    assert arrays_match(context.results, expected_result)
