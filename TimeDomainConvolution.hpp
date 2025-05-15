#include <iterator>  // distance, reverse_iterator, iterator_traits
#include <algorithm> // fill
#include <stdexcept> // invalid_argument
#include <tuple>

#include <cstddef>   // ptrdiff_t

// Time-domain convolution template.
// All arguments are begin() and end() iterators of the two input sequences and
//   the output sequence. (For C arrays and manually-allocated arrays, replace
//   the begin() iterator with the array pointer, and the end() iterator with a
//   pointer to the element one past the end of the array; see the test code.)
// The size of the output sequence must be one less than the sum of the sizes of
//   the two input sequences. This means that the last argument (zZ) currently
//   is redundant -- but it may not stay that way; I might add the option to
//   output only certain coefficients.
// Returns an iterator to the first non-leading output lag.

// See the end of this file for more programming notes.

template <typename XIt,    // Iterator type of first input sequence
          typename YIt,    // Iterator type of second input sequence
          typename ZIt>    // Iterator type of output sequence
          requires std::bidirectional_iterator<XIt> &&
                   std::forward_iterator<YIt> &&
                   std::forward_iterator<ZIt>
ZIt convolve_time( XIt x0, // begin() of first input sequence
                   XIt xX, // end() of first input sequence
                   YIt y0, // begin() of second input sequence
                   YIt yY, // end() of second input sequence
                   ZIt z0, // begin() of output sequence
                   ZIt zZ) // end() of output sequence
{
  // Get X and Y, the lengths of the two input sequences, and Z, the length of
  //   the output sequence. To add and subtract these quantities, we need to
  //   know their common difference type, which should be something like
  // 
  // using XYZ_dt = std::common_type<std::iterator_traits<XIt>::difference_type,
  //                                 std::iterator_traits<YIt>::difference_type,
  //                                 std::iterator_traits<ZIt>::difference_type
  //                                >::type;
  // 
  // The above looks sensible, but (knowing C++) it probably has some sort of
  //   counterintuitive, unanticipatable flaw. So, for now, ptrdiff_t should be
  //   a safe alternative (at least until I can read up on this a bit more).
 
  using XYZ_dt = ptrdiff_t;

  XYZ_dt X = std::distance(x0, xX); // Length of first input sequence
  XYZ_dt Y = std::distance(y0, yY); // Length of second input sequence
  XYZ_dt Z = std::distance(z0, zZ); // Length of output sequence

  // Argument checks. There are others that could be run here, e.g. whether
  //   any of X, Y, and Z overflow the limits of the XYZ_dt type.

  XYZ_dt zer0 = 0; // so we don't have to keep saying ptrdiff_t(0)

  if ((X == zer0) || (Y == zer0))
    throw std::invalid_argument("Input sequence has zero length");

  if ((X < zer0) || (Y < zer0) || (Z < zer0))
    throw std::invalid_argument("End pointer argument unreachable from begin "
                                "pointer argument");
  if (Z != X+Y-1)
    throw std::invalid_argument("Output range has wrong length");

  // Zero the output sequence.

  using z_vt = std::iterator_traits<ZIt>::value_type;

  std::fill(z0, zZ, z_vt(0)); // we're including <algorithm> only for this

  // L and H are the initial begin and end points of the convolution sum,
  //   expressed as indices on y (the second input sequence).
  // Normally, L is always 0, and H is always 1.
  // During development, I decided to see whether I could get the main loop to
  //   work (with no access errors) if one or the other (not both) of the two
  //   input series had zero length. These definitions of L and H do allow that.
  //   The output will be all-zeros, with length one less than that of the
  //   other, non-empty input sequence.
  // I had to disallow this because it caused problems for the return value.
  // If you want to play with it, replace the || in the zero-length input
  //   sequence check with && (note this invalidates the error message).
  // This will probably be taken out in the next revision.

  auto L = (zer0 > 1 - X)   // 1 if X is 0, else 0 (hence always 0 now)
         ?  zer0 : 1 - X;
  auto H = (Y < 1 ? Y : 1); // 0 if Y is 0, else 1 (hence always 1 now)

  // yL, yH, and xTmL are iterators to y[L], y[H], and x[T-L] (see below).
  
  auto yL = std::next(y0, L);
  auto yH = std::next(y0, H);
  auto xTmL = std::reverse_iterator(std::next(x0, 1-L));
  
  /*************\
   * MAIN LOOP *
  \*************/
  
  for (auto [T,zT]=std::tuple{zer0,z0};  T<Z;  ++T,++zT)
  {
    // For the current T, calculate z[T] = sum_{L}^{H-1} y[t] x[T-t].

    for (auto [yt,xTmt]=std::tuple{yL,xTmL};  yt!=yH;  ++yt,++xTmt)
    {
      *zT += static_cast<z_vt>( (*yt) * (*xTmt) );
    }
   
    // Update yL, yH, and xTmL for the next value of T.
    // (Earlier versions of the above sum used indices instead of iterators, so
    //   L and H were updated as well; the comments show where.)

    if (T >= X - 1)
      ++yL;   // ++L
    else
      --xTmL; // no changes to L or H

    if (T < Y - 1)
      ++yH;   // ++H
  }
  
  // Return a pointer to the first non-leading ("valid") lag.
  // [NB. This will need updating if we allow the user to bobtail the number of
  //   lags being returned in a revision.]
  
  return std::next(z0, (X < Y ? X : Y)-1);
}