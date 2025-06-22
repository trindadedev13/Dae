
def color(text, code)
  "\e[#{code}m#{text}\e[0m"
end

def green(text); color(text, 32); end
def red(text);   color(text, 31); end
def yellow(text);color(text, 33); end