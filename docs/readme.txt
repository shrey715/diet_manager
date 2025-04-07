# Diet Manager

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

