Feature: prints an error message if there is a duplicate id
    Scenario: we attempt to insert two rows with the same id and get an error message
        Given we insert two rows with the same id
        Then we should get a duplicate id error message.
