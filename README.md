# Diet Manager

A simple command-line application to track food consumption and manage dietary goals.

## Features

- Maintain a database of basic food items with nutritional information
- Create composite foods from existing basic and composite foods
- Track daily food consumption in logs
- Set and monitor caloric intake goals based on personal profile
- Calculate target calorie intake using different methods (Harris-Benedict, Mifflin-St Jeor, or WHO equation)
- Undo/redo functionality for log operations
- Search foods by keywords

## Building

### Prerequisites

- C++17 compatible compiler
- CMake (3.10 or higher)
- nlohmann_json library

### Installing Dependencies

```bash
# For Ubuntu/Debian
sudo apt-get install build-essential cmake
sudo apt-get install nlohmann-json3-dev

# For Fedora
sudo dnf install gcc-c++ cmake
sudo dnf install nlohmann-json-devel

# For Arch Linux
sudo pacman -S base-devel cmake
sudo pacman -S nlohmann-json
```

If you can't install the nlohmann_json library system-wide, the project includes alternative methods for handling this dependency. See the "Handling nlohmann_json" section below.

### Build Instructions

Using CMake:

```bash
mkdir build
cd build
cmake ..
make
```

Or using the Makefile directly:

```bash
make
```

### Handling nlohmann_json

If the system-wide installation doesn't work, you can use one of these alternatives:

#### Option 1: Include as Single Header

```bash
mkdir -p include
curl -o include/json.hpp https://raw.githubusercontent.com/nlohmann/json/develop/single_include/nlohmann/json.hpp
```

Then update CMakeLists.txt to include this directory.

#### Option 2: Use FetchContent in CMake

Add to CMakeLists.txt:

```cmake
include(FetchContent)
FetchContent_Declare(
    json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.2
)
FetchContent_MakeAvailable(json)
```

Then link with `nlohmann_json::nlohmann_json`.

## Running

```bash
./diet_manager
```

## Usage

The application provides a command-line interface with the following commands:

### General Commands

- `help` - Display available commands
- `quit` or `exit` - Exit the program

### Food Database Commands

- `add-basic-food <calories> <keyword1> [keyword2] ...` - Add a new basic food
- `list-foods` - List all available foods
- `search-foods <keyword1> [keyword2] ... [--all]` - Search for foods by keywords
- `create-composite <keyword1> [keyword2] ... --components <food1> <servings1> [<food2> <servings2> ...]` - Create a composite food

### Log Management Commands

- `add-food <food_id> <servings>` - Add food to the current day's log
- `remove-food <food_id>` - Remove food from the log
- `view-log [date]` - View the log for a specific date or current date
- `set-date <YYYY-MM-DD>` - Set the current working date
- `undo` - Undo the last log operation
- `redo` - Redo the last undone operation

### User Profile Commands

- `profile` - Display the user profile
- `profile <attribute> <value>` - Update a profile attribute
- `calories [date]` - Show calorie intake and target

### Data Management Commands

- `save` - Save all data to files
- `load` - Load all data from files

## Data Storage

The application uses JSON files to store data:

- `basic_food.json` - Basic food items database
- `composite_food.json` - Composite food items database
- `logs.json` - Daily food consumption logs
- `user.json` - User profile information

## Example

```
> add-basic-food apple 95 fruit sweet
Food 'apple' added successfully.

> add-food apple 2
Added 2 serving(s) of 'apple' (190 calories) to the log.

> calories
Calorie Summary for 2023-04-01:
Consumed Calories: 190
Target Calories: 2500
Difference: -2310 (Under target)
```

## Design Features

### Extensibility

1. **New Calorie Calculation Methods:** The system is designed to easily incorporate new methods for calculating target calories by simply adding a new calculation method to the User class and extending the CalorieCalculationMethod enum.

2. **External Food Data Sources:** The FoodDatabase class includes a plugin architecture allowing new food data sources to be registered using the registerFoodDataSource method, enabling the application to import food data from external sources like restaurant websites or nutritional databases.

3. **Command Pattern for Logs:** Log operations use the Command pattern to enable unlimited undo/redo functionality while keeping a single source of truth for data.

### Efficiency

1. **Shared Food References:** The system uses shared pointers to food objects, ensuring that multiple references to the same food don't duplicate memory.

2. **Composite Pattern for Foods:** The application implements the Composite pattern for food items, allowing complex foods to be built from simpler ones while maintaining a consistent interface.

## Notes

This is an academic project built as a prototype. Some features may not be fully implemented or tested.