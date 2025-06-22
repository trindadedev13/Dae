require "fileutils"
require "./color.rb"

def run(cmd)
  success = system(cmd)
  abort(red("Failed to run: #{cmd}")) unless success
end

def check_clang_format
  found = system("clang-format --version > #{dev_null} 2>&1")
  unless found
    puts red("clang-format not found!")
    puts yellow("Please install clang-format and try again.")
    exit(1)
  end
end

def dev_null
  Gem.win_platform? ? "NUL" : "/dev/null"
end

if ARGV.length < 1
  puts red("Usage: ruby format.rb <base_dir>")
  exit(1)
end

check_clang_format

base_dir = File.expand_path(ARGV[0])
extensions = ["c", "h", "cpp", "hpp"]

extensions.each do |ext|
  Dir.glob(File.join(base_dir, "**", "*.#{ext}")).each do |file|
    next if file.split(File::SEPARATOR).include?("build")

    puts green("Formatted: ") + file
    run("clang-format -i \"#{file}\"")
  end
end