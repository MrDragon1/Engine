#include "UniformBuffer.h"

namespace Ethereal {
UniformBuffer::UniformBuffer(uint32_t size) : mSize(size), mDirty(true) {
    mBuffer = UniformBuffer::Alloc(mSize);
    memset(mBuffer, 0, mSize);
}

void* UniformBuffer::Alloc(size_t size) { return ::malloc(size); }

void UniformBuffer::Free(void* addr) { return ::free(addr); }

BufferInterfaceBlock::Builder& BufferInterfaceBlock::Builder::Name(std::string_view interfaceBlockName) {
    mName = {interfaceBlockName.data(), interfaceBlockName.size()};
    return *this;
}

BufferInterfaceBlock::Builder& BufferInterfaceBlock::Builder::Qualifier(BufferInterfaceBlock::Qualifier qualifier) {
    mQualifiers |= uint8_t(qualifier);
    return *this;
}

BufferInterfaceBlock::Builder& BufferInterfaceBlock::Builder::Add(std::initializer_list<InterfaceBlockEntry> list) {
    mEntries.reserve(mEntries.size() + list.size());
    for (auto const& item : list) {
        mEntries.push_back({{item.name.data(), item.name.size()},
                            0,
                            uint8_t(item.stride),
                            item.type,
                            item.size > 0,
                            item.size,
                            item.precision,
                            {item.structName.data(), item.structName.size()},
                            {item.sizeName.data(), item.sizeName.size()}});
    }
    return *this;
}

BufferInterfaceBlock::Builder& BufferInterfaceBlock::Builder::AddVariableSizedArray(InterfaceBlockEntry const& item) {
    mHasVariableSizeArray = true;
    mEntries.push_back({{item.name.data(), item.name.size()},
                        0,
                        uint8_t(item.stride),
                        item.type,
                        true,
                        0,
                        item.precision,
                        {item.structName.data(), item.structName.size()},
                        {item.sizeName.data(), item.sizeName.size()}});
    return *this;
}

BufferInterfaceBlock BufferInterfaceBlock::Builder::Build() {
    // look for the first variable-size array
    auto pos = std::find_if(mEntries.begin(), mEntries.end(), [](FieldInfo const& item) -> bool { return item.isArray && !item.size; });

    // if there is one, check it's the last entry
    ET_CORE_ASSERT(pos == mEntries.end() || pos == mEntries.end() - 1, "the variable-size array must be the last entry");

    return BufferInterfaceBlock(*this);
}

BufferInterfaceBlock::BufferInterfaceBlock(Builder const& builder) noexcept : mName(builder.mName), mFieldInfoList(builder.mEntries.size()) {
    auto& infoMap = mInfoMap;
    infoMap.reserve(builder.mEntries.size());

    auto& uniformsInfoList = mFieldInfoList;

    uint32_t i = 0;
    uint16_t offset = 0;
    for (auto const& e : builder.mEntries) {
        size_t alignment = BaseAlignmentForType(e.type);
        size_t stride = StrideForType(e.type, e.stride);

        if (e.isArray) {  // this is an array
            alignment = 4;
            // the stride of an array is always rounded to its alignment (which is POT)
            stride = (stride + alignment - 1) & ~(alignment - 1);
        }

        // calculate the offset for this uniform
        size_t padding = (alignment - (offset % alignment)) % alignment;
        offset += padding;

        FieldInfo& info = uniformsInfoList[i];
        info = {e.name, offset, uint8_t(stride), e.type, e.isArray, e.size, e.precision, e.structName, e.sizeName};

        // record this uniform info
        infoMap[{info.name.data(), info.name.size()}] = i;

        // advance offset to next slot
        offset += stride * std::max(1u, e.size);
        ++i;
    }

    // round size to the next multiple of 4 and convert to bytes
    mSize = sizeof(uint32_t) * ((offset + 3) & ~3);
}

uint8_t BufferInterfaceBlock::BaseAlignmentForType(Type type) noexcept {
    switch (type) {
        case Type::BOOL:
        case Type::FLOAT:
        case Type::INT:
        case Type::UINT:
            return 1;
        case Type::BOOL2:
        case Type::FLOAT2:
        case Type::INT2:
        case Type::UINT2:
            return 2;
        case Type::BOOL3:
        case Type::BOOL4:
        case Type::FLOAT3:
        case Type::FLOAT4:
        case Type::INT3:
        case Type::INT4:
        case Type::UINT3:
        case Type::UINT4:
        case Type::MAT3:
        case Type::MAT4:
        case Type::STRUCT:
            return 4;
    }
}

uint8_t BufferInterfaceBlock::StrideForType(Type type, uint32_t stride) noexcept {
    switch (type) {
        case Type::BOOL:
        case Type::INT:
        case Type::UINT:
        case Type::FLOAT:
            return 1;
        case Type::BOOL2:
        case Type::INT2:
        case Type::UINT2:
        case Type::FLOAT2:
            return 2;
        case Type::BOOL3:
        case Type::INT3:
        case Type::UINT3:
        case Type::FLOAT3:
            return 3;
        case Type::BOOL4:
        case Type::INT4:
        case Type::UINT4:
        case Type::FLOAT4:
            return 4;
        case Type::MAT3:
            return 12;
        case Type::MAT4:
            return 16;
        case Type::STRUCT:
            return stride;
    }
}

size_t BufferInterfaceBlock::GetFieldOffset(std::string_view name, size_t index) const {
    auto const* info = GetFieldInfo(name);
    ET_CORE_ASSERT(info);
    return (size_t)info->GetBufferOffset(index);
}

BufferInterfaceBlock::FieldInfo const* BufferInterfaceBlock::GetFieldInfo(std::string_view name) const {
    auto pos = mInfoMap.find(name);
    ET_CORE_ASSERT(pos != mInfoMap.end(), "uniform named \"%.*s\" not found", name.size(), name.data());
    return &mFieldInfoList[pos->second];
}

template <size_t Size>
void UniformBuffer::SetUniformUntyped(size_t offset, void const* v) noexcept {
    SetUniformUntyped<Size>(InvalidateUniforms(offset, Size), 0ul, v);
}
template void UniformBuffer::SetUniformUntyped<4ul>(size_t offset, void const* v) noexcept;
template void UniformBuffer::SetUniformUntyped<8ul>(size_t offset, void const* v) noexcept;
template void UniformBuffer::SetUniformUntyped<12ul>(size_t offset, void const* v) noexcept;
template void UniformBuffer::SetUniformUntyped<16ul>(size_t offset, void const* v) noexcept;
template void UniformBuffer::SetUniformUntyped<64ul>(size_t offset, void const* v) noexcept;

template <size_t Size>
void UniformBuffer::SetUniformArrayUntyped(size_t offset, void const* begin, size_t count) noexcept {
    constexpr size_t stride = (Size + 0xFu) & ~0xFu;
    size_t arraySize = stride * count - stride + Size;
    void* p = InvalidateUniforms(offset, arraySize);
    for (size_t i = 0; i < count; i++) {
        SetUniformUntyped<Size>(p, 0ul, static_cast<const char*>(begin) + i * Size);
        p = (void*)(uintptr_t(p) + uintptr_t(stride));
    }
}
template void UniformBuffer::SetUniformArrayUntyped<4ul>(size_t offset, void const* begin, size_t count) noexcept;
template void UniformBuffer::SetUniformArrayUntyped<8ul>(size_t offset, void const* begin, size_t count) noexcept;
template void UniformBuffer::SetUniformArrayUntyped<12ul>(size_t offset, void const* begin, size_t count) noexcept;
template void UniformBuffer::SetUniformArrayUntyped<16ul>(size_t offset, void const* begin, size_t count) noexcept;
template void UniformBuffer::SetUniformArrayUntyped<64ul>(size_t offset, void const* begin, size_t count) noexcept;

}  // namespace Ethereal
