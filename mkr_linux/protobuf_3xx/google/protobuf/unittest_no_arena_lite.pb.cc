// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: google/protobuf/unittest_no_arena_lite.proto

#define INTERNAL_SUPPRESS_PROTOBUF_FIELD_DEPRECATION
#include <google/protobuf/unittest_no_arena_lite.pb.h>

#include <algorithm>

#include <google/protobuf/stubs/common.h>
#include <google/protobuf/stubs/port.h>
#include <google/protobuf/stubs/once.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/wire_format_lite_inl.h>
#include <google/protobuf/io/zero_copy_stream_impl_lite.h>
// @@protoc_insertion_point(includes)

namespace protobuf_unittest_no_arena {

void protobuf_ShutdownFile_google_2fprotobuf_2funittest_5fno_5farena_5flite_2eproto() {
  delete ForeignMessageLite::default_instance_;
}

#ifdef GOOGLE_PROTOBUF_NO_STATIC_INITIALIZER
void protobuf_AddDesc_google_2fprotobuf_2funittest_5fno_5farena_5flite_2eproto_impl() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

#else
void protobuf_AddDesc_google_2fprotobuf_2funittest_5fno_5farena_5flite_2eproto() {
  static bool already_here = false;
  if (already_here) return;
  already_here = true;
  GOOGLE_PROTOBUF_VERIFY_VERSION;

#endif
  ForeignMessageLite::default_instance_ = new ForeignMessageLite();
  ForeignMessageLite::default_instance_->InitAsDefaultInstance();
  ::google::protobuf::internal::OnShutdown(&protobuf_ShutdownFile_google_2fprotobuf_2funittest_5fno_5farena_5flite_2eproto);
}

#ifdef GOOGLE_PROTOBUF_NO_STATIC_INITIALIZER
GOOGLE_PROTOBUF_DECLARE_ONCE(protobuf_AddDesc_google_2fprotobuf_2funittest_5fno_5farena_5flite_2eproto_once_);
void protobuf_AddDesc_google_2fprotobuf_2funittest_5fno_5farena_5flite_2eproto() {
  ::google::protobuf::GoogleOnceInit(&protobuf_AddDesc_google_2fprotobuf_2funittest_5fno_5farena_5flite_2eproto_once_,
                 &protobuf_AddDesc_google_2fprotobuf_2funittest_5fno_5farena_5flite_2eproto_impl);
}
#else
// Force AddDescriptors() to be called at static initialization time.
struct StaticDescriptorInitializer_google_2fprotobuf_2funittest_5fno_5farena_5flite_2eproto {
  StaticDescriptorInitializer_google_2fprotobuf_2funittest_5fno_5farena_5flite_2eproto() {
    protobuf_AddDesc_google_2fprotobuf_2funittest_5fno_5farena_5flite_2eproto();
  }
} static_descriptor_initializer_google_2fprotobuf_2funittest_5fno_5farena_5flite_2eproto_;
#endif

namespace {

static void MergeFromFail(int line) GOOGLE_ATTRIBUTE_COLD;
static void MergeFromFail(int line) {
  GOOGLE_CHECK(false) << __FILE__ << ":" << line;
}

}  // namespace


// ===================================================================

static ::std::string* MutableUnknownFieldsForForeignMessageLite(
    ForeignMessageLite* ptr) {
  return ptr->mutable_unknown_fields();
}

#if !defined(_MSC_VER) || _MSC_VER >= 1900
const int ForeignMessageLite::kCFieldNumber;
#endif  // !defined(_MSC_VER) || _MSC_VER >= 1900

ForeignMessageLite::ForeignMessageLite()
  : ::google::protobuf::MessageLite(), _arena_ptr_(NULL) {
  SharedCtor();
  // @@protoc_insertion_point(constructor:protobuf_unittest_no_arena.ForeignMessageLite)
}

void ForeignMessageLite::InitAsDefaultInstance() {
}

ForeignMessageLite::ForeignMessageLite(const ForeignMessageLite& from)
  : ::google::protobuf::MessageLite(),
    _arena_ptr_(NULL) {
  SharedCtor();
  MergeFrom(from);
  // @@protoc_insertion_point(copy_constructor:protobuf_unittest_no_arena.ForeignMessageLite)
}

void ForeignMessageLite::SharedCtor() {
  ::google::protobuf::internal::GetEmptyString();
  _cached_size_ = 0;
  _unknown_fields_.UnsafeSetDefault(
      &::google::protobuf::internal::GetEmptyStringAlreadyInited());
  c_ = 0;
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

ForeignMessageLite::~ForeignMessageLite() {
  // @@protoc_insertion_point(destructor:protobuf_unittest_no_arena.ForeignMessageLite)
  SharedDtor();
}

void ForeignMessageLite::SharedDtor() {
  _unknown_fields_.DestroyNoArena(
      &::google::protobuf::internal::GetEmptyStringAlreadyInited());
  #ifdef GOOGLE_PROTOBUF_NO_STATIC_INITIALIZER
  if (this != &default_instance()) {
  #else
  if (this != default_instance_) {
  #endif
  }
}

void ForeignMessageLite::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ForeignMessageLite& ForeignMessageLite::default_instance() {
#ifdef GOOGLE_PROTOBUF_NO_STATIC_INITIALIZER
  protobuf_AddDesc_google_2fprotobuf_2funittest_5fno_5farena_5flite_2eproto();
#else
  if (default_instance_ == NULL) protobuf_AddDesc_google_2fprotobuf_2funittest_5fno_5farena_5flite_2eproto();
#endif
  return *default_instance_;
}

ForeignMessageLite* ForeignMessageLite::default_instance_ = NULL;

ForeignMessageLite* ForeignMessageLite::New(::google::protobuf::Arena* arena) const {
  ForeignMessageLite* n = new ForeignMessageLite;
  if (arena != NULL) {
    arena->Own(n);
  }
  return n;
}

void ForeignMessageLite::Clear() {
  c_ = 0;
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  _unknown_fields_.ClearToEmptyNoArena(
      &::google::protobuf::internal::GetEmptyStringAlreadyInited());
}

bool ForeignMessageLite::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) goto failure
  ::google::protobuf::uint32 tag;
  ::google::protobuf::io::LazyStringOutputStream unknown_fields_string(
      ::google::protobuf::internal::NewPermanentCallback(
          &MutableUnknownFieldsForForeignMessageLite, this));
  ::google::protobuf::io::CodedOutputStream unknown_fields_stream(
      &unknown_fields_string, false);
  // @@protoc_insertion_point(parse_start:protobuf_unittest_no_arena.ForeignMessageLite)
  for (;;) {
    ::std::pair< ::google::protobuf::uint32, bool> p = input->ReadTagWithCutoff(127);
    tag = p.first;
    if (!p.second) goto handle_unusual;
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // optional int32 c = 1;
      case 1: {
        if (tag == 8) {
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 input, &c_)));
          set_has_c();
        } else {
          goto handle_unusual;
        }
        if (input->ExpectAtEnd()) goto success;
        break;
      }

      default: {
      handle_unusual:
        if (tag == 0 ||
            ::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_END_GROUP) {
          goto success;
        }
        DO_(::google::protobuf::internal::WireFormatLite::SkipField(
            input, tag, &unknown_fields_stream));
        break;
      }
    }
  }
success:
  // @@protoc_insertion_point(parse_success:protobuf_unittest_no_arena.ForeignMessageLite)
  return true;
failure:
  // @@protoc_insertion_point(parse_failure:protobuf_unittest_no_arena.ForeignMessageLite)
  return false;
#undef DO_
}

void ForeignMessageLite::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // @@protoc_insertion_point(serialize_start:protobuf_unittest_no_arena.ForeignMessageLite)
  // optional int32 c = 1;
  if (has_c()) {
    ::google::protobuf::internal::WireFormatLite::WriteInt32(1, this->c(), output);
  }

  output->WriteRaw(unknown_fields().data(),
                   static_cast<int>(unknown_fields().size()));
  // @@protoc_insertion_point(serialize_end:protobuf_unittest_no_arena.ForeignMessageLite)
}

int ForeignMessageLite::ByteSize() const {
  int total_size = 0;

  // optional int32 c = 1;
  if (has_c()) {
    total_size += 1 +
      ::google::protobuf::internal::WireFormatLite::Int32Size(
        this->c());
  }

  total_size += unknown_fields().size();

  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = total_size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
  return total_size;
}

void ForeignMessageLite::CheckTypeAndMergeFrom(
    const ::google::protobuf::MessageLite& from) {
  MergeFrom(*::google::protobuf::down_cast<const ForeignMessageLite*>(&from));
}

void ForeignMessageLite::MergeFrom(const ForeignMessageLite& from) {
  if (GOOGLE_PREDICT_FALSE(&from == this)) MergeFromFail(__LINE__);
  if (from._has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (from.has_c()) {
      set_c(from.c());
    }
  }
  if (!from.unknown_fields().empty()) {
    mutable_unknown_fields()->append(from.unknown_fields());
  }
}

void ForeignMessageLite::CopyFrom(const ForeignMessageLite& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool ForeignMessageLite::IsInitialized() const {

  return true;
}

void ForeignMessageLite::Swap(ForeignMessageLite* other) {
  if (other == this) return;
  InternalSwap(other);
}
void ForeignMessageLite::InternalSwap(ForeignMessageLite* other) {
  std::swap(c_, other->c_);
  std::swap(_has_bits_[0], other->_has_bits_[0]);
  _unknown_fields_.Swap(&other->_unknown_fields_);
  std::swap(_cached_size_, other->_cached_size_);
}

::std::string ForeignMessageLite::GetTypeName() const {
  return "protobuf_unittest_no_arena.ForeignMessageLite";
}

#if PROTOBUF_INLINE_NOT_IN_HEADERS
// ForeignMessageLite

// optional int32 c = 1;
bool ForeignMessageLite::has_c() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
void ForeignMessageLite::set_has_c() {
  _has_bits_[0] |= 0x00000001u;
}
void ForeignMessageLite::clear_has_c() {
  _has_bits_[0] &= ~0x00000001u;
}
void ForeignMessageLite::clear_c() {
  c_ = 0;
  clear_has_c();
}
 ::google::protobuf::int32 ForeignMessageLite::c() const {
  // @@protoc_insertion_point(field_get:protobuf_unittest_no_arena.ForeignMessageLite.c)
  return c_;
}
 void ForeignMessageLite::set_c(::google::protobuf::int32 value) {
  set_has_c();
  c_ = value;
  // @@protoc_insertion_point(field_set:protobuf_unittest_no_arena.ForeignMessageLite.c)
}

#endif  // PROTOBUF_INLINE_NOT_IN_HEADERS

// @@protoc_insertion_point(namespace_scope)

}  // namespace protobuf_unittest_no_arena

// @@protoc_insertion_point(global_scope)
