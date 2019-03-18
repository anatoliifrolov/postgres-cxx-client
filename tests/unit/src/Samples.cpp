#include "Samples.h"

std::string postgres::makeTimeStrSampleNanoTz() {
    std::string res(64, 0);
    auto        uni = TIME_SAMPLE;
    tm          parts{};
    res.resize(strftime(&res[0], res.size(), "%FT%T.987654321 %z", localtime_r(&uni, &parts)));
    return res;
}
