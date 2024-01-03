#FIXME: Getting a BrokenPipeError when running this feature.
#Feature: prints error message when table is full
#    Scenario: we fill the table with too many entries
#        Given we insert 1401 rows
#        Then we should recieve a table full error msg.
