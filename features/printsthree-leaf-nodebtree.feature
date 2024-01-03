Feature: it prints out the structure of a 3-leaf-node btree
    Scenario: We insert 14 rows into the db and run btree
        Given we insert 14 rows into the db
        Then btree should properly display a 3-leaf-node btree.
