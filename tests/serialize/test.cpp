#define BOOST_TEST_MODULE test_serialize
#include <boost/test/included/unit_test.hpp>
#include "../../serialize/serialize.h"
#include <rttr/registration>
#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <memory>
using namespace rttr;
using namespace io::serialize;
using std::cout;
using std::endl;
using std::vector;
using std::map;
using std::string;
using std::shared_ptr;
using std::make_shared;

struct Sub
{
    string str;
    bool b;
    double d;
};

struct Sub2
{
    vector<int> ints;
};

struct Sub3
{
    map<int, int> imap;
    map<string, int> smap;
    map<Sub*, int> cmap;
};

struct Sample
{
    vector<Sub> subs;
    Sub sub;
    Sub2* sub2;
    shared_ptr<Sub3> sub3;
};

struct Data
{
    int a;
};
struct Container
{
    shared_ptr<Data> data;
    vector<shared_ptr<Data>> datas;
};

RTTR_REGISTRATION
{
    registration::class_<Sub>("Sub")
	.constructor<>().
	property("str", &Sub::str).
	property("b", &Sub::b).
	property("d", &Sub::d);

    registration::class_<Sub2>("Sub2")
	.constructor<>()
	.property("ints", &Sub2::ints);

    registration::class_<Sub3>("Sub3")
	.constructor<>()
	.property("imap", &Sub3::imap)
	.property("smap", &Sub3::smap)
	.property("cmap", &Sub3::cmap);

    registration::class_<Sample>("Sample")
	.constructor<>()
	.property("subs", &Sample::subs)
	.property("sub", &Sample::sub)
	.property("sub2", &Sample::sub2)
	.property("sub3", &Sample::sub3);

    registration::class_<Data>("Data")
	.constructor<>()
	.property("a", &Data::a);

    registration::class_<Container>("Container")
	.constructor<>()
	.property("data", &Container::data)
	.property("datas", &Container::datas);
}

BOOST_AUTO_TEST_CASE(base_serialize)
{
    Sub sub;
    sub.str = "sample";
    sub.b = true;
    sub.d = .1;

    auto str = serialize(sub);
    cout << "Sub: " << str << endl;
    Sub tmp;
    deserialize(str, tmp);

    BOOST_REQUIRE_EQUAL("sample", tmp.str);
    BOOST_REQUIRE_EQUAL(true, tmp.b);
    BOOST_REQUIRE_EQUAL(.1, tmp.d);
}

BOOST_AUTO_TEST_CASE(seq_serialize)
{
    Sub2 sub2;
    sub2.ints.push_back(1);
    sub2.ints.push_back(2);

    auto str = serialize(sub2);
    cout << "Sub2: " << str << endl;
    Sub2 tmp;
    deserialize(str, tmp);
    for(auto i : tmp.ints) cout << i << " ";
    cout << endl;
    
    BOOST_REQUIRE_EQUAL(2, tmp.ints.size());
    BOOST_REQUIRE_EQUAL(1, tmp.ints[0]);
    BOOST_REQUIRE_EQUAL(2, tmp.ints[1]);
}

BOOST_AUTO_TEST_CASE(map_serialize)
{
    Sub sub;
    sub.str = "hello";
    sub.b = false;
    sub.d = 2.3;

    Sub3 sub3;
    sub3.imap[1] = 2;
    sub3.imap[2] = 3;
    sub3.smap["a"] = 1;
    sub3.smap["b"] = 2;
    sub3.cmap[&sub] = 1;

    auto str = serialize(sub3);
    cout << "Sub3: " << str << endl;
}

BOOST_AUTO_TEST_CASE(Test)
{
    Sub sub;
    sub.str = "hello";
    sub.b = false;
    sub.d = 2.3;

    Sub2 sub2;
    sub2.ints.push_back(1);
    sub2.ints.push_back(2);

    Sub3 sub3;
    sub3.imap[1] = 2;
    sub3.imap[2] = 3;
    sub3.smap["a"] = 1;
    sub3.smap["b"] = 2;
    sub3.cmap[&sub] = 1;

    Sample sample;
    sample.subs.emplace_back(sub);
    sample.sub.str = "in";
    sample.sub.b = true;
    sample.sub.d = 3.3;
    sample.sub2 = &sub2;
    shared_ptr<Sub3> psub3 = std::make_shared<Sub3>();
    psub3->imap[4] = 22;
    psub3->imap[51] = 3;
    psub3->smap["asda"] = 1;
    psub3->smap["bgsag"] = 2;
    psub3->cmap[&sub] = 1;
    sample.sub3 = psub3;

    string sub_str = serialize(sub);
    cout << "\nSub:\n" << sub_str << endl;
    string sub2_str = serialize(sub2);
    cout << "\n\nSub2:\n" << sub2_str << endl;
    string sub3_str = serialize(sub3);
    cout << "\n\nSub3:\n" << sub3_str << endl;
    string psub3_str = serialize(*psub3);
    cout << "\n\nPSub3:\n" << psub3_str << endl;
    string parsed = serialize(sample);
    cout << "\n\nSample:\n" << parsed << endl;
	
    BOOST_TEST(1 == 1);
    BOOST_TEST(true);

}

BOOST_AUTO_TEST_CASE(DeserializeTest)
{
    Sub sub_sample = {"sample", false, 0.3};
    auto sub_json = serialize(sub_sample);
    Sub sub;
    BOOST_REQUIRE(deserialize(sub_json, sub));
    BOOST_CHECK(!sub.b);
    BOOST_CHECK(sub.d == 0.3);
    BOOST_CHECK(sub.str == "sample");

    Sub2 sub2_sample;
    sub2_sample.ints.push_back(1);
    sub2_sample.ints.push_back(2);
    auto sub2_json = serialize(sub2_sample);
    Sub2 sub2;
    BOOST_REQUIRE(deserialize(sub2_json, sub2));
    BOOST_CHECK(sub2.ints.size() == 2);
    BOOST_CHECK(sub2.ints[0] == 1);
    BOOST_CHECK(sub2.ints[1] == 2);

    Sub3 sub3_sample;
    sub3_sample.imap[1] = 2;
    sub3_sample.imap[2] = 3;
    sub3_sample.smap["a"] = 1;
    sub3_sample.smap["b"] = 2;
    sub3_sample.cmap[&sub] = 1;
    auto sub3_json = serialize(sub3_sample);
    Sub3 sub3;
    BOOST_REQUIRE(deserialize(sub3_json, sub3));
    BOOST_CHECK(sub3.imap.size() == 2);
    BOOST_CHECK(sub3.imap[1] == 2);
    BOOST_CHECK(sub3.imap[2] == 3);
    BOOST_CHECK(sub3.smap.size() == 2);
    BOOST_CHECK(sub3.smap["a"] == 1);
    BOOST_CHECK(sub3.smap["b"] == 2);
    BOOST_CHECK(sub3.cmap.size() == 1);
}

BOOST_AUTO_TEST_CASE(test_shared_ptr_serialize)
{
    auto data = make_shared<Data>();
    auto data1 = make_shared<Data>();
    auto data2 = make_shared<Data>();
    auto data3 = make_shared<Data>();
    auto container = make_shared<Container>();
    Container cc;
    data->a = 0;
    data1->a = 1;
    data2->a = 2;
    data3->a = 3;
    cc.data = data;
    cc.datas.emplace_back(data1);
    cc.datas.emplace_back(data2);
    cc.datas.emplace_back(data3);
    container->data = data;
    container->datas.emplace_back(data1);
    container->datas.emplace_back(data2);
    container->datas.emplace_back(data3);

    auto j = serialize(cc);
    auto j2 = serialize(*container);
    cout << j << endl;
    cout << j2 << endl;
}


BOOST_AUTO_TEST_CASE(test_rttr)
{
    auto sp = make_shared<Data>();
    const instance& inst = sp;
    cout << "sp is wrapper: " << inst.get_type().get_raw_type().is_wrapper() << endl;
    const auto& rinst = inst.get_wrapped_instance();
    cout << "real type: " << rinst.get_type().get_name() << ": " << rinst.get_type().is_pointer() << endl;

    vector<shared_ptr<Data>> sps;
    const instance& vinst = sps;
    cout << "sps vinst is seq: " << vinst.get_type().is_sequential_container() << endl;

    Container c;
}
