#include <vector>
#include <list>
#include <forward_list>
#include <deque>
#include <array>
#include <iostream>
#include <iterator> // distance
#include <algorithm> // copy

#include <cstdio> // puts

#include "TimeDomainConvolution.hpp"

// Programs for testing the convolve_time template.

void printseq(auto begin, auto end)
{
  // Numerical sequence printer.

  std::cout << '[';
  for (auto si = begin; si != end; ++si)
    std::cout << *si << ' ';
  std::cout << ']';
}

template <typename XIt, typename YIt, typename ZIt>
void conv_both_ways(XIt xb, XIt xe, YIt yb, YIt ye, ZIt zb, ZIt ze)
{
  // Given inputs x and y, this calculates x*y and y*x, and prints both results
  //   to the screen.

  auto X = std::distance(xb, xe);
  auto Y = std::distance(yb, ye);
  auto Z = std::distance(zb, ze);

  std::cout << "X:" << X << " Y:" << Y << " Z:" << Z << "-->";
  
  convolve_time(xb, xe, yb, ye, zb, ze);

  printseq(zb, ze);

  std::cout << "\nY:" << Y << " X:" << X << " Z:" << Z << "-->";
  
  convolve_time(yb, ye, xb, xe, zb, ze);

  printseq(zb, ze);

  std::cout << '\n';
}

template <typename XIt, typename YIt, typename ZIt>
void iterator_combos(XIt x0, XIt xX, YIt y0, YIt yY, ZIt z0, ZIt zZ)
{
  // This template takes two iterator ranges [x0,xX) and [y0,yY), and runs all
  //   their possible sub-ranges through the two-way convolution routine.
  // This should turn up any weird access errors that might occur.
  // The awkward do-while structure is needed to allow xX and yY to appear as
  //   end-iterators of the sub-ranges without memory errors occurring (e.g. we
  //   can't compare xbeg to std::next(xX) because the latter is out of bounds).
  // Note also that we assume xX is reachable from x0, and so forth.

  auto xbeg = x0;
  bool xbegflag = false;
  do
  {
    auto xend = xbeg;
    bool xendflag = false;

    do
    {
      auto ybeg = y0;
      bool ybegflag = false;

      do
      {
        auto yend = ybeg;
        bool yendflag = false;

        do
        {
          auto X = std::distance(xbeg, xend);
          auto Y = std::distance(ybeg, yend);

          // To allow one input to have zero length, replace || with && here.

          if (!((X == 0) || (Y == 0)))
          {
            // Yes, I know that it would be more efficient to store some of
            //   these distances in local variables outside the loop.
            // But that would clutter the code and confuse the issue.
            // The purpose here is testing and clarity.

            std::cout << "x in [" << std::distance(x0,xbeg) << ',' 
                                  << std::distance(x0,xend) << "), "
                      << "y in [" << std::distance(y0,ybeg) << ','
                                  << std::distance(y0,yend) << "):\n";

            conv_both_ways(xbeg, xend, ybeg, yend,
                           z0, std::next(z0, X + Y - 1));
          }

          if (yend != yY)
            ++yend;
          else
            yendflag = true;
        } while (yendflag == false);

        if (ybeg != yY)
          ++ybeg;
        else
          ybegflag = true;
      } while (ybegflag == false);

      if (xend != xX)
        ++xend;
      else
        xendflag = true;
    } while (xendflag == false);

    if (xbeg != xX)
      ++xbeg;
    else
      xbegflag = true;
  } while (xbegflag == false);

}

int main()
{
  // A few initializer_lists, formed from two short time series 'x' and 'y'.
  // There are multiple versions of 'x' and 'y', with different value_types;
  //   this helps with brevity later on in the test code.
  // The initializer_lists are declared static constexpr to force initialization
  //   at compile time. This way, their size()s can be used to allocate C arrays
  //   and std::arrays later on in this script. (I am not an expert on 
  //   constexpr, and make no claim that this is the best way to do this.)

  static constexpr auto xi = { 4, 7, -3, 5 };
  static constexpr auto xs = { short(4), short(7), short(-3), short(5) };
  static constexpr auto xf = { 4.f, 7.f, -3.f, 5.f };
  static constexpr auto xd = { 4., 7., -3., 5. };
  static constexpr auto yf = { 6.f, -3.f, 0.f, 2.f, 4.f, 9.f };
  static constexpr auto yd = { 6., -3., 0., 2., 4., 9. };
  static constexpr auto yl = { 6L, -3L, 0L, 2L, 4L, 9L };

  constexpr auto xN = xi.size(); // Make sure xi/xs/xf/xd all have same length
  constexpr auto yN = yf.size(); // Same for yf/yd/yl

  using xi_t = decltype(xi)::value_type; // Long winded, yes... but may cut down
  using xs_t = decltype(xs)::value_type; //   on errors later, if the above
  using yf_t = decltype(yf)::value_type; //   lists are ever messed with
  using yd_t = decltype(yd)::value_type;
  using yl_t = decltype(yl)::value_type;

  // Test 1: vector<float> = vector<int> * list<double>
  // This one calls iterator_combos to check all the subrange combinations.
  // You can do this with the subsequent tests by changing conv_both_ways() to
  //   iterator_combos(), which has the same set of arguments.

  std::puts("=======\nTest 1:\n=======\n");

  std::vector<xi_t>  xvi{ xi };
  std::list  <yd_t>  yld{ yd };
  std::vector<float> zvf(xvi.size()+yld.size()-1, 0.f);
  
  iterator_combos(xvi.begin(), xvi.end(),
                  yld.begin(), yld.end(),
                  zvf.begin(), zvf.end());

  // Test 2: vector<double> = vector<short> * deque<float>

  std::puts("\n=======\nTest 2:\n=======\n");

  std::vector<xs_t>   xvs{ xs };
  std::deque <yf_t>   ydf{ yf };
  std::vector<double> zvd(xvs.size() + ydf.size() - 1, 0.);

  conv_both_ways(xvs.begin(), xvs.end(),
                 ydf.begin(), ydf.end(),
                 zvd.begin(), zvd.end());

  // Test 3: list<short> = deque<double> * array<long long>

  std::puts("\n=======\nTest 3:\n=======\n");

  std::list  <xs_t>    xls{ xs };
  std::array <yl_t,yN> yal;
  std::copy(yl.begin(), yl.end(), yal.begin());
  std::deque <double>  zdd(xls.size() + yal.size() - 1, 0.);

  conv_both_ways(xls.begin(), xls.end(),
                 yal.begin(), yal.end(),
                 zdd.begin(), zdd.end());

  // Test 4: vector<common<float,int>> = int[] * vector<float>
  
  std::puts("\n=======\nTest 4:\n=======\n");

  xi_t xci[xN];
  std::copy(xi.begin(), xi.end(), xci);
  
  std::vector<yf_t> yvf{ yf };

  using c_t = std::common_type<decltype(yvf)::value_type, xi_t>::type;

  std::vector<c_t> zvc(xN + yvf.size() - 1, c_t(0));

  conv_both_ways(xci, xci + xN, yvf.begin(), yvf.end(), zvc.begin(), zvc.end());
  
  // Test 5: float[] = initializer_list<int> * initializer_list<float> 

  std::puts("\n=======\nTest 5:\n=======\n");

  constexpr auto zcfN = xN + yN - 1;
  float zcf[zcfN];
  
  conv_both_ways(xi.begin(), xi.end(), yf.begin(), yf.end(),
                 zcf, zcf + xi.size() + yf.size() - 1);

  // Test 6: Re-run of Test 3, with a forward_list<> in place of the list<>.
  // This version will not compile if you reverse the roles of 'xls' and 'yfd',
  //   because the convolution sum traverses the first input sequence backwards.
  //   Hence the first two arguments to convolve_time must be bidirectional
  //   iterators, which forward_list<> does not possess.
  // Here we also use the return value of convolve_time to print the valid
  //   range of output lags.

  std::puts("\n=======\nTest 6:\n=======\n");
  
  std::forward_list<yd_t> yfd(yld.begin(), yld.end());

  auto [valid_begin,valid_past_end]
  = convolve_time(xls.begin(), xls.end(),
                  yfd.begin(), yfd.end(),
                  zdd.begin(), zdd.end());
  
  printseq(zdd.begin(), zdd.end());
  std::cout << "\nValid range : \n";
  printseq(valid_begin, valid_past_end);
  std::cout << '\n';
}