import os
import re

def parse_files(directory):
    results = [[] for _ in range(10)]
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
                            num_spaces = len(line) - len(line.lstrip())
                            if total > 0 and (best / total) > 0.95:
                                results[num_spaces // 4].append((filename, line_num, line.strip()))
    return results

def main():
    directory = "/tmp/evals"  # Change this to the path where your files are located if necessary
    
    if not os.path.exists(directory):
        print(f"Directory {directory} does not exist.")
        return
    
    occurrences = parse_files(directory)
    
    for depth, depth_occurrences in enumerate(occurrences):
        print(f"Found {len(depth_occurrences)} occurrences at depth {depth}:")
        for occurrence in depth_occurrences:
            print(f"File: {occurrence[0]}, Line: {occurrence[1]}, Content: {occurrence[2]}")

if __name__ == "__main__":
    main()