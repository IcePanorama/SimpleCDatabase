# Simple C Database

A simple SQLite Database written in C.

This project was made using [the following guide as reference](https://cstack.github.io/db_tutorial/).

As of December 23rd, 2023, this project is completely caught up with the aforementioned guide although I will continue to work on it a bit on my own. For a full list of modifications that I have made to the program, see [Noteable Modifications](#noteable-modifications).

## Building and running

To try this database out for yourself, do the following:

1. Clone the repo onto your machine.
    - Use the green `Code` button above for instructions on how to do so.
    - Note: The Makefile for this project is only designed with Linux machines in mind.
2. Unzip the repo and cd into the new folder.
3. Build the project using `make`.
4. Run the database using `./main mydb.db`.

## TODO

* Fix bugs with splitting internal nodes.

* Fix automated tests.

## Noteable Modifications

Since catching up with the guide referenced above, I have made the following modifications to the program:

* Program now automatically searches for a mydb.db file and creates one if it doesn't exist.
    * Users can still pass in their own file if they choose to do so.

