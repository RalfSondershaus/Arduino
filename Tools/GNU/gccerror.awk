#
# gcc errors:         file.cpp:line:col: error: text
# output format MSVC: file.cpp(line,col): error text
#
# For MSVC error formatting, see
# https://docs.microsoft.com/de-de/cpp/build/formatting-the-output-of-a-custom-build-step-or-build-event?view=vs-2019
#
# Note: call of gensub (and substitute text snippets) doesn't work. MSVC prints "awk: " or "EXEC: " before file.cpp.
# So the following awk script does not work:
#
#  if ($0 ~ /(c[pp]*):([[:digit:]]*):([[:digit:]]*):/)
#  {
#    str = gensub(/(c[pp]*):([[:digit:]]*):([[:digit:]]*):/, "\\1(\\2,\\3)", "g", $0)
#  }
#  else
#  {
#    str = $0
#  }
#  print str
#

BEGIN {
  # Field separator: use ':' for gcc error messages
  FS = ":"
}
{
  if (NF > 3)
  {
    print $1 "(" $2 "," $3 "):" $4 ":" $5 " " $6 " " $7 " " $8 " " $9
  }
  else
  {
    print $0
  }
}
