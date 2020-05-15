# Diff Project
This is a recreation of the main functionality of the UNIX diff command.

Usage information: (taken from the --help command)
Usage: ./diff [OPTION]... FILE1 FILE2
Compare files line by line.

--help  Output this help.
-v  --version  Output version info.
--normal  Output a normal diff.
-q  --brief  Output only whether files differ.
-s  --report-identical-files  Report when two files are the same.
-y  --side-by-side  Output in two columns.
  --left-column  Output only the left column of common lines.
  --suppress-common-lines  Do not output common lines.

Not fully implimented yet:
-c [NUM]  --context[=NUM]  Output NUM (default 3) lines of copied context.
-u [NUM]  --unified[=NUM]  Output NUM (default 3) lines of unified context.
