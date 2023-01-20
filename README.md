# TTracker [WIP]

TTracker is a time tracker for those doing their work from the command line.
Sometimes, when working for pay, your employer/customer want's to know how you spend your time.
This is an extremely difficult task for programmers. Luckily, programmers like command line tools.
TTracker is ment for easy time tracking via the command line.

## Usage

Use `ttracker --help` to print available commands.

## Building
run `cmake -S . -B. & make` to build.

Requires sqlite3 and fmt development libraries.

## Installing
`sudo make install`

Add the bash\_complete.sh text to your bashrc for some auto completion.

TODO: Improve the completion.

