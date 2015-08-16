
#include "node_mmdb.h"

#include <stdexcept>
#include <sstream>

static Handle<Value> convertToV8Helper(MMDB_entry_data_list_s **entry_data_list) {

    switch ((*entry_data_list)->entry_data.type) {

        case MMDB_DATA_TYPE_MAP: {

            Handle<Object> object = NanNew<Object>();

            uint32_t size = (*entry_data_list)->entry_data.data_size;

            for (*entry_data_list = (*entry_data_list)->next; size && *entry_data_list; size--) {

                std::string key((char *)(*entry_data_list)->entry_data.utf8_string,
                    (*entry_data_list)->entry_data.data_size);

                *entry_data_list = (*entry_data_list)->next;
                object->Set(NanNew(key), convertToV8Helper(entry_data_list));
            }
            return object;
        }
        case MMDB_DATA_TYPE_ARRAY: {

            uint32_t size = (*entry_data_list)->entry_data.data_size;

            Handle<Array> array = NanNew<Array>(size);

            int i = 0;
            for (*entry_data_list = (*entry_data_list)->next; size && *entry_data_list; size--) {
                array->Set(i++, convertToV8Helper(entry_data_list));
            }

            return array;
        }
        case MMDB_DATA_TYPE_UTF8_STRING: {
            std::string str((char *)(*entry_data_list)->entry_data.utf8_string,
                (*entry_data_list)->entry_data.data_size);

            *entry_data_list = (*entry_data_list)->next;
            return NanNew(str);
        }
        case MMDB_DATA_TYPE_DOUBLE: {
            Handle<Value> val = NanNew<Number>((*entry_data_list)->entry_data.double_value);
            *entry_data_list = (*entry_data_list)->next;
            return val;
        }
        case MMDB_DATA_TYPE_FLOAT: {
            Handle<Value> val = NanNew<Number>((*entry_data_list)->entry_data.float_value);
            *entry_data_list = (*entry_data_list)->next;
            return val;
        }
        case MMDB_DATA_TYPE_UINT16: {
            Handle<Value> val = NanNew((*entry_data_list)->entry_data.uint16);
            *entry_data_list = (*entry_data_list)->next;
            return val;
        }
        case MMDB_DATA_TYPE_UINT32: {
            Handle<Value> val = NanNew((*entry_data_list)->entry_data.uint32);
            *entry_data_list = (*entry_data_list)->next;
            return val;
        }
        case MMDB_DATA_TYPE_BOOLEAN: {
            Handle<Value> val = NanNew<Boolean>((*entry_data_list)->entry_data.boolean);
            *entry_data_list = (*entry_data_list)->next;
            return val;
        }
        case MMDB_DATA_TYPE_UINT64: {
            Handle<Value> val = NanNew((double)(*entry_data_list)->entry_data.uint64);
            *entry_data_list = (*entry_data_list)->next;
            return val;
        }
        case MMDB_DATA_TYPE_INT32: {
            Handle<Value> val = NanNew((*entry_data_list)->entry_data.int32);
            *entry_data_list = (*entry_data_list)->next;
            return val;
        }

        case MMDB_DATA_TYPE_UINT128:
        case MMDB_DATA_TYPE_BYTES:
            // Not implemented
            *entry_data_list = (*entry_data_list)->next;
        default:
            return NanNull();
    }
}

static Handle<Value> convertToV8(MMDB_entry_data_list_s *entry_data_list) {
    
    return convertToV8Helper(&entry_data_list);
}

NodeMMDB::NodeMMDB(const std::string &filePath) {

    memset(&_mmdb, 0, sizeof(MMDB_s));

    int status = MMDB_open(filePath.c_str(), MMDB_MODE_MMAP, &_mmdb);

    if (status != MMDB_SUCCESS) {
        throw std::runtime_error(std::string("Error opening MMDB: ") + MMDB_strerror(status));
    }
}

NodeMMDB::~NodeMMDB() {

    MMDB_close(&_mmdb);
}

NAN_METHOD(NodeMMDB::New) {

    NanScope();

    NodeMMDB *wrapper;
    if( args.Length() == 1 && args[0]->IsString()) {
        try {
            std::string filePath = *NanUtf8String(args[0]);
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

NAN_METHOD(NodeMMDB::LookupSync) {

    NanScope();

    if (args.Length() != 1 || ! args[0]->IsString()) {
        NanThrowTypeError("Invalid arguments, expecting a string argument.");
        NanReturnUndefined();
    }

    NodeMMDB *wrapper = Unwrap<NodeMMDB>(args.Holder());
    std::string lookupStr = *NanUtf8String(args[0]);

    int gai_error, mmdb_error;

    MMDB_lookup_result_s result =
        MMDB_lookup_string(&wrapper->_mmdb, lookupStr.c_str(), &gai_error, &mmdb_error);

    if (gai_error != 0) {
        std::ostringstream os;
        os << "Error parsing address " << lookupStr << ": " << gai_strerror(gai_error);
        NanThrowError(os.str().c_str());
        NanReturnUndefined();
    }
    else if (mmdb_error != MMDB_SUCCESS) {
        std::ostringstream os;
        os << "Error looking up address " << lookupStr << ": " << MMDB_strerror(mmdb_error);
        NanThrowError(os.str().c_str());
        NanReturnUndefined();
    }

    if (result.found_entry) {
        
        MMDB_entry_data_list_s *resultList;

        int status = MMDB_get_entry_data_list(&result.entry,
                                              &resultList);

        if (status != MMDB_SUCCESS) {
            std::ostringstream os;
            os << "Error looking up address data: " << MMDB_strerror(status);
            NanThrowError(os.str().c_str());
            NanReturnUndefined();
        }
        else {
            
            //MMDB_dump_entry_data_list(stdout, resultList, 2);

            Handle<Value> ret = convertToV8(resultList);
            MMDB_free_entry_data_list(resultList);
            NanReturnValue(ret);
        }
    }
    else {
        NanReturnValue(NanNull());
    }
}

NAN_METHOD(NodeMMDB::Lookup) {

    NanScope();

    if (args.Length() != 2 || ! args[0]->IsString() || ! args[1]->IsFunction()) {
        NanThrowTypeError("Invalid arguments, expecting an string argument and a function callback argument.");
        NanReturnUndefined();
    }

    NodeMMDB *wrapper = Unwrap<NodeMMDB>(args.Holder());

    std::string lookupStr = *NanUtf8String(args[0]);
    NanCallback *callback = new NanCallback(args[1].As<Function>());

    NodeMMDBWorker *worker = new NodeMMDBWorker(callback, wrapper, lookupStr);
    worker->SaveToPersistent("mmdb", args.Holder());
    NanAsyncQueueWorker(worker);

    NanReturnUndefined();
}

NodeMMDBWorker::NodeMMDBWorker(NanCallback *callback, NodeMMDB *mmdb, const std::string &lookupStr) :
    NanAsyncWorker(callback),
    mmdb(mmdb),
    lookupStr(lookupStr),
    resultList(NULL)
{

}

void NodeMMDBWorker::Execute() {

    int gai_error, mmdb_error;

    MMDB_lookup_result_s result =
        MMDB_lookup_string(&mmdb->_mmdb, lookupStr.c_str(), &gai_error, &mmdb_error);

    if (gai_error != 0) {
        std::ostringstream os;
        os << "Error parsing address " << lookupStr << ": " << gai_strerror(gai_error);
        SetErrorMessage(os.str().c_str());
    }
    else if (mmdb_error != MMDB_SUCCESS) {
        std::ostringstream os;
        os << "Error looking up address " << lookupStr << ": " << MMDB_strerror(mmdb_error);
        SetErrorMessage(os.str().c_str());
    }

    if (result.found_entry) {
        
        int status = MMDB_get_entry_data_list(&result.entry,
                                              &resultList);

        if (status != MMDB_SUCCESS) {
            std::ostringstream os;
            os << "Error looking up address data: " << MMDB_strerror(status);
            SetErrorMessage(os.str().c_str());
        }
    }
}

void NodeMMDBWorker::HandleOKCallback() {

    NanScope();

    Local<Value> argv[] = {
        NanNull(),
        convertToV8(resultList)
    };

    callback->Call(2, argv);
}

void NodeMMDBWorker::Destroy() {

    MMDB_free_entry_data_list(resultList);
    NanAsyncWorker::Destroy();
}

static void init(Handle<Object> exports) {

     /** MMDB **/
    Handle<FunctionTemplate> mmdbTpl =  NanNew<FunctionTemplate>(NodeMMDB::New);
    mmdbTpl->SetClassName(NanNew<String>("MMDB"));
    mmdbTpl->InstanceTemplate()->SetInternalFieldCount(1);

    NODE_SET_PROTOTYPE_METHOD(mmdbTpl, "lookup",     NodeMMDB::Lookup);
    NODE_SET_PROTOTYPE_METHOD(mmdbTpl, "lookupSync", NodeMMDB::LookupSync);

    exports->Set(NanNew<String>("MMDB"), mmdbTpl->GetFunction());
}

NODE_MODULE(node_mmdb, init)
