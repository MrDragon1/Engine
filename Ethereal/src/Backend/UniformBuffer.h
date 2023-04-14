#pragma once
#include <cstddef>
#include <string>

#include "Utils/EngineMarco.h"
#include "Backend\BufferDescriptor.h"
#include "Backend\DriverEnum.h"

namespace Ethereal {
class BufferInterfaceBlock {
   public:
    using Type = UniformType;
    using Precision = Precision;

    struct InterfaceBlockEntry {
        std::string_view name;
        uint32_t size;
        Type type;
        Precision precision{};
        std::string_view structName{};
        uint32_t stride{};
        std::string_view sizeName{};
    };
    enum class Qualifier : uint8_t {
        COHERENT = 0x01,
        WRITEONLY = 0x02,
        READONLY = 0x04,
        VOLATILE = 0x08,
        RESTRICT = 0x10,
    };

    struct FieldInfo {
        std::string name;        // name of this field
        uint16_t offset;         // offset in "uint32_t" of this field in the buffer
        uint8_t stride;          // stride in "uint32_t" to the next element
        Type type;               // type of this field
        bool isArray;            // true if the field is an array
        uint32_t size;           // size of the array in elements, or 0 if not an array
        Precision precision;     // precision of this field
        std::string structName;  // name of this field structure if type is STRUCT
        std::string sizeName;    // name of the size parameter in the shader
        // returns offset in bytes of this field (at index if an array)
        inline size_t GetBufferOffset(size_t index = 0) const {
            ET_CORE_ASSERT(index < std::max(1u, size), "Index out of range!");
            return (offset + stride * index) * sizeof(uint32_t);
        }
    };

    class Builder {
       public:
        Builder() noexcept = default;
        ~Builder() noexcept = default;

        Builder(Builder const& rhs) = default;
        Builder(Builder&& rhs) noexcept = default;
        Builder& operator=(Builder const& rhs) = default;
        Builder& operator=(Builder&& rhs) noexcept = default;

        Builder& Name(std::string_view interfaceBlockName);
        Builder& Qualifier(BufferInterfaceBlock::Qualifier qualifier);
        Builder& Add(std::initializer_list<InterfaceBlockEntry> list);
        Builder& AddVariableSizedArray(InterfaceBlockEntry const& item);
        BufferInterfaceBlock Build();

        bool HasVariableSizeArray() const { return mHasVariableSizeArray; };

       private:
        friend class BufferInterfaceBlock;
        std::string mName;
        std::vector<FieldInfo> mEntries;
        uint8_t mQualifiers = 0;
        bool mHasVariableSizeArray = false;
    };

    BufferInterfaceBlock() = default;

    BufferInterfaceBlock(const BufferInterfaceBlock& rhs) = delete;
    BufferInterfaceBlock& operator=(const BufferInterfaceBlock& rhs) = delete;

    BufferInterfaceBlock(BufferInterfaceBlock&& rhs) noexcept = default;
    BufferInterfaceBlock& operator=(BufferInterfaceBlock&& rhs) noexcept = default;

    ~BufferInterfaceBlock() noexcept = default;

    std::string_view GetName() const noexcept { return {mName.data(), mName.size()}; }
    size_t GetSize() const noexcept { return mSize; }
    std::vector<FieldInfo> const& getFieldInfoList() const noexcept { return mFieldInfoList; }

    size_t GetFieldOffset(std::string_view name, size_t index) const;

    FieldInfo const* GetFieldInfo(std::string_view name) const;

    bool HasField(std::string_view name) const noexcept { return mInfoMap.find(name) != mInfoMap.end(); }

    bool IsEmpty() const noexcept { return mFieldInfoList.empty(); }

   private:
    friend class Builder;

    explicit BufferInterfaceBlock(Builder const& builder) noexcept;

    static uint8_t BaseAlignmentForType(Type type) noexcept;
    static uint8_t StrideForType(Type type, uint32_t stride) noexcept;

    std::string mName;
    std::vector<FieldInfo> mFieldInfoList;
    std::unordered_map<std::string_view, uint32_t> mInfoMap;
    uint32_t mSize = 0;  // size in bytes rounded to multiple of 4
    uint8_t mQualifiers = 0;
};

class UniformBuffer {
   public:
    UniformBuffer() = default;
    UniformBuffer(uint32_t size);
    // Cant copy
    UniformBuffer(const UniformBuffer& rhs) = delete;
    UniformBuffer& operator=(const UniformBuffer& rhs) = delete;

    ~UniformBuffer() {
        if (mBuffer) UniformBuffer::Free(mBuffer);
    }
    template <typename T>
    void SetUniformArray(size_t offset, T const* begin, size_t count) noexcept {
        SetUniformArrayUntyped<sizeof(T)>(offset, begin, count);
    }

    template <typename T>
    static inline void SetUniform(void* addr, size_t offset, const T& v) noexcept {
        SetUniformUntyped<sizeof(T)>(addr, offset, &v);
    }

    template <typename T>
    inline void SetUniform(size_t offset, const T& v) noexcept {
        SetUniformUntyped<sizeof(T)>(offset, &v);
    }

    template <typename T>
    T GetUniform(size_t offset) const noexcept {
        return *reinterpret_cast<T const*>(static_cast<char const*>(mBuffer) + offset);
    }

    BufferDescriptor toBufferDescriptor() const noexcept { return toBufferDescriptor(0, GetSize()); }

    // copy the UBO data and cleans the dirty bits
    BufferDescriptor toBufferDescriptor(size_t offset, size_t size) const noexcept {
        BufferDescriptor p;
        p.size = size;
        p.buffer = (void*)new char[p.size];
        memcpy(p.buffer, static_cast<const char*>(GetBuffer()) + offset, p.size);  // inlined
        Clean();
        return p;
    }

    void* Invalidate() noexcept { return InvalidateUniforms(0, mSize); }

    void* InvalidateUniforms(size_t offset, size_t size) {
        ET_CORE_ASSERT(offset + size <= mSize);
        mDirty = true;
        return static_cast<char*>(mBuffer) + offset;
    }

    bool IsDirty() const { return mDirty; }
    uint32_t GetSize() const { return mSize; }
    void const* GetBuffer() const { return mBuffer; }
    void Clean() const { mDirty = false; }

    template <size_t Size>
    void SetUniformUntyped(size_t offset, void const* v) noexcept;

    template <size_t Size>
    void SetUniformArrayUntyped(size_t offset, void const* begin, size_t count) noexcept;

   private:
    static void* Alloc(size_t size);
    static void Free(void* addr);

    template <size_t Size, std::enable_if_t<Size == 4 || Size == 8 || Size == 12 || Size == 16 || Size == 64, bool> = true>
    inline static void SetUniformUntyped(void* addr, size_t offset, void const* v) noexcept {
        memcpy(static_cast<char*>(addr) + offset, v, Size);
    }

    void* mBuffer;
    uint32_t mSize;
    mutable bool mDirty = false;
};

}  // namespace Ethereal
