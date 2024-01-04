# Simple C Database

A simple SQLite Database written in C.

This project was made using [the following guide as reference](https://cstack.github.io/db_tutorial/).

As of December 23rd, 2023, this project is completely caught up with the aforementioned guide although I will continue to work on it a bit on my own. For a full list of modifications that I have made to the program, see [Noteable Modifications](#noteable-modifications).

## Building and running

To try this database out for yourself, do the following:

1. Clone the repo onto your machine.
    - Using HTTPS: `git clone https://github.com/IcePanorama/SimpleCDatabase.git`
    - Using SSH: `git clone git@github.com:IcePanorama/SimpleCDatabase.git`
    - **Note: The Makefile for this project is primarily designed with Linux machines in mind.**
    - The project may still build on non-Linux operating systems, but your mileage may vary.
2. Move into the new directory using `cd SimpleCDatabase`.
3. Build the project using `make`.
4. Finally, run the database using `./database`.

## TODO

* Fix bugs with splitting internal nodes.

## Noteable Modifications

Since catching up with the guide referenced above, I have made the following modifications to the program:

* Program now automatically searches for a mydb.db file and creates one if it doesn't exist.
    * Users can still pass in their own file if they choose to do so.

* Converted automated testing from 'rspec' (written in Ruby) to 'behave' (written in Python).
    * Makefile now automatically runs tests after building.
    * Currently, 3 test fail due to the aforementioned bug with splitting internal nodes.
