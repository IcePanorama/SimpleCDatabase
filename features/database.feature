Feature: We insert and retrieve data from the database
    Scenario: we insert and retrieve a row from the database
        Given we insert a row into the database
        Then we should be able to retrieve that row from the database.

        #Scenario: we insert a row, close the db, and retrieve that row.
        #Given we insert a row
        #Then we should still be able to see the row we inserted after closing.
