Feature: prints out the structure of a 7-leaf-node btree
    Scenario: we add 63 rows to the db and run btree
        Given we add 63 rows to the db and run btree
        Then the db should properly display a 7-leaf-node btree.
