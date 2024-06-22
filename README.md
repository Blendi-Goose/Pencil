# Pencil

a simple text editor written in C with ncurses

the text editor consists of 2 main parts:
- the actual text
- the commands

you can move around the cursor in the text using the arrow keys

to modify the text use any of these commands:
- ``/b``: this is the command for backspace. you can put a number after it to remove multiple characters at once, such as: ``/b3`` to remove 3 characters.
- ``/n``: this command creates a newline at the cursor position.
- ``/s``: this command adds a space at the cursor position, can also take a number.
- ``/t``: this command adds a tab, or well, 4 spaces. can also tak ea number.
- ``/q``: this command quits the pencil editor
- ``/s``: this command saves the file

anything that does not start with any of these commands will simply be added at the cursor as regular text.

## Known Issues
can't save as another name
can't scroll to the right on a big line