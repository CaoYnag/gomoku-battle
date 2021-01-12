#include <iostream>
#include <string>
#include <curl/curl.h>
#include <boost/thread.hpp>
#include <ctime>
#include <vector>
using namespace std;
using namespace boost;

size_t data(void* buffer, size_t size, size_t nmemb, void* usrep)
{
    //do nothing
    return size * nmemb;
}

long getms(const timeval& tv)
{
    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

vector<thread> ths;
vector<long> costs;
vector<int> errs;
char* url = "www.bing.com";

void test_once(int id)
{
    CURL* handle = curl_easy_init();
    if(!handle)
    {
        cerr << "cannot init libcurl." << endl;
        curl_global_cleanup();
        exit(0);
    }
    curl_easy_setopt(handle, CURLOPT_URL, url);
    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, data);
    curl_easy_setopt(handle, CURLOPT_NOSIGNAL, 1l);
    timeval s, e;
    gettimeofday(&s, nullptr);
    CURLcode ret = curl_easy_perform(handle);
    gettimeofday(&e, nullptr);
    long epl = getms(e) - getms(s);
    if(ret == CURLE_OK)
        costs[id] += epl;
    else
        errs[id] += 1;
    curl_easy_cleanup(handle);
}

int users = 10;
int loops = 10;
int running = 0;
mutex guard;
int start = 1;
void test_user(int id)
{
    {
        lock_guard<mutex> lock(guard);
        ++running;
    }
    //sleep(1);
    while(start);//wait for other threads
    for(int i = 0; i < loops; ++i)
        test_once(id);
    lock_guard<mutex> lock(guard);
    --running;
}

void scene()
{
    cout << "starting scene...\t";
    for(int i = 0; i < ths.size(); ++i)
        ths[i].detach();
    cout << ths.size() << " users started." << endl;
}

void init()
{
    for(int i = 0; i < users; ++i)
    {
        errs.emplace_back(0);
        costs.emplace_back(0);
        ths.emplace_back(bind(test_user, i));
    }
    cout << ths.size() << " users ready." << endl;
}

void watch()
{
    while(running < users);//wait all threads start up
    start = 0;//all threads start to work
    while(running)
    {
        sleep(1);
        cout << running << "/" << ths.size() << " users running." << endl;
    }
    cout << "\nall users complete.";
    long cost = 0;
    for(auto c : costs)
        cost += c;
    int err = 0;
    for(auto e : errs)
        err += e;
    int total = users * loops;
    int succ = total - err;
    cout << err << " fail, " << ((err * 100) / (total + 0.0f)) << "%.";
    if(succ > 0)
        cout << " avg cost: " << (cost / (succ + 0.0f)) << "ms.";
    cout << endl;
}

int main(int argc, char* argv[])
{
    if(argc >= 2)
        users = atoi(argv[1]);
    if(argc >= 3)
        loops = atoi(argv[2]);
    if(argc >= 4)
        url = argv[3];
    cout << "using configuration: " << users << " users loops " << loops << " times to request " << url << endl;
    curl_global_init(CURL_GLOBAL_ALL);
    init();
    scene();
    watch();
    curl_global_cleanup();
    return 0;
}