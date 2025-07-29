#include "utils.h"
#include <ctime>

std::string get_timestamp() {
    time_t now = time(nullptr);
    char buf[100];
    ctime_s(buf, sizeof(buf), &now);
    buf[strcspn(buf, "\n")] = '\0';
    return std::string(buf);
}