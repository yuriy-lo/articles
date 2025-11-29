#pragma once

#include <array>
#include <meta>
#include <optional>
#include <string_view>
#include <unordered_map>
#include <cstring>
#include <tuple>
#include <type_traits>
#include <charconv>

namespace argparse
{
    // Parse command line of the form:
    //   program command --param1 val1 --param2 val2 ...
    //   program command --param1=val1 --param2=val2 ...
    // Values are required
    std::pair<std::string_view, std::unordered_map<std::string_view, std::string_view>> parse_cmd(int argc, char *argv[])
    {
        using MapT = std::unordered_map<std::string_view, std::string_view>;
        if (argc < 2)
            return {std::string_view{}, MapT{}}; // no command provided

        std::string_view command{argv[1]};
        MapT params;

        for (int i = 2; i < argc; ++i)
        {
            const char *arg = argv[i];
            if (std::strncmp(arg, "--", 2) != 0)
            {
                // skip unexpected token (could be positional argument)
                continue;
            }

            const char *name_start = arg + 2; // skip leading '--'

            // Support --name=value form
            const char *eq = std::strchr(name_start, '=');
            if (eq)
            {
                std::string_view name{name_start, static_cast<size_t>(eq - name_start)};
                std::string_view value{eq + 1};
                params.emplace(name, value);
                continue;
            }

            // Otherwise expect next argv element to be the value
            std::string_view name{name_start};
            // Only add the parameter if a value follows (i.e. not another `--` option
            // and not end-of-argv). If no value is present we skip adding the param.
            if (i + 1 < argc && std::strncmp(argv[i + 1], "--", 2) != 0)
            {
                std::string_view value{argv[i + 1]};
                params.emplace(name, value);
                ++i; // consume value
            }
            // else: no value available -> do not insert the parameter
        }

        return {std::move(command), std::move(params)};
    }

    template <typename T>
    std::optional<T> convert_string(std::string_view s);

    template <>
    std::optional<int> convert_string<int>(std::string_view s)
    {
        int result;
        auto [ptr, ec] = std::from_chars(s.data(), s.data() + s.size(), result);
        if (ec == std::errc())
        {
            return result;
        }
        return std::nullopt;
    }

    template <>
    std::optional<std::string_view> convert_string<std::string_view>(std::string_view s)
    {
        return s;
    }

    template <>
    std::optional<bool> convert_string<bool>(std::string_view s)
    {
        if (s == "true" || s == "1")
            return true;
        if (s == "false" || s == "0")
            return false;
        return std::nullopt;
    }

    namespace __impl
    {
        template <auto... vals>
        struct replicator_type
        {
            template <typename F>
            constexpr auto make_tuple(F body) const
            {
                return std::make_tuple(body.template operator()<vals>()...);
            }
        };

        template <auto... vals>
        replicator_type<vals...> replicator = {};

        // Implementation was copied from paper P2996R13 "Reflection for C++26", see
        // https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2025/p2996r13.html#implementation-status
        template <typename R>
        consteval auto expand(R range)
        {
            std::vector<std::meta::info> args;
            for (auto r : range)
            {
                args.push_back(std::meta::reflect_constant(r));
            }
            return std::meta::substitute(^^__impl::replicator, args);
        }
    }

    // Creates std::tuple containing values of function parameters by converting them from std::string_view.
    // Returns std::optional which has no value if one of function parameters was not specified or was ill-formed.
    template <auto Function>
    auto get_parameters_values(const std::unordered_map<std::string_view, std::string_view> &parameters)
    {
        bool all_params_parsed = true;
        auto values = [:__impl::expand(std::meta::parameters_of(Function)):].make_tuple([&]<auto param>
        {
            constexpr auto param_name = std::meta::identifier_of(param);
            auto it = parameters.find(std::string_view(param_name));
            if (it != parameters.end())
            {
                const auto &raw_value = it->second;
                auto value = convert_string<typename[:std::meta::type_of(param):]>(raw_value);
                if (value)
                {
                    return *value;
                }
            }
            all_params_parsed = false;
            return typename[:std::meta::type_of(param):]{};
        });
        using Result = std::optional<decltype(values)>;
        return all_params_parsed ? Result{values} : Result{};
    }

    // Selects a function by runtime command name and passes control to it.
    // Fuction parameters values are converted from std::string_view.
    template <auto... Functions>
    int run(std::string_view command, const std::unordered_map<std::string_view, std::string_view> &params)
    {
        constexpr auto function_infos = std::array{std::meta::reflect_function(*Functions)...};
        template for (constexpr auto function_info : function_infos)
        {
            if (std::meta::identifier_of(function_info) == command)
            {
                if (auto values = get_parameters_values<function_info>(params))
                {
                    return std::apply([:function_info:], *values);
                }
            }
        }
        return 1;
    }

    // Parses command line of the form:
    //   program command --param1 val1 --param2 val2 ...
    // Selects a function by runtime command name and passes control to it.
    template <auto... Functions>
    int run(int argc, char *argv[])
    {
        const auto arguments = parse_cmd(argc, argv);
        return run<Functions...>(arguments.first, arguments.second);
    }
}
