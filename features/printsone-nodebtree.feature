Feature: allows printing out the structure of a one-node btree
    Scenario: We insert 3 rows and then run the btree command
        Given we insert 3 rows into the db and run btree
        Then the db should properly display the one-node btree.
