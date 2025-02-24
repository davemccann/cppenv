import argparse
from pathlib import Path

def collect_commands(path, searchPattern, outputPath):
    compile_commands = []
    for file in Path(path).rglob(searchPattern):
        if Path(outputPath).exists() and file.samefile(outputPath):
            continue
        with open(file, "r") as f:
            compile_commands.append(f.read())
    return compile_commands

def save(compile_commands, output_file):
    with open(output_file, "w") as f:
        f.write('[')
        for compile_command in compile_commands:
            f.write(compile_command)
        f.write(']')

def main():
 
    parser = argparse.ArgumentParser()
    parser.add_argument("-d", "--dir", help="The root directory to start the file search at", required=True)
    parser.add_argument("-p", "--pattern", help = "The file pattern to recursively search for that will be merged", required=True)
    parser.add_argument("-o", "--output", help = "The filepath to output the merged contents to", required=True)
    args = parser.parse_args()

    print("Merging compile_commands.json...")
    compile_commands = collect_commands(args.dir, args.pattern, args.output)
    save(compile_commands, args.output)
    print("Successfully merged")

if __name__ == "__main__":
    main()
