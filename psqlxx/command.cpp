#include <psqlxx/command.hpp>

#include <cassert>

#include <iomanip>
#include <iostream>
#include <regex>

#include <psqlxx/exception.hpp>


using namespace std::string_literals;


namespace {

[[nodiscard]]
inline bool validName(const std::string_view name) {
    if (name.empty()) {
        return true;
    }

    static const std::regex name_pattern("[a-zA-Z]+");
    return std::regex_match(name.data(), name_pattern);
}

[[nodiscard]]
inline bool validArgument(const std::string_view argument) {
    if (argument.empty()) {
        return false;
    }

    static const std::regex argument_pattern("[A-Z]+");
    return std::regex_match(argument.data(), argument_pattern);
}

}


namespace psqlxx {

namespace internal {

bool validCommand(const Command::NamesType &names,
                  const Command::NamesType &arguments,
                  const Command::ActionType action) {
    if (not action) {
        return false;
    }

    for (const auto &a_name : names) {
        if (not validName(a_name)) {
            return false;
        }
    }

    for (const auto &an_argument : arguments) {
        if (not validArgument(an_argument)) {
            return false;
        }
    }

    return true;
}

}//namespace internal


void Command::Help() const {
    // std::cout << names << '\t' << m_arguments << '\t' << m_description << std::endl;
}

CommandResult Command::operator()(const char **words,
                                  const int word_count) const {
    assert(m_action);
    assert(words);
    assert(word_count > 0);

    const ArgumentsType::size_type number_arguments = word_count - 1;
    if (number_arguments > m_arguments.size()) {
        std::cerr << "Command (" << words[0] << ") failed: Too many arguments. Expected " <<
                  m_arguments.size() << ", but " << number_arguments << " were given." << std::endl;
        return CommandResult::failure;
    }

    return m_action(words, word_count);
}


void CommandGroup::AddOneOption(Command::NamesType names,
                                Command::NamesType arguments,
                                Command::ActionType action,
                                std::string description) {
    assert(internal::validCommand(names, arguments, action));

    if (names.empty()) {
        names.push_back("");
    }

    const auto command_ptr = std::make_shared<Command>(std::move(names),
                                                       std::move(arguments), action, description);
    for (const char *command_name : command_ptr->names) {
        if (not command_name or strlen(command_name) == 0) {
            if (m_anonymous_command)
                throw CommandException{"Only one anonymous command is allowed within one group"};
            m_anonymous_command = command_ptr;
        } else {
            const auto insertion_happened = m_name_command_map.emplace(command_name,
                                                                       command_ptr).second;
            if (not insertion_happened) {
                throw CommandException{"Duplicate command name '"s + command_name + "' within one group"};
            }
        }
    }

    m_commands.push_back(command_ptr);
}

void CommandGroup::Help() const {
    for (const auto command : m_commands) {
        command->Help();
    }
}

void CommandGroup::Describe() const {
    std::cout << m_name << '\t' << m_description << std::endl;
}

CommandResult CommandGroup::operator()(const char **words,
                                       const int word_count) const {
    assert(words);
    assert(word_count > 0);

    const auto iter = m_name_command_map.find(words[0]);
    if (iter != m_name_command_map.cend()) {
        return (*(iter->second))(words, word_count);
    }

    if (m_anonymous_command) {
        return (*m_anonymous_command)(words, word_count);
    }

    return CommandResult::unknown;
}


CommandResult HelpGroups(const std::vector<CommandGroup> &groups,
                         const std::string_view name) {
    if (name.empty()) {
        for (const auto &one_group : groups) {
            one_group.Describe();
        }
    } else {

        bool match_found = false;
        for (const auto &one_group : groups) {
            if (one_group.Name() == name) {
                one_group.Help();
                match_found = true;
                break;
            }
        }

        if (not match_found) {
            std::cerr << "Unknown command group " << std::quoted(name) << std::endl;
            return CommandResult::failure;
        }
    }

    return CommandResult::success;
}

}//namespace psqlxx
