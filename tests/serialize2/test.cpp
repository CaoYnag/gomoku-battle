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

struct Data
{
	int a;
};
struct Container
{
	shared_ptr<Data> data;
	vector<shared_ptr<Data>> datas;

	Container()
	{
		data = make_shared<Data>();
	}
};

struct NormalContainer
{
	Data a;
	vector<Data> b;
};

struct PointerContainer
{
	Data* a;
	vector<Data*> b;
};

RTTR_REGISTRATION
{
	registration::class_<Data>("Data")
		.constructor<>()
		.property("a", &Data::a);

	registration::class_<Container>("Container")
		.constructor<>()
		.property("data", &Container::data)
		.property("datas", &Container::datas);

	registration::class_<NormalContainer>("NormalContainer")
		.constructor<>()
		.property("a", &NormalContainer::a)
		.property("b", &NormalContainer::b);

	registration::class_<PointerContainer>("PointerContainer")
		.constructor<>()
		.property("a", &PointerContainer::a)
		.property("b", &PointerContainer::b);
}


int main()
{
	cout << "--------normal container--------" << endl;
	NormalContainer nc;
	nc.a.a = 0;
	nc.b.resize(3);
	nc.b[0].a = 1;
	nc.b[1].a = 2;
	nc.b[2].a = 3;
	auto ncj = serialize(nc);
	cout << ncj << endl;
	NormalContainer dnc;
	deserialize(ncj, dnc);
	cout << dnc.a.a << " : " << dnc.b.size() << endl;
	for(auto& d : dnc.b)
		cout << d.a << " ";
	cout << endl;

	cout << "--------pointer container--------" << endl;
	PointerContainer pc;
	pc.a = new Data();
	pc.a->a = 0;
	pc.b.resize(3);
	pc.b[0] = new Data();
	pc.b[0]->a = 1;
	pc.b[1] = new Data();
	pc.b[1]->a = 2;
	pc.b[2] = new Data();
	pc.b[2]->a = 3;
	auto pcj = serialize(pc);
	cout << pcj << endl;
	PointerContainer dpc;
	dpc.a = new Data();
	dpc.b.resize(3);
	deserialize(pcj, dpc);
	cout << dpc.a->a << " : " << dpc.b.size() << endl;
	for(auto& d : dpc.b)
		cout << d->a << " ";
	cout << endl;

	cout << "--------shared ptr container--------" << endl;
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
	//auto j2 = serialize(*container);
	auto j3 = serialize(container);
	cout << j << endl;
	//cout << j2 << endl;
	cout << j3 << endl;

	auto sp = make_shared<Data>();
	const instance& inst = sp;
	cout << "sp is wrapper: " << inst.get_type().get_raw_type().is_wrapper() << endl;
	const auto& rinst = inst.get_wrapped_instance();
	cout << "real type: " << rinst.get_type().get_name() << ": " << rinst.get_type().is_pointer() << endl;

	vector<shared_ptr<Data>> sps;
	const instance& vinst = sps;
	cout << "sps vinst is seq: " << vinst.get_type().is_sequential_container() << endl;


	Container dc;
	shared_ptr<Container> dsp = make_shared<Container>();
	deserialize(j, dc);

	cout << dc.datas.size() << endl;

	for(auto& d : dc.datas)
		cout << d->a << " ";
	cout << endl;
	//deserialize(j3, dsp);
	// cout << dsp->datas.size() << endl;
	// for(auto& d : dsp->datas)
	// 	cout << d->a << " ";
	// cout << endl;

	return 0;
}