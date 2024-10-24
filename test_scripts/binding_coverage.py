import inspect
import zoom_meeting_sdk as zoom

import subprocess
import sys
import os

def strip_prefix(name, prefixes=[]):    
    lower_name = name.lower()
    for prefix in prefixes:
        if lower_name.startswith(prefix.lower()):
            return name[len(prefix):]
    return name

def generate_tags(directory):
    """Generate ctags output for all header files in directory."""
    # Run ctags with needed options
    # --fields=+S adds scope information
    # --c++-kinds=+p includes function declarations
    # -R for recursive
    cmd = [
        'ctags',
        '--fields=+S',  # Include scope information
        '--c++-kinds=+p',  # Include prototypes
        '-R',  # Recursive
        '--language-force=C++',
        '-f', '-',  # Output to stdout
        '--output-format=json',  # JSON output for easier parsing
        directory
    ]
    
    try:
        result = subprocess.run(cmd, capture_output=True, text=True)
        return result.stdout.splitlines()
    except subprocess.CalledProcessError as e:
        print(f"Error running ctags: {e}", file=sys.stderr)
        sys.exit(1)

def analyze_tags(tags_output):
    """Analyze tags output and categorize items."""
    classes = []
    top_level_functions = []
    
    for line in tags_output:
        if not line:
            continue
            
        # Parse JSON line
        try:
            import json
            tag = json.loads(line)
        except json.JSONDecodeError:
            continue
            
        kind = tag.get('kind', '')
        name = tag.get('name', '')
        scope = tag.get('scope', '')

        # Collect classes
        if kind == 'class':
            classes.append(name)
            
        # Collect structs
        elif kind == 'struct':
            classes.append(strip_prefix(name, ["tag"]))
            
        # Collect only top-level functions (those without a scope)
        elif kind in ('function', 'prototype') and not scope:
            top_level_functions.append(name)
    
    return set(classes), set(top_level_functions)

if __name__ == "__main__":

    tags_output = generate_tags("src/zoomsdk/h")
    classes, functions = analyze_tags(tags_output)
    
    functions_that_get_missed_by_analyze_tags = {"GetZoomLastError", "GetSDKVersion", "CleanUPSDK", "DestroyNetworkConnectionHelper", "CreateNetworkConnectionHelper", "DestroySettingService", "CreateSettingService", "DestroyAuthService", "CreateAuthService", "DestroyMeetingService", "CreateMeetingService", "SwitchDomain", "InitSDK"}
    functions = functions.union(functions_that_get_missed_by_analyze_tags)

    bound_classes = set([name for name, obj in inspect.getmembers(zoom) if type(obj).__name__ == 'nb_type_0']).intersection(classes)
    bound_functions = set([name for name, obj in inspect.getmembers(zoom) if type(obj).__name__ == 'nb_func']).intersection(functions)

    print("classes coverage = ", len(bound_classes), '/', len(classes), '=', len(bound_classes) / float(len(classes)))
    print("functions coverage = ", len(bound_functions), '/', len(functions), '=', len(bound_functions) / float(len(functions)))