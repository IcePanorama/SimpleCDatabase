Feature: We insert and retrieve data from the database
    Scenario: we insert and retrieve a row from the database
        Given we insert a row into the database and run the select command
        Then we should be able to retrieve that row from the database.
