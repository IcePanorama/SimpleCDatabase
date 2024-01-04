from behave import *
import subprocess


def run_script(commands: list[str]) -> list[str]:
    raw_output = None
    with subprocess.Popen(["./database"], stdin=subprocess.PIPE,
                          stdout=subprocess.PIPE, text=True) as process:
        for command in commands:
            try:
                process.stdin.write(command + "\n")
                process.stdin.flush()
            except BrokenPipeError:
                break
        # Closing stdin & waiting to prevent duplicate results error.
        process.stdin.close()
        process.wait()
        raw_output = str(process.stdout.read()).split("\n")

    return raw_output


def arrays_match(arr0: list[str], arr1: list[str]) -> bool:
    if len(arr0) != len(arr1):
        return False

    for i in range(len(arr0)):
        if arr0[i] != arr1[i]:
            return False
    return True


@given('we insert a row into the database and run the select command')
def step_impl(context):
    script = [
            "insert 1 user1 person1@example.com",
            "select",
            ".exit"
            ]
    context.results = run_script(script)


@then('we should be able to retrieve that row from the database.')
def step_impl(context):
    expected_result = ["db > Executed.",
                       "db > (1, user1, person1@example.com)",
                       "Executed.",
                       "db > "]
    assert arrays_match(context.results, expected_result)


@given('we insert a row')
def step_impl(context):
    script = [
            "insert 1 user1 person1@example.com",
            ".exit"
            ]
    context.results = run_script(script)

    expected_result = [
            "db > Executed.",
            "db > ",
            ]

    assert arrays_match(context.results, expected_result)


@then('we should still be able to see the row after closing the db.')
def step_impl(context):
    script = [
            "select",
            ".exit"
            ]
    context.results = run_script(script)

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
        commands.append("insert " + str(i) + " user" + str(i) + " person" 
                        + str(i) + "@example.com")
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
            "insert 1 " + context.long_username + " " + context.long_email,
            "select",
            ".exit"
            ]

    context.results = run_script(script)


@then('we should expect the database to behave normally.')
def step_impl(context):
    expected_results = [
            "db > Executed.",
            "db > (1, " + context.long_username + ", " + context.long_email + ")",
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


@given('we add 29 rows to our db and run btree')
def step_impl(context):
    script = [
            "insert 18 user18 person18@example.com",
            "insert 7 user7 person7@example.com",
            "insert 10 user10 person10@example.com",
            "insert 29 user29 person29@example.com",
            "insert 23 user23 person23@example.com",
            "insert 4 user4 person4@example.com",
            "insert 14 user14 person14@example.com",
            "insert 30 user30 person30@example.com",
            "insert 15 user15 person15@example.com",
            "insert 26 user26 person26@example.com",
            "insert 22 user22 person22@example.com",
            "insert 19 user19 person19@example.com",
            "insert 2 user2 person2@example.com",
            "insert 1 user1 person1@example.com",
            "insert 21 user21 person21@example.com",
            "insert 11 user11 person11@example.com",
            "insert 6 user6 person6@example.com",
            "insert 20 user20 person20@example.com",
            "insert 5 user5 person5@example.com",
            "insert 8 user8 person8@example.com",
            "insert 9 user9 person9@example.com",
            "insert 3 user3 person3@example.com",
            "insert 12 user12 person12@example.com",
            "insert 27 user27 person27@example.com",
            "insert 17 user17 person17@example.com",
            "insert 16 user16 person16@example.com",
            "insert 13 user13 person13@example.com",
            "insert 24 user24 person24@example.com",
            "insert 25 user25 person25@example.com",
            "insert 28 user28 person28@example.com",
            ".btree",
            ".exit",
            ]

    _results = run_script(script)
    context.results = []
    for i in range(30, len(_results)):
        context.results.append(_results[i])


@then('the btree should properly display a 4-leaf-node btree.')
def step_impl(context):
    expected_results = [
            "db > Tree:",
            "- internal (size 3)",
            "  - leaf (size 7)",
            "    - 1",
            "    - 2",
            "    - 3",
            "    - 4",
            "    - 5",
            "    - 6",
            "    - 7",
            "  - key 7",
            "  - leaf (size 8)",
            "    - 8",
            "    - 9",
            "    - 10",
            "    - 11",
            "    - 12",
            "    - 13",
            "    - 14",
            "    - 15",
            "  - key 15",
            "  - leaf (size 7)",
            "    - 16",
            "    - 17",
            "    - 18",
            "    - 19",
            "    - 20",
            "    - 21",
            "    - 22",
            "  - key 22",
            "  - leaf (size 8)",
            "    - 23",
            "    - 24",
            "    - 25",
            "    - 26",
            "    - 27",
            "    - 28",
            "    - 29",
            "    - 30",
            "db > ",
            ]

    assert arrays_match(context.results, expected_results)


@given('we add 63 rows to the db and run btree')
def step_impl(context):
    script = [
            "insert 58 user58 person58@example.com",
            "insert 56 user56 person56@example.com",
            "insert 8 user8 person8@example.com",
            "insert 54 user54 person54@example.com",
            "insert 77 user77 person77@example.com",
            "insert 7 user7 person7@example.com",
            "insert 25 user25 person25@example.com",
            "insert 71 user71 person71@example.com",
            "insert 13 user13 person13@example.com",
            "insert 22 user22 person22@example.com",
            "insert 53 user53 person53@example.com",
            "insert 51 user51 person51@example.com",
            "insert 59 user59 person59@example.com",
            "insert 32 user32 person32@example.com",
            "insert 36 user36 person36@example.com",
            "insert 79 user79 person79@example.com",
            "insert 10 user10 person10@example.com",
            "insert 33 user33 person33@example.com",
            "insert 20 user20 person20@example.com",
            "insert 4 user4 person4@example.com",
            "insert 35 user35 person35@example.com",
            "insert 76 user76 person76@example.com",
            "insert 49 user49 person49@example.com",
            "insert 24 user24 person24@example.com",
            "insert 70 user70 person70@example.com",
            "insert 48 user48 person48@example.com",
            "insert 39 user39 person39@example.com",
            "insert 15 user15 person15@example.com",
            "insert 47 user47 person47@example.com",
            "insert 30 user30 person30@example.com",
            "insert 86 user86 person86@example.com",
            "insert 31 user31 person31@example.com",
            "insert 68 user68 person68@example.com",
            "insert 37 user37 person37@example.com",
            "insert 66 user66 person66@example.com",
            "insert 63 user63 person63@example.com",
            "insert 40 user40 person40@example.com",
            "insert 78 user78 person78@example.com",
            "insert 19 user19 person19@example.com",
            "insert 46 user46 person46@example.com",
            "insert 14 user14 person14@example.com",
            "insert 81 user81 person81@example.com",
            "insert 72 user72 person72@example.com",
            "insert 6 user6 person6@example.com",
            "insert 50 user50 person50@example.com",
            "insert 85 user85 person85@example.com",
            "insert 67 user67 person67@example.com",
            "insert 2 user2 person2@example.com",
            "insert 55 user55 person55@example.com",
            "insert 69 user69 person69@example.com",
            "insert 5 user5 person5@example.com",
            "insert 65 user65 person65@example.com",
            "insert 52 user52 person52@example.com",
            "insert 1 user1 person1@example.com",
            "insert 29 user29 person29@example.com",
            "insert 9 user9 person9@example.com",
            "insert 43 user43 person43@example.com",
            "insert 75 user75 person75@example.com",
            "insert 21 user21 person21@example.com",
            "insert 82 user82 person82@example.com",
            "insert 12 user12 person12@example.com",
            "insert 18 user18 person18@example.com",
            "insert 60 user60 person60@example.com",
            "insert 44 user44 person44@example.com",
            ".btree",
            ".exit",
            ]

    _results = run_script(script)
    context.results = []
    for i in range(64, len(_results)):
        context.results.append(_results[i])


@then('the db should properly display a 7-leaf-node btree.')
def step_impl(context):
    expected_results = [
            "db > Tree:",
            "- internal (size 1)",
            "  - internal (size 2)",
            "    - leaf (size 7)",
            "      - 1",
            "      - 2",
            "      - 4",
            "      - 5",
            "      - 6",
            "      - 7",
            "      - 8",
            "    - key 8",
            "    - leaf (size 11)",
            "      - 9",
            "      - 10",
            "      - 12",
            "      - 13",
            "      - 14",
            "      - 15",
            "      - 18",
            "      - 19",
            "      - 20",
            "      - 21",
            "      - 22",
            "    - key 22",
            "    - leaf (size 8)",
            "      - 24",
            "      - 25",
            "      - 29",
            "      - 30",
            "      - 31",
            "      - 32",
            "      - 33",
            "      - 35",
            "  - key 35",
            "  - internal (size 3)",
            "    - leaf (size 12)",
            "      - 36",
            "      - 37",
            "      - 39",
            "      - 40",
            "      - 43",
            "      - 44",
            "      - 46",
            "      - 47",
            "      - 48",
            "      - 49",
            "      - 50",
            "      - 51",
            "    - key 51",
            "    - leaf (size 11)",
            "      - 52",
            "      - 53",
            "      - 54",
            "      - 55",
            "      - 56",
            "      - 58",
            "      - 59",
            "      - 60",
            "      - 63",
            "      - 65",
            "      - 66",
            "    - key 66",
            "    - leaf (size 7)",
            "      - 67",
            "      - 68",
            "      - 69",
            "      - 70",
            "      - 71",
            "      - 72",
            "      - 75",
            "    - key 75",
            "    - leaf (size 8)",
            "      - 76",
            "      - 77",
            "      - 78",
            "      - 79",
            "      - 81",
            "      - 82",
            "      - 85",
            "      - 86",
            "db > ",
            ]

    assert arrays_match(context.results, expected_results)


@given('we enter the constants command')
def step_impl(context):
    script = [
            ".constants",
            ".exit",
            ]

    context.results = run_script(script)


@then('the db should output a list of constants.')
def step_impl(context):
    expected_results = [
            "db > Constants:",
            "ROW_SIZE: 293",
            "COMMON_NODE_HEADER_SIZE: 6",
            "LEAF_NODE_HEADER_SIZE: 14",
            "LEAF_NODE_CELL_SIZE: 297",
            "LEAF_NODE_SPACE_FOR_CELLS: 4082",
            "LEAF_NODE_MAX_CELLS: 13",
            "db > ",
            ]

    assert arrays_match(context.results, expected_results)


@given('we insert 15 elements into the db and run select')
def step_impl(context):
    script = []
    for i in range(1, 16):
        script.append("insert " + str(i) + " user" + str(i) + " person"
                      + str(i) + "@example.com")
    script.append("select")
    script.append(".exit")

    _results = run_script(script)

    context.results = []
    for i in range(15, len(_results)):
        context.results.append(_results[i])


@then('the database should properly display all 15 rows.')
def step_impl(context):
    expected_results = [
            "db > (1, user1, person1@example.com)",
            "(2, user2, person2@example.com)",
            "(3, user3, person3@example.com)",
            "(4, user4, person4@example.com)",
            "(5, user5, person5@example.com)",
            "(6, user6, person6@example.com)",
            "(7, user7, person7@example.com)",
            "(8, user8, person8@example.com)",
            "(9, user9, person9@example.com)",
            "(10, user10, person10@example.com)",
            "(11, user11, person11@example.com)",
            "(12, user12, person12@example.com)",
            "(13, user13, person13@example.com)",
            "(14, user14, person14@example.com)",
            "(15, user15, person15@example.com)",
            "Executed.",
            "db > ",
            ]

    assert arrays_match(context.results, expected_results)
