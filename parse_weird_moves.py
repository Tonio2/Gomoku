import os
import re

def parse_files(directory):
    results = []
    for filename in os.listdir(directory):
        if filename.endswith('.txt'):
            filepath = os.path.join(directory, filename)
            
            with open(filepath, 'r') as file:
                lines = file.readlines()
                for line_num, line in enumerate(lines, start=1):
                    if "Stats: Best:" in line:
                        match = re.search(r"Best:\s*(\d+)\s*\|\s*Evaluated:\s*(\d+)\s*\|\s*Total:\s*(\d+)", line)
                        if match:
                            best = int(match.group(1))
                            total = int(match.group(3))
                            if total > 0 and (best / total) > 0.95:
                                results.append((filename, line_num, line.strip()))
    return results

def parse_files_for_weird_first_move(directory):
    results = []
    for filename in os.listdir(directory):
        if filename.endswith('.txt'):
            filepath = os.path.join(directory, filename)
            
            with open(filepath, 'r') as file:
                lines = file.readlines()
                line = lines[1]
                if "Stats: Best:" in line:
                    match = re.search(r"Best:\s*(\d+)\s*\|\s*Evaluated:\s*(\d+)\s*\|\s*Total:\s*(\d+)", line)
                    if match:
                        best = int(match.group(1))
                        total = int(match.group(3))
                        if total > 0 and (best / total) > 0.90:
                            results.append((filename, 1, line.strip()))
    return results

def main():
    directory = "/tmp/move_evaluation"  # Change this to the path where your files are located if necessary
    # occurrences = parse_files(directory)
    occurrences = parse_files_for_weird_first_move(directory)
    
    for occurrence in occurrences:
        print(f"File: {occurrence[0]}, Line: {occurrence[1]}, Content: {occurrence[2]}")

if __name__ == "__main__":
    main()