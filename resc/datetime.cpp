#include "datetime.h"
#include <iomanip>
#include <sstream>

namespace dt {

static std::tm parse(const std::string &s) {
    std::tm tm{};
    std::istringstream iss(s);
    iss >> std::get_time(&tm, "%Y-%m-%d");
    return tm;
}

static std::string format(const std::tm &tm) {
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d");
    return oss.str();
}

std::string today() {
    std::time_t t = std::time(nullptr);
    std::tm tm{};
#if defined(_WIN32)
    localtime_s(&tm, &t);
#else
    tm = *std::localtime(&t);
#endif
    return format(tm);
}

std::string add_days(const std::string &yyyy_mm_dd, int n) {
    std::tm tm = parse(yyyy_mm_dd);
    std::time_t tt = std::mktime(&tm);
    tt += static_cast<long long>(n) * 24 * 60 * 60;
#if defined(_WIN32)
    std::tm out{};
    localtime_s(&out, &tt);
    return format(out);
#else
    return format(*std::localtime(&tt));
#endif
}

int compare(const std::string &a, const std::string &b) {
    if (a == b) return 0;
    return a < b ? -1 : 1; // lexicographic works for YYYY-MM-DD
}

bool same_day(const std::string &a, const std::string &b) {
    return compare(a, b) == 0;
}

bool in_last_n_days(const std::string &d, int n, const std::string &ref) {
    std::string r = ref.empty() ? today() : ref;
    std::string from = add_days(r, -(n - 1));
    return compare(d, from) >= 0 && compare(d, r) <= 0;
}

bool in_last_month(const std::string &d, const std::string &ref) {
    return in_last_n_days(d, 30, ref);
}

}
