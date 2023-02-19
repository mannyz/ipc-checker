#include <cstring>

#include <argp.h>

#include "run/as/analyzer.hpp"
#include "run/as/consumer.hpp"
#include "run/as/producer.hpp"
#include "run/as/run_test.hpp"
#include "run/types.hpp"

namespace {
namespace arguments {

char documentation[] =
    IPC_CHECKER_PROGRAM_NAME " -- a simple hometool for measuring IPC";

char usage[] = "<target> <target's settings>";

argp_option options[] = {
    {0, 0, 0, 0, "Target:"},
    {"analyzer", 0, 0, OPTION_DOC | OPTION_NO_USAGE,
     "Analyzer got tick measurements from Producer/Consumer and performs "
     "analysis"},
    {"consumer", 0, 0, OPTION_DOC | OPTION_NO_USAGE,
     "Consumer receives messages from the producer"},
    {"producer", 0, 0, OPTION_DOC | OPTION_NO_USAGE,
     "Producer generate and sends messages to consumer"},
    {"run_test", 0, 0, OPTION_DOC | OPTION_NO_USAGE,
     "Run test with measurements. For this purpose,"
     " all measurement participants (producer, consumer and analyzer) will be "
     "launched locally."},
    {0}};

struct Arguments {
  Mode mode;
};

static error_t parser(int key, char *arg, argp_state *state) {
  switch (key) {
  case ARGP_KEY_ARG: {
    auto *arguments = static_cast<Arguments *>(state->input);
    arguments->mode = Convert({arg, strlen(arg)}, Mode{});
    state->next = state->argc;
  } break;
  case ARGP_KEY_NO_ARGS:
    argp_usage(state);
    break;
  case ARGP_KEY_END:
    break;
  case ARGP_KEY_ERROR:
    break;
  default:
    break;
  }
  return 0;
}

argp definition = {options, parser, usage, documentation};

} // namespace arguments
} // namespace

const char *argp_program_version = IPC_CHECKER_PROGRAM_VERSION;
const char *argp_program_bug_address = IPC_CHECKER_PROGRAM_BUG_ADDRESS;

int main(int argc, char *argv[]) {
  arguments::Arguments arguments;
  argp_parse(&arguments::definition, argc, argv, ARGP_IN_ORDER, 0, &arguments);

  switch (arguments.mode) {
  case Mode::Producer:
    return runAsProducer(argc, argv);
  case Mode::Consumer:
    return runAsConsumer(argc, argv);
  case Mode::Analyzer:
    return runAsAnalyzer(argc, argv);
  case Mode::RunTest:
    return runAsRunTest(argc, argv);
  default:
    break;
  }

  return 0;
}