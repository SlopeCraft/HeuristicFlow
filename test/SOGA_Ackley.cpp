
#include <Eigen/Dense>
#include <HeuristicFlow/Genetic>
#include <iostream>
#include <ctime>
using namespace Eigen;
using namespace std;

// Ackely function is a single objective testing function with great number of local minimum points, but its global
// minimum point is [0,0] and corresponding value is 0. A good solver should be able to find the global minimum point.
void testAckley_withRecord() {
  using args_t = heu::BoxNdS<2, heu::Std>;

  using solver_t = heu::SOGA<array<double, 2>, heu::FITNESS_LESS_BETTER, heu::RECORD_FITNESS, args_t,
                             heu::GADefaults<array<double, 2>, args_t, heu::Std>::iFunNd<>, nullptr,
                             heu::GADefaults<array<double, 2>, args_t, heu::Std>::cFunNd,
                             heu::GADefaults<array<double, 2>, args_t, heu::Std>::mFun_d<>>;
  solver_t algo;

  heu::GAOption opt;
  opt.populationSize = 50;
  opt.maxFailTimes = 50;  // The solver will end if it hasn't been finding a better solution for 50 generations.
  opt.maxGenerations = 100;

  algo.setOption(opt);

  {
    args_t args;
    args.setMin(-5);
    args.setMax(5);
    args.setLearnRate(0.05);
    algo.setArgs(args);
  }

  algo.setfFun(
      // Ackely function
      [](const array<double, 2>* _x, const solver_t::ArgsType*, double* f) {
        double x = _x->operator[](0), y = _x->operator[](1);
        *f = -20 * exp(-0.2 * sqrt(0.5 * (x * x + y * y))) - exp(0.5 * (cos(M_2_PI * x) + cos(M_2_PI * y))) + 20 + M_E;
      });

  algo.initializePop();

  std::clock_t t = std::clock();
  algo.run();
  t = std::clock() - t;
  // cout<<algo.bestFitness();

  cout << "Solving spend " << algo.generation() << " generations in " << double(t) / CLOCKS_PER_SEC << " sec\n";
  cout << "Result = [" << algo.result()[0] << " , " << algo.result()[1] << "]\n";

  cout << "Fitness history :\n";

  for (auto i : algo.record()) {
    cout << i << '\n';
  }
  cout << endl;
}

int main() {
  testAckley_withRecord();
  system("pause");
  return 0;
}
