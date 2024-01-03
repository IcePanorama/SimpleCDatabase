Feature: it prints all the rows in a multi-level tree
    Scenario: We insert 15 elements and then run the select command
        Given we insert 15 elements into the db and run select
        Then the database should properly display all 15 rows.
