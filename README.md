# Time domain convolution template (one-dimensional)

**template \<typename XIt, typename YIt, typename ZIt>**\
**std::pair<ZIt,ZIt>\
convolve_time\( XIt x0, XIt xX, YIt y0, YIt yY, ZIt z0, ZIt zZ )**

### Template parameters

The template parameters XIt, YIt, ZIt are all iterator types.\
XIt must satisfy the requirements of a bidirectional iterator.\
YIt and ZIt must satisfy the requirements of a forward iterator.

### Arguments

All six arguments are begin and past-end iterators.\
x0 and xX are the begin and past-end iterators of the first input sequence.\
y0 and yY are the begin and past-end iterators of the second input sequence.\
z0 and zZ are the begin and past-end iterators of the output sequence.

The sizes of both input sequences must be positive.\
Their sum must be one greater than the size of the output sequence.

If any input or output sequences are C arrays or dynamically allocated arrays,
  then the begin and past-end iterators of those sequences should be replaced
  in the argument list with the array pointer, and with a pointer to the first
  element past the end of the array.

## Return value

Returns a pair of iterators to the first and past-last "valid" elements of the
  output sequence, i.e. the bounding elements of the part of the output sequence
  that can be obtained without zero padding.
  (The number of valid elements is one greater than the absolute difference of
  the lengths of the input sequences.)

## Programming notes:

1. I have had to rely on error messages from Visual Studio libraries for memory
     error checking in this project. Normally, Valgrind is my tool of choice,
     but I don't have access to a Linux machine at the moment. I'll try to
     revisit this point once I get a bit more Visual Studio experience.
2. There is no check for narrowing conversions in the convolution sum.
     This did throw a warning once during testing; hence the static_cast.
3. This template is my first attempt at including iterator requirements.
     They should be correct, but, no promises.
4. Todo: There is no check that the input and output sequences are arithmetic.
     This could be turned into another requirement; or, perhaps a static_assert
     would be more appropriate. Will have to mull over this.
5. I have not tested all container types. Most of the ones that haven't been
     tested would probably fail to compile (and should probably be disallowed
     if they did).