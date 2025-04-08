Diet Manager - README
=====================

This readme file describes how to compile, run, and use the Diet Manager application.

COMPILATION INSTRUCTIONS
-----------------------
Option 1: Using CMake
1. Ensure you have CMake (3.10+) and a C++17 compatible compiler installed
2. Ensure you have the nlohmann_json library installed
3. Run the following commands:
   mkdir build
   cd build
   cmake ..
   make

Option 2: Using Makefile directly
1. Run the following command from the project root:
   make

RUNNING THE APPLICATION
-----------------------
After compilation, run the executable:
   ./diet_manager

The application will start in the command-line interface mode.

FEATURES AND USAGE
-----------------
1. First Time Setup:
   - On first run, you will be prompted to create a user profile
   - Enter your name, age, gender, height, weight, and other required information

2. General Commands:
   - help                     : Display available commands
   - help <command>           : Show detailed help for a specific command
   - exit or quit             : Exit the program

3. Food Database Management:
   - add-basic-food <calories> <keyword1> [keyword2] ...
     Example: add-basic-food 95 apple fruit
   
   - list-foods               : List all available foods in the database
   
   - search-foods <keyword1> [keyword2] ... [--all]
     Example: search-foods fruit --all

   - create-composite <keyword1> [keyword2] ... --components <food1> <servings1> [<food2> <servings2> ...]
     Example: create-composite fruit salad dessert --components apple 1 banana 1 orange 1

4. Log Management:
   - set-date <YYYY-MM-DD>    : Set the current working date
     Example: set-date 2023-04-15
   
   - add-food <food_id> <servings>
     Example: add-food apple 2
   
   - remove-food <food_id>    : Remove a food from the current day's log
   
   - view-log [date]          : View the food log for the current or specified date
   
   - undo                     : Undo the last log operation
   
   - redo                     : Redo the last undone operation

5. User Profile and Calorie Tracking:
   - profile                  : Display user profile information
   
   - profile <attribute> <value>
     Example: profile weight 70
     Example: profile activity 3
   
   - calories [date]          : Show calorie intake and target for a specific date

6. Data Management:
   - save                     : Save all data to files
   
   - load                     : Load all data from files

EXAMPLE SESSION
--------------
Here's a quick sample session to get you started:

> add-basic-food 200 chicken protein meat
Food 'chicken' added successfully.

> add-food chicken 2
Added 2 serving(s) of 'chicken' (400 calories) to the log.

> view-log
Food Log for 2023-04-15:
Food                 Servings   Calories
--------------------------------------------------
chicken              2          400
--------------------------------------------------
TOTAL                           400

> calories
Calorie Summary for 2023-04-15:
Consumed Calories: 400
Target Calories: 2100
Difference: -1700 (Under target)

INCLUDED TEST DATA
-----------------
The application comes with pre-populated data:

1. Basic Foods (18 items):
   - Common foods like apple, banana, chicken, rice, etc.
   - Each with associated calories and keywords

2. Composite Foods (15 items):
   - Combinations like fruit_salad, chicken_rice_bowl, etc.
   - Shows how basic foods can be combined

3. Sample Logs:
   - A sample log entry is included to demonstrate format

You can view this data using the list-foods and view-log commands.
