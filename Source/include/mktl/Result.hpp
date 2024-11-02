/********************************************************************************
 *  MCKRUEG STL - mckrueg's standard template library of C++ useful stuff       *
 *  Copyright (C) 2024 Matthew Krueger <contact@matthewkrueger.com>             *
 *                                                                              *
 *  This program is free software: you can redistribute it and/or modify        *
 *  it under the terms of the GNU General Public License as published by        *
 *  the Free Software Foundation, either version 3 of the License, or           *
 *  (at your option) any later version.                                         *
 *                                                                              *
 *  This program is distributed in the hope that it will be useful,             *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of              *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the               *
 *  GNU General Public License for more details.                                *
 *                                                                              *
 *  You should have received a copy of the GNU General Public License           *
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.      *
 ********************************************************************************/

/*************************************
 * The mckrueg stl requires
 * C++ 17
 *************************************/

#if __cplusplus < 201703L
# error MCKRUEG STL requires the use of C++ 17
#endif

#ifndef ASSIGNMENT_4_MCKRUEGSTL_RESULT_HPP
#define ASSIGNMENT_4_MCKRUEGSTL_RESULT_HPP

#include <iostream>
#include <type_traits>
#include <utility>

// Helper to check if a type can be used with std::ostream
template<typename T>
auto has_ostream_operator(int) -> decltype(std::declval<std::ostream&>() << std::declval<T>(), std::true_type{});

template<typename>
auto has_ostream_operator(...) -> std::false_type;

// Panic macro without error
#define panic(message) \
    do { \
        std::cerr << message << std::endl; \
        exit(0xFF); \
    } while(0)

// Panic macro with error checking for operator<<
#define panic_error(message, error) \
    { \
        std::cerr << message; \
        if constexpr (decltype(has_ostream_operator<decltype(error)>(0))::value) { \
            std::cerr << "\n\t Message: " << error; \
        } else { \
            std::cerr << "\n\t Message: [Error not printable]"; \
        } \
        std::cerr << std::endl; \
        exit(0xFF); \
    } (void(0))

    //god this took me a long time to get right. thanks Grok for helping.

namespace mckrueg::stl{

    /**
     * \brief A result monad inspired by rust
     * @tparam Ok The wanted type
     * @tparam Err The unwanted type
     */
    template<typename Ok, typename Err>
    class Result{
    public:
        Result() = delete;

        Result(const Ok& value) : m_Value(value){}
        Result(const Err& value) : m_Value(value){}

        // Don't need a copy constructor as variant will copy correctly anyway.

        /**
         * Copy assignment operator
         * @param other The rhs
         * @return The lhs
         */
        inline Result<Ok, Err>& operator=(const Result<Ok, Err>& other){
            if (this != &other) {
                m_Value = other.m_Value;
            }
            return *this;
        }

        /**
         * Checks if the stored value is of type OK
         * @return if the stored value is of type OK
         */
        [[nodiscard]] inline bool is_ok() const noexcept { return std::holds_alternative<Ok>(m_Value); }

        /**
         * Checks if the value is of type error
         * @return if the value is of type error
         */
        [[nodiscard]] inline bool is_err() const noexcept { return std::holds_alternative<Err>(m_Value); }

        /**
         * \brief Returns the value stored in the monad
         * \note Panics if not a Ok type
         * @return the Ok value
         */
        [[nodiscard]] inline Ok unwrap() const noexcept { return expect("Result is not an Ok type"); };

        /**
         * \brief Unwraps the monad.
         * \note Panics with message specified here
         * @param message message to panic with
         * @return The Ok value
         */
        Ok expect(const std::string& message) const noexcept;

        /**
         * \brief Unwraps the monad or returns a default value.
         * @param defaultValue The default value to return
         * @return The monad value or default value
         */
        Ok unwrap_or(Ok defaultValue) const noexcept;

        /**
         * \brief Unwraps the monad or runs func
         * @param func A function to run if the value is of type error
         * @return The Ok value, or what was computed by func.
         */
        Ok unwrap_or_else(std::function<Ok()> func) const noexcept;

        /**
         * \brief Unwraps an error value
         * @return
         */
        Err unwrap_err() const;

        /**
         * \brief Unwraps the monad in a similar method to Swift bang operator
         * \note This is done this way because I cannot create random operators in c++ :(
         * \note will panic if not of the Ok type
         * @return The OK value.
         */
        Ok operator!() const noexcept { return unwrap(); }


        /**
         * \brief Mimics a match statement from rust.
         * @tparam OkFunc The lambda to run when the value is OK
         * @tparam ErrFunc The value to run when the value is Err
         * @param ok The Ok Function
         * @param err The Err Function
         * @return The type retuned by the OK and Error functions.
         */
        template<typename OkFunc, typename ErrFunc,
                typename = std::enable_if_t<
                        std::is_void_v<decltype(std::declval<ErrFunc>()(Err()))> &&
                        std::is_void_v<decltype(std::declval<OkFunc>()(Ok()))>
                >> // thank god for AI. Holy crap. this I struggled with. I will admit that I copied this from AI
        void match(OkFunc&& ok, ErrFunc&& err) const noexcept;

        /**
         * \brief Syntactic sugar for map
         * Errors are forwarded to the new result if they exist
         * @tparam U New type that's being transformed into
         * @tparam F The function doing the transforming
         * @param func The parameter of the function doing the transforming
         * @return The transformed result
         */
        template<typename U, typename F>
        Result<U, Err> and_then(F&& func) const noexcept { return map<U>(func); };

        /**
         * \brief Maps a Result<Ok, Err> to a Result<U, Err>
         * Errors are forwarded to the new result if they exist
         * @tparam U New type that's being transformed into
         * @tparam F The function doing the transforming
         * @param func The parameter of the function doing the transforming
         * @return The transformed result
         */
        template<typename U, typename F>
        Result<U, Err> map(F&& func) const noexcept;

        /**
         * \brief Maps a Result<Ok, Err> to a Result<Ok, E>
         * Results  are forwarded to the new result if they exist
         * @tparam E New type that's being transformed into
         * @tparam F The function doing the transforming
         * @param func The parameter of the function doing the transforming
         * @return The transformed result
         */
        template<typename E, typename F>
        Result<Ok, E> map_err(F&& func) const noexcept;

        /**
         * \brief Non-panic version of unwrap
         * @return An optional wrapped OK
         */
        std::optional<Ok> ok() const noexcept;

        /**
         * \brief Non-panic version of err
         * @return An optional wrapped err
         */
        std::optional<Err> err() const noexcept;

        /**
         * Returns Either an OK result, OR returns a specified result
         * @param other The other result
         * @return The computed result
         */
        Result<Ok, Err>& either_or(Result<Ok, Err>& other) const noexcept;

        /**
         * \brief If the result stored Ok value is equal to given value. Requires operator= on Ok
         * @param value Value to compare
         * @return if Value is the ok value
         */
        bool contains(const Ok& value) const noexcept;
        /**
         * \brief If the result stored Err value is equal to given value. Requires operator= on Err
         * @param error Value to compare
         * @return if Error is the Error value
         */
        bool contains_err(const Err& error) const noexcept;

    private:
        std::variant<Ok, Err> m_Value;
    };

    class Error{
    public:

        Error() = default;
        explicit Error(std::string value) : m_Value(std::move(value)){};

        inline Error& operator=(const Error& rhs){
            if (this != &rhs) {
                m_Value = rhs.m_Value;
            }
            return *this;
        }

        [[nodiscard]] inline const std::string& get() const { return m_Value; }

        inline friend std::ostream& operator<<(std::ostream& os, const Error& rhs) { os << rhs.m_Value; return os; }

    private:
        std::string m_Value;

    };


    template<typename Ok, typename Err>
    Ok Result<Ok, Err>::expect(const std::string &message) const noexcept {
        if(is_ok()){
            return std::get<Ok>(m_Value);
        }else{
            panic_error(message,std::get<Err>(m_Value));
        }
    }

    template<typename Ok, typename Err>
    Ok Result<Ok, Err>::unwrap_or(Ok defaultValue) const noexcept {
        if(is_ok()){
            return std::get<Ok>(m_Value);
        }else{
            return defaultValue;
        }
    }

    template<typename Ok, typename Err>
    Ok Result<Ok, Err>::unwrap_or_else(std::function<Ok()> func) const noexcept {
        if(is_ok()){
            return std::get<Ok>(m_Value);
        }else{
            return func();
        }
    }

    template<typename Ok, typename Err>
    Err Result<Ok, Err>::unwrap_err() const{
        if(is_err()){
            return std::get<Err>(m_Value);
        }else{
            panic("Cannot unwrap an valid type as an error");
        }
    };


    // I'm going to be honest, I copy and pasted this one from AI. It kicked my butt
    template<typename Ok, typename Err>
    template<typename OkFunc, typename ErrFunc, typename>
    void Result<Ok, Err>::match(OkFunc &&okfunction, ErrFunc &&errfunction) const noexcept {

        std::visit([&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, Ok>) {
                okfunction(std::forward<decltype(arg)>(arg));
            } else if constexpr (std::is_same_v<T, Err>) {
                errfunction(std::forward<decltype(arg)>(arg));
            }
        }, m_Value);

    }

    template<typename Ok, typename Err>
    template<typename U, typename F>
    Result<U, Err> Result<Ok, Err>::map(F &&func) const noexcept{

        if(is_ok()){
            return Result<U, Err>(func(std::get<Ok>(m_Value)));
        } else{
            return Result<U, Err>(std::get<Err>(m_Value));
        }

    }

    template<typename Ok, typename Err>
    template<typename E, typename F>
    Result<Ok, E> Result<Ok, Err>::map_err(F &&func) const noexcept {

        if(is_err()){
            return Result<Ok, E>(func(std::get<Err>(m_Value)));
        } else{
            return Result<Ok, E>(std::get<Ok>(m_Value));
        }

    }

    template<typename Ok, typename Err>
    std::optional<Ok> Result<Ok, Err>::ok() const noexcept {
        if (is_ok()){
            return {std::get<Ok>()};
        } else {
            return {};
        }
    }

    template<typename Ok, typename Err>
    std::optional<Err> Result<Ok, Err>::err() const noexcept {
        if (is_err()){
            return {std::get<Err>()};
        } else {
            return {};
        }
    }

    template<typename Ok, typename Err>
    Result<Ok, Err>& Result<Ok, Err>::either_or(Result<Ok, Err>& other) const noexcept {
        if(is_ok()){
            return *this;
        }else{
            return other;
        }
    }

    template<typename Ok, typename Err>
    bool Result<Ok, Err>::contains(const Ok &value) const noexcept {
        if(is_err()) return false;
        if(unwrap() == value) return true;
        return false;
    }

    template<typename Ok, typename Err>
    bool Result<Ok, Err>::contains_err(const Err &error) const noexcept {
        if(is_ok()) return false;
        if(unwrap_err() == error) return true;
        return false;
    }

}


#endif //ASSIGNMENT_4_MCKRUEGSTL_RESULT_HPP
