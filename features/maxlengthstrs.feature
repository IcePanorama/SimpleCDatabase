Feature: allows inserting strings that are the maximum length
    Scenario: we insert a 32 character long username and a 255 character long email
        Given we insert a 32 character long username and a 255 character long email
        Then we should expect the database to behave normally.
