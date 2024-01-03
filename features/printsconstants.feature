Feature: prints out constants
    Scenario: we enter the constants command and get a list of constants
        Given we enter the constants command
        Then the db should output a list of constants.
