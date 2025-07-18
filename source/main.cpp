#include "simulation.hpp"

int main(int argc, char* argv[]) {
  // MouzeSimulation is a singleton.
  MouzeSimulation::instance().initialize(argc, argv); 
  MouzeSimulation::instance().run_tests();
  // The Game Loop.
  while (not MouzeSimulation::instance().is_over()) {
    MouzeSimulation::instance().process_events();
    MouzeSimulation::instance().update();
    MouzeSimulation::instance().render();
  }


  return 0;
}

