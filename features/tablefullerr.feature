Feature: prints error message when table is full
    Scenario: we fill the table with too many entries
        #FIXME: BrokenPipeError
        Given we insert 1401 rows
        Then we should recieve a table full error msg.
