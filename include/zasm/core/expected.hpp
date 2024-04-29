#pragma once

#include <cassert>
#include <variant>

namespace zasm
{
    namespace detail
    {
        // Internal wrapper type for unexpected.
        template<typename T> struct Unexpected
        {
            const T failure;

            template<typename... TArgs>
            constexpr Unexpected(TArgs&&... args) noexcept
                : failure{ std::forward<TArgs>(args)... }
            {
            }
        };
    } // namespace detail

    /// <summary>
    /// Provides a way to return a value or an error.
    /// This is not an exact replacement for std::expected but it is close enough.
    /// To return an error you can use the makeUnexpected function.
    /// </summary>
    /// <typeparam name="TSuccess">Success Type</typeparam>
    /// <typeparam name="TFailure">Failure Type</typeparam>
    template<typename TSuccess, typename TFailure> class Expected
    {
        std::variant<TSuccess, ::zasm::detail::Unexpected<TFailure>> data;

    public:
        constexpr Expected(const TSuccess& val) noexcept
            : data{ val }
        {
        }
        constexpr Expected(TSuccess&& val) noexcept
            : data{ std::move(val) }
        {
        }
        constexpr Expected(const ::zasm::detail::Unexpected<TFailure>& val) noexcept
            : data{ val }
        {
        }
        constexpr Expected(::zasm::detail::Unexpected<TFailure>&& val) noexcept
            : data{ std::move(val) }
        {
        }

        constexpr operator bool() const noexcept
        {
            return hasValue();
        }

        constexpr bool hasValue() const noexcept
        {
            return std::holds_alternative<TSuccess>(data);
        }

        constexpr const TFailure& error() const noexcept
        {
            assert(!hasValue());
            return std::get_if<::zasm::detail::Unexpected<TFailure>>(&data)->failure;
        }

        constexpr TSuccess& value() noexcept
        {
            assert(hasValue());
            return *std::get_if<TSuccess>(&data);
        }

        constexpr const TSuccess& value() const noexcept
        {
            assert(hasValue());
            return *std::get_if<TSuccess>(&data);
        }

        constexpr TSuccess& operator*() noexcept
        {
            return value();
        }

        constexpr const TSuccess& operator*() const noexcept
        {
            return value();
        }

        constexpr TSuccess* operator->() noexcept
        {
            assert(hasValue());
            return std::get_if<TSuccess>(&data);
        }

        constexpr const TSuccess* operator->() const noexcept
        {
            assert(hasValue());
            return std::get_if<TSuccess>(&data);
        }
    };

    template<typename T> inline constexpr auto makeUnexpected(T&& e) noexcept
    {
        return ::zasm::detail::Unexpected<std::decay_t<T>>(std::forward<T>(e));
    }

    template<typename T, typename... TArgs>
    inline constexpr auto makeUnexpected(TArgs&&... args) noexcept
    {
        return ::zasm::detail::Unexpected<std::decay_t<T>>(std::forward<TArgs>(args)...);
    }

} // namespace zasm
