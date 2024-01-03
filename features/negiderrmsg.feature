Feature: prints an error message if id is negative
    Scenario: we insert a row into the db with a negative id and we get an error message
        Given we insert a row into the db with a negative id
        Then we should recieve an id is negative error message.
