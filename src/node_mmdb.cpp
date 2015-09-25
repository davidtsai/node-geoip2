
#include "node_mmdb.h"

#include <stdexcept>
#include <sstream>

static Local<Value> convertToV8Helper(MMDB_entry_data_list_s **entry_data_list) {

    if (! *entry_data_list) {
        return Nan::Null();
    }

    switch ((*entry_data_list)->entry_data.type) {

        case MMDB_DATA_TYPE_MAP: {

            Local<Object> object = Nan::New<Object>();

            uint32_t size = (*entry_data_list)->entry_data.data_size;

            for (*entry_data_list = (*entry_data_list)->next; size && *entry_data_list; size--) {

                std::string key((char *)(*entry_data_list)->entry_data.utf8_string,
                    (*entry_data_list)->entry_data.data_size);

                *entry_data_list = (*entry_data_list)->next;
                object->Set(Nan::New(key).ToLocalChecked(), convertToV8Helper(entry_data_list));
            }
            return object;
        }
        case MMDB_DATA_TYPE_ARRAY: {

            uint32_t size = (*entry_data_list)->entry_data.data_size;

            Local<Array> array = Nan::New<Array>(size);

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
            return Nan::New(str).ToLocalChecked();
        }
        case MMDB_DATA_TYPE_DOUBLE: {
            Local<Value> val = Nan::New<Number>((*entry_data_list)->entry_data.double_value);
            *entry_data_list = (*entry_data_list)->next;
            return val;
        }
        case MMDB_DATA_TYPE_FLOAT: {
            Local<Value> val = Nan::New<Number>((*entry_data_list)->entry_data.float_value);
            *entry_data_list = (*entry_data_list)->next;
            return val;
        }
        case MMDB_DATA_TYPE_UINT16: {
            Local<Value> val = Nan::New((*entry_data_list)->entry_data.uint16);
            *entry_data_list = (*entry_data_list)->next;
            return val;
        }
        case MMDB_DATA_TYPE_UINT32: {
            Local<Value> val = Nan::New((*entry_data_list)->entry_data.uint32);
            *entry_data_list = (*entry_data_list)->next;
            return val;
        }
        case MMDB_DATA_TYPE_BOOLEAN: {
            Local<Value> val = Nan::New<Boolean>((*entry_data_list)->entry_data.boolean);
            *entry_data_list = (*entry_data_list)->next;
            return val;
        }
        case MMDB_DATA_TYPE_UINT64: {
            Local<Value> val = Nan::New((double)(*entry_data_list)->entry_data.uint64);
            *entry_data_list = (*entry_data_list)->next;
            return val;
        }
        case MMDB_DATA_TYPE_INT32: {
            Local<Value> val = Nan::New((*entry_data_list)->entry_data.int32);
            *entry_data_list = (*entry_data_list)->next;
            return val;
        }

        case MMDB_DATA_TYPE_UINT128:
        case MMDB_DATA_TYPE_BYTES:
            // Not implemented
            *entry_data_list = (*entry_data_list)->next;
        default:
            return Nan::Null();
    }
}

static Local<Value> convertToV8(MMDB_entry_data_list_s *entry_data_list) {
    
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

    Nan::HandleScope scope; 

    NodeMMDB *wrapper = nullptr;
    if( info.Length() == 1 && info[0]->IsString()) {
        try {
            std::string filePath = *Nan::Utf8String(info[0]);
            wrapper = new NodeMMDB(filePath);
        }
        catch(const std::exception &e) {
            Nan::ThrowError(e.what());
	    info.GetReturnValue().Set(Nan::Undefined());
        }
    }
    else {
        Nan::ThrowTypeError("Invalid arguments, expecting a string argument.");
        info.GetReturnValue().Set(Nan::Undefined());
    }

    wrapper->Wrap(info.Holder());
    info.GetReturnValue().Set(info.Holder());
}

NAN_METHOD(NodeMMDB::LookupSync) {

    Nan::EscapableHandleScope scope;

    if (info.Length() != 1 || ! info[0]->IsString()) {
        Nan::ThrowTypeError("Invalid arguments, expecting a string argument.");
    }

    NodeMMDB *wrapper = Unwrap<NodeMMDB>(info.Holder());
    std::string lookupStr = *Nan::Utf8String(info[0]);

    int gai_error, mmdb_error;

    MMDB_lookup_result_s result =
        MMDB_lookup_string(&wrapper->_mmdb, lookupStr.c_str(), &gai_error, &mmdb_error);

    if (gai_error != 0) {
        std::ostringstream os;
        os << "Error parsing address " << lookupStr << ": " << gai_strerror(gai_error);
        Nan::ThrowError(os.str().c_str());
    }
    else if (mmdb_error != MMDB_SUCCESS) {
        std::ostringstream os;
        os << "Error looking up address " << lookupStr << ": " << MMDB_strerror(mmdb_error);
        Nan::ThrowError(os.str().c_str());
    }

    if (result.found_entry) {
        
        MMDB_entry_data_list_s *resultList;

        int status = MMDB_get_entry_data_list(&result.entry,
                                              &resultList);

        if (status != MMDB_SUCCESS) {
            std::ostringstream os;
            os << "Error looking up address data: " << MMDB_strerror(status);
            MMDB_free_entry_data_list(resultList);
            Nan::ThrowError(os.str().c_str());
        }
        else {
            Local<Value> ret = convertToV8(resultList);
            MMDB_free_entry_data_list(resultList);
	        info.GetReturnValue().Set(scope.Escape(ret));
        }
    }
    else {
	info.GetReturnValue().Set(Nan::Null());
    }
}

NAN_METHOD(NodeMMDB::Lookup) {

    Nan::HandleScope scope;

    if (info.Length() != 2 || ! info[0]->IsString() || ! info[1]->IsFunction()) {
        Nan::ThrowTypeError("Invalid arguments, expecting an string argument and a function callback argument.");
    }

    NodeMMDB *wrapper = Unwrap<NodeMMDB>(info.Holder());

    std::string lookupStr = *Nan::Utf8String(info[0]);
    Nan::Callback *callback = new Nan::Callback(info[1].As<Function>());

    NodeMMDBWorker *worker = new NodeMMDBWorker(callback, wrapper, lookupStr);
    worker->SaveToPersistent("mmdb", info.Holder());
    Nan::AsyncQueueWorker(worker);
}

NodeMMDBWorker::NodeMMDBWorker(Nan::Callback *callback, NodeMMDB *mmdb, const std::string &lookupStr) :
    Nan::AsyncWorker(callback),
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

    Nan::EscapableHandleScope scope;

    Local<Value> argv[] = {
        Nan::Null(),
        scope.Escape(convertToV8(resultList))
    };

    callback->Call(2, argv);
}

void NodeMMDBWorker::Destroy() {

    MMDB_free_entry_data_list(resultList);
    Nan::AsyncWorker::Destroy();
}

static void init(Handle<Object> exports) {

     /** MMDB **/
    Handle<FunctionTemplate> mmdbTpl =  Nan::New<FunctionTemplate>(NodeMMDB::New);
    mmdbTpl->SetClassName(Nan::New("MMDB").ToLocalChecked());
    mmdbTpl->InstanceTemplate()->SetInternalFieldCount(1);

    Nan::SetPrototypeMethod(mmdbTpl, "lookup",     NodeMMDB::Lookup);
    Nan::SetPrototypeMethod(mmdbTpl, "lookupSync", NodeMMDB::LookupSync);

    exports->Set(Nan::New("MMDB").ToLocalChecked(), mmdbTpl->GetFunction());
}

NODE_MODULE(node_mmdb, init)
