FMU UUID generator
==================
A small program that generates universally unique identifiers (UUID) for
functional mock-up units (FMU).

The program uses the contents of an FMU's model description file
(`modelDescription.xml`) to generate its UUID.  Any modifications to
the model description, however small, will cause the UUID to change
completely.  Thus, the UUID is kind of a "fingerprint" for the FMU.

If you don't know what an FMU is, you're probably in the wrong place, but
check out [the FMI page on viproma.no](http://viproma.no/doku.php?id=vpf:fmi)
in any case.

Issues and contributions
------------------------
Please use [GitHub](http://github.com/viproma/fmu-uuid) to report issues
and submit pull requests.

Usage
-----
To use the UUID generator, create your model description file
(`modelDescription.xml`) as usual, but just put a placeholder in the `guid`
attribute of the `<fmiModelDescription>` element, like this:

    <fmiModelDescription
        ...
        guid="@MY_UUID@"
        ...>

The placeholder can be anything you like, but the program will just perform
a dumb search-and-replace afterwards, so it should be a name which will
not occur anywhere else in the file.

The program will take the model description file as input, generate a
UUID based on its contents, and write two output files:

  * A new model description where the placeholder has been replaced with
    the actual UUID.

  * A C header file that defines a macro whose value is a string literal
    that contains the UUID.  This file you'll then include in your model
    code and use to verify the `guid` value supplied by the simulation
    environment.

The program is run like this:

    fmu-uuid <input-md> <placeholder> <output-md> <output-hdr> <macro-name>

Here, `<input-md>` is the path to the input model description file,
`<placeholder>` is the placeholder, `<output-md>` is the path to the output
model description file, `<output-hdr>` is the path to the output header file,
and `<macro-name>` is the name of the macro that will be defined in that
header.  If the output files don't exist they will be created, and if they
do exist they'll be overwritten.

Here is an example:

    fmu-uuid source/modelDescription.xml @MY_UUID@ output/modelDescription.xml output/my-uuid.h MY_UUID

Building
--------
The whole program is just one C++ source file, so you can easily build it
using manual compiler invocation if you like.  That said, we have provided
the means to build it using [CMake](https://cmake.org).

If you're not already familiar with CMake, just open a shell (aka. command
prompt) and run the following commands:

    cd path/to/fmu-uuid
    mkdir build
    cd build
    cmake ..
    cmake --build .

You'll need a fairly up-to-date C++ compiler (VS 2013 or GCC 4.9 will do),
and you'll need to have [Boost UUID](http://www.boost.org/doc/libs/release/libs/uuid/uuid.html)
installed.

Terms of use
------------
This software is subject to the terms of the 3-Clause BSD License.
