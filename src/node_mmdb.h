
#include <maxminddb.h>
#include <node.h>
#include <v8.h>
#include <uv.h>
#include "nan.h"

using namespace v8;

class NodeMMDB : public Nan::ObjectWrap {

public:

    NodeMMDB(const std::string &filePath);
    virtual ~NodeMMDB();

    static NAN_METHOD(New);
    static NAN_METHOD(Lookup);
    static NAN_METHOD(LookupSync);

protected:

    MMDB_s _mmdb;

    friend class NodeMMDBWorker;
};

class NodeMMDBWorker : public Nan::AsyncWorker {

public:
    NodeMMDBWorker(Nan::Callback *callback, NodeMMDB *mmdb, const std::string &lookupStr);

    void Execute() override;
    void HandleOKCallback() override;
    void Destroy() override;

protected:
    NodeMMDB *mmdb;
    std::string lookupStr;
    MMDB_entry_data_list_s *resultList;
};
