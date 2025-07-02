require 'find'

project_dir = ARGV[0] || "."
output_file = "rep.txt"

def to_snake_case(name)
  name.gsub(/([a-z0-9])([A-Z])/, '\1_\2').downcase
end

def extract_function_names_from_line(line)
  match = line.match(/\b([a-zA-Z_][a-zA-Z0-9_]*)\s*\(.*\)\s*;/)
  match ? match[1] : nil
end

function_names = []

Find.find(project_dir) do |path|
  next unless path.end_with?(".h")

  File.foreach(path) do |line|
    line.strip!
    next if line.empty? || line.start_with?("//") || line.start_with?("#")

    name = extract_function_names_from_line(line)
    function_names << name if name
  end
end

File.open(output_file, "w") do |file|
  function_names.uniq.each do |fn|
    torep = to_snake_case(fn)
    file.puts "#{fn}:klt_#{torep}"
  end
end