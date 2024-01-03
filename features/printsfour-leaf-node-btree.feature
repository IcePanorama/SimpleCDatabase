Feature: prints out the structure of a 4-leaf-node btree
    Scenario: we add 29 rows to our db and run btree
        Given we add 29 rows to our db and run btree
        Then the btree should properly display a 4-leaf-node btree.
