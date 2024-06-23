# Pencil

a simple text editor written in C with ncurses

the text editor consists of 2 main parts:
- the actual text
- the commands

you can move around the cursor in the text using the arrow keys

to modify the text use any of these commands:
- ``/b``: this is the command for backspace. you can put a number directly after it to remove multiple characters at once, such as: ``/b3`` to remove 3 characters.
- ``/n``: this command creates a newline at the cursor position, can take a number for amount of newlines to add.
- ``/w``: this command adds a space at the cursor position, can also take a number for amount of spaces.
- ``/t``: this command adds a tab, or well, 4 spaces. can also take a number.
- ``/q``: this command quits the pencil editor
- ``/s``: this command saves the file
- ``/r``: this command moves the cursor right, can take a number
- ``/l``: this command moves the cursor left, can take a number
- ``/u``: this command moves the cursor up, can take a number
- ``/d``: this command moves the cursor down, can take a number

anything that does not start with any of these commands will simply be added at the cursor as regular text.

## Known Issues
- can't save as another name
- pencil currently does not handle tabs correctly
