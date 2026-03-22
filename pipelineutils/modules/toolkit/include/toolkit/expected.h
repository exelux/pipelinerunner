#pragma once

#include <Plt2/Tools/STL/optional>

namespace Toolkit {

namespace STL = Plt2::Tools::STL;

template<typename T, typename E>
class Expected
{
    static_assert(!std::is_same<T, E>::value,
                  "Expected: ResultType and ErrorType must be different types");

public:
    using ResultType = T;
    using ErrorType = E;

public:
    constexpr Expected(ResultType&& result)
        : _result(std::forward<ResultType>(result))
        , _error(STL::nullopt)
    {
    }

    constexpr Expected(ErrorType&& error)
        : _result(STL::nullopt)
        , _error(std::forward<ErrorType>(error))
    {
    }

    Expected(const Expected& other)
        : _result(other._result)
        , _error(other._error)
    {
    }

    Expected(Expected&& other) noexcept(std::is_nothrow_move_constructible<ResultType>::value
                                        && std::is_nothrow_move_constructible<ErrorType>::value)
        : _result(std::move(other._result))
        , _error(std::move(other._error))
    {
    }

public:
    Expected& operator=(const Expected& other)
    {
        if (this != &other)
        {
            _result = other._result;
            _error = other._error;
        }

        return *this;
    }

    Expected& operator=(Expected&& other) noexcept(
        std::is_nothrow_move_assignable<ResultType>::value
        && std::is_nothrow_move_assignable<ErrorType>::value)
    {
        if (this != &other)
        {
            _result = std::move(other._result);
            _error = std::move(other._error);
        }

        return *this;
    }

public:
    template<typename... Args>
    static Expected success(Args&&... args)
    {
        return Expected(ResultType(std::forward<Args>(args)...));
    }

    template<typename... Args>
    static Expected error(Args&&... args)
    {
        return Expected(ErrorType(std::forward<Args>(args)...));
    }

public:
    constexpr bool hasResult() const noexcept
    {
        return _result.has_value();
    }
    constexpr bool hasError() const noexcept
    {
        return _error.has_value();
    }
    constexpr explicit operator bool() const noexcept
    {
        return hasResult();
    }

public:
    ResultType& result() &
    {
        return _result.value();
    }
    ResultType&& result() &&
    {
        return std::move(_result.value());
    }
    const ResultType& result() const&
    {
        return _result.value();
    }

    ErrorType& error() &
    {
        return _error.value();
    }
    ErrorType&& error() &&
    {
        return std::move(_error.value());
    }
    const ErrorType& error() const&
    {
        return _error.value();
    }

private:
    STL::optional<T> _result;
    STL::optional<E> _error;
};

} // namespace Toolkit
