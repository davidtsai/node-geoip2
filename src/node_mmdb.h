
#include <maxminddb.h>
#include <node.h>
#include <v8.h>
#include <uv.h>
#include "nan.h"

using namespace v8;

class NodeMMDB : public node::ObjectWrap {

public:

    NodeMMDB(const std::string &filePath);
    ~NodeMMDB();

    static NAN_METHOD(New);
    // static NAN_METHOD(Lookup);

protected:

    MMDB_s _mmdb;
};
