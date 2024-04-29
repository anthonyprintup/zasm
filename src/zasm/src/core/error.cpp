#include <cassert>
#include <string>
#include <zasm/core/errors.hpp>

namespace zasm
{
    static constexpr std::uint64_t kErrorExtMask = 0x8000000000000000ULL;

    struct ErrorExt
    {
        ErrorCode code;
        std::string message;
    };

    ErrorExt* toErrorExt(std::uint64_t data) noexcept
    {
        assert((data & kErrorExtMask) != 0);

        auto* ext = reinterpret_cast<ErrorExt*>(data & ~kErrorExtMask);
        return ext;
    }

    Error::Error(ErrorCode code) noexcept
    {
        std::uint64_t data = static_cast<std::uint64_t>(code);
        _data = data;
    }

    Error::~Error() noexcept
    {
        if (_data & kErrorExtMask)
        {
            const auto* ext = toErrorExt(_data);
            delete ext;
        }
        _data = 0;
    }

    bool Error::operator==(ErrorCode code) const noexcept
    {
        return getCode() == code;
    }

    bool Error::operator!=(ErrorCode code) const noexcept
    {
        return !(*this == code);
    }

    static ErrorCode getErrorCode(std::uint64_t data) noexcept
    {
        if (data == 0)
        {
            return ErrorCode::None;
        }

        if (data & kErrorExtMask)
        {
            const auto* ext = toErrorExt(data);
            return ext->code;
        }
        else
        {
            return static_cast<ErrorCode>(data);
        }
    }

    ErrorCode Error::getCode() const noexcept
    {
        return getErrorCode(_data);
    }

    static constexpr const char* getErrorCodeName(ErrorCode err) noexcept
    {
#define ERROR_STRING(e)                                                                                                        \
    case e:                                                                                                                    \
        return #e

        switch (err)
        {
            ERROR_STRING(ErrorCode::None);
            ERROR_STRING(ErrorCode::InvalidMode);
            ERROR_STRING(ErrorCode::NotInitialized);
            ERROR_STRING(ErrorCode::InvalidOperation);
            ERROR_STRING(ErrorCode::InvalidParameter);
            ERROR_STRING(ErrorCode::FileNotFound);
            ERROR_STRING(ErrorCode::AccessDenied);
            ERROR_STRING(ErrorCode::OutOfMemory);
            ERROR_STRING(ErrorCode::LabelNotFound);
            ERROR_STRING(ErrorCode::UnresolvedLabel);
            ERROR_STRING(ErrorCode::InvalidLabel);
            ERROR_STRING(ErrorCode::LabelAlreadyBound);
            ERROR_STRING(ErrorCode::SectionNotFound);
            ERROR_STRING(ErrorCode::SectionAlreadyBound);
            ERROR_STRING(ErrorCode::SignatureMismatch);
            ERROR_STRING(ErrorCode::InvalidInstruction);
            ERROR_STRING(ErrorCode::OutOfBounds);
            ERROR_STRING(ErrorCode::ImpossibleInstruction);
            ERROR_STRING(ErrorCode::EmptyState);
            ERROR_STRING(ErrorCode::ImpossibleRelocation);
            default:
                assert(false);
                break;
        }
#undef ERROR_STRING
        return nullptr;
    }

    const char* Error::getErrorName() const noexcept
    {
        return getErrorCodeName(getCode());
    }

    const char* Error::getErrorMessage() const noexcept
    {
        if (_data & kErrorExtMask)
        {
            const auto* ext = toErrorExt(_data);
            return ext->message.c_str();
        }
        else
        {
            // Give a default message for known error codes.
            switch (getCode())
            {
                case ErrorCode::None:
                    return "No error";
                case ErrorCode::InvalidMode:
                    return "Invalid mode";
                case ErrorCode::NotInitialized:
                    return "Not initialized";
                case ErrorCode::InvalidOperation:
                    return "Invalid operation";
                case ErrorCode::InvalidParameter:
                    return "Invalid parameter";
                case ErrorCode::FileNotFound:
                    return "File not found";
                case ErrorCode::AccessDenied:
                    return "Access denied";
                case ErrorCode::OutOfMemory:
                    return "Out of memory";
                case ErrorCode::LabelNotFound:
                    return "Label not found";
                case ErrorCode::UnresolvedLabel:
                    return "Unresolved label";
                case ErrorCode::InvalidLabel:
                    return "Invalid label";
                case ErrorCode::LabelAlreadyBound:
                    return "Label already bound";
                case ErrorCode::SectionNotFound:
                    return "Section not found";
                case ErrorCode::SectionAlreadyBound:
                    return "Section already bound";
                case ErrorCode::SignatureMismatch:
                    return "Signature mismatch";
                case ErrorCode::InvalidInstruction:
                    return "Invalid instruction";
                case ErrorCode::OutOfBounds:
                    return "Out of bounds";
                case ErrorCode::ImpossibleInstruction:
                    return "Impossible instruction";
                case ErrorCode::EmptyState:
                    return "Empty state";
                case ErrorCode::ImpossibleRelocation:
                    return "Impossible relocation";
                default:
                    break;
            }
        }
        return "";
    }

} // namespace zasm
