#include <postgres_client/args.hpp>

#include <gtest/gtest.h>


using namespace postgres_client;


class ArgvBuilder {
    std::vector<std::string> m_args_buffer;
    std::unique_ptr<char *[]> m_argv;

public:
    const int argc;

    explicit ArgvBuilder(std::initializer_list<const char *> args)
        : m_argv(new char *[args.size()]),
          argc(args.size()) {
        std::size_t i = 0;
        for (auto iter = args.begin(); iter != args.end(); ++i, ++iter) {
            m_args_buffer.emplace_back(*iter);
            m_argv.get()[i] = const_cast<char *>(m_args_buffer.back().c_str());
        }
    }

    char **argv() const {
        return m_argv.get();
    }
};


TEST(HandleOptionsTests, UnrecognisedOptionsDonotThrow) {
    auto options = BuildOptions();
    ArgvBuilder argv_builder{"runner", "--no-such-option"};

    ASSERT_NO_THROW(HandleOptions(options, argv_builder.argc, argv_builder.argv()));
}


TEST(HandleOptionsTests, MissingValueOptionsDonotThrow) {
    const std::string OPTION_NAME = "a-test-option";
    const std::string FULL_OPTION_NAME = "--" + OPTION_NAME;
    auto options = BuildOptions();
    options.add_options()(OPTION_NAME, "A test option", cxxopts::value<int>());
    ArgvBuilder argv_builder{"runner", FULL_OPTION_NAME.c_str()};

    ASSERT_NO_THROW(HandleOptions(options, argv_builder.argc, argv_builder.argv()));
}
