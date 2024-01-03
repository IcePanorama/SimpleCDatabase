Feature: prints an error message if strings are too long
    Scenario: we insert a row with a 33 character-long username and a 256 character-long email
        Given we insert a 33 character-long username and a 256 character-long email
        Then we should expect a string too long error message.
