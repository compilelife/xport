#include "media_request.h"
#include "httplib.h"

using namespace std;
using namespace httplib;

namespace xport
{

MediaRequest::MediaRequest(MediaRequestImpl* impl)
    :mImpl(impl){
}

MediaRequest::~MediaRequest(){}

static vector<string> split(const string& str, const string& pattern){
    string::size_type pos = 0;
    vector<string> result;
    auto s = str.substr(1)+"/";

    while (pos < s.size()){
        auto last = pos;
        pos = s.find(pattern, last);
        if (pos == string::npos)
            break;
        result.push_back(s.substr(last, pos - last));
        pos+=1;
    }
    
    return result;
}

vector<string> MediaRequest::segments(){
    auto req = (Request*)mImpl;
    return split(req->path, "/");
}

multimap<string, string> MediaRequest::params(){
    auto req = (Request*)mImpl;
    return req->params;
}

bool MediaRequest::hasParam(const char *key){
    auto req = (Request*)mImpl;
    return req->has_param(key);
}

string MediaRequest::getParamValue(const char *key, size_t id){
    auto req = (Request*)mImpl;
    return req->get_param_value(key, id);
}

size_t MediaRequest::getParamValueCount(const char *key){
    auto req = (Request*)mImpl;
    return req->get_param_value_count(key);
}

} // namespace xport
