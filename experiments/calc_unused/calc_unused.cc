#include <array>
#include <map>


int main(int argc, char *argv[])
{
  if (argc <= 2) {
    printf("usage %s <log_filename> <intervals_filename>\n", argv[0]);
    return 0;
  }

  auto log_filename = argv[1];
  auto intervals_filename = argv[2];
  std::map<double, bool> log;
  
  {
    char c;
    double d;

    FILE* log_file = fopen(log_filename, "r");

    while(fscanf(log_file, "%c %lf", &c, &d) != EOF) {
      log[d] = false;
    }
    fclose(log_file);
  }
  
  {
    FILE* intervals_file = fopen(intervals_filename, "r");

    std::array<double, 2> d;
    while(fscanf(intervals_file, "%lf %lf", d.data(), d.data() + 1) != EOF) {
      auto it_lower = log.lower_bound(d[0]);
      auto it_upper = log.upper_bound(d[1]);
      it_lower--;
      while (it_lower != it_upper) {
        it_lower->second = true;
        it_lower++;
      }
      it_upper->second = true;
    }

    fclose(intervals_file);
  }

  {
    std::size_t used_events = 0;
    for (const auto &p: log) {
      if (p.second) {
        used_events++;
      }
    }
    printf("total events: %ld used events: %ld used rate: %lf\n", log.size(), used_events, used_events / static_cast<double>(log.size()));
  }

  return 0;
}
