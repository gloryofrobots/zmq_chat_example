// Generated by the protocol buffer compiler.  DO NOT EDIT!

#define INTERNAL_SUPPRESS_PROTOBUF_FIELD_DEPRECATION
#include "message.pb.h"

#include <algorithm>

#include <google/protobuf/stubs/once.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/wire_format_lite_inl.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
// @@protoc_insertion_point(includes)

namespace {

const ::google::protobuf::Descriptor* Message_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  Message_reflection_ = NULL;
const ::google::protobuf::EnumDescriptor* Message_MessageType_descriptor_ = NULL;

}  // namespace


void protobuf_AssignDesc_message_2eproto() {
  protobuf_AddDesc_message_2eproto();
  const ::google::protobuf::FileDescriptor* file =
    ::google::protobuf::DescriptorPool::generated_pool()->FindFileByName(
      "message.proto");
  GOOGLE_CHECK(file != NULL);
  Message_descriptor_ = file->message_type(0);
  static const int Message_offsets_[4] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Message, type_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Message, id_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Message, login_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Message, value_),
  };
  Message_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      Message_descriptor_,
      Message::default_instance_,
      Message_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Message, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Message, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(Message));
  Message_MessageType_descriptor_ = Message_descriptor_->enum_type(0);
}

namespace {

GOOGLE_PROTOBUF_DECLARE_ONCE(protobuf_AssignDescriptors_once_);
inline void protobuf_AssignDescriptorsOnce() {
  ::google::protobuf::GoogleOnceInit(&protobuf_AssignDescriptors_once_,
                 &protobuf_AssignDesc_message_2eproto);
}

void protobuf_RegisterTypes(const ::std::string&) {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    Message_descriptor_, &Message::default_instance());
}

}  // namespace

void protobuf_ShutdownFile_message_2eproto() {
  delete Message::default_instance_;
  delete Message_reflection_;
}

void protobuf_AddDesc_message_2eproto() {
  static bool already_here = false;
  if (already_here) return;
  already_here = true;
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  ::google::protobuf::DescriptorPool::InternalAddGeneratedFile(
    "\n\rmessage.proto\"\202\001\n\007Message\022\"\n\004type\030\001 \002("
    "\0162\024.Message.MessageType\022\n\n\002id\030\002 \002(\005\022\r\n\005l"
    "ogin\030\003 \001(\t\022\r\n\005value\030\004 \001(\t\")\n\013MessageType"
    "\022\r\n\tHEARTBEAT\020\000\022\013\n\007MESSAGE\020\001", 148);
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedFile(
    "message.proto", &protobuf_RegisterTypes);
  Message::default_instance_ = new Message();
  Message::default_instance_->InitAsDefaultInstance();
  ::google::protobuf::internal::OnShutdown(&protobuf_ShutdownFile_message_2eproto);
}

// Force AddDescriptors() to be called at static initialization time.
struct StaticDescriptorInitializer_message_2eproto {
  StaticDescriptorInitializer_message_2eproto() {
    protobuf_AddDesc_message_2eproto();
  }
} static_descriptor_initializer_message_2eproto_;


// ===================================================================

const ::google::protobuf::EnumDescriptor* Message_MessageType_descriptor() {
  protobuf_AssignDescriptorsOnce();
  return Message_MessageType_descriptor_;
}
bool Message_MessageType_IsValid(int value) {
  switch(value) {
    case 0:
    case 1:
      return true;
    default:
      return false;
  }
}

#ifndef _MSC_VER
const Message_MessageType Message::HEARTBEAT;
const Message_MessageType Message::MESSAGE;
const Message_MessageType Message::MessageType_MIN;
const Message_MessageType Message::MessageType_MAX;
const int Message::MessageType_ARRAYSIZE;
#endif  // _MSC_VER
#ifndef _MSC_VER
const int Message::kTypeFieldNumber;
const int Message::kIdFieldNumber;
const int Message::kLoginFieldNumber;
const int Message::kValueFieldNumber;
#endif  // !_MSC_VER

Message::Message()
  : ::google::protobuf::Message() {
  SharedCtor();
}

void Message::InitAsDefaultInstance() {
}

Message::Message(const Message& from)
  : ::google::protobuf::Message() {
  SharedCtor();
  MergeFrom(from);
}

void Message::SharedCtor() {
  _cached_size_ = 0;
  type_ = 0;
  id_ = 0;
  login_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
  value_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

Message::~Message() {
  SharedDtor();
}

void Message::SharedDtor() {
  if (login_ != &::google::protobuf::internal::kEmptyString) {
    delete login_;
  }
  if (value_ != &::google::protobuf::internal::kEmptyString) {
    delete value_;
  }
  if (this != default_instance_) {
  }
}

void Message::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* Message::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return Message_descriptor_;
}

const Message& Message::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_message_2eproto();  return *default_instance_;
}

Message* Message::default_instance_ = NULL;

Message* Message::New() const {
  return new Message;
}

void Message::Clear() {
  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    type_ = 0;
    id_ = 0;
    if (has_login()) {
      if (login_ != &::google::protobuf::internal::kEmptyString) {
        login_->clear();
      }
    }
    if (has_value()) {
      if (value_ != &::google::protobuf::internal::kEmptyString) {
        value_->clear();
      }
    }
  }
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->Clear();
}

bool Message::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) return false
  ::google::protobuf::uint32 tag;
  while ((tag = input->ReadTag()) != 0) {
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // required .Message.MessageType type = 1;
      case 1: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
          int value;
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   int, ::google::protobuf::internal::WireFormatLite::TYPE_ENUM>(
                 input, &value)));
          if (::Message_MessageType_IsValid(value)) {
            set_type(static_cast< ::Message_MessageType >(value));
          } else {
            mutable_unknown_fields()->AddVarint(1, value);
          }
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(16)) goto parse_id;
        break;
      }
      
      // required int32 id = 2;
      case 2: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
         parse_id:
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 input, &id_)));
          set_has_id();
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(26)) goto parse_login;
        break;
      }
      
      // optional string login = 3;
      case 3: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
         parse_login:
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->mutable_login()));
          ::google::protobuf::internal::WireFormat::VerifyUTF8String(
            this->login().data(), this->login().length(),
            ::google::protobuf::internal::WireFormat::PARSE);
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(34)) goto parse_value;
        break;
      }
      
      // optional string value = 4;
      case 4: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
         parse_value:
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->mutable_value()));
          ::google::protobuf::internal::WireFormat::VerifyUTF8String(
            this->value().data(), this->value().length(),
            ::google::protobuf::internal::WireFormat::PARSE);
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectAtEnd()) return true;
        break;
      }
      
      default: {
      handle_uninterpreted:
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_END_GROUP) {
          return true;
        }
        DO_(::google::protobuf::internal::WireFormat::SkipField(
              input, tag, mutable_unknown_fields()));
        break;
      }
    }
  }
  return true;
#undef DO_
}

void Message::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // required .Message.MessageType type = 1;
  if (has_type()) {
    ::google::protobuf::internal::WireFormatLite::WriteEnum(
      1, this->type(), output);
  }
  
  // required int32 id = 2;
  if (has_id()) {
    ::google::protobuf::internal::WireFormatLite::WriteInt32(2, this->id(), output);
  }
  
  // optional string login = 3;
  if (has_login()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->login().data(), this->login().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    ::google::protobuf::internal::WireFormatLite::WriteString(
      3, this->login(), output);
  }
  
  // optional string value = 4;
  if (has_value()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->value().data(), this->value().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    ::google::protobuf::internal::WireFormatLite::WriteString(
      4, this->value(), output);
  }
  
  if (!unknown_fields().empty()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
}

::google::protobuf::uint8* Message::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  // required .Message.MessageType type = 1;
  if (has_type()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteEnumToArray(
      1, this->type(), target);
  }
  
  // required int32 id = 2;
  if (has_id()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteInt32ToArray(2, this->id(), target);
  }
  
  // optional string login = 3;
  if (has_login()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->login().data(), this->login().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    target =
      ::google::protobuf::internal::WireFormatLite::WriteStringToArray(
        3, this->login(), target);
  }
  
  // optional string value = 4;
  if (has_value()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->value().data(), this->value().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    target =
      ::google::protobuf::internal::WireFormatLite::WriteStringToArray(
        4, this->value(), target);
  }
  
  if (!unknown_fields().empty()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  return target;
}

int Message::ByteSize() const {
  int total_size = 0;
  
  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    // required .Message.MessageType type = 1;
    if (has_type()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::EnumSize(this->type());
    }
    
    // required int32 id = 2;
    if (has_id()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::Int32Size(
          this->id());
    }
    
    // optional string login = 3;
    if (has_login()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::StringSize(
          this->login());
    }
    
    // optional string value = 4;
    if (has_value()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::StringSize(
          this->value());
    }
    
  }
  if (!unknown_fields().empty()) {
    total_size +=
      ::google::protobuf::internal::WireFormat::ComputeUnknownFieldsSize(
        unknown_fields());
  }
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = total_size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
  return total_size;
}

void Message::MergeFrom(const ::google::protobuf::Message& from) {
  GOOGLE_CHECK_NE(&from, this);
  const Message* source =
    ::google::protobuf::internal::dynamic_cast_if_available<const Message*>(
      &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
}

void Message::MergeFrom(const Message& from) {
  GOOGLE_CHECK_NE(&from, this);
  if (from._has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (from.has_type()) {
      set_type(from.type());
    }
    if (from.has_id()) {
      set_id(from.id());
    }
    if (from.has_login()) {
      set_login(from.login());
    }
    if (from.has_value()) {
      set_value(from.value());
    }
  }
  mutable_unknown_fields()->MergeFrom(from.unknown_fields());
}

void Message::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void Message::CopyFrom(const Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool Message::IsInitialized() const {
  if ((_has_bits_[0] & 0x00000003) != 0x00000003) return false;
  
  return true;
}

void Message::Swap(Message* other) {
  if (other != this) {
    std::swap(type_, other->type_);
    std::swap(id_, other->id_);
    std::swap(login_, other->login_);
    std::swap(value_, other->value_);
    std::swap(_has_bits_[0], other->_has_bits_[0]);
    _unknown_fields_.Swap(&other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

::google::protobuf::Metadata Message::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = Message_descriptor_;
  metadata.reflection = Message_reflection_;
  return metadata;
}


// @@protoc_insertion_point(namespace_scope)

// @@protoc_insertion_point(global_scope)