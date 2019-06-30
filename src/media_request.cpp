#include "media_request.h"
#include "httplib.h"
#include <regex>

using namespace std;
using namespace httplib;

namespace xport
{

MediaRequest::MediaRequest(MediaRequestImpl* impl)
    :mImpl(impl){
}

MediaRequest::~MediaRequest(){}

vector<string> MediaRequest::segments(){
    auto req = (Request*)mImpl;
    
    regex delim("/");
    return vector<string>(sregex_token_iterator(req->path.begin()+1, req->path.end(), delim, -1), sregex_token_iterator());
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
