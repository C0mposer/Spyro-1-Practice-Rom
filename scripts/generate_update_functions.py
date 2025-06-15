import os
import re

# Regex to find functions like: void AnyName_Update(void)
# It captures the function name.
print(os.getcwd())
function_pattern = re.compile(r"void\s+([a-zA-Z0-9_]+_Update)\s*\(\s*void\s*\)")
source_dir = "mods/PracticeCodes/src"  # Your source code directory
output_file = "include/autogen_updaters.h"

update_functions = []

# Find all function names
for filename in os.listdir(source_dir):
    if filename.endswith(".c"):
        with open(os.path.join(source_dir, filename), 'r') as f:
            content = f.read()
            matches = function_pattern.findall(content)
            update_functions.extend(matches)

# Generate the C file
with open(output_file, 'w') as f:
    f.write('// This file is auto-generated. DO NOT EDIT.\n\n')

    # Write extern declarations for all found functions
    for func_name in update_functions:
        f.write(f'extern void {func_name}(void);\n')
    f.write('\n')

    # Create the array of function pointers
    f.write('const UpdateFunc g_UpdateFunctions[] = {\n')
    for func_name in update_functions:
        f.write(f'    &{func_name},\n')
    f.write('};\n\n')

    # Create a count of the functions
    f.write('const int g_UpdateFunctionCount = sizeof(g_UpdateFunctions) / sizeof(g_UpdateFunctions[0]);\n')

print(f"Generated {output_file} with {len(update_functions)} functions.")