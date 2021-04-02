#pragma once

#include <functional>
#include <memory>
#include <vector>
#include <unordered_map>

namespace psqlxx {

enum class CommandResult {
    success,
    failure,
    unknown,
    exit,
};


struct Command {
    using NamesType = std::vector<const char *>;
    using ArgumentsType = std::vector<std::string_view>;
    using ActionType = std::function<CommandResult(const char **, const int)>;

    NamesType names;
    ArgumentsType arguments;
    ActionType action;
    std::string description;

    void Help() const;
    [[nodiscard]]
    CommandResult operator()(const char **words, const int word_count) const;
};


class CommandGroup {
    class OptionAdder {
        CommandGroup &m_group;

    public:
        explicit OptionAdder(CommandGroup &group): m_group(group) {
        }

        template <typename... Args>
        const OptionAdder &operator()(Args &&... args) const {
            m_group.AddOptions(std::forward<Args>(args)...);
            return *this;
        }
    };

    std::string_view m_name;
    std::string m_description;
    std::vector<std::shared_ptr<Command>> m_commands;
    std::unordered_map<Command::NamesType::value_type, std::shared_ptr<Command>>
            m_name_command_map;
    std::shared_ptr<Command> m_anonymous_command;

public:
    CommandGroup(const std::string_view name,
                 std::string description): m_name(name), m_description(std::move(description)) {
    }

    [[nodiscard]]
    const auto Name() const {
        return m_name;
    }

    [[nodiscard]]
    const OptionAdder AddOptions() {
        return OptionAdder{*this};
    }

    void AddOneOption(Command::NamesType names,
                      Command::NamesType arguments,
                      Command::ActionType action,
                      std::string description);

    void Help() const;
    void Describe() const;
    [[nodiscard]]
    CommandResult operator()(const char **words, const int word_count) const;
};


[[nodiscard]]
inline const auto Quit(const char **, const int) {
    return CommandResult::exit;
}

[[nodiscard]]
CommandResult HelpGroups(const std::vector<CommandGroup> &groups,
                         const std::string_view name = {});


namespace internal {

[[nodiscard]]
bool validCommand(const Command::NamesType &names,
                  const Command::NamesType &arguments,
                  const Command::ActionType action);

}//namespace internal

}//namespace psqlxx
