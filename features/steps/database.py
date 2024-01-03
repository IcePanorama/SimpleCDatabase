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
    assert arrays_match(context.results, expected_result)


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


@given('we insert 1401 rows')
def step_impl(context):
    commands = []
    for i in range(1, 1401):
        commands.append("insert " + str(i) + " user" + str(i) + " person" + str(i) + "@example.com")
    commands.append(".exit")

    context.results = run_script(commands)


@then('we should recieve a table full error msg.')
def step_impl(context):
    expected_results = [
            "db > Executed.",
            "db > "
            ]

    assert arrays_match([context.results[-2], context.results[-1]], expected_results)


@given('we insert a 32 character long username and a 255 character long email')
def step_impl(context):
    context.long_username = "a"*32
    context.long_email = "a"*255
    script = [
            "insert 1 " + str(context.long_username) + " " + str(context.long_email),
            "select",
            ".exit"
            ]
    context.results = run_script(script)


@then('we should expect the database to behave normally.')
def step_impl(context):
    expected_results = [
            "db > Executed.",
            "db > (1, " + str(context.long_username) + ", " + str(context.long_email) + ")",
            "Executed.",
            "db > "
            ]

    assert arrays_match(context.results, expected_results)


@given('we insert a 33 character-long username and a 256 character-long email')
def step_impl(context):
    context.long_username = "a"*33
    context.long_email = "a"*256
    script = [
            "insert 1 " + str(context.long_username) + " " + str(context.long_email),
            "select",
            ".exit"
            ]
    context.results = run_script(script)


@then('we should expect a string too long error message.')
def step_impl(context):
    expect_results = [
            "db > String is too long.",
            "db > Executed.",
            "db > "
            ]
    assert arrays_match(context.results, expect_results)


@given('we insert a row into the db with a negative id')
def step_impl(context):
    script = [
            "insert -1 cstack foo@bar.com",
            "select",
            ".exit"
            ]
    context.results = run_script(script)


@then('we should recieve an id is negative error message.')
def step_impl(context):
    expected_results = [
            "db > ID must be positive.",
            "db > Executed.",
            "db > "
            ]

    assert arrays_match(context.results, expected_results)


@given('we insert two rows with the same id')
def step_impl(context):
    script = [
            "insert 1 user1 person1@example.com",
            "insert 1 user1 person1@example.com",
            "select",
            ".exit"
            ]
    context.results = run_script(script)


@then('we should get a duplicate id error message.')
def step_impl(context):
    expected_results = [
            "db > Executed.",
            "db > Error: Duplicate key.",
            "db > (1, user1, person1@example.com)",
            "Executed.",
            "db > "
            ]

    assert arrays_match(context.results, expected_results)


@given('we insert 3 rows into the db and run btree')
def step_impl(context):
    script = [
            "insert 3 user3 person3@example.com",
            "insert 1 user1 person1@example.com",
            "insert 2 user2 person2@example.com",
            ".btree",
            ".exit"
            ]
    context.results = run_script(script)


@then('the db should properly display the one-node btree.')
def step_impl(context):
    expected_results = [
            "db > Executed.",
            "db > Executed.",
            "db > Executed.",
            "db > Tree:",
            "- leaf (size 3)",
            "  - 1",
            "  - 2",
            "  - 3",
            "db > "
            ]

    assert arrays_match(context.results, expected_results)


@given('we insert 14 rows into the db')
def step_impl(context):
    script = []
    for i in range(1, 15):
        script.append("insert " + str(i) + " user" + str(i) + " person"
                      + str(i) + "@example.com")
    script.append(".btree")
    script.append(".exit")

    _results = run_script(script)
    context.results = []
    for i in range(14, len(_results)):
        context.results.append(_results[i])


@then('btree should properly display a 3-leaf-node btree.')
def step_impl(context):
    expected_results = [
            "db > Tree:",
            "- internal (size 1)",
            "  - leaf (size 7)",
            "    - 1",
            "    - 2",
            "    - 3",
            "    - 4",
            "    - 5",
            "    - 6",
            "    - 7",
            "  - key 7",
            "  - leaf (size 7)",
            "    - 8",
            "    - 9",
            "    - 10",
            "    - 11",
            "    - 12",
            "    - 13",
            "    - 14",
            "db > ",
            ]

    assert arrays_match(context.results, expected_results)
