Feature: data persists in the database
    Scenario: we insert a row, close the db, and retrieve that row
        Given we insert a row
        Then we should still be able to see the row after closing the db.
