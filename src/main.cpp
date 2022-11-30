#include "levenshtein.hpp"

#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <utility>
#include <limits>

int results_[64];
const char* prg_name;

enum class Action {
  cutoff,
  compute,
  best,
  expecting_a_number,
  undecided
};

void clean(int* results) {
  if (results != results_) { free(results); }
}

template<class ...Ts>
inline void debug_(const char* fmt, Ts&&... ts) {
  fprintf(stderr, fmt, std::forward<Ts>(ts)...);
  fflush(stderr);
}

void usage(FILE* file = stdout) {
  fprintf(file, "USAGE: \n"
      "    %s [[-c|--cutoff CUTOFF]|[--d|--distance]|[--best|-b]] SOURCE TARGETS...\n\n"
      "    Compute the Levenshtein distance between SOURCE and each TARGET.\n\n"
      "OPTIONS\n\n"
      "    --help|-h)\n"
      "        Print this message and exit\n\n"
      "    --cutoff|-c)\n"
      "        Print only the TARGETs whose Levenshtein distance is less or equal to CUTOFF, separated by newline characters. A higher number means more tolerance.\n\n"
      "    --distance|-d)\n"
      "        Print the Levenshtein distance of each TARGET, separated by newline characters. This is the default.\n"
      "    --best|-b)\n"
      "        Print the best match among TARGETS. If there are several qualifying TARGETs, they are printed all separated by newlines.\n"
      , prg_name);
}

#ifdef DEBUG
  #define debug(...) debug_(__VA_ARGS__)
#else
  #define debug(...)
#endif

template<class ...Ts>
void print_error(const char* error, Ts&& ...params) {
  usage(stderr);
  fprintf(stderr, error, std::forward<Ts>(params)...);
}

int main(int argc, const char** argv) {
  prg_name=argv[0];

  debug("Starting\n");
  const char* source = nullptr;
  int source_len = 0;
  int cutoff = 2;
  int* results = [argc] {
    if (argc < 67) {
      return static_cast<int*>(results_);
    }
    return static_cast<int*>(malloc(sizeof(int) * (argc - 2)));
  }();
  debug("Allocation done\n");

  Action action = Action::undecided;

  for (int i = 1; i < argc; ++i) {
    const char* current_string = argv[i];
    debug("Current parameter: %s\n", current_string);

    if (action == Action::expecting_a_number) {
      debug("Processing number expectation\n");
      const char* ptr = &current_string[0];
      while (*ptr != 0) {
        if (*ptr < '0' || *ptr > '9') {
          print_error("Expected a positive number as parameter to --cutoff\n");
          clean(results);
          return 1;
        }
        ++ptr;
      }
      cutoff = atoi(current_string);
      action = Action::cutoff;
      debug("Cutoff updated to %i\n", cutoff);

      for(int j = 0; j < i - 3; ++j) {
        debug("Looking at stored number at %i: %i\n", j, results[j]);
        if (results[j] >= 0 && results[j] <= cutoff) {
          printf("%s\n", argv[j + 2]);
        }
      }
    }
    // This command argument is an option
    else if (current_string[0] == '-') {
      // Help, print usage and exit
      if (strcmp(current_string, "--help") == 0
          || strcmp(current_string, "-h") == 0) {
        usage();
        goto end;
      }
      // Cutoff, next parameter should be a number
      else if (strcmp(current_string, "--cutoff") == 0
          || strcmp(current_string, "-c") == 0) {
        debug("Processing 'cutoff' command\n");
        if (action != Action::undecided) {
          print_error("Please specify at most one action\n");
          clean(results);
          return 1;
        }

        action=Action::expecting_a_number;
      }
      // Distance, print all distances computed so far then resume
      else if (strcmp(current_string, "--distance") == 0
          || strcmp(current_string, "-d") == 0) {
        debug("Processing 'distance' command\n");
        if (action != Action::undecided) {
          print_error("Please specify at most one action\n");
          clean(results);
          return 1;
        }

        action=Action::compute;

        for (int j = 0; j < i - 2; ++j) {
          debug("Looking at stored number at %i: %i\n", j, results[j]);
          if (results[j] >= 0) {
            printf("%i\n", results[j]);
          }
        }
      } else if (strcmp(current_string, "--best") == 0
        || strcmp(current_string, "-b") == 0) {
        debug("Processing 'best' command\n");
        if (action != Action::undecided) {
          print_error("Please specify at most one action\n");
          clean(results);
          return 1;
        }
        action=Action::best;
        results[i - 2] = -1;
      }

      results[i - 2] = -1;
    }
    // Source is filled, this is a target
    else if (source != nullptr) {
      const int current_len = strlen(current_string);
      const int distance = levenshtein(source, source_len, current_string, current_len);
      debug("Computed distance for %s (len %i) / %s (len %i) -> %i\n", source, source_len, current_string, current_len, distance);
      if (action == Action::compute) {
        printf("%i\n", distance);
      }
      else if (action == Action::cutoff && distance <= cutoff) {
        printf("%s\n", current_string);
      }
      else {
        results[i - 2] = distance;
      }
    }
    // No source yet, this is it
    else {
      debug("Updating source -> %s\n", current_string);
      source = current_string;
      source_len = strlen(current_string);
      if (i > 1) {
        results[i - 2] = -1;
      }
    }
  } // End of for loop


  // If this triggers we've been either in the 'update source' or 'command' branch
  if (argc < 3) {
    print_error("Expecting at least 2 arguments\n");
    clean(results);
    return 1;
  }

  if (action == Action::undecided) {
    for (int i = 0; i < argc - 2; ++i) {
      printf("%i\n", results[i]);
    }
  } else if (action == Action::best) {
    int best = std::numeric_limits<int>::max();
    for (int i = 0; i < argc - 2; ++i) {
      debug("Checking best: %i (%s)", results[i], argv[i+2]);
      if (results[i] >= 0 && results[i] < best) {
        debug("   New best!");
        best = results[i];
      }
      debug("\n");
    }
    for (int i = 0; i < argc - 2; ++i) {
      if (results[i] == best) {
        printf("%s\n", argv[i + 2]);
      }
    }
  }

end: clean(results);
  return 0;
}
