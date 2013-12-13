#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/test/unit_test.hpp>

#include "util.h"

BOOST_AUTO_TEST_SUITE(getarg_tests)

static void
ResetArgs(const std::string& strArg)
{
    std::vector<std::string> vecArg;
    boost::split(vecArg, strArg, boost::is_space(), boost::token_compress_on);

    // Insert dummy executable name:
    vecArg.insert(vecArg.begin(), "testbitcoin");

    // Convert to char*:
    std::vector<const char*> vecChar;
    BOOST_FOREACH(std::string& s, vecArg)
        vecChar.push_back(s.c_str());

    ParseParameters(vecChar.size(), &vecChar[0]);
}

BOOST_AUTO_TEST_CASE(boolarg)
{
    ResetArgs("-bau");
    BOOST_CHECK(GetBoolArg("-bau"));
    BOOST_CHECK(GetBoolArg("-bau", false));
    BOOST_CHECK(GetBoolArg("-bau", true));

    BOOST_CHECK(!GetBoolArg("-fo"));
    BOOST_CHECK(!GetBoolArg("-fo", false));
    BOOST_CHECK(GetBoolArg("-fo", true));

    BOOST_CHECK(!GetBoolArg("-bauo"));
    BOOST_CHECK(!GetBoolArg("-bauo", false));
    BOOST_CHECK(GetBoolArg("-bauo", true));

    ResetArgs("-bau=0");
    BOOST_CHECK(!GetBoolArg("-bau"));
    BOOST_CHECK(!GetBoolArg("-bau", false));
    BOOST_CHECK(!GetBoolArg("-bau", true));

    ResetArgs("-bau=1");
    BOOST_CHECK(GetBoolArg("-bau"));
    BOOST_CHECK(GetBoolArg("-bau", false));
    BOOST_CHECK(GetBoolArg("-bau", true));

    // New 0.6 feature: auto-map -nosomething to !-something:
    ResetArgs("-nobau");
    BOOST_CHECK(!GetBoolArg("-bau"));
    BOOST_CHECK(!GetBoolArg("-bau", false));
    BOOST_CHECK(!GetBoolArg("-bau", true));

    ResetArgs("-nobau=1");
    BOOST_CHECK(!GetBoolArg("-bau"));
    BOOST_CHECK(!GetBoolArg("-bau", false));
    BOOST_CHECK(!GetBoolArg("-bau", true));

    ResetArgs("-bau -nobau");  // -bau should win
    BOOST_CHECK(GetBoolArg("-bau"));
    BOOST_CHECK(GetBoolArg("-bau", false));
    BOOST_CHECK(GetBoolArg("-bau", true));

    ResetArgs("-bau=1 -nobau=1");  // -bau should win
    BOOST_CHECK(GetBoolArg("-bau"));
    BOOST_CHECK(GetBoolArg("-bau", false));
    BOOST_CHECK(GetBoolArg("-bau", true));

    ResetArgs("-bau=0 -nobau=0");  // -bau should win
    BOOST_CHECK(!GetBoolArg("-bau"));
    BOOST_CHECK(!GetBoolArg("-bau", false));
    BOOST_CHECK(!GetBoolArg("-bau", true));

    // New 0.6 feature: treat -- same as -:
    ResetArgs("--bau=1");
    BOOST_CHECK(GetBoolArg("-bau"));
    BOOST_CHECK(GetBoolArg("-bau", false));
    BOOST_CHECK(GetBoolArg("-bau", true));

    ResetArgs("--nobau=1");
    BOOST_CHECK(!GetBoolArg("-bau"));
    BOOST_CHECK(!GetBoolArg("-bau", false));
    BOOST_CHECK(!GetBoolArg("-bau", true));

}

BOOST_AUTO_TEST_CASE(stringarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-bau", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-bau", "eleven"), "eleven");

    ResetArgs("-bau -bar");
    BOOST_CHECK_EQUAL(GetArg("-bau", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-bau", "eleven"), "");

    ResetArgs("-bau=");
    BOOST_CHECK_EQUAL(GetArg("-bau", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-bau", "eleven"), "");

    ResetArgs("-bau=11");
    BOOST_CHECK_EQUAL(GetArg("-bau", ""), "11");
    BOOST_CHECK_EQUAL(GetArg("-bau", "eleven"), "11");

    ResetArgs("-bau=eleven");
    BOOST_CHECK_EQUAL(GetArg("-bau", ""), "eleven");
    BOOST_CHECK_EQUAL(GetArg("-bau", "eleven"), "eleven");

}

BOOST_AUTO_TEST_CASE(intarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-bau", 11), 11);
    BOOST_CHECK_EQUAL(GetArg("-bau", 0), 0);

    ResetArgs("-bau -bar");
    BOOST_CHECK_EQUAL(GetArg("-bau", 11), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);

    ResetArgs("-bau=11 -bar=12");
    BOOST_CHECK_EQUAL(GetArg("-bau", 0), 11);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 12);

    ResetArgs("-bau=NaN -bar=NotANumber");
    BOOST_CHECK_EQUAL(GetArg("-bau", 1), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);
}

BOOST_AUTO_TEST_CASE(doubledash)
{
    ResetArgs("--bau");
    BOOST_CHECK_EQUAL(GetBoolArg("-bau"), true);

    ResetArgs("--bau=verbose --bar=1");
    BOOST_CHECK_EQUAL(GetArg("-bau", ""), "verbose");
    BOOST_CHECK_EQUAL(GetArg("-bar", 0), 1);
}

BOOST_AUTO_TEST_CASE(boolargno)
{
    ResetArgs("-nobau");
    BOOST_CHECK(!GetBoolArg("-bau"));
    BOOST_CHECK(!GetBoolArg("-bau", true));
    BOOST_CHECK(!GetBoolArg("-bau", false));

    ResetArgs("-nobau=1");
    BOOST_CHECK(!GetBoolArg("-bau"));
    BOOST_CHECK(!GetBoolArg("-bau", true));
    BOOST_CHECK(!GetBoolArg("-bau", false));

    ResetArgs("-nobau=0");
    BOOST_CHECK(GetBoolArg("-bau"));
    BOOST_CHECK(GetBoolArg("-bau", true));
    BOOST_CHECK(GetBoolArg("-bau", false));

    ResetArgs("-bau --nobau");
    BOOST_CHECK(GetBoolArg("-bau"));

    ResetArgs("-nobau -bau"); // bau always wins:
    BOOST_CHECK(GetBoolArg("-bau"));
}

BOOST_AUTO_TEST_SUITE_END()
