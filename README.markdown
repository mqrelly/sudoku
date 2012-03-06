Sudoku solver in c++
====================

This I made for a university in-house competition.

Nothing fancy, just a fast sudoku solver.

There is a standalone app for demoing the solver
(`sdk-demo`), and a batch-mode app (`sdk-batch`).


License
-------

This program is released under the [glp-v3](http://www.gnu.org/licenses/gpl-3.0.html "See the website!")
license. See the `LICENSE.txt` file.


Author
------

Hi, my name is Mark Szabadkai. You can contact me at
[mqrelly@gmail.com](mailto:mqrelly@gmail.com).


Compile
-------

Currently may not compile on Windows systems.

+ **sdk-demo**: `g++ -osdk-demo -O3 sdk-demo.cc sudoku/*.cc`
  
+ **sdk-batch**: `g++ -osdk-batch -O3 sdk-batch.cc sudoku/*.cc`

Usage
-----

+ **sdk-demo** Expects a sudoku problem in its _stdin_.
  In the [following](#table_format "Table format") format.

  E.g.: `./sdk-demo < samples/6h.table`

+ **sdk-batch** Expects a list of sudoku problems.
  Every line in the list is a path to a sudoku table file,
  except empty lines and the ones starting with a hashmark
  (_#_).

  E. g.: `./sdk-batch < samples/test.set`


### Table format<a id="table_format"/>

The expected text representation of a sudoku table must contain
exactly **81** (9 rows, 9 columns) integers. Any other character
will be skipped, so feel free to format your table data as You
please. E.g. this:

    This is a simple problem,
    and other comments.
    1|2|3| |4|5|6| |7|8|9
    ------+-------+------
    4|5|6| |7|8|9| |1|2|3
    ------+-------+------
    ...

Is equivalent to `1 2 3 4 5 6 7 8 9 4 5 6 7 8 9 1 2 3 ...`

The zero (`0`) character means an empty cell.

When outputted, the tables are formatted nicely with spaces,
and the empty cells represented with a dot (`.`).
