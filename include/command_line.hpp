#ifndef COMMAND_LINE_HPP
#define COMMAND_LINE_HPP

#include <algorithm>
#include <vector>
#include <string>
#include <sstream>
#include <unordered_map>
#include <getopt.h>

/*!
@namespace cmdl
@brief Command Line
*/
namespace cmdl
{
    class Parser
    {
    public:
        Parser(int argc, char* argv[]) :
            argc_{ argc },
            argv_{ argv },
            command_name_{ argv[0] }
        {
        }

        void add(const std::string& name, char short_name, std::string description,bool has_argument)
        {
            options_.emplace_back(name, short_name, description, has_argument);
        }

        bool parse()
        {
            std::string optstring;

            for(const auto& o : options_) {
                optstring += o.short_name;

                if(o.has_argument) {
                    optstring += ':';
                }
            }

            // getopt.hで宣言されているoption構造体を用いる
            std::vector<option> longopts;

            for(const auto& o : options_) {
                longopts.push_back({ o.name.c_str(), static_cast<int>(o.has_argument), nullptr, static_cast<int>(o.short_name) });
            }

            // ゼロ終端
            longopts.push_back({ 0, 0, 0, 0 });


            int opt;
            int longindex;

            while((opt = getopt_long(argc_, argv_, optstring.c_str(), longopts.data(), &longindex)) != -1) {
                if(opt == '?') {
                    return false;
                }

                auto it = std::find_if(
                        options_.begin(),
                        options_.end(),
                        [opt](const Option& o) {
                            return o.short_name == static_cast<char>(opt);
                        }
                );

                if(it == options_.end()) {
                    return false;
                }

                if (it->has_argument) {
                    results_.emplace(it->name, Result{ it->name, it->short_name, optarg, it->has_argument });
                } else {
                    results_.emplace(it->name, Result{ it->name, it->short_name, "", it->has_argument });
                }
            }

            // オプション以外の引数を格納
            for(int i = optind; i < argc_; ++i) {
                arguments_.emplace_back(argv_[i]);
            }

            return true;
        }

        template<typename T>
        T get(const std::string& name, T initial_value = T())
        {
            if(!exist(name)) {
                return T(initial_value);
            }

            T r;
            std::istringstream ss(results_[name].argument);
            ss >> r;

            return r;
        }

        // template<typename T>
        // T getRestArgument(size_t i)
        // {
        //     std::string arg = arguments_[i];
        //
        //     T r;
        //     std::istringstream ss(arg);
        //     ss >> r;
        //
        //     return r;
        // }

        bool exist(const std::string& name)
        {
            auto it = results_.find(name);

            if(it == results_.end()) {
                return false;
            }

            return true;
        }

        std::string usage()
        {
            std::ostringstream ss;

            ss << "Usage:" << std::endl;
            ss << "    " << command_name_ << " [options] ..." << std::endl;
            ss << '\n';

            size_t max_name_length = 0;
            for (const auto& o : options_) {
                max_name_length = std::max(max_name_length, o.name.length());
            }

            ss << "Options:" << std::endl;
            for (const auto& o : options_) {
                ss << "    -" << o.short_name << ", --" << o.name;
                for (size_t i = o.name.length(); i < max_name_length; ++i) {
                    ss << ' ';
                }

                ss << "    " << o.description << std::endl;
            }

            return ss.str();
        }

    private:
        struct Result
        {
            std::string name;
            char short_name;
            std::string argument;

            bool has_argument;
        };

        struct Option
        {
            std::string name;
            char short_name;
            std::string description;
            bool has_argument;

            Option(const std::string& _name, char _short_name, std::string _description, bool _has_argument) :
                name{ _name },
                short_name{ _short_name },
                description{ _description },
                has_argument{ _has_argument }
            {}
        };

    private:
        int argc_;
        char** argv_;

        std::string command_name_;

        std::vector<Option> options_;
        std::unordered_map<std::string, Result> results_;

        std::vector<std::string> arguments_;
    };
}

#endif
