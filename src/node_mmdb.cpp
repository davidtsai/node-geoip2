
#include "node_mmdb.h"


NodeMMDB::NodeMMDB(const std::string &filePath) {

    memset(&_mmdb, 0, sizeof(MMDB_s));
    
    int status = MMDB_open(filePath.c_str(), MMDB_MODE_MMAP, &_mmdb);

    if (status != MMDB_SUCCESS) {
        throw std::runtime_error(std::string("Error opening MMDB: ") + MMDB_strerror(status));
    }
}

NodeMMDB::~NodeMMDB() {

}

NAN_METHOD(NodeMMDB::New) {

    NanScope();

    NodeMMDB *wrapper;
    if( args.Length() == 1 && args[0]->IsString()) {
        try {
            std::string filePath = *NanUtf8String(args[1]);
            wrapper = new NodeMMDB(filePath);
        }
        catch(const std::exception &e) {
            NanThrowError(e.what());
            NanReturnUndefined();
        }
    }
    else {
        NanThrowTypeError("Invalid arguments, expecting a string argument.");
        NanReturnUndefined();
    }

    wrapper->Wrap(args.Holder());
    NanReturnValue(args.Holder());
}

static void init(Handle<Object> exports) {

     /** MMDB **/
    Handle<FunctionTemplate> mmdbTpl =  NanNew<FunctionTemplate>(NodeMMDB::New);
    mmdbTpl->SetClassName(NanNew<String>("MMDB"));
    mmdbTpl->InstanceTemplate()->SetInternalFieldCount(1);

    exports->Set(NanNew<String>("MMDB"), mmdbTpl->GetFunction());
}

NODE_MODULE(node_mmdb, init)
