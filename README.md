# Conway's Game of Life

This is my implementation of Conway's Game of Life using an implementation of the Hashlife algorithm.
This was also written to relearn C++, so any suggestions on improving this implementation is greatly appreciated.

This program uses Make to build, and assumes that SDL2 and boost is in the pkg-config path. The program accepts either
raw points on the command line, or -f lifeFile.life. Where a life file is a collection of points. Its similar to a .lif 1.06 file, only without the header (see TODO for more info on that).

## Directions
White-space separated points: `./conway x0 y0 x1 y1` (parens and commas may be used for clarity) or a -f flag with a file containing points ala above (`./conway -f examples/acorn.life`)

In the program WASD moves the camera, left bracket zooms out, right bracket zooms in, - slows the simulation, = speeds it up, and escape quits.

## Issues/TODO

*   QuadTreeNode isn't immutable enough. This is fine given my current implementation, but I would like to const-up the four quad pointers in each node. I didn't do this as I ran out of time, the implementation works as is, and this requires a bit of refactoring on this class, the cache it uses, and its next pointer.
*   Currently set/get cells operate via one point only. Set should be changed to take a subset of nodes. Since we are recursively returning new nodes with the set value, we can easily filter on which points should go into what quad, and create the nodes appropriately. For getting it may be worth looking into doing a depth-first-search to retrieve the relevant points within a given area instead of doing it one-by-one.
*   The GUI could use a lot of additions - specifying the current speed and zoom level, the current position of the camera, etc.
*   The SDL application could use some further improvements - such as allowing for quicker movement, jumping to points, etc.
*   Investigate garbage-collection to prevent an massive cache. I did not do this as I didn't have a notion of ownership and the current implementation works - I've been able to run the Lidka pattern until it stablizes, run multiple glider guns, etc without a problem, but its still worth looking into. This would include changing the quadrant pointers to be shared pointers, and the value in the cache would be a weak pointer.
*   The InputParser is currently very liberal of input. A nice-to-have would be to validate input, and support common Game of Life files - .rle files, 1.05 .lif files and 1.06 .lif files. I didn't get to this with the time I had, and I didn't want to take the time I was using to write tests and find examples by doing string handling.
*   Generally cleanup the code. I think my implementation is pretty good as is, but I am sure there are improvements that could be made.
*   Improve the tests and increase code coverage. Most of the tests were written to validate the behavior after I wrote a specific method, or to test a bug I had encountered, which is why they may seem kind of over the place. I could take some time to clean these up, but since they were alerting me to issues I was having, they served their purpose and a cleanup would be warranted after the above todos.


## Resources Used

A lot of the following resources were used in searching to solve specific problems. The first three books were purchased and read throughout the week before tackling the problem. I didn't have the opportunity to finish any of these books, but I did use them to look up how to do things and how things worked.

The two blogs and Gosper paper were referenced once I decided to use a QuadTree to solve the issue of an arbitrary board. As I ran into performance and implementation issues my search lead me to Hashlife, and my implementation uses an implementation of this algorithm.

Stack Overflow, Herb Sutter's blog, and Dr. Dobbs were also referenced heavily as I was learning how to do specific things in C++. Here's a short list of the resources I remembered to link:

Accelerated C++, Andrew Koeing & Barbara E. Moo

Effective C++, Scott Meyers

Effective Modern C++, Scott Meyers

[CPlusPlus.com](http://www.cplusplus.com/)

[Conway's Game of Life wikipedia](https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life)

[Hashlife Wikipedia](https://en.wikipedia.org/wiki/Hashlife)

[An Algorithm for Compressing Space and Time](http://drdobbs.com/high-performance-computing/184406478)

[Lowely Programmer - Hashlife](http://www.thelowlyprogrammer.com/2011/05/game-of-life-part-2-hashlife.html)

[Jenny Owen's Hashlife article](http://jennyhasahat.github.io/hashlife.html)

[Exploiting Regularities in Large Cellular Spaces](https://www.lri.fr/~filliatr/m1/gol/gosper-84.pdf)

[Compress split tokens on string](http://stackoverflow.com/questions/15690389/how-to-use-boost-split-to-split-a-string-and-ignore-empty-values)

[Conway Life Wiki](http://www.conwaylife.com/wiki)

[Best Way To Trim StdString](http://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring)

[Herb Sutter on const](https://herbsutter.com/2013/05/28/gotw-6b-solution-const-correctness-part-2/)

[Herb Sutter's Smart Cache](http://stackoverflow.com/questions/24379786/how-to-implement-cache-manager-using-stdshared-ptr)
