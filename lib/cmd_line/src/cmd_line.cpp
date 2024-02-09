#include <algorithm>
#include <iomanip>
#include <sstream>
#include "cmd_line.hpp"

Command_line::Command_line(const std::string& description)
  : description(description),
	arguments()
{
}

void Command_line::add_argument(std::vector<std::string> const& flags,
								const Command_line::Value& value,
								std::string const& help,
								bool required)
{
	arguments.emplace_back(Argument{flags, value, help, required, false});
}

void Command_line::print_help(std::ostream& os) const
{
	// Print the general description.
	if(!description.empty())
		os << description << std::endl;
	os << "Arguments:" << std::endl;
	print_arguments(os, arguments);
}

void Command_line::parse(int argc, char* argv[])
{
	// Skip the first argument (name of the program).
	int i = 1;
	while(i < argc)
	{
		// First we have to identify wether the value is separated by a space
		// or a '='.
		std::string flag(argv[i]);
		std::string value;
		bool valueIsSeparate = false;

		// If there is an '=' in the flag, the part after the '=' is actually
		// the value.
		size_t equalPos = flag.find('=');
		if(equalPos != std::string::npos)
		{
			value = flag.substr(equalPos + 1);
			flag = flag.substr(0, equalPos);
		}
		// Else the following argument is the value.
		else if(i + 1 < argc)
		{
			value = argv[i + 1];
			valueIsSeparate = true;
		}

		// Search for an argument with the provided flag.
		bool foundArgument = false;

		for(auto& argument: arguments)
		{
			foundArgument = std::find(argument.flags.begin(), argument.flags.end(), flag) != std::end(argument.flags);
			if(foundArgument)
			{
				// In the case of booleans, there must not be a value present.
				// So if the value is neither 'true' nor 'false' it is considered
				// to be the next argument.
				if(std::holds_alternative<bool*>(argument.value))
				{
					if(!value.empty() && value != "true" && value != "false")
					{
						valueIsSeparate = false;
					}
					*std::get<bool*>(argument.value) = (value != "false");
				}
				// In all other cases there must be a value.
				else if(value.empty())
				{
					throw std::runtime_error(
						"Failed to parse command line arguments: "
						"Missing value for argument \"" +
						flag + "\"!");
				}
				// For a std::string, we take the entire value.
				else if(std::holds_alternative<std::string*>(argument.value))
				{
					*std::get<std::string*>(argument.value) = value;
				}
				// In all other cases we use a std::stringstream to
				// convert the value.
				else
				{
					std::visit(
						[&value](auto&& arg) {
							std::stringstream sstr(value);
							sstr >> *arg;
						},
						argument.value);
				}
				argument.initialized = true;
				break;
			}
		}

		if(!foundArgument)
		{
			// throw an exception if there was an unknown argument.
			throw std::runtime_error("Unknown command line argument \"" + flag + "\".");
		}

		// Advance to the next flag.
		++i;

		// If the value was separated, we have to advance our index once more.
		if(foundArgument && valueIsSeparate)
		{
			++i;
		}
	}
	std::vector<Argument> uninitialized_arguments;
	uninitialized_arguments.reserve(arguments.size());
	for(auto const& argument: arguments)
	{
		if(argument.required && !argument.initialized)
			uninitialized_arguments.push_back(argument);
	}
	// throw an exception if some required arguments are missing.
	if(!uninitialized_arguments.empty())
	{
		std::stringstream sstr;
		sstr << std::endl;
		print_arguments(sstr, uninitialized_arguments);
		throw std::runtime_error("The following command line arguments are missing:" + sstr.str());
	}
	if(description.empty())
		description = argv[0];
}

void Command_line::print_arguments(std::ostream& os, const std::vector<Argument>& arguments)
{
	// Find the argument with the longest combined flag length (in order
	// to align the help messages).

	uint32_t maxFlagLength = 0;

	for(auto const& argument: arguments)
	{
		uint32_t flagLength = 9;
		for(auto const& flag: argument.flags)
		{
			// Plus comma and space.
			flagLength += static_cast<uint32_t>(flag.size()) + 2;
		}

		maxFlagLength = std::max(maxFlagLength, flagLength);
	}

	// Now print each argument.
	for(auto const& argument: arguments)
	{
		std::string flags(argument.required ? "Required " : "Optional ");
		for(auto const& flag: argument.flags)
			flags += flag + ", ";

		// Remove last comma and space and add padding according to the
		// longest flags in order to align the help messages.
		std::stringstream sstr;
		sstr << std::left << std::setw(maxFlagLength) << flags.substr(0, flags.size() - 2);

		// Print the help for each argument. This is a bit more involved
		// since we do line wrapping for long descriptions.
		size_t spacePos = 0;
		size_t lineWidth = 0;
		while(spacePos != std::string::npos)
		{
			size_t nextspacePos = argument.help.find_first_of(' ', spacePos + 1);
			sstr << argument.help.substr(spacePos, nextspacePos - spacePos);
			lineWidth += nextspacePos - spacePos;
			spacePos = nextspacePos;

			if(lineWidth > 60)
			{
				os << sstr.str() << std::endl;
				sstr = std::stringstream();
				sstr << std::left << std::setw(maxFlagLength - 1) << " ";
				lineWidth = 0;
			}
		}
	}
}
