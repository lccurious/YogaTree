#pragma once

#include "Types.h"
#include <vector>

class Metadata
{
public:
    using Ptr = SharedPtr<Metadata>;
    using ConstPtr = SharedPtr<const Metadata>;

    Metadata();
    virtual ~Metadata() {}

    //Disallow copying of instance of this class.
    Metadata(const Metadata&) = delete;
    Metadata& operator=(const Metadata&) = delete;

    /// return the boolean representation of this metadata (empty strings
    /// and zeroVals evaluate to false; most other values evaluate to true).
    virtual bool asBool() const = 0;

    /// Return a copy of the metadata.
    virtual Metadata::Ptr copy() const = 0;

    /// Copy the given metadata into this metadata.
    virtual void copy(const Metadata& other) = 0;

    /// Return @c true if the given metadata is equivalent to this metadata
    bool operator == (const Metadata& other) const;

    bool operator != (const Metadata& other) const { return !(*this == other); }

    /// Return the size of this metadata in bytes.
    virtual Index32 size() const = 0;

    /// Unserialize this metadata from a stream.
    void read(std::istream&);
    /// Serialize this metadata to a stream.
    void write(std::ostream&) const;


protected:
    /// Read the size of the metadata from a stream.
    static Index32 readSize(std::istream&);
    /// Write the size of the metadata to a stream.
    void writeSize(std::ostream&) const;

    /// Read the metadata from a stream.
    virtual void readValue(std::istream&, Index32 numBytes) = 0;
    /// Write the metadata to a stream.
    virtual void writeValue(std::ostream&) const = 0;
};

class NeuMetadata: public Metadata
{
public:
    using ByteVec = std::vector<uint8_t>;
    Metadata::Ptr copy() const override;
    Index32 size() const override { return static_cast<Index32>(mBytes.size()); }

    void setValue(const ByteVec& bytes) { mBytes = bytes; }
    const ByteVec& value() const { return mBytes; }

protected:
    void readValue(std::istream&, Index32 numBytes) override;
    void writeValue(std::ostream&) const override;

private:
    ByteVec mBytes;
};

