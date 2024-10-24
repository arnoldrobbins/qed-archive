The 1980 USENIX Caltech _Qed_ distribution
==========================================

This is the 1980 Caltech distribution of _Qed_ containing the then
current _Qed,_ the University of Toronto _Ed,_ and some other
supporting programs.

It is available at the Unix Archive in [`Applications/Shoppa_Tapes/`][A].

[A]: https://www.tuhs.org/Archive/Applications/Shoppa_Tapes/

Inside the [`usenix_80_delaware.tar.gz`][B] tarball, the Caltech
distribution is located at `boulder/caltech`

[B]: https://www.tuhs.org/Archive/Applications/Shoppa_Tapes/usenix_80_delaware.tar.gz

The `caltech` directory is (confusingly) the top-level
of the distribution. Originally Warren Toomey had pointed
out the existence of the _Qed_
files in the `boulder/caltech/q` directory, apparently overlooking
the fact that the parent directory is the top-level of a complete _Qed_
distribution. Previously, the files in this part of the `qed-archive`
were simply the contents of `./q`.

Highlights
----------

The toplevel `read.me` file was written by Rob Pike, and contains
a useful roadmap of the contents of the distribution.

The development of _Qed_ and the University of Toronto _Ed_ were
intimately connected, and the UofT _Ed_ features extensively in
Rob Pike's _Qed Tutorial_. The enhancements that the
University of Toronto _Ed_ received are
documented in `./ed/ed.ut` (which is formatted in `troff`), in
addition to its own `ed(1)` manpage at `./doc/ed.1`.

Of particular interest is the then current version of the 
_Qed Tutorial_ at `./doc/tutorial` (also `troff`).

The version of `grep` included in the distribution (at `./ed/grep.c`)
had been tweaked so that its regular expression language included
the U of T _Ed_'s extensions. The initial comment header in
the `C` sourcecode notes:

    /*
     * grep -- print lines matching (or not matching) a pattern
     *
     * brutally attacked by Rob Pike Jan/80 to correspond to ed.
     */

Finally, comparison with Rob Pike's last (1992) version of _Qed_ (in
the `unix-1992` directory of this repository) should prove instructive.
The sources for this last version of _Qed_ are also in [_Research Unix Version 8_][U], though lacking all the interesting `read.me` files and _Qed_ programs.

[U]: https://www.tuhs.org/cgi-bin/utree.pl?file=V8/usr/src/cmd/qed

Changes
-------

For uploading to the `qed-archive`, I have removed the following precompiled
object files and directories mentioned in Rob's `read.me`:

* `./lib`
* `./bin/`
* `./objectfiles/`
* `./support/qedfile`

These were intended as a binary distribution for Version 6 Unix, and
would have been generated from the included sources.

#### Last Updated

Thu Oct 24 21:11:41 IDT 2024

