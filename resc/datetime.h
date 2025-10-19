#ifndef DATETIME_H
#define DATETIME_H

#include <string>
#include <ctime>


namespace dt {







using namespace std;

string today();
string add_days(const string &yyyy_mm_dd, int n);
int compare(const string &a, const string &b);
bool same_day(const string &a, const string &b);
bool in_last_n_days(const string &d, int n, const string &ref = "");
bool in_last_month(const string &d, const string &ref = "");

}

#endif // DATETIME_H